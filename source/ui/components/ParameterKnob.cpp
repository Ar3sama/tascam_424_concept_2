#include "ParameterKnob.h"

ParameterKnob::ParameterKnob (const juce::String& name)
{
    nameLabel.setText (name, juce::dontSendNotification);
    nameLabel.setJustificationType (juce::Justification::centred);
    nameLabel.setInterceptsMouseClicks (false, false);
    addAndMakeVisible (nameLabel);

    slider.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    slider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 64, 18);
    slider.setNumDecimalPlacesToDisplay (1);
    slider.setPopupDisplayEnabled (true, true, this);
    addAndMakeVisible (slider);
}

juce::Slider& ParameterKnob::getSlider() noexcept
{
    return slider;
}

const juce::Slider& ParameterKnob::getSlider() const noexcept
{
    return slider;
}

void ParameterKnob::resized()
{
    auto area = getLocalBounds();
    nameLabel.setBounds (area.removeFromTop (18));
    slider.setBounds (area);
}
