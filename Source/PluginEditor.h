#pragma once

#include <juce_audio_utils/juce_audio_utils.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_graphics/juce_graphics.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include "PluginProcessor.h"

//==============================================================================
class ChobitsLookAndFeel : public juce::LookAndFeel_V4
{
public:
    ChobitsLookAndFeel();

    void drawRotarySlider (juce::Graphics&, int x, int y, int width, int height,
                           float sliderPos, float rotaryStartAngle, float rotaryEndAngle,
                           juce::Slider&) override;

    void drawButtonBackground (juce::Graphics&, juce::Button&,
                                const juce::Colour&, bool highlighted, bool down) override;

    void drawButtonText (juce::Graphics&, juce::TextButton&,
                         bool highlighted, bool down) override;

    void drawComboBox (juce::Graphics&, int width, int height,
                       bool isButtonDown, int bx, int by, int bw, int bh,
                       juce::ComboBox&) override;

    void drawLabel (juce::Graphics&, juce::Label&) override;

    // Palette — sampled from the Chobits image
    static const juce::Colour bgDeep;         // deep night purple
    static const juce::Colour bgMid;          // mid purple
    static const juce::Colour starGold;       // gold stars
    static const juce::Colour pinkAccent;     // Chi's bow pink
    static const juce::Colour silhouetteBlue; // city silhouette
    static const juce::Colour hairGold;       // Chi's hair gold
    static const juce::Colour panelFill;      // translucent panel bg
};

//==============================================================================
struct StarParticle { float x, y, size, opacity, speed, angle, life; };

//==============================================================================
class PersoFormantAudioProcessorEditor : public juce::AudioProcessorEditor,
                                         private juce::Timer
{
public:
    explicit PersoFormantAudioProcessorEditor (PersoFormantAudioProcessor&);
    ~PersoFormantAudioProcessorEditor() override;

    void paint   (juce::Graphics&) override;
    void resized () override;

private:
    void timerCallback() override;
    void spawnStars (int count, float cx, float cy);
    void drawStarShape (juce::Graphics&, float cx, float cy, float r,
                        float opacity, juce::Colour, int pts = 5);
    void drawPanel (juce::Graphics&, juce::Rectangle<float> bounds,
                    const juce::String& title, juce::Colour accent);
    void drawEnvelopeMeter (juce::Graphics&);
    void drawLfoIndicator  (juce::Graphics&);

    PersoFormantAudioProcessor& audioProcessor;
    ChobitsLookAndFeel laf;
    juce::Image bgImage;

    // Sliders
    juce::Slider morphSlider, lfoRateSlider, lfoDepthSlider;
    juce::Slider envThreshSlider, envDepthSlider;
    juce::Slider driveSlider, panSlider, gainSlider, mixSlider;

    // ComboBoxes
    juce::ComboBox vowel1Box, vowel2Box, genderBox;

    // Buttons
    juce::ToggleButton lfoSyncButton;

    // Labels
    juce::Label morphLabel,    lfoRateLabel,  lfoDepthLabel;
    juce::Label envThreshLabel,envDepthLabel;
    juce::Label driveLabel,    panLabel,      gainLabel,   mixLabel;
    juce::Label vowel1Label,   vowel2Label,   genderLabel, lfoSyncLabel;
    juce::Label titleLabel,    vendorLabel;

    // APVTS attachments
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
        morphAtt, lfoRateAtt, lfoDepthAtt,
        envThreshAtt, envDepthAtt,
        driveAtt, panAtt, gainAtt, mixAtt;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment>
        vowel1Att, vowel2Att, genderAtt;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment>
        lfoSyncAtt;

    // Particles & animation
    std::vector<StarParticle> particles;
    juce::Random rng;
    float animPhase   = 0.0f;
    bool  lastEnvTrig = false;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PersoFormantAudioProcessorEditor)
};
