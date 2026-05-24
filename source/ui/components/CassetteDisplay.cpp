#include "CassetteDisplay.h"

void CassetteDisplay::setStatus (bool dbxEnabled,
                                 bool bypassed,
                                 const juce::String& speed,
                                 const juce::String& oversampling)
{
    if (dbx == dbxEnabled
        && bypass == bypassed
        && tapeSpeed == speed
        && oversamplingMode == oversampling)
        return;

    dbx = dbxEnabled;
    bypass = bypassed;
    tapeSpeed = speed;
    oversamplingMode = oversampling;
    repaint();
}

void CassetteDisplay::paint (juce::Graphics& g)
{
    auto area = getLocalBounds().toFloat().reduced (6.0f);

    g.setColour (juce::Colour::fromRGB (19, 20, 20));
    g.fillRoundedRectangle (area, 7.0f);
    g.setColour (juce::Colour::fromRGB (73, 76, 72));
    g.drawRoundedRectangle (area, 7.0f, 1.0f);

    auto window = area.reduced (18.0f, 13.0f);
    g.setColour (juce::Colour::fromRGB (32, 34, 33));
    g.fillRoundedRectangle (window, 5.0f);

    const auto reelRadius = juce::jmin (window.getHeight() * 0.32f, window.getWidth() * 0.13f);
    const auto leftCentre = window.getCentre() - juce::Point<float> (window.getWidth() * 0.24f, 0.0f);
    const auto rightCentre = window.getCentre() + juce::Point<float> (window.getWidth() * 0.24f, 0.0f);

    g.setColour (juce::Colour::fromRGB (156, 151, 132));
    g.drawEllipse (leftCentre.x - reelRadius, leftCentre.y - reelRadius, reelRadius * 2.0f, reelRadius * 2.0f, 2.0f);
    g.drawEllipse (rightCentre.x - reelRadius, rightCentre.y - reelRadius, reelRadius * 2.0f, reelRadius * 2.0f, 2.0f);

    g.setColour (juce::Colour::fromRGB (94, 83, 64));
    g.drawLine (leftCentre.x + reelRadius, leftCentre.y, rightCentre.x - reelRadius, rightCentre.y, 2.0f);

    const auto drawLed = [&g] (juce::Rectangle<float> ledBounds, bool isOn, juce::Colour onColour)
    {
        g.setColour (isOn ? onColour : juce::Colour::fromRGB (48, 45, 42));
        g.fillEllipse (ledBounds);
        g.setColour (juce::Colours::black.withAlpha (0.45f));
        g.drawEllipse (ledBounds, 1.0f);
    };

    auto bottom = area.removeFromBottom (18.0f).reduced (16.0f, 0.0f);
    drawLed (bottom.removeFromLeft (10.0f).reduced (1.0f), dbx, juce::Colour::fromRGB (81, 191, 142));

    g.setColour (juce::Colour::fromRGB (197, 190, 166));
    g.setFont (juce::FontOptions { 10.5f, juce::Font::bold });
    g.drawText ("dbx", bottom.removeFromLeft (32.0f), juce::Justification::centredLeft);

    drawLed (bottom.removeFromLeft (10.0f).reduced (1.0f), bypass, juce::Colour::fromRGB (222, 80, 66));
    g.drawText ("BYP", bottom.removeFromLeft (36.0f), juce::Justification::centredLeft);
    g.drawText (tapeSpeed + " / " + oversamplingMode, bottom, juce::Justification::centredRight);
}
