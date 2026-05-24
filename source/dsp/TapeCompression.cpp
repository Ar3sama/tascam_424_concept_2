#include "TapeCompression.h"

void TapeCompression::prepare (double sampleRate, int numChannels)
{
    currentSampleRate = sampleRate;
    attackCoefficient = std::exp (-1.0f / static_cast<float> (currentSampleRate * 0.004));
    releaseCoefficient = std::exp (-1.0f / static_cast<float> (currentSampleRate * 0.180));
    states.resize (static_cast<size_t> (juce::jmax (1, numChannels)));
    reset();
}

void TapeCompression::reset()
{
    for (auto& state : states)
    {
        state.envelope = 0.0f;
        state.previousInput = 0.0f;
    }
}

float TapeCompression::processSample (int channel, float input, float amount, float drive) noexcept
{
    const auto compressionAmount = PortastudioDsp::clamp01 (amount);

    if (compressionAmount <= 0.0001f || states.empty())
        return input;

    auto& state = states[static_cast<size_t> (juce::jlimit (0, static_cast<int> (states.size()) - 1, channel))];

    const auto squared = input * input;
    const auto coefficient = squared > state.envelope ? attackCoefficient : releaseCoefficient;
    state.envelope = coefficient * state.envelope + (1.0f - coefficient) * squared;

    const auto level = std::sqrt (state.envelope + PortastudioDsp::minimumLevel);
    const auto over = juce::jmax (0.0f, level - 0.22f);
    const auto magneticPush = 1.0f + PortastudioDsp::clamp01 (drive) * 1.1f;
    const auto gain = 1.0f / (1.0f + over * (1.6f + magneticPush) * compressionAmount);

    const auto transient = input - state.previousInput;
    state.previousInput = input;

    return input * gain - transient * (0.018f * compressionAmount * juce::jlimit (0.0f, 1.0f, over * 5.0f));
}
