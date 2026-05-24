#pragma once

#include <juce_dsp/juce_dsp.h>

#include "DspUtilities.h"

class CrosstalkProcessor
{
public:
    void prepare (double sampleRate);
    void reset();

    void processBlock (juce::dsp::AudioBlock<float>& block, float amount) noexcept;

private:
    PortastudioDsp::OnePoleLowpass leftIntoRightLowpass;
    PortastudioDsp::OnePoleLowpass rightIntoLeftLowpass;
};
