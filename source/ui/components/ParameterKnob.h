#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

class ParameterKnob final : public juce::Component
{
public:
    explicit ParameterKnob (const juce::String& name);

    juce::Slider& getSlider() noexcept;
    const juce::Slider& getSlider() const noexcept;

    void resized() override;

private:
    juce::Label nameLabel;
    juce::Slider slider;
};
