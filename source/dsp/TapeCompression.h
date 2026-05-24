#pragma once

#include <vector>

#include "DspUtilities.h"

class TapeCompression
{
public:
    void prepare (double sampleRate, int numChannels);
    void reset();

    float processSample (int channel, float input, float amount, float drive) noexcept;

private:
    struct ChannelState
    {
        float envelope = 0.0f;
        float previousInput = 0.0f;
    };

    double currentSampleRate = 44100.0;
    float attackCoefficient = 0.0f;
    float releaseCoefficient = 0.0f;
    std::vector<ChannelState> states;
};
