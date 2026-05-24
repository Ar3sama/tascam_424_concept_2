#pragma once

#include <juce_dsp/juce_dsp.h>

#include <vector>

#include "DspUtilities.h"

class NoiseProcessor
{
public:
    void prepare (double sampleRate, int numChannels);
    void reset();

    void processBlock (juce::dsp::AudioBlock<float>& block,
                       float noiseAmount,
                       float dropouts,
                       int tapeSpeedIndex) noexcept;

private:
    struct ChannelState
    {
        PortastudioDsp::FastRandom random;
        float lowNoise = 0.0f;
        float slowModulation = 0.0f;
        float dropoutGain = 1.0f;
        float dropoutTarget = 1.0f;
        int dropoutSamplesRemaining = 0;
    };

    static float getNoiseTrimForSpeed (int tapeSpeedIndex) noexcept;
    float processDropout (ChannelState& state, float dropouts) const noexcept;

    double currentSampleRate = 44100.0;
    float dropoutSmoothingCoefficient = 0.0f;
    std::vector<ChannelState> states;
};
