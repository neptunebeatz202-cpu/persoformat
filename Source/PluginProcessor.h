#pragma once
#include <JuceHeader.h>

//==============================================================================
// Formant data: {F1, F2, F3} center frequencies (Hz), gains (dB), Q-factors
struct FormantData
{
    float freq[3];
    float gain[3];
    float q[3];
};

// Standard male formant frequencies for 5 vowels
// Source: Peterson & Barney (1952) / IPA reference values
static const FormantData kVowelFormants[5] = {
    // 'a' (as in "father")
    { { 800.0f,  1200.0f, 2500.0f }, { 0.0f, -4.0f, -8.0f  }, { 8.0f, 10.0f, 12.0f } },
    // 'e' (as in "bed")
    { { 600.0f,  1900.0f, 2550.0f }, { 0.0f, -2.0f, -6.0f  }, { 8.0f, 10.0f, 12.0f } },
    // 'i' (as in "feet")
    { { 300.0f,  2200.0f, 3000.0f }, { 0.0f, -3.0f, -7.0f  }, { 8.0f, 10.0f, 12.0f } },
    // 'o' (as in "go")
    { { 450.0f,   750.0f, 2400.0f }, { 0.0f, -5.0f, -9.0f  }, { 8.0f, 10.0f, 12.0f } },
    // 'u' (as in "boot")
    { { 300.0f,   700.0f, 2200.0f }, { 0.0f, -6.0f, -10.0f }, { 8.0f, 10.0f, 12.0f } },
};

//==============================================================================
// Smooth single-pole smoother for parameter gliding
class ParameterSmoother
{
public:
    void reset (double sampleRate, double smoothTimeMs)
    {
        coeff = std::exp (-1.0 / (sampleRate * smoothTimeMs * 0.001));
        currentValue = 0.0f;
    }
    float process (float target)
    {
        currentValue = currentValue * (float)coeff + target * (1.0f - (float)coeff);
        return currentValue;
    }
    void setCurrentAndTarget (float v) { currentValue = v; }
    float getCurrentValue() const { return currentValue; }
private:
    double coeff = 0.0;
    float currentValue = 0.0f;
};

//==============================================================================
// Single bandpass filter (biquad) for one formant peak
class FormantFilter
{
public:
    void reset() { x1 = x2 = y1 = y2 = 0.0f; }

    void setCoefficients (float centerFreq, float gainDB, float q, float sampleRate)
    {
        // Peaking EQ (Bell) biquad — ideal for formant shaping
        float A  = std::pow (10.0f, gainDB / 40.0f);
        float w0 = juce::MathConstants<float>::twoPi * centerFreq / sampleRate;
        float cosw0 = std::cos (w0);
        float sinw0 = std::sin (w0);
        float alpha  = sinw0 / (2.0f * q);

        float b0 =  1.0f + alpha * A;
        float b1 = -2.0f * cosw0;
        float b2 =  1.0f - alpha * A;
        float a0 =  1.0f + alpha / A;
        float a1 = -2.0f * cosw0;
        float a2 =  1.0f - alpha / A;

        float invA0 = 1.0f / a0;
        c_b0 = b0 * invA0;
        c_b1 = b1 * invA0;
        c_b2 = b2 * invA0;
        c_a1 = a1 * invA0;
        c_a2 = a2 * invA0;
    }

    inline float process (float x)
    {
        float y = c_b0 * x + c_b1 * x1 + c_b2 * x2 - c_a1 * y1 - c_a2 * y2;
        x2 = x1; x1 = x;
        y2 = y1; y1 = y;
        return y;
    }

private:
    float c_b0=1,c_b1=0,c_b2=0,c_a1=0,c_a2=0;
    float x1=0,x2=0,y1=0,y2=0;
};

//==============================================================================
// Envelope follower for input-sync (auto-vowel trigger)
class EnvelopeFollower
{
public:
    void reset (double sampleRate)
    {
        attackCoeff  = std::exp (-1.0 / (sampleRate * 0.010)); // 10ms attack
        releaseCoeff = std::exp (-1.0 / (sampleRate * 0.100)); // 100ms release
        env = 0.0f;
    }
    float process (float in)
    {
        float absIn = std::abs (in);
        float coeff = absIn > env ? attackCoeff : releaseCoeff;
        env = env * coeff + absIn * (1.0f - coeff);
        return env;
    }
private:
    float attackCoeff=0, releaseCoeff=0, env=0;
};

//==============================================================================
class PersoFormantAudioProcessor : public juce::AudioProcessor,
                                   public juce::AudioProcessorValueTreeState::Listener
{
public:
    PersoFormantAudioProcessor();
    ~PersoFormantAudioProcessor() override;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return JucePlugin_Name; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram (int) override {}
    const juce::String getProgramName (int) override { return {}; }
    void changeProgramName (int, const juce::String&) override {}

    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    void parameterChanged (const juce::String& paramID, float newValue) override;

    juce::AudioProcessorValueTreeState apvts;

    // Called from editor to get LFO phase for animation
    float getLfoPhase() const { return lfoPhase; }
    float getEnvelopeLevel() const { return envelopeLevel.load(); }

private:
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    // Parameter helpers
    float getParam (const juce::String& id) const
    {
        return apvts.getRawParameterValue (id)->load();
    }

    // DSP state
    double currentSampleRate = 44100.0;
    int    currentBlockSize  = 512;

    // Per-channel formant filters (3 formants × 2 channels)
    FormantFilter formantFilters[2][3];

    // Smoothed morph value (prevents clicks during morph changes)
    ParameterSmoother morphSmoother[2]; // per channel (identical target, separate state)
    float smoothedMorph = 0.0f;

    // Smoothed formant frequencies for glide
    float currentFreq[3]  = { 0,0,0 };
    float currentGain[3]  = { 0,0,0 };
    float currentQ[3]     = { 0,0,0 };
    float targetFreq[3]   = { 0,0,0 };
    float targetGain[3]   = { 0,0,0 };
    float targetQ[3]      = { 0,0,0 };
    float freqGlideCoeff  = 0.999f; // smoothing coefficient for freq glide

    // LFO
    float lfoPhase = 0.0f;

    // Envelope follower
    EnvelopeFollower envFollower[2];
    std::atomic<float> envelopeLevel { 0.0f };

    // Drive (soft-clip saturation)
    static inline float softClip (float x, float drive)
    {
        x *= drive;
        return x / (1.0f + std::abs (x));
    }

    void updateFormantCoeffs (float morph, float formantShift, float sampleRate);
    FormantData interpolateFormants (int vowel1, int vowel2, float t, float shiftMult) const;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PersoFormantAudioProcessor)
};
