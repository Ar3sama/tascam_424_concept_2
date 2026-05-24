#include "PluginProcessor.h"
#include "PluginEditor.h"

#include "PluginConfig.h"
#include "parameters/ParameterIDs.h"

namespace
{
    void drawSection (juce::Graphics& g, juce::Rectangle<int> bounds, const juce::String& title)
    {
        const auto section = bounds.toFloat();
        g.setColour (juce::Colour::fromRGB (22, 23, 23));
        g.fillRoundedRectangle (section, 7.0f);
        g.setColour (juce::Colour::fromRGB (66, 69, 66));
        g.drawRoundedRectangle (section, 7.0f, 1.0f);

        auto titleBounds = bounds.reduced (12, 0).removeFromTop (24);
        g.setColour (juce::Colour::fromRGB (221, 176, 79));
        g.setFont (juce::FontOptions { 12.0f, juce::Font::bold });
        g.drawText (title, titleBounds, juce::Justification::centredLeft);
    }

    float readParameterValue (juce::AudioProcessorValueTreeState& parameters, const char* parameterID)
    {
        if (auto* parameter = parameters.getRawParameterValue (parameterID))
            return parameter->load();

        return 0.0f;
    }
}

//==============================================================================
AudioPluginAudioProcessorEditor::AudioPluginAudioProcessorEditor (AudioPluginAudioProcessor& p)
    : AudioProcessorEditor (&p), processorRef (p)
{
    setLookAndFeel (&lookAndFeel);
    setResizable (true, true);
    setResizeLimits (760, 500, 1200, 780);

    titleLabel.setText ("424 Concept", juce::dontSendNotification);
    titleLabel.setJustificationType (juce::Justification::centredLeft);
    titleLabel.setFont (juce::FontOptions { 26.0f, juce::Font::bold });
    addAndMakeVisible (titleLabel);

    subtitleLabel.setText ("Portastudio workflow emulator", juce::dontSendNotification);
    subtitleLabel.setJustificationType (juce::Justification::centredLeft);
    subtitleLabel.setFont (juce::FontOptions { 12.0f });
    addAndMakeVisible (subtitleLabel);

    addKnob (inputGainKnob, ParameterIDs::inputGain, 0.0);
    addKnob (outputGainKnob, ParameterIDs::outputGain, 0.0);
    addKnob (driveKnob, ParameterIDs::drive, 35.0);
    addKnob (tapeAmountKnob, ParameterIDs::tapeAmount, 65.0);
    addKnob (saturationKnob, ParameterIDs::saturation, 45.0);
    addKnob (compressionKnob, ParameterIDs::compression, 35.0);
    addKnob (mixKnob, ParameterIDs::mix, 100.0);

    addKnob (wowKnob, ParameterIDs::wow, 18.0);
    addKnob (flutterKnob, ParameterIDs::flutter, 12.0);
    addKnob (bandwidthKnob, ParameterIDs::bandwidth, 65.0);
    addKnob (hysteresisKnob, ParameterIDs::hysteresis, 35.0);
    addKnob (biasKnob, ParameterIDs::bias, 50.0);

    addKnob (noiseAmountKnob, ParameterIDs::noiseAmount, 12.0);
    addKnob (crosstalkKnob, ParameterIDs::crosstalk, 18.0);
    addKnob (dropoutsKnob, ParameterIDs::dropouts, 5.0);
    addKnob (dbxAmountKnob, ParameterIDs::dbxAmount, 55.0);

    tapeSpeedLabel.setText ("Speed", juce::dontSendNotification);
    tapeSpeedLabel.setJustificationType (juce::Justification::centredLeft);
    addAndMakeVisible (tapeSpeedLabel);

    tapeSpeedBox.addItem ("Low", 1);
    tapeSpeedBox.addItem ("Normal", 2);
    tapeSpeedBox.addItem ("High", 3);
    addAndMakeVisible (tapeSpeedBox);
    comboBoxAttachments.push_back (std::make_unique<ComboBoxAttachment> (
        processorRef.parameters, ParameterIDs::tapeSpeed, tapeSpeedBox));

    oversamplingLabel.setText ("OS", juce::dontSendNotification);
    oversamplingLabel.setJustificationType (juce::Justification::centredLeft);
    addAndMakeVisible (oversamplingLabel);

    oversamplingBox.addItem ("4x", 1);
    oversamplingBox.addItem ("8x", 2);
    addAndMakeVisible (oversamplingBox);
    comboBoxAttachments.push_back (std::make_unique<ComboBoxAttachment> (
        processorRef.parameters, ParameterIDs::oversampling, oversamplingBox));

    dbxEnableButton.setClickingTogglesState (true);
    addAndMakeVisible (dbxEnableButton);
    buttonAttachments.push_back (std::make_unique<ButtonAttachment> (
        processorRef.parameters, ParameterIDs::dbxEnabled, dbxEnableButton));

    bypassButton.setClickingTogglesState (true);
    addAndMakeVisible (bypassButton);
    buttonAttachments.push_back (std::make_unique<ButtonAttachment> (
        processorRef.parameters, ParameterIDs::bypass, bypassButton));

    addAndMakeVisible (vuMeter);
    addAndMakeVisible (cassetteDisplay);

    setSize (PluginConfig::defaultEditorWidth, PluginConfig::defaultEditorHeight);
    startTimerHz (30);
}

AudioPluginAudioProcessorEditor::~AudioPluginAudioProcessorEditor()
{
    setLookAndFeel (nullptr);
}

//==============================================================================
void AudioPluginAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (findColour (juce::ResizableWindow::backgroundColourId));

    auto header = getLocalBounds().reduced (PluginConfig::editorMargin).removeFromTop (88);
    g.setColour (juce::Colour::fromRGB (36, 38, 37));
    g.fillRoundedRectangle (header.toFloat(), 7.0f);
    g.setColour (juce::Colour::fromRGB (70, 73, 70));
    g.drawRoundedRectangle (header.toFloat(), 7.0f, 1.0f);

    drawSection (g, coreSectionBounds, "CORE");
    drawSection (g, tapeSectionBounds, "TAPE");
    drawSection (g, noiseSectionBounds, "NOISE");
    drawSection (g, utilitySectionBounds, "UTILITY");
}

void AudioPluginAudioProcessorEditor::resized()
{
    auto area = getLocalBounds().reduced (PluginConfig::editorMargin);

    auto header = area.removeFromTop (88).reduced (16, 10);
    auto titleArea = header.removeFromLeft (230);
    titleLabel.setBounds (titleArea.removeFromTop (36));
    subtitleLabel.setBounds (titleArea);

    vuMeter.setBounds (header.removeFromRight (230).reduced (4, 4));
    cassetteDisplay.setBounds (header.reduced (18, 0));

    area.removeFromTop (10);

    auto bottomRow = area.removeFromBottom (142);
    area.removeFromBottom (10);

    coreSectionBounds = area.removeFromLeft (juce::roundToInt (area.getWidth() * 0.52f));
    area.removeFromLeft (10);
    tapeSectionBounds = area;

    noiseSectionBounds = bottomRow.removeFromLeft (juce::roundToInt (bottomRow.getWidth() * 0.55f));
    bottomRow.removeFromLeft (10);
    utilitySectionBounds = bottomRow;

    layoutKnobGrid (coreSectionBounds.reduced (12).withTrimmedTop (28),
                    { &inputGainKnob,
                      &driveKnob,
                      &tapeAmountKnob,
                      &saturationKnob,
                      &compressionKnob,
                      &mixKnob,
                      &outputGainKnob },
                    4);

    auto tapeContent = tapeSectionBounds.reduced (12).withTrimmedTop (28);
    auto tapeSpeedArea = tapeContent.removeFromBottom (34);
    tapeSpeedLabel.setBounds (tapeSpeedArea.removeFromLeft (58));
    tapeSpeedBox.setBounds (tapeSpeedArea.removeFromLeft (126).reduced (0, 3));
    layoutKnobGrid (tapeContent,
                    { &wowKnob,
                      &flutterKnob,
                      &bandwidthKnob,
                      &hysteresisKnob,
                      &biasKnob },
                    3);

    layoutKnobGrid (noiseSectionBounds.reduced (12).withTrimmedTop (28),
                    { &noiseAmountKnob,
                      &crosstalkKnob,
                      &dropoutsKnob },
                    3);

    auto utilityContent = utilitySectionBounds.reduced (12).withTrimmedTop (28);
    dbxAmountKnob.setBounds (utilityContent.removeFromLeft (96));
    utilityContent.removeFromLeft (10);
    dbxEnableButton.setBounds (utilityContent.removeFromTop (28));
    bypassButton.setBounds (utilityContent.removeFromTop (28));
    utilityContent.removeFromTop (8);

    auto oversamplingArea = utilityContent.removeFromTop (30);
    oversamplingLabel.setBounds (oversamplingArea.removeFromLeft (38));
    oversamplingBox.setBounds (oversamplingArea.reduced (0, 2));
}

void AudioPluginAudioProcessorEditor::timerCallback()
{
    vuMeter.setLevels (processorRef.getInputMeterLevel(), processorRef.getOutputMeterLevel());
    updateCassetteStatus();
}

void AudioPluginAudioProcessorEditor::addKnob (ParameterKnob& knob,
                                               const char* parameterID,
                                               double doubleClickValue)
{
    knob.getSlider().setDoubleClickReturnValue (true, doubleClickValue);
    addAndMakeVisible (knob);
    sliderAttachments.push_back (std::make_unique<SliderAttachment> (
        processorRef.parameters, parameterID, knob.getSlider()));
}

void AudioPluginAudioProcessorEditor::layoutKnobGrid (juce::Rectangle<int> bounds,
                                                      std::initializer_list<ParameterKnob*> knobs,
                                                      int columns)
{
    const auto count = static_cast<int> (knobs.size());

    if (count <= 0 || columns <= 0)
        return;

    const auto rows = (count + columns - 1) / columns;
    const auto cellWidth = bounds.getWidth() / columns;
    const auto cellHeight = bounds.getHeight() / rows;
    int index = 0;

    for (auto* knob : knobs)
    {
        const auto column = index % columns;
        const auto row = index / columns;
        knob->setBounds (bounds.getX() + column * cellWidth,
                         bounds.getY() + row * cellHeight,
                         cellWidth,
                         cellHeight);
        knob->setBounds (knob->getBounds().reduced (4));
        ++index;
    }
}

void AudioPluginAudioProcessorEditor::updateCassetteStatus()
{
    const auto dbxEnabled = readParameterValue (processorRef.parameters, ParameterIDs::dbxEnabled) > 0.5f;
    const auto bypassed = readParameterValue (processorRef.parameters, ParameterIDs::bypass) > 0.5f;
    const auto speedIndex = static_cast<int> (readParameterValue (processorRef.parameters, ParameterIDs::tapeSpeed) + 0.5f);
    const auto oversamplingIndex = static_cast<int> (readParameterValue (processorRef.parameters, ParameterIDs::oversampling) + 0.5f);

    const juce::String speed = speedIndex <= 0 ? "Low" : (speedIndex >= 2 ? "High" : "Normal");
    const juce::String oversampling = oversamplingIndex > 0 ? "8x" : "4x";

    cassetteDisplay.setStatus (dbxEnabled, bypassed, speed, oversampling);
}
