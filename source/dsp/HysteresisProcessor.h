#pragma once

#include <vector>

#include "DspUtilities.h"

class HysteresisProcessor
{
public:
    void prepare (int numChannels);
    void reset();

    float processSample (int channel, float input, float amount, float bias) noexcept;

private:
    struct ChannelState
    {
        float memory = 0.0f;
        float previousInput = 0.0f;
    };

    std::vector<ChannelState> states;
};
