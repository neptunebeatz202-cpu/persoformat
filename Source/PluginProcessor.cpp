#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
juce::AudioProcessorValueTreeState::ParameterLayout
PersoFormantAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    // Vowel selectors (0=a,1=e,2=i,3=o,4=u)
    params.push_back (std::make_unique<juce::AudioParameterInt>  ("vowel1",    "Vowel 1",      0, 4, 0));
    params.push_back (std::make_unique<juce::AudioParameterInt>  ("vowel2",    "Vowel 2",      0, 4, 2));

    // Morph (0=Vowel1, 1=Vowel2)
    params.push_back (std::make_unique<juce::AudioParameterFloat>("morph",     "Manual Morph",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.001f), 0.0f));

    // LFO
    params.push_back (std::make_unique<juce::AudioParameterFloat>("lfoRate",   "LFO Rate",
        juce::NormalisableRange<float>(0.1f, 10.0f, 0.01f, 0.5f), 1.0f));
    params.push_back (std::make_unique<juce::AudioParameterFloat>("lfoDepth",  "LFO Depth",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.001f), 0.0f));
    params.push_back (std::make_unique<juce::AudioParameterBool> ("lfoSync",   "LFO Sync",     false));

    // Envelope follower
    params.push_back (std::make_unique<juce::AudioParameterFloat>("envThresh", "Env Threshold",
        juce::NormalisableRange<float>(-60.0f, 0.0f, 0.1f), -20.0f));
    params.push_back (std::make_unique<juce::AudioParameterFloat>("envDepth",  "Env Depth",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.001f), 0.0f));

    // Drive
    params.push_back (std::make_unique<juce::AudioParameterFloat>("drive",     "Drive",
        juce::NormalisableRange<float>(1.0f, 10.0f, 0.01f, 0.5f), 1.0f));

    // Formant shift (0=masculine, 1=neutral, 2=feminine)
    params.push_back (std::make_unique<juce::AudioParameterInt>  ("genderShift","Gender Shift", 0, 2, 1));

    // Output
    params.push_back (std::make_unique<juce::AudioParameterFloat>("pan",       "Pan",
        juce::NormalisableRange<float>(-1.0f, 1.0f, 0.001f), 0.0f));
    params.push_back (std::make_unique<juce::AudioParameterFloat>("gain",      "Output Gain",
        juce::NormalisableRange<float>(-24.0f, 12.0f, 0.1f), 0.0f));

    // Mix (dry/wet)
    params.push_back (std::make_unique<juce::AudioParameterFloat>("mix",       "Mix",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.001f), 1.0f));

    return { params.begin(), params.end() };
}

//==============================================================================
PersoFormantAudioProcessor::PersoFormantAudioProcessor()
    : AudioProcessor (BusesProperties()
        .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
        .withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
      apvts (*this, nullptr, "PersoFormantState", createParameterLayout())
{
    apvts.addParameterListener ("vowel1",     this);
    apvts.addParameterListener ("vowel2",     this);
    apvts.addParameterListener ("morph",      this);
    apvts.addParameterListener ("genderShift",this);
}

PersoFormantAudioProcessor::~PersoFormantAudioProcessor()
{
    apvts.removeParameterListener ("vowel1",     this);
    apvts.removeParameterListener ("vowel2",     this);
    apvts.removeParameterListener ("morph",      this);
    apvts.removeParameterListener ("genderShift",this);
}

//==============================================================================
void PersoFormantAudioProcessor::parameterChanged (const juce::String&, float)
{
    // Triggers re-computation of glide targets on next block
}

//==============================================================================
FormantData PersoFormantAudioProcessor::interpolateFormants (
    int vowel1, int vowel2, float t, float shiftMult) const
{
    vowel1 = juce::jlimit (0, 4, vowel1);
    vowel2 = juce::jlimit (0, 4, vowel2);
    t      = juce::jlimit (0.0f, 1.0f, t);

    const FormantData& f1 = kVowelFormants[vowel1];
    const FormantData& f2 = kVowelFormants[vowel2];

    FormantData result;
    for (int k = 0; k < 3; ++k)
    {
        result.freq[k] = (f1.freq[k] + t * (f2.freq[k] - f1.freq[k])) * shiftMult;
        result.gain[k] =  f1.gain[k] + t * (f2.gain[k] - f1.gain[k]);
        // Keep Q proportional to frequency shift so bandwidth scales naturally
        result.q[k]    =  f1.q[k]   + t * (f2.q[k]   - f1.q[k]);
    }
    return result;
}

//==============================================================================
void PersoFormantAudioProcessor::updateFormantCoeffs (
    float morph, float formantShift, float sampleRate)
{
    int v1 = (int) getParam ("vowel1");
    int v2 = (int) getParam ("vowel2");

    FormantData fd = interpolateFormants (v1, v2, morph, formantShift);

    // Glide coefficients toward target (prevents frequency jumping)
    for (int k = 0; k < 3; ++k)
    {
        targetFreq[k] = fd.freq[k];
        targetGain[k] = fd.gain[k];
        targetQ[k]    = fd.q[k];

        // Smooth glide
        currentFreq[k] = currentFreq[k] * freqGlideCoeff + targetFreq[k] * (1.0f - freqGlideCoeff);
        currentGain[k] = currentGain[k] * freqGlideCoeff + targetGain[k] * (1.0f - freqGlideCoeff);
        currentQ[k]    = currentQ[k]    * freqGlideCoeff + targetQ[k]    * (1.0f - freqGlideCoeff);

        float safeFreq = juce::jlimit (20.0f, (float)(sampleRate * 0.49), currentFreq[k]);
        float safeQ    = juce::jlimit (0.5f, 50.0f, currentQ[k]);

        for (int ch = 0; ch < 2; ++ch)
            formantFilters[ch][k].setCoefficients (safeFreq, currentGain[k], safeQ, (float)sampleRate);
    }
}

//==============================================================================
void PersoFormantAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    currentSampleRate = sampleRate;
    currentBlockSize  = samplesPerBlock;

    // Glide time ~30ms at the given sample rate
    freqGlideCoeff = std::exp (-1.0 / (sampleRate * 0.030));

    for (int ch = 0; ch < 2; ++ch)
    {
        for (int k = 0; k < 3; ++k)
            formantFilters[ch][k].reset();

        morphSmoother[ch].reset (sampleRate, 20.0); // 20ms morph smoothing
        envFollower[ch].reset (sampleRate);
    }

    // Initialise current formant freqs to vowel1
    int v1 = (int) getParam ("vowel1");
    for (int k = 0; k < 3; ++k)
    {
        currentFreq[k] = kVowelFormants[v1].freq[k];
        currentGain[k] = kVowelFormants[v1].gain[k];
        currentQ[k]    = kVowelFormants[v1].q[k];
    }
}

void PersoFormantAudioProcessor::releaseResources() {}

//==============================================================================
void PersoFormantAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer,
                                               juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;

    const int numSamples  = buffer.getNumSamples();
    const int numChannels = juce::jmin (buffer.getNumChannels(), 2);

    // --- Read parameters ---
    float manualMorph  = getParam ("morph");
    float lfoRate      = getParam ("lfoRate");
    float lfoDepth     = getParam ("lfoDepth");
    float envThreshDB  = getParam ("envThresh");
    float envDepth     = getParam ("envDepth");
    float drive        = getParam ("drive");
    int   genderShift  = (int) getParam ("genderShift");
    float pan          = getParam ("pan");
    float gainDB       = getParam ("gain");
    float mix          = getParam ("mix");

    float formantShiftMult = (genderShift == 0) ? 0.87f : (genderShift == 2) ? 1.12f : 1.0f;
    float outputGain   = juce::Decibels::decibelsToGain (gainDB);
    float envThreshLin = juce::Decibels::decibelsToGain (envThreshDB);

    // LFO increment per sample
    float lfoInc = juce::MathConstants<float>::twoPi * lfoRate / (float)currentSampleRate;

    // Pan law (equal power)
    float panAngle = (pan + 1.0f) * juce::MathConstants<float>::halfPi * 0.5f;
    float panL = std::cos (panAngle);
    float panR = std::sin (panAngle);

    // Store dry signal for mix
    juce::AudioBuffer<float> dryBuffer;
    dryBuffer.makeCopyOf (buffer);

    for (int s = 0; s < numSamples; ++s)
    {
        // --- LFO ---
        float lfoVal = std::sin (lfoPhase) * 0.5f + 0.5f; // 0..1
        lfoPhase += lfoInc;
        if (lfoPhase >= juce::MathConstants<float>::twoPi)
            lfoPhase -= juce::MathConstants<float>::twoPi;

        // --- Envelope follower (average channels) ---
        float envIn = 0.0f;
        for (int ch = 0; ch < numChannels; ++ch)
            envIn += std::abs (buffer.getSample (ch, s));
        if (numChannels > 0) envIn /= numChannels;

        float envVal = envFollower[0].process (envIn);
        envelopeLevel.store (envVal);

        // Envelope-to-morph: above threshold, env drives morph toward 1
        float envMorphMod = 0.0f;
        if (envVal > envThreshLin)
            envMorphMod = juce::jlimit (0.0f, 1.0f, (envVal - envThreshLin) / (1.0f - envThreshLin + 1e-6f));

        // Final morph = manual + LFO mod + env mod (clamped)
        float finalMorph = juce::jlimit (0.0f, 1.0f,
            manualMorph + lfoDepth * lfoVal + envDepth * envMorphMod);

        // Update formant filters once per sample with smoothing
        updateFormantCoeffs (finalMorph, formantShiftMult, currentSampleRate);

        // --- Per-channel processing ---
        for (int ch = 0; ch < numChannels; ++ch)
        {
            float x = buffer.getSample (ch, s);

            // Drive (soft clip before filter — adds harmonics for better vowel audibility)
            x = softClip (x, drive);
            // Compensate drive gain
            x /= drive;

            // Serial formant filter chain (3 bandpass peaks)
            for (int k = 0; k < 3; ++k)
                x = formantFilters[ch][k].process (x);

            buffer.setSample (ch, s, x);
        }
    }

    // Apply mix (dry/wet)
    for (int ch = 0; ch < numChannels; ++ch)
    {
        auto* wet = buffer.getWritePointer (ch);
        auto* dry = dryBuffer.getReadPointer (ch);
        for (int s = 0; s < numSamples; ++s)
            wet[s] = dry[s] * (1.0f - mix) + wet[s] * mix;
    }

    // Apply pan and output gain
    if (numChannels >= 2)
    {
        buffer.applyGain (0, 0, numSamples, outputGain * panL * 2.0f);
        buffer.applyGain (1, 0, numSamples, outputGain * panR * 2.0f);
    }
    else if (numChannels == 1)
    {
        buffer.applyGain (outputGain);
    }
}

//==============================================================================
void PersoFormantAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml (state.createXml());
    copyXmlToBinary (*xml, destData);
}

void PersoFormantAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));
    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName (apvts.state.getType()))
            apvts.replaceState (juce::ValueTree::fromXml (*xmlState));
}

//==============================================================================
juce::AudioProcessorEditor* PersoFormantAudioProcessor::createEditor()
{
    return new PersoFormantAudioProcessorEditor (*this);
}

//==============================================================================
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new PersoFormantAudioProcessor();
}
