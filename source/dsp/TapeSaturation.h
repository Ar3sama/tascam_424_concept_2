#pragma once

#include <vector>

#include "DspUtilities.h"

class TapeSaturation
{
public:
    void prepare (double sampleRate, int numChannels);
    void reset();

    float processSample (int channel,
                         float input,
                         float drive,
                         float tapeAmount,
                         float saturation,
                         float bias) noexcept;

private:
    struct ChannelState
    {
        PortastudioDsp::OnePoleLowpass firstStageTone;
        PortastudioDsp::OnePoleLowpass secondStageTone;
    };

    std::vector<ChannelState> states;
};
