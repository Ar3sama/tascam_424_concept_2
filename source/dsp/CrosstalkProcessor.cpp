#include "CrosstalkProcessor.h"

void CrosstalkProcessor::prepare (double sampleRate)
{
    leftIntoRightLowpass.prepare (sampleRate, 720.0f);
    rightIntoLeftLowpass.prepare (sampleRate, 720.0f);
}

void CrosstalkProcessor::reset()
{
    leftIntoRightLowpass.reset();
    rightIntoLeftLowpass.reset();
}

void CrosstalkProcessor::processBlock (juce::dsp::AudioBlock<float>& block, float amount) noexcept
{
    const auto crosstalkAmount = PortastudioDsp::clamp01 (amount);

    if (crosstalkAmount <= 0.0001f || block.getNumChannels() < 2)
        return;

    auto* left = block.getChannelPointer (0);
    auto* right = block.getChannelPointer (1);
    const auto numSamples = static_cast<int> (block.getNumSamples());
    const auto bleedGain = PortastudioDsp::dbToGain (-36.0f + crosstalkAmount * 10.0f) * crosstalkAmount;

    for (int sample = 0; sample < numSamples; ++sample)
    {
        const auto leftInput = left[sample];
        const auto rightInput = right[sample];
        const auto rightBleed = leftIntoRightLowpass.process (leftInput);
        const auto leftBleed = rightIntoLeftLowpass.process (rightInput);

        left[sample] = leftInput + leftBleed * bleedGain;
        right[sample] = rightInput + rightBleed * bleedGain;
    }
}
