#include "NoiseProcessor.h"

void NoiseProcessor::prepare (double sampleRate, int numChannels)
{
    currentSampleRate = sampleRate;
    dropoutSmoothingCoefficient = 1.0f - std::exp (-1.0f / static_cast<float> (currentSampleRate * 0.0025));
    states.resize (static_cast<size_t> (juce::jmax (1, numChannels)));

    for (size_t index = 0; index < states.size(); ++index)
        states[index].random.setSeed (0x515100u + static_cast<std::uint32_t> (index * 3571u));

    reset();
}

void NoiseProcessor::reset()
{
    for (auto& state : states)
    {
        state.lowNoise = 0.0f;
        state.slowModulation = 0.0f;
        state.dropoutGain = 1.0f;
        state.dropoutTarget = 1.0f;
        state.dropoutSamplesRemaining = 0;
    }
}

void NoiseProcessor::processBlock (juce::dsp::AudioBlock<float>& block,
                                   float noiseAmount,
                                   float dropouts,
                                   int tapeSpeedIndex) noexcept
{
    if (states.empty())
        return;

    const auto numChannels = static_cast<int> (juce::jmin (block.getNumChannels(), states.size()));
    const auto numSamples = static_cast<int> (block.getNumSamples());
    const auto noise = PortastudioDsp::clamp01 (noiseAmount);
    const auto dropoutAmount = PortastudioDsp::clamp01 (dropouts);
    const auto noiseGain = PortastudioDsp::dbToGain (-76.0f + noise * 26.0f) * noise;
    const auto noiseTrim = getNoiseTrimForSpeed (tapeSpeedIndex);

    for (int channel = 0; channel < numChannels; ++channel)
    {
        auto& state = states[static_cast<size_t> (channel)];
        auto* data = block.getChannelPointer (static_cast<size_t> (channel));

        for (int sample = 0; sample < numSamples; ++sample)
        {
            auto output = data[sample] * processDropout (state, dropoutAmount);

            if (noise > 0.0001f)
            {
                const auto random = state.random.nextBipolar();
                state.lowNoise += (random - state.lowNoise) * 0.025f;
                state.slowModulation += (state.random.nextBipolar() - state.slowModulation) * 0.00008f;

                const auto colouredNoise = state.lowNoise * 0.72f + random * 0.28f;
                output += colouredNoise * noiseGain * noiseTrim * (1.0f + state.slowModulation * 0.08f);
            }

            data[sample] = output;
        }
    }
}

float NoiseProcessor::getNoiseTrimForSpeed (int tapeSpeedIndex) noexcept
{
    if (tapeSpeedIndex <= 0)
        return 1.35f;

    if (tapeSpeedIndex >= 2)
        return 0.82f;

    return 1.0f;
}

float NoiseProcessor::processDropout (ChannelState& state, float dropouts) const noexcept
{
    if (dropouts <= 0.0001f)
    {
        state.dropoutTarget = 1.0f;
        state.dropoutGain += (state.dropoutTarget - state.dropoutGain) * dropoutSmoothingCoefficient;
        return state.dropoutGain;
    }

    if (state.dropoutSamplesRemaining > 0)
    {
        --state.dropoutSamplesRemaining;
    }
    else
    {
        state.dropoutTarget = 1.0f;

        const auto chance = dropouts / static_cast<float> (currentSampleRate * 2.8);

        if (state.random.nextFloat() < chance)
        {
            const auto lengthSeconds = 0.004f + state.random.nextFloat() * 0.018f;
            state.dropoutSamplesRemaining = static_cast<int> (lengthSeconds * static_cast<float> (currentSampleRate));
            state.dropoutTarget = 1.0f - dropouts * (0.08f + state.random.nextFloat() * 0.24f);
        }
    }

    state.dropoutGain += (state.dropoutTarget - state.dropoutGain) * dropoutSmoothingCoefficient;
    return state.dropoutGain;
}
