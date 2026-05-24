#include "DbxProcessor.h"

void DbxProcessor::prepare (double sampleRate, int numChannels)
{
    currentSampleRate = sampleRate;
    attackCoefficient = std::exp (-1.0f / static_cast<float> (currentSampleRate * 0.006));
    releaseCoefficient = std::exp (-1.0f / static_cast<float> (currentSampleRate * 0.120));

    states.resize (static_cast<size_t> (juce::jmax (1, numChannels)));

    for (auto& state : states)
        state.emphasisLowpass.prepare (currentSampleRate, 2600.0f);

    reset();
}

void DbxProcessor::reset()
{
    for (auto& state : states)
    {
        state.envelope = 0.0f;
        state.emphasisLowpass.reset();
    }
}

float DbxProcessor::processEncoderSample (int channel, float input, float amount) noexcept
{
    const auto dbxAmount = PortastudioDsp::clamp01 (amount);

    if (dbxAmount <= 0.0001f || states.empty())
        return input;

    auto& state = states[static_cast<size_t> (juce::jlimit (0, static_cast<int> (states.size()) - 1, channel))];
    const auto high = input - state.emphasisLowpass.process (input);
    const auto emphasised = input + high * (0.14f * dbxAmount);
    const auto envelope = updateEnvelope (state, emphasised);
    const auto levelDb = PortastudioDsp::gainToDb (std::sqrt (envelope));

    constexpr auto referenceDb = -18.0f;
    const auto gainDb = juce::jlimit (-10.0f, 12.0f, (referenceDb - levelDb) * 0.5f * dbxAmount);

    return emphasised * PortastudioDsp::dbToGain (gainDb);
}

float DbxProcessor::processDecoderSample (int channel, float input, float amount) noexcept
{
    const auto dbxAmount = PortastudioDsp::clamp01 (amount);

    if (dbxAmount <= 0.0001f || states.empty())
        return input;

    auto& state = states[static_cast<size_t> (juce::jlimit (0, static_cast<int> (states.size()) - 1, channel))];
    const auto envelope = updateEnvelope (state, input);
    const auto levelDb = PortastudioDsp::gainToDb (std::sqrt (envelope));

    constexpr auto referenceDb = -18.0f;
    const auto gainDb = juce::jlimit (-12.0f, 10.0f, (levelDb - referenceDb) * 0.5f * dbxAmount);
    const auto expanded = input * PortastudioDsp::dbToGain (gainDb);
    const auto high = expanded - state.emphasisLowpass.process (expanded);

    return expanded - high * (0.12f * dbxAmount);
}

float DbxProcessor::updateEnvelope (ChannelState& state, float input) const noexcept
{
    const auto squared = input * input;
    const auto coefficient = squared > state.envelope ? attackCoefficient : releaseCoefficient;

    state.envelope = coefficient * state.envelope + (1.0f - coefficient) * squared;
    return state.envelope + PortastudioDsp::minimumLevel;
}
