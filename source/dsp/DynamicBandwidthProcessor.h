#pragma once

#include <vector>

#include "DspUtilities.h"

class DynamicBandwidthProcessor
{
public:
    void prepare (double sampleRate, int numChannels);
    void reset();

    float processSample (int channel,
                         float input,
                         float bandwidth,
                         float tapeAmount,
                         float drive,
                         int tapeSpeedIndex) noexcept;

private:
    struct ChannelState
    {
        float levelEnvelope = 0.0f;
        float filterState = 0.0f;
        float coefficient = 0.1f;
        int coefficientCounter = 0;
    };

    static float getBaseCutoffForSpeed (int tapeSpeedIndex) noexcept;

    double currentSampleRate = 44100.0;
    float levelCoefficient = 0.0f;
    std::vector<ChannelState> states;
};
