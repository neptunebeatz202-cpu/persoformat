#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "BinaryData.h"

//==============================================================================
// Colour palette
const juce::Colour ChobitsLookAndFeel::bgDeep        = juce::Colour(0xff0a0318);
const juce::Colour ChobitsLookAndFeel::bgMid          = juce::Colour(0xff2d1b5e);
const juce::Colour ChobitsLookAndFeel::starGold        = juce::Colour(0xffffd700);
const juce::Colour ChobitsLookAndFeel::pinkAccent      = juce::Colour(0xffff6b9d);
const juce::Colour ChobitsLookAndFeel::silhouetteBlue  = juce::Colour(0xff3a4a7a);
const juce::Colour ChobitsLookAndFeel::hairGold        = juce::Colour(0xffe8c56d);
const juce::Colour ChobitsLookAndFeel::panelFill       = juce::Colour(0xb80d0620);

//==============================================================================
ChobitsLookAndFeel::ChobitsLookAndFeel()
{
    setColour(juce::Slider::rotarySliderFillColourId,    starGold);
    setColour(juce::Slider::rotarySliderOutlineColourId, bgMid);
    setColour(juce::Slider::thumbColourId,               pinkAccent);
    setColour(juce::Slider::textBoxTextColourId,         hairGold);
    setColour(juce::Slider::textBoxBackgroundColourId,   juce::Colours::transparentBlack);
    setColour(juce::Slider::textBoxOutlineColourId,      juce::Colours::transparentBlack);
    setColour(juce::ComboBox::backgroundColourId,        panelFill);
    setColour(juce::ComboBox::outlineColourId,           pinkAccent);
    setColour(juce::ComboBox::textColourId,              starGold);
    setColour(juce::ComboBox::arrowColourId,             starGold);
    setColour(juce::PopupMenu::backgroundColourId,       bgDeep);
    setColour(juce::PopupMenu::textColourId,             starGold);
    setColour(juce::PopupMenu::highlightedBackgroundColourId, bgMid);
    setColour(juce::PopupMenu::highlightedTextColourId,  pinkAccent);
    setColour(juce::TextButton::buttonColourId,          panelFill);
    setColour(juce::TextButton::buttonOnColourId,        pinkAccent.withAlpha(0.8f));
    setColour(juce::TextButton::textColourOffId,         hairGold);
    setColour(juce::TextButton::textColourOnId,          juce::Colours::white);
    setColour(juce::ToggleButton::tickColourId,          pinkAccent);
    setColour(juce::ToggleButton::tickDisabledColourId,  bgMid);
    setColour(juce::Label::textColourId,                 hairGold);
    setColour(juce::Label::backgroundColourId,           juce::Colours::transparentBlack);
}

//==============================================================================
void ChobitsLookAndFeel::drawRotarySlider(juce::Graphics& g,
    int x,int y,int width,int height,float sliderPos,float startAngle,float endAngle,juce::Slider&)
{
    float cx=x+width*0.5f,cy=y+height*0.5f,r=juce::jmin(width,height)*0.38f;
    float angle=startAngle+sliderPos*(endAngle-startAngle);
    g.setColour(panelFill.withAlpha(0.85f));
    g.fillEllipse(cx-r,cy-r,r*2.f,r*2.f);
    g.setColour(bgMid.withAlpha(0.9f));
    g.drawEllipse(cx-r-1,cy-r-1,(r+1)*2.f,(r+1)*2.f,1.5f);
    { juce::Path arc; arc.addArc(cx-r,cy-r,r*2.f,r*2.f,startAngle,angle,true);
      g.setColour(starGold.withAlpha(0.9f));
      g.strokePath(arc,juce::PathStrokeType(3.f,juce::PathStrokeType::curved,juce::PathStrokeType::rounded)); }
    { juce::Path hi; float sp=0.20f;
      hi.addArc(cx-r*0.86f,cy-r*0.86f,r*1.72f,r*1.72f,angle-sp,angle+sp,true);
      g.setColour(pinkAccent.withAlpha(0.5f)); g.strokePath(hi,juce::PathStrokeType(1.6f)); }
    { juce::ColourGradient glow(hairGold.withAlpha(0.22f),cx,cy,bgDeep.withAlpha(0.f),cx+r*0.6f,cy+r*0.6f,true);
      g.setGradientFill(glow); g.fillEllipse(cx-r*0.58f,cy-r*0.58f,r*1.16f,r*1.16f); }
    float tx=cx+r*0.78f*std::sin(angle),ty=cy-r*0.78f*std::cos(angle);
    g.setColour(pinkAccent); g.fillEllipse(tx-4.f,ty-4.f,8.f,8.f);
    g.setColour(juce::Colours::white.withAlpha(0.85f)); g.fillEllipse(tx-1.8f,ty-1.8f,3.6f,3.6f);
}

void ChobitsLookAndFeel::drawButtonBackground(juce::Graphics& g,juce::Button& btn,
    const juce::Colour&,bool hi,bool dn)
{
    auto b=btn.getLocalBounds().toFloat().reduced(1.f);
    g.setColour(dn?pinkAccent.withAlpha(0.85f):hi?bgMid.brighter(0.15f):panelFill);
    g.fillRoundedRectangle(b,5.f);
    g.setColour(btn.getToggleState()?pinkAccent:starGold.withAlpha(0.5f));
    g.drawRoundedRectangle(b,5.f,1.3f);
    if (btn.getToggleState()) {
        juce::ColourGradient glow(pinkAccent.withAlpha(0.30f),b.getCentreX(),b.getCentreY(),
                                   pinkAccent.withAlpha(0.f),b.getRight(),b.getBottom(),true);
        g.setGradientFill(glow); g.fillRoundedRectangle(b,5.f);
    }
}
void ChobitsLookAndFeel::drawButtonText(juce::Graphics& g,juce::TextButton& btn,bool,bool)
{
    g.setColour(btn.getToggleState()?juce::Colours::white:hairGold);
    g.setFont(juce::Font("Georgia",9.5f,juce::Font::bold));
    g.drawFittedText(btn.getButtonText(),btn.getLocalBounds(),juce::Justification::centred,1);
}
void ChobitsLookAndFeel::drawComboBox(juce::Graphics& g,int w,int h,bool,int,int,int,int,juce::ComboBox&)
{
    auto b=juce::Rectangle<float>(0,0,(float)w,(float)h);
    g.setColour(panelFill.withAlpha(0.88f)); g.fillRoundedRectangle(b,5.f);
    g.setColour(pinkAccent.withAlpha(0.7f)); g.drawRoundedRectangle(b.reduced(0.5f),5.f,1.3f);
    juce::Path arrow; float ax=w-13.f,ay=h*0.5f-3.f;
    arrow.addTriangle(ax,ay,ax+7.f,ay,ax+3.5f,ay+5.5f);
    g.setColour(starGold); g.fillPath(arrow);
}
void ChobitsLookAndFeel::drawLabel(juce::Graphics& g,juce::Label& label)
{
    g.fillAll(label.findColour(juce::Label::backgroundColourId));
    if (!label.isBeingEdited()) {
        g.setColour(hairGold.withMultipliedAlpha(label.isEnabled()?1.f:0.5f));
        g.setFont(label.getFont());
        g.drawFittedText(label.getText(),
            getLabelBorderSize(label).subtractedFrom(label.getLocalBounds()),
            label.getJustificationType(),2,label.getMinimumHorizontalScale());
    }
}

//==============================================================================
static void initKnob(juce::Slider& s)
{
    s.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    s.setTextBoxStyle(juce::Slider::TextBoxBelow,false,54,13);
    s.setColour(juce::Slider::textBoxTextColourId,       ChobitsLookAndFeel::hairGold);
    s.setColour(juce::Slider::textBoxBackgroundColourId, juce::Colours::transparentBlack);
    s.setColour(juce::Slider::textBoxOutlineColourId,    juce::Colours::transparentBlack);
}
static void initLabel(juce::Label& l,const juce::String& text)
{
    l.setText(text,juce::dontSendNotification);
    l.setFont(juce::Font("Georgia",9.f,juce::Font::bold));
    l.setJustificationType(juce::Justification::centred);
    l.setColour(juce::Label::textColourId,       ChobitsLookAndFeel::hairGold);
    l.setColour(juce::Label::backgroundColourId, juce::Colours::transparentBlack);
}
static void initRouteBtn(juce::TextButton& b,const juce::String& text)
{
    b.setButtonText(text);
    b.setClickingTogglesState(true);
}

//==============================================================================
PersoFormantAudioProcessorEditor::PersoFormantAudioProcessorEditor(
    PersoFormantAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
    setLookAndFeel(&laf);
    bgImage=juce::ImageCache::getFromMemory(BinaryData::chobits_jpg,BinaryData::chobits_jpgSize);
    setSize(1000,440); setResizable(false,false);

    // ---- Floating background stars ----
    struct StarSeed { float x,y,r; };
    static const StarSeed seeds[]={
        {105,32,9},{155,18,7},{210,42,6},{275,20,8},{335,35,7},
        {395,14,6},{450,28,9},{510,12,7},{575,38,6},{630,22,8},
        {695,30,7},{755,16,9},{815,42,6},{870,25,7},{930,18,8},
        {130,68,5},{260,75,6},{410,58,5},{545,72,6},{680,62,5},{810,70,6},
        {185,95,4},{360,88,5},{520,98,4},{700,90,5},
    };
    for (auto& s : seeds) {
        FloatStar fs;
        fs.x=s.x; fs.y=s.y; fs.r=s.r;
        fs.phase=(rng.nextFloat()*juce::MathConstants<float>::twoPi);
        fs.driftX=(rng.nextFloat()-0.5f)*0.012f;
        fs.driftY=(rng.nextFloat()-0.5f)*0.006f;
        floatStars.push_back(fs);
    }

    // ---- All knobs ----
    for (auto* s : {&morphSlider,&lfoRateSlider,&lfoDepthSlider,
                    &envThreshSlider,&envDepthSlider,
                    &formantResSlider,&formantShiftSlider,
                    &cutoffSlider,
                    &driveSlider,&wetGainSlider,&mixSlider})
        { initKnob(*s); addAndMakeVisible(*s); }

    // ---- All labels ----
    initLabel(morphLabel,       "MORPH");
    initLabel(lfoRateLabel,     "RATE");
    initLabel(lfoDepthLabel,    "DEPTH");
    initLabel(lfoSyncLabel,     "BPM SYNC");
    initLabel(envThreshLabel,   "THRESHOLD");
    initLabel(envDepthLabel,    "ENV\xe2\x86\x92MORPH");
    initLabel(formantResLabel,  "RESONANCE");
    initLabel(formantShiftLabel,"SHIFT");
    initLabel(cutoffLabel,      "CUTOFF");
    initLabel(driveLabel,       "DRIVE");
    initLabel(wetGainLabel,     "WET GAIN");
    initLabel(mixLabel,         "MIX");
    initLabel(vowel1Label,      "VOWEL A");
    initLabel(vowel2Label,      "VOWEL B");
    initLabel(genderLabel,      "GENDER");
    initLabel(lfoRouteLabel,    "LFO ROUTES");

    for (auto* l : {&morphLabel,&lfoRateLabel,&lfoDepthLabel,&lfoSyncLabel,
                    &envThreshLabel,&envDepthLabel,
                    &formantResLabel,&formantShiftLabel,
                    &cutoffLabel,
                    &driveLabel,&wetGainLabel,&mixLabel,
                    &vowel1Label,&vowel2Label,&genderLabel,&lfoRouteLabel})
        addAndMakeVisible(*l);

    // ---- Vowel combo boxes ----
    for (auto* box : {&vowel1Box,&vowel2Box}) {
        for (int i=0;i<12;++i) box->addItem(kVowelNames[i],i+1);
        box->setSelectedId(1); addAndMakeVisible(*box);
    }
    vowel2Box.setSelectedId(3);

    genderBox.addItem(juce::CharPointer_UTF8("\xe2\x99\x82 Masc"),1);
    genderBox.addItem(juce::CharPointer_UTF8("\xe2\x9c\xa6 Neut"),2);
    genderBox.addItem(juce::CharPointer_UTF8("\xe2\x99\x80 Fem"), 3);
    genderBox.setSelectedId(2); addAndMakeVisible(genderBox);

    for (int i=0;i<6;++i) lfoDivBox.addItem(kSyncDivLabels[i],i+1);
    lfoDivBox.setSelectedId(4); addAndMakeVisible(lfoDivBox);

    // ---- LFO sync ----
    lfoSyncButton.setButtonText("BPM");
    lfoSyncButton.setClickingTogglesState(true);
    addAndMakeVisible(lfoSyncButton);

    // ---- LFO routing buttons ----
    initRouteBtn(lfoToMorphBtn,  "\xe2\x86\x92 MORPH");   // → MORPH
    initRouteBtn(lfoToShiftBtn,  "\xe2\x86\x92 SHIFT");   // → SHIFT
    initRouteBtn(lfoToCutoffBtn, "\xe2\x86\x92 CUTOFF");  // → CUTOFF
    addAndMakeVisible(lfoToMorphBtn);
    addAndMakeVisible(lfoToShiftBtn);
    addAndMakeVisible(lfoToCutoffBtn);

    // ---- Cutoff enable button ----
    cutoffEnableBtn.setButtonText("ON");
    cutoffEnableBtn.setClickingTogglesState(true);
    addAndMakeVisible(cutoffEnableBtn);

    // ---- Title ----
    titleLabel .setText("PersoFormant",juce::dontSendNotification);
    vendorLabel.setText("by  Chobits", juce::dontSendNotification);
    titleLabel .setFont(juce::Font("Georgia",22.f,juce::Font::bold|juce::Font::italic));
    vendorLabel.setFont(juce::Font("Georgia",10.f,juce::Font::italic));
    for (auto* l : {&titleLabel,&vendorLabel}) {
        l->setJustificationType(juce::Justification::centred);
        l->setColour(juce::Label::backgroundColourId,juce::Colours::transparentBlack);
        addAndMakeVisible(*l);
    }
    titleLabel .setColour(juce::Label::textColourId,ChobitsLookAndFeel::starGold);
    vendorLabel.setColour(juce::Label::textColourId,ChobitsLookAndFeel::pinkAccent);

    // ---- APVTS Attachments ----
    morphAtt        = std::make_unique<SlAtt>(audioProcessor.apvts,"morph",            morphSlider);
    lfoRateAtt      = std::make_unique<SlAtt>(audioProcessor.apvts,"lfoRate",          lfoRateSlider);
    lfoDepthAtt     = std::make_unique<SlAtt>(audioProcessor.apvts,"lfoDepth",         lfoDepthSlider);
    envThreshAtt    = std::make_unique<SlAtt>(audioProcessor.apvts,"envThresh",        envThreshSlider);
    envDepthAtt     = std::make_unique<SlAtt>(audioProcessor.apvts,"envDepth",         envDepthSlider);
    formantResAtt   = std::make_unique<SlAtt>(audioProcessor.apvts,"formantRes",       formantResSlider);
    formantShiftAtt = std::make_unique<SlAtt>(audioProcessor.apvts,"formantShiftParam",formantShiftSlider);
    cutoffAtt       = std::make_unique<SlAtt>(audioProcessor.apvts,"cutoffOffset",     cutoffSlider);
    driveAtt        = std::make_unique<SlAtt>(audioProcessor.apvts,"drive",            driveSlider);
    wetGainAtt      = std::make_unique<SlAtt>(audioProcessor.apvts,"wetGain",          wetGainSlider);
    mixAtt          = std::make_unique<SlAtt>(audioProcessor.apvts,"mix",              mixSlider);
    vowel1Att       = std::make_unique<CbAtt>(audioProcessor.apvts,"vowel1",           vowel1Box);
    vowel2Att       = std::make_unique<CbAtt>(audioProcessor.apvts,"vowel2",           vowel2Box);
    genderAtt       = std::make_unique<CbAtt>(audioProcessor.apvts,"genderShift",      genderBox);
    lfoDivAtt       = std::make_unique<CbAtt>(audioProcessor.apvts,"lfoDiv",           lfoDivBox);
    lfoSyncAtt      = std::make_unique<BtAtt>(audioProcessor.apvts,"lfoSync",          lfoSyncButton);
    lfoToMorphAtt   = std::make_unique<BtAtt>(audioProcessor.apvts,"lfoToMorph",       lfoToMorphBtn);
    lfoToShiftAtt   = std::make_unique<BtAtt>(audioProcessor.apvts,"lfoToShift",       lfoToShiftBtn);
    lfoToCutoffAtt  = std::make_unique<BtAtt>(audioProcessor.apvts,"lfoToCutoff",      lfoToCutoffBtn);
    cutoffEnableAtt = std::make_unique<BtAtt>(audioProcessor.apvts,"cutoffEnabled",    cutoffEnableBtn);

    // Sync/LFO state
    lfoSyncButton.onClick=[this]{ updateSyncMode(); };
    updateSyncMode();

    // Enable/disable cutoff knob based on toggle
    auto updateCutoffState=[this]{
        cutoffSlider.setEnabled(cutoffEnableBtn.getToggleState());
        repaint();
    };
    cutoffEnableBtn.onClick=updateCutoffState;
    cutoffSlider.setEnabled(cutoffEnableBtn.getToggleState());

    // Star bursts on interaction
    auto burst=[this](juce::Slider& s){ s.onValueChange=[this,&s]{
        spawnStars(4,(float)s.getBounds().getCentreX(),(float)s.getBounds().getCentreY()); }; };
    for (auto* s : {&morphSlider,&lfoRateSlider,&lfoDepthSlider,
                    &envThreshSlider,&envDepthSlider,
                    &formantResSlider,&formantShiftSlider,
                    &cutoffSlider,
                    &driveSlider,&wetGainSlider,&mixSlider})
        burst(*s);
    for (auto* box : {&vowel1Box,&vowel2Box})
        box->onChange=[this,box]{ spawnStars(8,(float)box->getBounds().getCentreX(),(float)box->getBounds().getCentreY()); };

    startTimerHz(60);
}

PersoFormantAudioProcessorEditor::~PersoFormantAudioProcessorEditor()
{ stopTimer(); setLookAndFeel(nullptr); }

//==============================================================================
void PersoFormantAudioProcessorEditor::updateSyncMode()
{
    bool sync=lfoSyncButton.getToggleState();
    lfoRateSlider.setVisible(!sync); lfoRateLabel.setVisible(!sync);
    lfoDivBox    .setVisible( sync);
}

//==============================================================================
void PersoFormantAudioProcessorEditor::timerCallback()
{
    animPhase+=0.018f;
    if (animPhase>juce::MathConstants<float>::twoPi) animPhase-=juce::MathConstants<float>::twoPi;

    for (auto& fs : floatStars) {
        fs.x+=fs.driftX; fs.y+=fs.driftY;
        fs.phase+=0.022f;
        if (fs.phase>juce::MathConstants<float>::twoPi) fs.phase-=juce::MathConstants<float>::twoPi;
        if (fs.x<-20) fs.x=1020.f; if (fs.x>1020) fs.x=-20.f;
        if (fs.y<-20) fs.y=320.f;  if (fs.y>320)  fs.y=-20.f;
    }

    float rawLfo=std::sin(audioProcessor.getLfoPhase())*0.5f+0.5f;
    eyeGlow=eyeGlow*0.85f+rawLfo*0.15f;

    for (auto& p : particles) {
        p.x+=std::cos(p.angle)*p.speed; p.y+=std::sin(p.angle)*p.speed-0.3f;
        p.life-=0.020f; p.opacity=juce::jmax(0.f,p.life);
    }
    particles.erase(std::remove_if(particles.begin(),particles.end(),
        [](const StarParticle& s){ return s.life<=0.f; }),particles.end());

    float env=audioProcessor.getEnvelopeLevel(); bool trig=env>0.05f;
    if (trig&&!lastEnvTrig&&(int)particles.size()<60)
        spawnStars(6,getWidth()*0.5f,getHeight()*0.4f);
    lastEnvTrig=trig;
    repaint();
}

//==============================================================================
void PersoFormantAudioProcessorEditor::spawnStars(int n,float cx,float cy)
{
    for (int i=0;i<n;++i) {
        StarParticle p; p.x=cx; p.y=cy;
        p.size=rng.nextFloat()*7.f+3.f; p.speed=rng.nextFloat()*2.f+0.5f;
        p.angle=rng.nextFloat()*juce::MathConstants<float>::twoPi;
        p.life=1.f; p.opacity=1.f;
        particles.push_back(p);
    }
}

//==============================================================================
void PersoFormantAudioProcessorEditor::drawStarShape(juce::Graphics& g,
    float cx,float cy,float r,float opacity,juce::Colour col,int pts)
{
    juce::Path star; float inner=r*0.42f;
    for (int i=0;i<pts*2;++i) {
        float a=i*juce::MathConstants<float>::pi/pts-juce::MathConstants<float>::halfPi;
        float rad=(i%2==0)?r:inner;
        float px=cx+rad*std::cos(a),py=cy+rad*std::sin(a);
        if (i==0) star.startNewSubPath(px,py); else star.lineTo(px,py);
    }
    star.closeSubPath();
    g.setColour(col.withAlpha(opacity)); g.fillPath(star);
    g.setColour(col.brighter(0.4f).withAlpha(opacity*0.4f));
    g.strokePath(star,juce::PathStrokeType(0.7f));
}

//==============================================================================
void PersoFormantAudioProcessorEditor::drawGlassPanel(juce::Graphics& g,
    juce::Rectangle<float> b,const juce::String& title,juce::Colour accent)
{
    g.setColour(juce::Colour(0xb0050112));
    g.fillRoundedRectangle(b,9.f);
    {
        juce::Rectangle<float> lower(b.getX(),b.getCentreY(),b.getWidth(),b.getHeight()*0.5f);
        juce::ColourGradient sheen(juce::Colours::black.withAlpha(0.35f),b.getX(),b.getBottom(),
                                    juce::Colours::transparentBlack,b.getX(),b.getCentreY(),false);
        g.setGradientFill(sheen); g.fillRoundedRectangle(lower,0.f);
    }
    {
        juce::Rectangle<float> topHalf(b.getX()+2.f,b.getY()+2.f,b.getWidth()-4.f,b.getHeight()*0.42f);
        juce::ColourGradient gloss(juce::Colours::white.withAlpha(0.18f),b.getX(),b.getY(),
                                    juce::Colours::white.withAlpha(0.04f),b.getX(),b.getCentreY(),false);
        g.setGradientFill(gloss); g.fillRoundedRectangle(topHalf,7.f);
    }
    g.setColour(juce::Colours::white.withAlpha(0.22f));
    g.drawLine(b.getX()+9.f,b.getY()+1.f,b.getRight()-9.f,b.getY()+1.f,1.f);
    g.setColour(accent.withAlpha(0.55f));
    g.drawRoundedRectangle(b.reduced(0.5f),9.f,1.5f);
    g.setColour(accent.withAlpha(0.70f));
    g.fillRect(juce::Rectangle<float>(b.getX()+9.f,b.getY(),b.getWidth()-18.f,2.5f));
    g.setFont(juce::Font("Georgia",9.f,juce::Font::bold));
    g.setColour(accent.brighter(0.3f).withAlpha(0.95f));
    g.drawText(title,(int)b.getX(),(int)b.getY()+4,(int)b.getWidth(),14,juce::Justification::centred);
}

//==============================================================================
void PersoFormantAudioProcessorEditor::drawFloatingStars(juce::Graphics& g)
{
    for (auto& fs : floatStars) {
        float pulse=0.55f+0.45f*std::sin(fs.phase);
        float r=fs.r*pulse;
        float alpha=0.50f+0.45f*std::sin(fs.phase+0.8f);
        juce::ColourGradient glow(
            ChobitsLookAndFeel::starGold.withAlpha(alpha*0.35f),fs.x,fs.y,
            ChobitsLookAndFeel::starGold.withAlpha(0.f),fs.x+r*2.8f,fs.y,true);
        g.setGradientFill(glow);
        g.fillEllipse(fs.x-r*2.8f,fs.y-r*2.8f,r*5.6f,r*5.6f);
        drawStarShape(g,fs.x,fs.y,r,alpha,ChobitsLookAndFeel::starGold,5);
        g.setColour(juce::Colours::white.withAlpha(alpha*0.6f));
        g.fillEllipse(fs.x-r*0.22f,fs.y-r*0.22f,r*0.44f,r*0.44f);
    }
}

//==============================================================================
void PersoFormantAudioProcessorEditor::drawChiEyeGlow(juce::Graphics& g)
{
    if (eyeGlow<0.02f) return;
    const float eyes[2][2]={{456.f,198.f},{472.f,197.f}};
    for (auto& eye : eyes) {
        float cx=eye[0],cy=eye[1],brightness=eyeGlow,outerR=9.f+brightness*7.f;
        juce::ColourGradient iris(
            ChobitsLookAndFeel::hairGold.withAlpha(brightness*0.75f),cx,cy,
            ChobitsLookAndFeel::pinkAccent.withAlpha(brightness*0.20f),cx+outerR,cy,true);
        g.setGradientFill(iris); g.fillEllipse(cx-outerR,cy-outerR,outerR*2.f,outerR*2.f);
        juce::ColourGradient spec(
            juce::Colours::white.withAlpha(brightness*0.55f),cx,cy,
            juce::Colours::white.withAlpha(0.f),cx+3.f,cy,true);
        g.setGradientFill(spec); g.fillEllipse(cx-3.f,cy-3.f,6.f,6.f);
    }
}

//==============================================================================
// Evaluate the combined peaking-EQ response (sum of 3 bell filters) at a Hz
// Returns approximate magnitude in dB (linear sum of individual bell gains is an
// approximation but accurate enough for display purposes)
float PersoFormantAudioProcessorEditor::evalFormantResponse(
    float evalHz,
    const float freq[3],const float gain[3],const float q[3]) const
{
    float totalDB=0.f;
    for (int k=0;k<3;++k) {
        if (freq[k]<10.f) continue;
        // Bell peak approximated as Gaussian in log-frequency space
        float logF   = std::log2(evalHz);
        float logPk  = std::log2(freq[k]);
        float bwLog  = logPk / (q[k]*1.2f);       // bandwidth in log2 octaves
        float diff   = (logF-logPk)/(bwLog+0.001f);
        float bellDB = gain[k] * std::exp(-0.5f*diff*diff);
        totalDB     += bellDB;
    }
    return totalDB;
}

//==============================================================================
// THE NEW VISUALISER — full frequency-response curve like Serum Formant-II
void PersoFormantAudioProcessorEditor::drawSpectrumVisualiser(juce::Graphics& g)
{
    // Region: centred between left panels (end at ~215) and right panels (start at ~785)
    // Leave room for title at bottom
    const float VX = 218.f, VY = 10.f, VW = 564.f, VH = 310.f;
    const juce::Rectangle<float> vBounds(VX,VY,VW,VH);

    // ---- Background ----
    // Deep dark background for the scope — like Serum
    {
        juce::ColourGradient bg(
            juce::Colour(0xf0050215),VX,VY,
            juce::Colour(0xf00a0330),VX,VY+VH,false);
        g.setGradientFill(bg);
        g.fillRoundedRectangle(vBounds,8.f);
    }

    // Horizontal grid lines (dB guides: 0, ±10, ±20 dB)
    {
        g.setFont(juce::Font("Georgia",7.5f,juce::Font::plain));
        float midY=VY+VH*0.5f;  // 0 dB line
        float dbRange=28.f;      // ±28 dB display range
        for (float db : {-20.f,-10.f,0.f,10.f,20.f}) {
            float y=midY - (db/dbRange)*(VH*0.48f);
            g.setColour(db==0.f ? juce::Colour(0xff304070).withAlpha(0.9f)
                                : juce::Colour(0xff1a2040).withAlpha(0.7f));
            g.drawHorizontalLine((int)y,(float)VX,(float)(VX+VW));
            g.setColour(ChobitsLookAndFeel::silhouetteBlue.withAlpha(0.55f));
            g.drawText(juce::String((int)db)+"dB",(int)VX+4,(int)(y-6),30,12,
                       juce::Justification::left);
        }
    }

    // Vertical frequency grid (100, 300, 1k, 3k Hz)
    {
        const float fLo=80.f, fHi=4000.f;
        for (float f : {100.f,200.f,500.f,1000.f,2000.f,3000.f}) {
            float t=(std::log(f)-std::log(fLo))/(std::log(fHi)-std::log(fLo));
            float x=VX+t*VW;
            g.setColour(juce::Colour(0xff1a2040).withAlpha(0.7f));
            g.drawVerticalLine((int)x,(float)VY,(float)(VY+VH));
            g.setFont(juce::Font("Georgia",7.5f,juce::Font::plain));
            g.setColour(ChobitsLookAndFeel::silhouetteBlue.withAlpha(0.55f));
            juce::String label=(f>=1000.f)?juce::String((int)(f/1000.f))+"k":juce::String((int)f);
            g.drawText(label,(int)x-12,(int)(VY+VH-14),24,12,juce::Justification::centred);
        }
    }

    // ---- Fetch live formant state from the processor ----
    float freq[3],gain[3],q[3];
    for (int k=0;k<3;++k) {
        freq[k]=audioProcessor.getDisplayFreq(k);
        gain[k]=audioProcessor.getDisplayGain(k);
        q[k]   =audioProcessor.getDisplayQ(k);
        // If display values haven't been set yet, fill from vowel 0
        if (freq[k]<10.f) { freq[k]=kVowelFormants[0].freq[k]; gain[k]=kVowelFormants[0].gain[k]; q[k]=kVowelFormants[0].q[k]; }
    }

    const float fLo=80.f, fHi=4000.f;
    const int   NX  = (int)VW;
    const float midY= VY+VH*0.5f;
    const float dbRange=28.f;

    // ---- Draw individual per-formant filled blobs (dimmer, behind composite) ----
    const juce::Colour blobCols[3]={
        ChobitsLookAndFeel::pinkAccent,
        ChobitsLookAndFeel::starGold,
        ChobitsLookAndFeel::hairGold
    };
    for (int k=0;k<3;++k) {
        juce::Path blob;
        float singleFreq[3]={0,0,0}, singleGain[3]={0,0,0}, singleQ[3]={1,1,1};
        singleFreq[0]=freq[k]; singleGain[0]=gain[k]; singleQ[0]=q[k];

        for (int xi=0;xi<=NX;++xi) {
            float t=(float)xi/NX;
            float f=std::exp(std::log(fLo)+t*(std::log(fHi)-std::log(fLo)));
            float db=evalFormantResponse(f,singleFreq,singleGain,singleQ);
            float y=midY-(db/dbRange)*(VH*0.48f);
            y=juce::jlimit(VY+2.f,VY+VH-2.f,y);
            if (xi==0) blob.startNewSubPath(VX,y); else blob.lineTo(VX+xi,y);
        }
        blob.lineTo(VX+NX,midY); blob.lineTo(VX,midY); blob.closeSubPath();
        g.setColour(blobCols[k].withAlpha(0.10f));
        g.fillPath(blob);
        // Thin stroke outline
        {
            juce::Path line;
            for (int xi=0;xi<=NX;++xi) {
                float t=(float)xi/NX;
                float f=std::exp(std::log(fLo)+t*(std::log(fHi)-std::log(fLo)));
                float db=evalFormantResponse(f,singleFreq,singleGain,singleQ);
                float y=midY-(db/dbRange)*(VH*0.48f);
                y=juce::jlimit(VY+2.f,VY+VH-2.f,y);
                if (xi==0) line.startNewSubPath(VX,y); else line.lineTo(VX+xi,y);
            }
            g.setColour(blobCols[k].withAlpha(0.35f));
            g.strokePath(line,juce::PathStrokeType(1.f));
        }
    }

    // ---- Composite curve (all 3 peaks summed) — the main bright line ----
    juce::Path composite, fillPath;
    std::vector<float> ys(NX+1);
    for (int xi=0;xi<=NX;++xi) {
        float t=(float)xi/NX;
        float f=std::exp(std::log(fLo)+t*(std::log(fHi)-std::log(fLo)));
        float db=evalFormantResponse(f,freq,gain,q);
        float y=midY-(db/dbRange)*(VH*0.48f);
        y=juce::jlimit(VY+2.f,VY+VH-2.f,y);
        ys[xi]=y;
        if (xi==0) { composite.startNewSubPath(VX,y); fillPath.startNewSubPath(VX,y); }
        else        { composite.lineTo(VX+xi,y);      fillPath.lineTo(VX+xi,y); }
    }
    // Filled area under composite curve
    fillPath.lineTo(VX+NX,midY); fillPath.lineTo(VX,midY); fillPath.closeSubPath();

    // Gradient fill — cyan-to-purple like the Serum screenshot
    {
        juce::ColourGradient fillGrad(
            juce::Colour(0xff00d4ff).withAlpha(0.22f), VX, VY,
            juce::Colour(0xff6600cc).withAlpha(0.08f), VX, VY+VH, false);
        g.setGradientFill(fillGrad);
        g.fillPath(fillPath);
    }

    // Bright composite stroke — animated cyan glow
    {
        float glowPulse=0.75f+0.25f*std::sin(animPhase*1.5f);
        // Glow pass (thicker, more transparent)
        g.setColour(juce::Colour(0xff00d4ff).withAlpha(0.30f*glowPulse));
        g.strokePath(composite,juce::PathStrokeType(3.5f,juce::PathStrokeType::curved,juce::PathStrokeType::rounded));
        // Bright core line
        g.setColour(juce::Colour(0xff40e8ff).withAlpha(0.92f));
        g.strokePath(composite,juce::PathStrokeType(1.5f,juce::PathStrokeType::curved,juce::PathStrokeType::rounded));
    }

    // ---- Formant peak marker lines (vertical) ----
    for (int k=0;k<3;++k) {
        float t=(std::log(freq[k])-std::log(fLo))/(std::log(fHi)-std::log(fLo));
        if (t<0.f||t>1.f) continue;
        float x=VX+t*VW;

        // Peak line
        float lfoAlpha=0.5f+0.5f*std::sin(animPhase*2.f+(float)k*2.1f);
        g.setColour(blobCols[k].withAlpha(0.55f+0.25f*lfoAlpha));
        g.drawVerticalLine((int)x,(float)VY+VH*0.08f,(float)(VY+VH-2.f));

        // Peak label: F1/F2/F3 + Hz value
        g.setFont(juce::Font("Georgia",8.f,juce::Font::bold));
        g.setColour(blobCols[k].withAlpha(0.90f));
        juce::String lbl="F"+juce::String(k+1)+"\n"+juce::String((int)freq[k])+"Hz";
        g.drawText(lbl,(int)(x-22.f),(int)(VY+8.f),44,22,juce::Justification::centred);

        // Dot at intersection of peak-line and curve
        float peakY=ys[juce::jlimit(0,NX,(int)(t*NX))];
        g.setColour(blobCols[k]);
        g.fillEllipse(x-4.f,peakY-4.f,8.f,8.f);
        g.setColour(juce::Colours::white.withAlpha(0.9f));
        g.fillEllipse(x-2.f,peakY-2.f,4.f,4.f);
    }

    // ---- Morph scrubber — vertical line showing current morph position ----
    // Interpolate between F1 of vowel1 and F1 of vowel2 to get a representative x
    {
        float morph=audioProcessor.getSmoothedMorph();
        int   v1=juce::jlimit(0,11,(int)audioProcessor.apvts.getRawParameterValue("vowel1")->load());
        int   v2=juce::jlimit(0,11,(int)audioProcessor.apvts.getRawParameterValue("vowel2")->load());
        // Use mid-formant (F2) as the morph indicator frequency
        float fMorph=kVowelFormants[v1].freq[1]+morph*(kVowelFormants[v2].freq[1]-kVowelFormants[v1].freq[1]);
        float t=(std::log(fMorph)-std::log(fLo))/(std::log(fHi)-std::log(fLo));
        float x=juce::jlimit(VX+2.f,VX+VW-2.f,VX+t*VW);

        // Dashed morph scrubber line
        juce::Path dashed;
        for (float yy=VY+2.f;yy<VY+VH-2.f;yy+=8.f)
            dashed.addRectangle(x-0.5f,yy,1.f,5.f);
        g.setColour(ChobitsLookAndFeel::pinkAccent.withAlpha(0.7f));
        g.fillPath(dashed);

        // Morph label at bottom of scrubber
        g.setFont(juce::Font("Georgia",7.5f,juce::Font::bold));
        g.setColour(ChobitsLookAndFeel::pinkAccent.withAlpha(0.85f));
        g.drawText(juce::String((int)(morph*100.f))+"%",
                   (int)(x-14.f),(int)(VY+VH-24.f),28,12,juce::Justification::centred);
    }

    // ---- LFO rotating ring at top-right of visualiser ----
    {
        float lfoPhase=audioProcessor.getLfoPhase();
        float cx=VX+VW-22.f, cy=VY+22.f, dotR=14.f;
        juce::Path ring;
        ring.addArc(cx-dotR,cy-dotR,dotR*2.f,dotR*2.f,
                    lfoPhase-juce::MathConstants<float>::halfPi-1.5f,
                    lfoPhase-juce::MathConstants<float>::halfPi,true);
        g.setColour(ChobitsLookAndFeel::starGold.withAlpha(0.60f));
        g.strokePath(ring,juce::PathStrokeType(2.2f,juce::PathStrokeType::curved,juce::PathStrokeType::rounded));
        float dotX=cx+dotR*std::cos(lfoPhase-juce::MathConstants<float>::halfPi);
        float dotY=cy+dotR*std::sin(lfoPhase-juce::MathConstants<float>::halfPi);
        g.setColour(ChobitsLookAndFeel::pinkAccent); g.fillEllipse(dotX-3.5f,dotY-3.5f,7.f,7.f);
        g.setColour(juce::Colours::white.withAlpha(0.9f)); g.fillEllipse(dotX-1.5f,dotY-1.5f,3.f,3.f);
    }

    // ---- Cutoff offset indicator — vertical band showing offset direction ----
    bool cutoffOn=(bool)(int)audioProcessor.apvts.getRawParameterValue("cutoffEnabled")->load();
    if (cutoffOn) {
        float offset=audioProcessor.apvts.getRawParameterValue("cutoffOffset")->load();
        if (std::abs(offset)>5.f) {
            // Draw a faint tinted region showing the shift direction
            float refF=800.f; // reference "neutral" centre frequency
            float refT=(std::log(refF)-std::log(fLo))/(std::log(fHi)-std::log(fLo));
            float shiftedT=(std::log(refF+offset)-std::log(fLo))/(std::log(fHi)-std::log(fLo));
            float x1=VX+juce::jmin(refT,shiftedT)*VW;
            float x2=VX+juce::jmax(refT,shiftedT)*VW;
            juce::Colour bandCol=(offset>0)?ChobitsLookAndFeel::starGold:ChobitsLookAndFeel::pinkAccent;
            g.setColour(bandCol.withAlpha(0.12f));
            g.fillRect(x1,VY+2.f,x2-x1,VH-4.f);
            // Arrow showing direction
            float arrowX=(offset>0)?x2-2.f:x1+2.f;
            g.setColour(bandCol.withAlpha(0.7f));
            g.setFont(juce::Font("Georgia",11.f,juce::Font::bold));
            g.drawText(offset>0?"\xe2\x86\x92":"\xe2\x86\x90",
                       (int)(arrowX-8.f),(int)(midY-8.f),16,16,juce::Justification::centred);
        }
        // Cutoff ON label
        g.setFont(juce::Font("Georgia",8.f,juce::Font::bold));
        g.setColour(ChobitsLookAndFeel::starGold.withAlpha(0.85f));
        g.drawText("CUTOFF ON",
                   (int)VX+4,(int)(VY+VH-14.f),(int)VW-8,12,juce::Justification::right);
    }

    // ---- Active LFO routing labels ----
    {
        bool toMorph =(bool)(int)audioProcessor.apvts.getRawParameterValue("lfoToMorph") ->load();
        bool toShift =(bool)(int)audioProcessor.apvts.getRawParameterValue("lfoToShift") ->load();
        bool toCutoff=(bool)(int)audioProcessor.apvts.getRawParameterValue("lfoToCutoff")->load();
        g.setFont(juce::Font("Georgia",7.5f,juce::Font::bold));
        int lx2=(int)VX+4;
        if (toMorph)  { g.setColour(ChobitsLookAndFeel::pinkAccent.withAlpha(0.85f));
                        g.drawText("LFO\xe2\x86\x92MORPH", lx2,(int)VY+6,90,11,juce::Justification::left); lx2+=94; }
        if (toShift)  { g.setColour(ChobitsLookAndFeel::starGold.withAlpha(0.85f));
                        g.drawText("LFO\xe2\x86\x92SHIFT", lx2,(int)VY+6,90,11,juce::Justification::left); lx2+=94; }
        if (toCutoff) { g.setColour(ChobitsLookAndFeel::hairGold.withAlpha(0.85f));
                        g.drawText("LFO\xe2\x86\x92CUTOFF",lx2,(int)VY+6,90,11,juce::Justification::left); }
    }

    // ---- Vowel labels at sides ----
    {
        int v1=juce::jlimit(0,11,(int)audioProcessor.apvts.getRawParameterValue("vowel1")->load());
        int v2=juce::jlimit(0,11,(int)audioProcessor.apvts.getRawParameterValue("vowel2")->load());
        g.setFont(juce::Font("Georgia",10.f,juce::Font::bold|juce::Font::italic));
        g.setColour(ChobitsLookAndFeel::starGold.withAlpha(0.80f));
        g.drawText(juce::String(kVowelNames[v1]),(int)VX+4,(int)(VY+VH/2.f-8.f),20,16,
                   juce::Justification::left);
        g.setColour(ChobitsLookAndFeel::pinkAccent.withAlpha(0.80f));
        g.drawText(juce::String(kVowelNames[v2]),(int)(VX+VW-24.f),(int)(VY+VH/2.f-8.f),20,16,
                   juce::Justification::right);
    }

    // ---- Env level bar at bottom of visualiser ----
    {
        float env=audioProcessor.getEnvelopeLevel();
        float barW=VW*juce::jlimit(0.f,1.f,env*6.f);
        g.setColour(ChobitsLookAndFeel::pinkAccent.withAlpha(0.15f));
        g.fillRect(VX,VY+VH-4.f,VW,4.f);
        if (barW>1.f) {
            juce::ColourGradient bar(
                ChobitsLookAndFeel::pinkAccent.withAlpha(0.75f),VX,VY+VH,
                ChobitsLookAndFeel::starGold.withAlpha(0.75f),VX+VW,VY+VH,false);
            g.setGradientFill(bar);
            g.fillRect(VX,VY+VH-4.f,barW,4.f);
        }
    }

    // ---- Outer border glow ----
    {
        float glowA=0.3f+0.2f*std::sin(animPhase);
        g.setColour(ChobitsLookAndFeel::pinkAccent.withAlpha(glowA));
        g.drawRoundedRectangle(vBounds.reduced(0.5f),8.f,1.5f);
    }
}

//==============================================================================
void PersoFormantAudioProcessorEditor::drawEnvelopeMeter(juce::Graphics& g)
{
    float env=audioProcessor.getEnvelopeLevel();
    float mx=978.f,my=192.f,mW=8.f,mH=56.f;
    g.setColour(ChobitsLookAndFeel::bgDeep.withAlpha(0.7f));
    g.fillRoundedRectangle(mx,my,mW,mH,3.f);
    g.setColour(ChobitsLookAndFeel::pinkAccent.withAlpha(0.35f));
    g.drawRoundedRectangle(mx,my,mW,mH,3.f,1.f);
    float fillH=juce::jlimit(0.f,mH,env*mH*8.f);
    if (fillH>0.5f) {
        juce::ColourGradient fill(ChobitsLookAndFeel::pinkAccent,mx,my+mH,
                                   ChobitsLookAndFeel::starGold,mx,my,false);
        g.setGradientFill(fill); g.fillRoundedRectangle(mx,my+mH-fillH,mW,fillH,3.f);
    }
}

//==============================================================================
void PersoFormantAudioProcessorEditor::paint(juce::Graphics& g)
{
    int W=getWidth(),H=getHeight();

    if (bgImage.isValid()) {
        float imgW=(float)bgImage.getWidth(),imgH=(float)bgImage.getHeight();
        float scale=juce::jmax(W/imgW,H/imgH);
        int dW=juce::roundToInt(imgW*scale),dH=juce::roundToInt(imgH*scale);
        int offX=(W-dW)/2,offY=(H-dH)/3;
        g.drawImage(bgImage,offX,offY,dW,dH,0,0,bgImage.getWidth(),bgImage.getHeight());
        juce::ColourGradient vL(juce::Colours::black.withAlpha(0.75f),0.f,H*0.5f,
                                 juce::Colours::transparentBlack,240.f,H*0.5f,false);
        g.setGradientFill(vL); g.fillRect(0,0,W,H);
        juce::ColourGradient vR(juce::Colours::black.withAlpha(0.75f),(float)W,H*0.5f,
                                 juce::Colours::transparentBlack,760.f,H*0.5f,false);
        g.setGradientFill(vR); g.fillRect(0,0,W,H);
        g.setColour(juce::Colours::black.withAlpha(0.20f)); g.fillRect(0,0,W,H);
    } else {
        juce::ColourGradient sky(ChobitsLookAndFeel::bgDeep,0,0,
                                  ChobitsLookAndFeel::bgMid,(float)W,(float)H,false);
        g.setGradientFill(sky); g.fillRect(0,0,W,H);
    }

    drawFloatingStars(g);
    drawChiEyeGlow(g);

    // ---- Glass panels ----
    drawGlassPanel(g,{6.f,6.f,205.f,295.f},   "VOWEL  MORPH",   ChobitsLookAndFeel::pinkAccent);
    // LFO panel taller to fit routing buttons
    drawGlassPanel(g,{6.f,308.f,205.f,126.f},  "LFO",            ChobitsLookAndFeel::starGold);
    // Formant filter panel — now also holds Cutoff
    drawGlassPanel(g,{789.f,6.f,205.f,200.f},  "FORMANT  FILTER",ChobitsLookAndFeel::hairGold);
    drawGlassPanel(g,{789.f,212.f,205.f,90.f}, "ENV  FOLLOWER",  ChobitsLookAndFeel::pinkAccent);
    drawGlassPanel(g,{789.f,308.f,205.f,126.f},"OUTPUT",         ChobitsLookAndFeel::starGold);

    // Burst particles
    for (auto& p : particles) {
        juce::Colour col=ChobitsLookAndFeel::starGold.interpolatedWith(ChobitsLookAndFeel::pinkAccent,1.f-p.life);
        drawStarShape(g,p.x,p.y,p.size,p.opacity,col,5);
    }

    // ---- NEW spectrum visualiser ----
    drawSpectrumVisualiser(g);

    drawEnvelopeMeter(g);

    { juce::ColourGradient glow(ChobitsLookAndFeel::pinkAccent.withAlpha(0.14f),W*0.5f,(float)(H-55),
                                 juce::Colours::transparentBlack,W*0.5f,(float)(H-10),false);
      g.setGradientFill(glow); g.fillRect(320,H-70,360,55); }
}

//==============================================================================
void PersoFormantAudioProcessorEditor::resized()
{
    // ============================================================
    // LEFT — VOWEL MORPH  (x=6,y=6,w=205,h=295)
    // ============================================================
    int lx=11;
    vowel1Label.setBounds(lx,    22,90,13);
    vowel1Box  .setBounds(lx,    36,90,22);
    vowel2Label.setBounds(lx+107,22,90,13);
    vowel2Box  .setBounds(lx+107,36,90,22);
    genderLabel.setBounds(lx+62, 66,75,13);
    genderBox  .setBounds(lx+58, 80,82,22);
    morphLabel .setBounds(lx+62,112,75,13);
    morphSlider.setBounds(lx+46,124,108,108);

    // ============================================================
    // LEFT — LFO  (x=6,y=308,w=205,h=126)
    // ============================================================
    int ly=313;
    lfoRateLabel .setBounds(lx,      ly+2,74,13);
    lfoRateSlider.setBounds(lx-2,    ly+14,76,72);
    lfoDivBox    .setBounds(lx,      ly+28,74,26);

    lfoDepthLabel .setBounds(lx+82,  ly+2,74,13);
    lfoDepthSlider.setBounds(lx+80,  ly+14,76,72);

    // Sync toggle — small, below the knobs
    lfoSyncLabel .setBounds(lx+162, ly+2,38,13);
    lfoSyncButton.setBounds(lx+162, ly+16,40,18);

    // LFO routing — 3 small buttons across the bottom of LFO panel
    // Each 60px wide, 18px tall, with small gap
    lfoRouteLabel   .setBounds(lx,   ly+92, 200,11);
    lfoToMorphBtn   .setBounds(lx,   ly+104, 60,18);
    lfoToShiftBtn   .setBounds(lx+66,ly+104, 60,18);
    lfoToCutoffBtn  .setBounds(lx+132,ly+104,68,18);

    // ============================================================
    // RIGHT — FORMANT FILTER  (x=789,y=6,w=205,h=200)
    // Now holds: Resonance + Shift (top row), Cutoff knob + ON btn (bottom row)
    // ============================================================
    int rx=793;

    formantResLabel  .setBounds(rx,      22,94,13);
    formantResSlider .setBounds(rx-2,    34,98,94);

    formantShiftLabel.setBounds(rx+108,  22,94,13);
    formantShiftSlider.setBounds(rx+106, 34,98,94);

    // Cutoff row — centred below the two knobs
    cutoffLabel     .setBounds(rx+30,   136,88,13);
    cutoffSlider    .setBounds(rx+18,   148,98,42);
    cutoffEnableBtn .setBounds(rx+122,  154,60,22);

    // ============================================================
    // RIGHT — ENV FOLLOWER  (x=789,y=212,w=205,h=90)
    // ============================================================
    int ey=217;
    envThreshLabel .setBounds(rx,      ey+4,94,13);
    envThreshSlider.setBounds(rx-2,    ey+16,98,66);
    envDepthLabel .setBounds(rx+108,   ey+4,94,13);
    envDepthSlider.setBounds(rx+106,   ey+16,98,66);

    // ============================================================
    // RIGHT — OUTPUT  (x=789,y=308,w=205,h=126)
    // ============================================================
    int oy=313;
    driveLabel   .setBounds(rx,      oy+12,60,13);
    driveSlider  .setBounds(rx-2,    oy+24,64,76);
    wetGainLabel .setBounds(rx+70,   oy+12,62,13);
    wetGainSlider.setBounds(rx+68,   oy+24,64,76);
    mixLabel     .setBounds(rx+140,  oy+12,60,13);
    mixSlider    .setBounds(rx+138,  oy+24,64,76);

    // ============================================================
    // Title
    // ============================================================
    titleLabel .setBounds(370,getHeight()-70,260,30);
    vendorLabel.setBounds(370,getHeight()-42,260,16);
}
