#pragma once

#include <vector>

#include "DspUtilities.h"

class DbxProcessor
{
public:
    void prepare (double sampleRate, int numChannels);
    void reset();

    float processEncoderSample (int channel, float input, float amount) noexcept;
    float processDecoderSample (int channel, float input, float amount) noexcept;

private:
    struct ChannelState
    {
        float envelope = 0.0f;
        PortastudioDsp::OnePoleLowpass emphasisLowpass;
    };

    float updateEnvelope (ChannelState& state, float input) const noexcept;

    double currentSampleRate = 44100.0;
    float attackCoefficient = 0.0f;
    float releaseCoefficient = 0.0f;
    std::vector<ChannelState> states;
};
