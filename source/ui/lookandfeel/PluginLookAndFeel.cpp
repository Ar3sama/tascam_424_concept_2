#include "PluginLookAndFeel.h"

PluginLookAndFeel::PluginLookAndFeel()
{
    setColour (juce::ResizableWindow::backgroundColourId, juce::Colour::fromRGB (28, 29, 29));
    setColour (juce::Label::textColourId, juce::Colour::fromRGB (218, 211, 188));
    setColour (juce::Slider::rotarySliderFillColourId, juce::Colour::fromRGB (228, 178, 76));
    setColour (juce::Slider::rotarySliderOutlineColourId, juce::Colour::fromRGB (64, 67, 65));
    setColour (juce::Slider::thumbColourId, juce::Colour::fromRGB (234, 228, 206));
    setColour (juce::Slider::textBoxTextColourId, juce::Colour::fromRGB (225, 218, 196));
    setColour (juce::Slider::textBoxBackgroundColourId, juce::Colour::fromRGB (25, 26, 26));
    setColour (juce::Slider::textBoxOutlineColourId, juce::Colour::fromRGB (59, 62, 60));
    setColour (juce::TextButton::buttonColourId, juce::Colour::fromRGB (52, 55, 54));
    setColour (juce::TextButton::buttonOnColourId, juce::Colour::fromRGB (220, 180, 70));
    setColour (juce::TextButton::textColourOffId, juce::Colour::fromRGB (224, 217, 195));
    setColour (juce::TextButton::textColourOnId, juce::Colours::black);
    setColour (juce::ComboBox::backgroundColourId, juce::Colour::fromRGB (25, 26, 26));
    setColour (juce::ComboBox::outlineColourId, juce::Colour::fromRGB (77, 81, 78));
    setColour (juce::ComboBox::textColourId, juce::Colour::fromRGB (224, 217, 195));
    setColour (juce::PopupMenu::backgroundColourId, juce::Colour::fromRGB (30, 31, 31));
    setColour (juce::PopupMenu::textColourId, juce::Colour::fromRGB (224, 217, 195));
}

void PluginLookAndFeel::drawRotarySlider (juce::Graphics& g,
                                          int x,
                                          int y,
                                          int width,
                                          int height,
                                          float sliderPosProportional,
                                          float rotaryStartAngle,
                                          float rotaryEndAngle,
                                          juce::Slider& slider)
{
    const auto bounds = juce::Rectangle<float> (static_cast<float> (x),
                                               static_cast<float> (y),
                                               static_cast<float> (width),
                                               static_cast<float> (height)).reduced (7.0f);
    const auto radius = juce::jmin (bounds.getWidth(), bounds.getHeight()) * 0.44f;
    const auto centre = bounds.getCentre();
    const auto angle = rotaryStartAngle + sliderPosProportional * (rotaryEndAngle - rotaryStartAngle);

    juce::Path backgroundArc;
    backgroundArc.addCentredArc (centre.x,
                                 centre.y,
                                 radius,
                                 radius,
                                 0.0f,
                                 rotaryStartAngle,
                                 rotaryEndAngle,
                                 true);

    g.setColour (slider.findColour (juce::Slider::rotarySliderOutlineColourId));
    g.strokePath (backgroundArc, juce::PathStrokeType { 4.0f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded });

    juce::Path valueArc;
    valueArc.addCentredArc (centre.x,
                            centre.y,
                            radius,
                            radius,
                            0.0f,
                            rotaryStartAngle,
                            angle,
                            true);

    g.setColour (slider.findColour (juce::Slider::rotarySliderFillColourId));
    g.strokePath (valueArc, juce::PathStrokeType { 4.0f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded });

    const auto knobRadius = radius * 0.72f;
    auto knobBounds = juce::Rectangle<float> { knobRadius * 2.0f, knobRadius * 2.0f }.withCentre (centre);

    g.setColour (juce::Colour::fromRGB (39, 41, 40));
    g.fillEllipse (knobBounds);
    g.setColour (juce::Colour::fromRGB (88, 91, 87));
    g.drawEllipse (knobBounds, 1.0f);

    juce::Path pointer;
    pointer.addRoundedRectangle (-1.3f, -knobRadius + 5.0f, 2.6f, knobRadius * 0.52f, 1.3f);
    pointer.applyTransform (juce::AffineTransform::rotation (angle).translated (centre.x, centre.y));

    g.setColour (slider.findColour (juce::Slider::thumbColourId));
    g.fillPath (pointer);
}

void PluginLookAndFeel::drawButtonBackground (juce::Graphics& g,
                                              juce::Button& button,
                                              const juce::Colour& backgroundColour,
                                              bool shouldDrawButtonAsHighlighted,
                                              bool shouldDrawButtonAsDown)
{
    auto bounds = button.getLocalBounds().toFloat().reduced (1.0f);
    auto colour = button.getToggleState() ? findColour (juce::TextButton::buttonOnColourId) : backgroundColour;

    if (shouldDrawButtonAsHighlighted)
        colour = colour.brighter (0.08f);

    if (shouldDrawButtonAsDown)
        colour = colour.darker (0.12f);

    g.setColour (colour);
    g.fillRoundedRectangle (bounds, 6.0f);
    g.setColour (juce::Colour::fromRGB (76, 79, 76));
    g.drawRoundedRectangle (bounds, 6.0f, 1.0f);
}

void PluginLookAndFeel::drawToggleButton (juce::Graphics& g,
                                          juce::ToggleButton& button,
                                          bool shouldDrawButtonAsHighlighted,
                                          bool shouldDrawButtonAsDown)
{
    juce::ignoreUnused (shouldDrawButtonAsDown);

    auto area = button.getLocalBounds().reduced (2);
    const auto ledBounds = area.removeFromLeft (18).withSizeKeepingCentre (10, 10).toFloat();
    const auto isOn = button.getToggleState();

    g.setColour (isOn ? juce::Colour::fromRGB (224, 178, 73) : juce::Colour::fromRGB (54, 51, 47));
    g.fillEllipse (ledBounds);
    g.setColour (juce::Colours::black.withAlpha (0.45f));
    g.drawEllipse (ledBounds, 1.0f);

    g.setColour (shouldDrawButtonAsHighlighted ? juce::Colour::fromRGB (240, 232, 207)
                                               : juce::Colour::fromRGB (211, 204, 183));
    g.setFont (juce::FontOptions { 12.0f, juce::Font::bold });
    g.drawFittedText (button.getButtonText(), area, juce::Justification::centredLeft, 1);
}
