#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
juce::AudioProcessorValueTreeState::ParameterLayout
PersoFormantAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> p;

    // ---- Vowel morph ----
    p.push_back(std::make_unique<juce::AudioParameterInt>  ("vowel1",          "Vowel 1",      0,11,0));
    p.push_back(std::make_unique<juce::AudioParameterInt>  ("vowel2",          "Vowel 2",      0,11,2));
    p.push_back(std::make_unique<juce::AudioParameterFloat>("morph",           "Manual Morph",
        juce::NormalisableRange<float>(0.f,1.f,0.001f),0.5f));

    // ---- LFO ----
    p.push_back(std::make_unique<juce::AudioParameterFloat>("lfoRate",   "LFO Rate",
        juce::NormalisableRange<float>(0.1f,10.f,0.01f,0.5f),1.f));
    p.push_back(std::make_unique<juce::AudioParameterFloat>("lfoDepth",  "LFO Depth",
        juce::NormalisableRange<float>(0.f,1.f,0.001f),1.0f));
    p.push_back(std::make_unique<juce::AudioParameterBool> ("lfoSync",   "LFO Sync",   false));
    p.push_back(std::make_unique<juce::AudioParameterInt>  ("lfoDiv",    "LFO Division",0,5,3));

    // LFO routing — each independently toggles that destination
    p.push_back(std::make_unique<juce::AudioParameterBool>("lfoToMorph",  "LFO→Morph",  true));
    p.push_back(std::make_unique<juce::AudioParameterBool>("lfoToShift",  "LFO→Shift",  false));
    p.push_back(std::make_unique<juce::AudioParameterBool>("lfoToCutoff", "LFO→Cutoff", false));

    // ---- Envelope follower ----
    p.push_back(std::make_unique<juce::AudioParameterFloat>("envThresh","Env Threshold",
        juce::NormalisableRange<float>(-60.f,0.f,0.1f),-20.f));
    p.push_back(std::make_unique<juce::AudioParameterFloat>("envDepth", "Env Depth",
        juce::NormalisableRange<float>(0.f,1.f,0.001f),0.f));

    // ---- Formant filter ----
    p.push_back(std::make_unique<juce::AudioParameterFloat>("formantRes","Formant Res",
        juce::NormalisableRange<float>(0.5f,20.f,0.01f,0.5f),8.f));
    p.push_back(std::make_unique<juce::AudioParameterFloat>("formantShiftParam","Formant Shift",
        juce::NormalisableRange<float>(0.5f,2.0f,0.001f),1.0f));

    // ---- Formant Cutoff — global offset that slides all peaks together ----
    // Range: -800..+800 Hz offset, default 0 (centre = no change)
    // When cutoffEnabled=false the offset is forced to 0 in processBlock.
    p.push_back(std::make_unique<juce::AudioParameterFloat>("cutoffOffset","Cutoff Offset",
        juce::NormalisableRange<float>(-800.f,800.f,1.f),0.f));
    p.push_back(std::make_unique<juce::AudioParameterBool>("cutoffEnabled","Cutoff Enable",false));

    // ---- Gender ----
    p.push_back(std::make_unique<juce::AudioParameterInt>("genderShift","Gender Shift",0,2,1));

    // ---- Output ----
    p.push_back(std::make_unique<juce::AudioParameterFloat>("drive",  "Drive",
        juce::NormalisableRange<float>(1.f,10.f,0.01f,0.5f),1.f));
    p.push_back(std::make_unique<juce::AudioParameterFloat>("wetGain","Wet Gain",
        juce::NormalisableRange<float>(-12.f,12.f,0.1f),0.f));
    p.push_back(std::make_unique<juce::AudioParameterFloat>("mix",   "Mix",
        juce::NormalisableRange<float>(0.f,1.f,0.001f),1.f));

    return {p.begin(),p.end()};
}

//==============================================================================
PersoFormantAudioProcessor::PersoFormantAudioProcessor()
    : AudioProcessor(BusesProperties()
        .withInput ("Input",  juce::AudioChannelSet::stereo(),true)
        .withOutput("Output", juce::AudioChannelSet::stereo(),true)),
      apvts(*this,nullptr,"PersoFormantState",createParameterLayout())
{
    // Initialise display atomics
    for (int k=0;k<3;++k) {
        displayFreq[k].store(kVowelFormants[0].freq[k]);
        displayGain[k].store(kVowelFormants[0].gain[k]);
        displayQ[k]   .store(kVowelFormants[0].q[k]);
    }

    for (auto* id : {"vowel1","vowel2","morph","genderShift","formantRes","formantShiftParam"})
        apvts.addParameterListener(id,this);
}

PersoFormantAudioProcessor::~PersoFormantAudioProcessor()
{
    for (auto* id : {"vowel1","vowel2","morph","genderShift","formantRes","formantShiftParam"})
        apvts.removeParameterListener(id,this);
}

void PersoFormantAudioProcessor::parameterChanged(const juce::String&,float) {}

//==============================================================================
FormantData PersoFormantAudioProcessor::interpolateFormants(int v1,int v2,float t,float sh) const
{
    v1=juce::jlimit(0,11,v1); v2=juce::jlimit(0,11,v2); t=juce::jlimit(0.f,1.f,t);
    const FormantData& f1=kVowelFormants[v1]; const FormantData& f2=kVowelFormants[v2];
    FormantData out;
    for (int k=0;k<3;++k) {
        out.freq[k]=(f1.freq[k]+t*(f2.freq[k]-f1.freq[k]))*sh;
        out.gain[k]= f1.gain[k]+t*(f2.gain[k]-f1.gain[k]);
        out.q[k]   = f1.q[k]  +t*(f2.q[k]   -f1.q[k]);
    }
    return out;
}

//==============================================================================
float PersoFormantAudioProcessor::getLfoRateHz() const
{
    if (!(bool)(int)getParam("lfoSync")) return getParam("lfoRate");
    double bpm=120.0;
    if (auto* ph=getPlayHead())
        if (auto pos=ph->getPosition())
            if (pos->getBpm().hasValue()) bpm=*pos->getBpm();
    int div=juce::jlimit(0,5,(int)getParam("lfoDiv"));
    return (float)(bpm/60.0)/kSyncDivBeats[div];
}

//==============================================================================
void PersoFormantAudioProcessor::updateFormantCoeffs(
    float morph, float shiftMult, float cutoffOffsetHz, float sr)
{
    int   v1      = juce::jlimit(0,11,(int)getParam("vowel1"));
    int   v2      = juce::jlimit(0,11,(int)getParam("vowel2"));
    float userRes = getParam("formantRes");

    FormantData fd = interpolateFormants(v1,v2,morph,shiftMult);

    for (int k=0;k<3;++k) {
        // Apply the cutoff offset to all three peaks — this is the "slide all peaks" feel
        float rawFreq = fd.freq[k] + cutoffOffsetHz;

        targetFreq[k] = rawFreq;
        targetGain[k] = fd.gain[k];
        targetQ[k]    = fd.q[k] * (userRes/8.f);

        // Glide toward target
        currentFreq[k] = currentFreq[k]*freqGlideCoeff + targetFreq[k]*(1.f-freqGlideCoeff);
        currentGain[k] = currentGain[k]*freqGlideCoeff + targetGain[k]*(1.f-freqGlideCoeff);
        currentQ[k]    = currentQ[k]   *freqGlideCoeff + targetQ[k]   *(1.f-freqGlideCoeff);

        float safeFreq = juce::jlimit(30.f,(float)(sr*0.47f),currentFreq[k]);
        float safeQ    = juce::jlimit(0.5f,50.f,currentQ[k]);

        for (int ch=0;ch<2;++ch)
            formantFilters[ch][k].setCoefficients(safeFreq,currentGain[k],safeQ,sr);

        // Publish display values for the UI visualiser
        displayFreq[k].store(safeFreq);
        displayGain[k].store(currentGain[k]);
        displayQ[k]   .store(safeQ);
    }
}

//==============================================================================
void PersoFormantAudioProcessor::prepareToPlay(double sr,int blockSize)
{
    currentSampleRate=sr; currentBlockSize=blockSize;
    double bps=sr/(double)juce::jmax(1,blockSize);
    freqGlideCoeff=(float)std::exp(-1.0/(bps*0.030));

    for (int ch=0;ch<2;++ch) {
        for (int k=0;k<3;++k) formantFilters[ch][k].reset();
        morphSmoother[ch].reset(sr,20.0);
        envFollower[ch].reset(sr);
    }
    int v1=juce::jlimit(0,11,(int)getParam("vowel1"));
    for (int k=0;k<3;++k) {
        currentFreq[k]=targetFreq[k]=kVowelFormants[v1].freq[k];
        currentGain[k]=targetGain[k]=kVowelFormants[v1].gain[k];
        currentQ[k]   =targetQ[k]   =kVowelFormants[v1].q[k];
    }
    updateFormantCoeffs(0.f,1.f,0.f,(float)sr);
}

void PersoFormantAudioProcessor::releaseResources() {}

//==============================================================================
void PersoFormantAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer,juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;
    const int N=buffer.getNumSamples(), CH=juce::jmin(buffer.getNumChannels(),2);
    if (N==0||CH==0) return;

    // --- Read all parameters ---
    float manualMorph   = getParam("morph");
    float lfoDepth      = getParam("lfoDepth");
    float envThreshDB   = getParam("envThresh");
    float envDepth      = getParam("envDepth");
    float drive         = getParam("drive");
    int   gender        = (int)getParam("genderShift");
    float wetGainDB     = getParam("wetGain");
    float mix           = getParam("mix");
    float userShift     = getParam("formantShiftParam");
    float cutoffOffset  = getParam("cutoffOffset");
    bool  cutoffEnabled = (bool)(int)getParam("cutoffEnabled");

    // LFO routing flags
    bool lfoToMorph  = (bool)(int)getParam("lfoToMorph");
    bool lfoToShift  = (bool)(int)getParam("lfoToShift");
    bool lfoToCutoff = (bool)(int)getParam("lfoToCutoff");

    float genderMult   = (gender==0)?0.87f:(gender==2)?1.12f:1.0f;
    float wetGain      = juce::Decibels::decibelsToGain(wetGainDB);
    float envThreshLin = juce::Decibels::decibelsToGain(envThreshDB);

    // --- LFO ---
    float lfoHz  = getLfoRateHz();
    float lfoInc = juce::MathConstants<float>::twoPi*lfoHz/(float)currentSampleRate;
    float lfoVal = std::sin(lfoPhase)*0.5f+0.5f;   // 0..1 unipolar
    lfoPhase += lfoInc*(float)N;
    if (lfoPhase >= juce::MathConstants<float>::twoPi)
        lfoPhase -= juce::MathConstants<float>::twoPi;

    // --- Envelope follower ---
    float envIn=0.f;
    for (int ch=0;ch<CH;++ch)
        for (int s=0;s<N;++s) envIn+=std::abs(buffer.getSample(ch,s));
    envIn /= (float)(CH*N);
    float envVal = envFollower[0].process(envIn);
    envelopeLevel.store(envVal);

    float envMod=0.f;
    if (envVal>envThreshLin)
        envMod=juce::jlimit(0.f,1.f,(envVal-envThreshLin)/(1.f-envThreshLin+1e-6f));

    // --- Compute modulated parameters ---
    // Morph: manual + optional LFO + envelope
    float morphMod   = lfoToMorph  ? lfoDepth*lfoVal : 0.f;
    float finalMorph = juce::jlimit(0.f,1.f, manualMorph + morphMod + envDepth*envMod);
    currentMorphDisplay.store(finalMorph);

    // Shift: base * optional LFO (±20% swing)
    float shiftMod   = lfoToShift  ? 1.f + (lfoVal*2.f-1.f)*0.20f*lfoDepth : 1.f;
    float shiftMult  = userShift * genderMult * shiftMod;

    // Cutoff offset: off=0, on=knob value + optional LFO swing (±400Hz per depth)
    float activeCutoff = 0.f;
    if (cutoffEnabled) {
        float cutoffLfoMod = lfoToCutoff ? (lfoVal*2.f-1.f)*400.f*lfoDepth : 0.f;
        activeCutoff = cutoffOffset + cutoffLfoMod;
    }

    // --- Update filters (includes glide) ---
    updateFormantCoeffs(finalMorph, shiftMult, activeCutoff, (float)currentSampleRate);

    // --- Save dry copy ---
    juce::AudioBuffer<float> dry;
    dry.makeCopyOf(buffer);

    // --- Wet processing ---
    for (int s=0;s<N;++s)
        for (int ch=0;ch<CH;++ch) {
            float x=buffer.getSample(ch,s);
            if (drive>1.001f) x=softClip(x,drive);
            for (int k=0;k<3;++k) x=formantFilters[ch][k].process(x);
            buffer.setSample(ch,s,x);
        }

    buffer.applyGain(wetGain);

    // --- Dry/wet crossfade ---
    for (int ch=0;ch<CH;++ch) {
        auto* w=buffer.getWritePointer(ch);
        auto* d=dry.getReadPointer(ch);
        for (int s=0;s<N;++s) w[s]=d[s]*(1.f-mix)+w[s]*mix;
    }
}

//==============================================================================
void PersoFormantAudioProcessor::getStateInformation(juce::MemoryBlock& dest)
{
    auto state=apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml,dest);
}
void PersoFormantAudioProcessor::setStateInformation(const void* data,int size)
{
    std::unique_ptr<juce::XmlElement> xml(getXmlFromBinary(data,size));
    if (xml && xml->hasTagName(apvts.state.getType()))
        apvts.replaceState(juce::ValueTree::fromXml(*xml));
}

juce::AudioProcessorEditor* PersoFormantAudioProcessor::createEditor()
{ return new PersoFormantAudioProcessorEditor(*this); }

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{ return new PersoFormantAudioProcessor(); }
