#include "PluginProcessor.h"
#include "PluginEditor.h"

#include "PluginConfig.h"
#include "parameters/ParameterIDs.h"

//==============================================================================
AudioPluginAudioProcessorEditor::AudioPluginAudioProcessorEditor (AudioPluginAudioProcessor& p)
    : AudioProcessorEditor (&p), processorRef (p)
{
    setLookAndFeel (&lookAndFeel);

    titleLabel.setText ("Base Plugin Template", juce::dontSendNotification);
    titleLabel.setJustificationType (juce::Justification::centred);
    titleLabel.setFont (juce::FontOptions { 20.0f, juce::Font::bold });
    addAndMakeVisible (titleLabel);

    bypassButton.setClickingTogglesState (true);
    addAndMakeVisible (bypassButton);

    bypassAttachment = std::make_unique<ButtonAttachment> (
        processorRef.parameters, ParameterIDs::bypass, bypassButton);

    setSize (PluginConfig::defaultEditorWidth, PluginConfig::defaultEditorHeight);
}


AudioPluginAudioProcessorEditor::~AudioPluginAudioProcessorEditor()
{
    setLookAndFeel (nullptr);
}

//==============================================================================
void AudioPluginAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (findColour (juce::ResizableWindow::backgroundColourId));
}

void AudioPluginAudioProcessorEditor::resized()
{
    auto area = getLocalBounds().reduced (PluginConfig::editorMargin);

    titleLabel.setBounds (area.removeFromTop (40));
    bypassButton.setBounds (area.removeFromBottom (26).withSizeKeepingCentre (76, 24));
}
