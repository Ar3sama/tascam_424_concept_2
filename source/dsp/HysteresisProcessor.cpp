#include "HysteresisProcessor.h"

void HysteresisProcessor::prepare (int numChannels)
{
    states.resize (static_cast<size_t> (juce::jmax (1, numChannels)));
    reset();
}

void HysteresisProcessor::reset()
{
    for (auto& state : states)
    {
        state.memory = 0.0f;
        state.previousInput = 0.0f;
    }
}

float HysteresisProcessor::processSample (int channel, float input, float amount, float bias) noexcept
{
    const auto hysteresisAmount = PortastudioDsp::clamp01 (amount);

    if (hysteresisAmount <= 0.0001f || states.empty())
        return input;

    auto& state = states[static_cast<size_t> (juce::jlimit (0, static_cast<int> (states.size()) - 1, channel))];

    const auto target = input + (PortastudioDsp::clamp01 (bias) - 0.5f) * 0.035f;
    const auto follow = 0.00065f + hysteresisAmount * 0.0045f;
    state.memory += (target - state.memory) * follow;
    state.memory = juce::jlimit (-2.0f, 2.0f, state.memory);

    const auto magneticMemory = std::tanh (state.memory * 2.4f) * (0.075f * hysteresisAmount);
    const auto differentialLoss = (input - state.previousInput) * (0.012f * hysteresisAmount);
    state.previousInput = input;

    return input + magneticMemory - differentialLoss;
}
