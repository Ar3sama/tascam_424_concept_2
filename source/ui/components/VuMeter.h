#pragma once

#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_gui_basics/juce_gui_basics.h>

class VuMeter final : public juce::Component
{
public:
    void setLevels (float inputLevel, float outputLevel);
    void paint (juce::Graphics& g) override;

private:
    static float levelToProportion (float level) noexcept;
    void drawBar (juce::Graphics& g,
                  juce::Rectangle<int> bounds,
                  float level,
                  const juce::String& label) const;

    float input = 0.0f;
    float output = 0.0f;
};
