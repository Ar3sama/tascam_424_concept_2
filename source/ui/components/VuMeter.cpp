#include "VuMeter.h"

void VuMeter::setLevels (float inputLevel, float outputLevel)
{
    const auto newInput = juce::jlimit (0.0f, 2.0f, inputLevel);
    const auto newOutput = juce::jlimit (0.0f, 2.0f, outputLevel);

    if (std::abs (newInput - input) > 0.001f || std::abs (newOutput - output) > 0.001f)
    {
        input = newInput;
        output = newOutput;
        repaint();
    }
}

void VuMeter::paint (juce::Graphics& g)
{
    auto area = getLocalBounds().reduced (8);
    drawBar (g, area.removeFromTop (area.getHeight() / 2).reduced (0, 2), input, "IN");
    drawBar (g, area.reduced (0, 2), output, "OUT");
}

float VuMeter::levelToProportion (float level) noexcept
{
    const auto levelDb = juce::Decibels::gainToDecibels (juce::jmax (level, 0.000001f), -90.0f);
    return juce::jmap (juce::jlimit (-54.0f, 6.0f, levelDb), -54.0f, 6.0f, 0.0f, 1.0f);
}

void VuMeter::drawBar (juce::Graphics& g,
                       juce::Rectangle<int> bounds,
                       float level,
                       const juce::String& label) const
{
    const auto labelArea = bounds.removeFromLeft (34);
    g.setColour (juce::Colour::fromRGB (197, 190, 166));
    g.setFont (juce::FontOptions { 11.0f, juce::Font::bold });
    g.drawFittedText (label, labelArea, juce::Justification::centredLeft, 1);

    g.setColour (juce::Colour::fromRGB (23, 25, 25));
    g.fillRoundedRectangle (bounds.toFloat(), 4.0f);

    auto fill = bounds.reduced (2);
    fill.setWidth (juce::roundToInt (fill.getWidth() * levelToProportion (level)));

    juce::ColourGradient gradient { juce::Colour::fromRGB (82, 174, 141),
                                    static_cast<float> (bounds.getX()),
                                    0.0f,
                                    juce::Colour::fromRGB (234, 184, 74),
                                    static_cast<float> (bounds.getRight()),
                                    0.0f,
                                    false };
    gradient.addColour (0.82, juce::Colour::fromRGB (218, 71, 64));

    g.setGradientFill (gradient);
    g.fillRoundedRectangle (fill.toFloat(), 3.0f);

    g.setColour (juce::Colour::fromRGB (68, 72, 70));
    g.drawRoundedRectangle (bounds.toFloat(), 4.0f, 1.0f);
}
