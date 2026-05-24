#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

class CassetteDisplay final : public juce::Component
{
public:
    void setStatus (bool dbxEnabled,
                    bool bypassed,
                    const juce::String& speed,
                    const juce::String& oversampling);

    void paint (juce::Graphics& g) override;

private:
    bool dbx = true;
    bool bypass = false;
    juce::String tapeSpeed = "Normal";
    juce::String oversamplingMode = "4x";
};
