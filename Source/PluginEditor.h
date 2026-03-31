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
    void drawRotarySlider(juce::Graphics&,int x,int y,int w,int h,
                          float pos,float start,float end,juce::Slider&) override;
    void drawButtonBackground(juce::Graphics&,juce::Button&,const juce::Colour&,bool,bool) override;
    void drawButtonText(juce::Graphics&,juce::TextButton&,bool,bool) override;
    void drawComboBox(juce::Graphics&,int w,int h,bool,int,int,int,int,juce::ComboBox&) override;
    void drawLabel(juce::Graphics&,juce::Label&) override;

    static const juce::Colour bgDeep,bgMid,starGold,pinkAccent,silhouetteBlue,hairGold,panelFill;
};

//==============================================================================
struct StarParticle { float x,y,size,opacity,speed,angle,life; };
struct FloatStar    { float x,y,r,phase,driftX,driftY; };

//==============================================================================
class PersoFormantAudioProcessorEditor : public juce::AudioProcessorEditor,
                                         private juce::Timer
{
public:
    explicit PersoFormantAudioProcessorEditor(PersoFormantAudioProcessor&);
    ~PersoFormantAudioProcessorEditor() override;
    void paint(juce::Graphics&) override;
    void resized() override;

private:
    void timerCallback() override;
    void spawnStars(int n,float cx,float cy);
    void drawStarShape(juce::Graphics&,float cx,float cy,float r,float opacity,juce::Colour,int pts=5);
    void drawGlassPanel(juce::Graphics&,juce::Rectangle<float>,const juce::String&,juce::Colour accent);
    void drawSpectrumVisualiser(juce::Graphics&);   // NEW — real EQ curve display
    void drawEnvelopeMeter(juce::Graphics&);
    void drawChiEyeGlow(juce::Graphics&);
    void drawFloatingStars(juce::Graphics&);
    void updateSyncMode();

    // Helper: evaluate the summed bell response at a given Hz
    // freq, gain, q arrays → returns dB magnitude sum at evalHz
    float evalFormantResponse(float evalHz,
                               const float freq[3],const float gain[3],const float q[3]) const;

    PersoFormantAudioProcessor& audioProcessor;
    ChobitsLookAndFeel laf;
    juce::Image bgImage;

    // ---- Left — VOWEL MORPH panel ----
    juce::ComboBox vowel1Box,vowel2Box;
    juce::Slider   morphSlider;
    juce::ComboBox genderBox;
    juce::Label    vowel1Label,vowel2Label,morphLabel,genderLabel;

    // ---- Left — LFO panel ----
    juce::Slider       lfoRateSlider;
    juce::ComboBox     lfoDivBox;
    juce::Slider       lfoDepthSlider;
    juce::ToggleButton lfoSyncButton;
    juce::Label        lfoRateLabel,lfoDepthLabel,lfoSyncLabel;

    // LFO routing toggle buttons (3 destinations)
    juce::TextButton lfoToMorphBtn,lfoToShiftBtn,lfoToCutoffBtn;
    juce::Label      lfoRouteLabel;

    // ---- Right — FORMANT FILTER panel ----
    juce::Slider formantResSlider,formantShiftSlider;
    juce::Label  formantResLabel,formantShiftLabel;

    // Formant Cutoff: offset knob + enable toggle
    juce::Slider       cutoffSlider;
    juce::TextButton   cutoffEnableBtn;
    juce::Label        cutoffLabel;

    // ---- Right — ENV FOLLOWER panel ----
    juce::Slider envThreshSlider,envDepthSlider;
    juce::Label  envThreshLabel,envDepthLabel;

    // ---- Right — OUTPUT panel ----
    juce::Slider driveSlider,wetGainSlider,mixSlider;
    juce::Label  driveLabel,wetGainLabel,mixLabel;

    // ---- Title ----
    juce::Label titleLabel,vendorLabel;

    using SlAtt=juce::AudioProcessorValueTreeState::SliderAttachment;
    using CbAtt=juce::AudioProcessorValueTreeState::ComboBoxAttachment;
    using BtAtt=juce::AudioProcessorValueTreeState::ButtonAttachment;

    std::unique_ptr<SlAtt> morphAtt,lfoRateAtt,lfoDepthAtt,
                            envThreshAtt,envDepthAtt,
                            formantResAtt,formantShiftAtt,
                            cutoffAtt,
                            driveAtt,wetGainAtt,mixAtt;
    std::unique_ptr<CbAtt> vowel1Att,vowel2Att,genderAtt,lfoDivAtt;
    std::unique_ptr<BtAtt> lfoSyncAtt,lfoToMorphAtt,lfoToShiftAtt,lfoToCutoffAtt,cutoffEnableAtt;

    // ---- Animation ----
    std::vector<StarParticle> particles;
    std::vector<FloatStar>    floatStars;
    juce::Random rng;
    float animPhase   = 0.f;
    float eyeGlow     = 0.f;
    bool  lastEnvTrig = false;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PersoFormantAudioProcessorEditor)
};
