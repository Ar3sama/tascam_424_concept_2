#pragma once

#include "PluginProcessor.h"
#include "ui/lookandfeel/PluginLookAndFeel.h"

//==============================================================================
class AudioPluginAudioProcessorEditor final : public juce::AudioProcessorEditor
{
public:
    using ButtonAttachment = juce::AudioProcessorValueTreeState::ButtonAttachment;

    explicit AudioPluginAudioProcessorEditor (AudioPluginAudioProcessor&);
    ~AudioPluginAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.


    AudioPluginAudioProcessor& processorRef;

    PluginLookAndFeel lookAndFeel;
    juce::Label titleLabel;
    juce::TextButton bypassButton { "Bypass" };

    std::unique_ptr<ButtonAttachment> bypassAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioPluginAudioProcessorEditor)
};
