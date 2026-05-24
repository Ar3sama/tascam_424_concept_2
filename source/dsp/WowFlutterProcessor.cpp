#include "WowFlutterProcessor.h"

void WowFlutterProcessor::prepare (double sampleRate, int maximumBlockSize, int numChannels)
{
    currentSampleRate = sampleRate;
    maximumDelaySamples = static_cast<int> (std::ceil (currentSampleRate * 0.050));
    delayBufferSize = maximumDelaySamples + juce::jmax (64, maximumBlockSize) + 8;

    delayBuffer.setSize (juce::jmax (1, numChannels), delayBufferSize, false, false, true);
    states.resize (static_cast<size_t> (juce::jmax (1, numChannels)));

    for (size_t index = 0; index < states.size(); ++index)
        states[index].random.setSeed (0x424200u + static_cast<std::uint32_t> (index * 7919u));

    reset();
}

void WowFlutterProcessor::reset()
{
    delayBuffer.clear();

    const auto baseDelaySamples = static_cast<float> (currentSampleRate * 0.0035);

    for (size_t index = 0; index < states.size(); ++index)
    {
        auto& state = states[index];
        state.writePosition = 0;
        state.updateCounter = 0;
        state.currentDelaySamples = baseDelaySamples;
        state.targetDelaySamples = baseDelaySamples;
        state.phaseA = 0.11f + static_cast<float> (index) * 0.17f;
        state.phaseB = 0.37f + static_cast<float> (index) * 0.13f;
        state.phaseC = 0.23f + static_cast<float> (index) * 0.19f;
        state.phaseD = 0.61f + static_cast<float> (index) * 0.07f;
        state.randomDrift = 0.0f;
        state.randomFlutter = 0.0f;
    }
}

void WowFlutterProcessor::processBlock (juce::dsp::AudioBlock<float>& block,
                                        float wow,
                                        float flutter,
                                        int tapeSpeedIndex) noexcept
{
    if (delayBufferSize <= 8 || states.empty())
        return;

    const auto numChannels = static_cast<int> (juce::jmin (block.getNumChannels(), states.size()));
    const auto numSamples = static_cast<int> (block.getNumSamples());
    const auto wowAmount = PortastudioDsp::clamp01 (wow);
    const auto flutterAmount = PortastudioDsp::clamp01 (flutter);

    if (wowAmount <= 0.0001f && flutterAmount <= 0.0001f)
    {
        writeDryOnly (block, numChannels, numSamples);
        return;
    }

    const auto speedScale = getSpeedScale (tapeSpeedIndex);

    for (int sample = 0; sample < numSamples; ++sample)
    {
        for (int channel = 0; channel < numChannels; ++channel)
        {
            auto& state = states[static_cast<size_t> (channel)];
            auto* channelData = block.getChannelPointer (static_cast<size_t> (channel));
            const auto input = channelData[sample];
            const auto delaySamples = getNextDelaySamples (state, wowAmount, flutterAmount, speedScale);
            const auto delayed = readInterpolatedSample (channel, delaySamples, state.writePosition);

            delayBuffer.setSample (channel, state.writePosition, input);
            channelData[sample] = delayed;

            if (++state.writePosition >= delayBufferSize)
                state.writePosition = 0;
        }
    }
}

float WowFlutterProcessor::getSpeedScale (int tapeSpeedIndex) noexcept
{
    if (tapeSpeedIndex <= 0)
        return 0.72f;

    if (tapeSpeedIndex >= 2)
        return 1.28f;

    return 1.0f;
}

void WowFlutterProcessor::writeDryOnly (juce::dsp::AudioBlock<float>& block,
                                        int numChannels,
                                        int numSamples) noexcept
{
    for (int sample = 0; sample < numSamples; ++sample)
    {
        for (int channel = 0; channel < numChannels; ++channel)
        {
            auto& state = states[static_cast<size_t> (channel)];
            const auto* channelData = block.getChannelPointer (static_cast<size_t> (channel));
            delayBuffer.setSample (channel, state.writePosition, channelData[sample]);

            if (++state.writePosition >= delayBufferSize)
                state.writePosition = 0;
        }
    }
}

float WowFlutterProcessor::getNextDelaySamples (ChannelState& state,
                                                float wow,
                                                float flutter,
                                                float speedScale) noexcept
{
    constexpr auto updateInterval = 16.0f;

    if (--state.updateCounter <= 0)
    {
        const auto phaseAdvance = updateInterval / static_cast<float> (currentSampleRate);

        state.phaseA += 0.32f * speedScale * phaseAdvance;
        state.phaseB += 0.47f * speedScale * phaseAdvance;
        state.phaseC += 5.70f * speedScale * phaseAdvance;
        state.phaseD += 8.90f * speedScale * phaseAdvance;

        state.phaseA -= std::floor (state.phaseA);
        state.phaseB -= std::floor (state.phaseB);
        state.phaseC -= std::floor (state.phaseC);
        state.phaseD -= std::floor (state.phaseD);

        state.randomDrift += (state.random.nextBipolar() - state.randomDrift) * 0.018f;
        state.randomFlutter += (state.random.nextBipolar() - state.randomFlutter) * 0.10f;

        const auto slowDrift = 0.52f * std::sin (juce::MathConstants<float>::twoPi * state.phaseA)
                             + 0.31f * std::sin (juce::MathConstants<float>::twoPi * state.phaseB)
                             + 0.17f * state.randomDrift;

        const auto fastMotion = 0.48f * std::sin (juce::MathConstants<float>::twoPi * state.phaseC)
                              + 0.34f * std::sin (juce::MathConstants<float>::twoPi * state.phaseD)
                              + 0.18f * state.randomFlutter;

        const auto baseDelaySamples = static_cast<float> (currentSampleRate * 0.0035);
        const auto wowDepthSamples = wow * static_cast<float> (currentSampleRate * 0.00022);
        const auto flutterDepthSamples = flutter * static_cast<float> (currentSampleRate * 0.000035);

        state.targetDelaySamples = baseDelaySamples
                                 + slowDrift * wowDepthSamples
                                 + fastMotion * flutterDepthSamples;
        state.targetDelaySamples = juce::jlimit (2.0f,
                                                 static_cast<float> (maximumDelaySamples - 4),
                                                 state.targetDelaySamples);
        state.updateCounter = static_cast<int> (updateInterval);
    }

    state.currentDelaySamples += (state.targetDelaySamples - state.currentDelaySamples) * 0.08f;
    return state.currentDelaySamples;
}

float WowFlutterProcessor::readInterpolatedSample (int channel,
                                                   float delaySamples,
                                                   int writePosition) const noexcept
{
    auto readPosition = static_cast<float> (writePosition) - delaySamples;

    while (readPosition < 0.0f)
        readPosition += static_cast<float> (delayBufferSize);

    while (readPosition >= static_cast<float> (delayBufferSize))
        readPosition -= static_cast<float> (delayBufferSize);

    const auto index1 = static_cast<int> (readPosition);
    const auto fraction = readPosition - static_cast<float> (index1);
    const auto* data = delayBuffer.getReadPointer (channel);

    const auto wrap = [this] (int index) noexcept
    {
        while (index < 0)
            index += delayBufferSize;

        while (index >= delayBufferSize)
            index -= delayBufferSize;

        return index;
    };

    const auto y0 = data[wrap (index1 - 1)];
    const auto y1 = data[wrap (index1)];
    const auto y2 = data[wrap (index1 + 1)];
    const auto y3 = data[wrap (index1 + 2)];
    const auto fraction2 = fraction * fraction;
    const auto fraction3 = fraction2 * fraction;

    return 0.5f * ((2.0f * y1)
                   + (-y0 + y2) * fraction
                   + (2.0f * y0 - 5.0f * y1 + 4.0f * y2 - y3) * fraction2
                   + (-y0 + 3.0f * y1 - 3.0f * y2 + y3) * fraction3);
}
