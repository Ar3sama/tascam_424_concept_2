#pragma once

#include "PluginProcessor.h"
#include "ui/components/CassetteDisplay.h"
#include "ui/components/ParameterKnob.h"
#include "ui/components/VuMeter.h"
#include "ui/lookandfeel/PluginLookAndFeel.h"

#include <initializer_list>
#include <memory>
#include <vector>

//==============================================================================
class AudioPluginAudioProcessorEditor final : public juce::AudioProcessorEditor,
                                              private juce::Timer
{
public:
    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    using ButtonAttachment = juce::AudioProcessorValueTreeState::ButtonAttachment;
    using ComboBoxAttachment = juce::AudioProcessorValueTreeState::ComboBoxAttachment;

    explicit AudioPluginAudioProcessorEditor (AudioPluginAudioProcessor&);
    ~AudioPluginAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    void timerCallback() override;

    void addKnob (ParameterKnob& knob, const char* parameterID, double doubleClickValue);
    void layoutKnobGrid (juce::Rectangle<int> bounds,
                         std::initializer_list<ParameterKnob*> knobs,
                         int columns);
    void updateCassetteStatus();

    AudioPluginAudioProcessor& processorRef;

    PluginLookAndFeel lookAndFeel;
    juce::Label titleLabel;
    juce::Label subtitleLabel;

    ParameterKnob inputGainKnob { "Input" };
    ParameterKnob outputGainKnob { "Output" };
    ParameterKnob driveKnob { "Drive" };
    ParameterKnob tapeAmountKnob { "Tape" };
    ParameterKnob saturationKnob { "Sat" };
    ParameterKnob compressionKnob { "Comp" };
    ParameterKnob mixKnob { "Mix" };

    ParameterKnob wowKnob { "Wow" };
    ParameterKnob flutterKnob { "Flutter" };
    ParameterKnob bandwidthKnob { "Width" };
    ParameterKnob hysteresisKnob { "Memory" };
    ParameterKnob biasKnob { "Bias" };

    ParameterKnob noiseAmountKnob { "Noise" };
    ParameterKnob crosstalkKnob { "Bleed" };
    ParameterKnob dropoutsKnob { "Dropouts" };

    ParameterKnob dbxAmountKnob { "dbx Amt" };

    juce::ComboBox tapeSpeedBox;
    juce::ComboBox oversamplingBox;
    juce::Label tapeSpeedLabel;
    juce::Label oversamplingLabel;
    juce::ToggleButton dbxEnableButton { "dbx" };
    juce::ToggleButton bypassButton { "Bypass" };
    VuMeter vuMeter;
    CassetteDisplay cassetteDisplay;

    juce::Rectangle<int> coreSectionBounds;
    juce::Rectangle<int> tapeSectionBounds;
    juce::Rectangle<int> noiseSectionBounds;
    juce::Rectangle<int> utilitySectionBounds;

    std::vector<std::unique_ptr<SliderAttachment>> sliderAttachments;
    std::vector<std::unique_ptr<ButtonAttachment>> buttonAttachments;
    std::vector<std::unique_ptr<ComboBoxAttachment>> comboBoxAttachments;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioPluginAudioProcessorEditor)
};
