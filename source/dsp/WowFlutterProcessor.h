#pragma once

#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_dsp/juce_dsp.h>

#include <vector>

#include "DspUtilities.h"

class WowFlutterProcessor
{
public:
    void prepare (double sampleRate, int maximumBlockSize, int numChannels);
    void reset();

    void processBlock (juce::dsp::AudioBlock<float>& block,
                       float wow,
                       float flutter,
                       int tapeSpeedIndex) noexcept;

private:
    struct ChannelState
    {
        int writePosition = 0;
        int updateCounter = 0;
        float currentDelaySamples = 1.0f;
        float targetDelaySamples = 1.0f;
        float phaseA = 0.0f;
        float phaseB = 0.0f;
        float phaseC = 0.0f;
        float phaseD = 0.0f;
        float randomDrift = 0.0f;
        float randomFlutter = 0.0f;
        PortastudioDsp::FastRandom random;
    };

    static float getSpeedScale (int tapeSpeedIndex) noexcept;

    void writeDryOnly (juce::dsp::AudioBlock<float>& block, int numChannels, int numSamples) noexcept;
    float getNextDelaySamples (ChannelState& state,
                               float wow,
                               float flutter,
                               float speedScale) noexcept;
    float readInterpolatedSample (int channel, float delaySamples, int writePosition) const noexcept;

    double currentSampleRate = 44100.0;
    int delayBufferSize = 0;
    int maximumDelaySamples = 0;
    juce::AudioBuffer<float> delayBuffer;
    std::vector<ChannelState> states;
};
