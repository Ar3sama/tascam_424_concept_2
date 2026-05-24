#include "DynamicBandwidthProcessor.h"

void DynamicBandwidthProcessor::prepare (double sampleRate, int numChannels)
{
    currentSampleRate = sampleRate;
    levelCoefficient = std::exp (-1.0f / static_cast<float> (currentSampleRate * 0.060));
    states.resize (static_cast<size_t> (juce::jmax (1, numChannels)));
    reset();
}

void DynamicBandwidthProcessor::reset()
{
    const auto defaultCoefficient = PortastudioDsp::onePoleCoefficient (16000.0f, currentSampleRate);

    for (auto& state : states)
    {
        state.levelEnvelope = 0.0f;
        state.filterState = 0.0f;
        state.coefficient = defaultCoefficient;
        state.coefficientCounter = 0;
    }
}

float DynamicBandwidthProcessor::processSample (int channel,
                                                float input,
                                                float bandwidth,
                                                float tapeAmount,
                                                float drive,
                                                int tapeSpeedIndex) noexcept
{
    if (states.empty())
        return input;

    auto& state = states[static_cast<size_t> (juce::jlimit (0, static_cast<int> (states.size()) - 1, channel))];

    const auto squared = input * input;
    state.levelEnvelope = levelCoefficient * state.levelEnvelope + (1.0f - levelCoefficient) * squared;
    const auto level = juce::jlimit (0.0f, 1.0f, std::sqrt (state.levelEnvelope + PortastudioDsp::minimumLevel) * 2.0f);

    if (--state.coefficientCounter <= 0)
    {
        const auto width = 0.45f + PortastudioDsp::clamp01 (bandwidth) * 0.55f;
        const auto saturationLoss = 1.0f - level * (0.32f * PortastudioDsp::clamp01 (tapeAmount)
                                                    + 0.18f * PortastudioDsp::clamp01 (drive));
        const auto cutoff = getBaseCutoffForSpeed (tapeSpeedIndex) * width * juce::jlimit (0.45f, 1.0f, saturationLoss);
        const auto targetCoefficient = PortastudioDsp::onePoleCoefficient (cutoff, currentSampleRate);

        state.coefficient += (targetCoefficient - state.coefficient) * 0.18f;
        state.coefficientCounter = 16;
    }

    state.filterState += state.coefficient * (input - state.filterState);
    return state.filterState;
}

float DynamicBandwidthProcessor::getBaseCutoffForSpeed (int tapeSpeedIndex) noexcept
{
    if (tapeSpeedIndex <= 0)
        return 9500.0f;

    if (tapeSpeedIndex >= 2)
        return 17500.0f;

    return 13200.0f;
}
