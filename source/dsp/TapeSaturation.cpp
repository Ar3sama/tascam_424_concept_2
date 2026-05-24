#include "TapeSaturation.h"

void TapeSaturation::prepare (double sampleRate, int numChannels)
{
    states.resize (static_cast<size_t> (juce::jmax (1, numChannels)));

    for (auto& state : states)
    {
        state.firstStageTone.prepare (sampleRate, 18500.0f);
        state.secondStageTone.prepare (sampleRate, 14500.0f);
    }

    reset();
}

void TapeSaturation::reset()
{
    for (auto& state : states)
    {
        state.firstStageTone.reset();
        state.secondStageTone.reset();
    }
}

float TapeSaturation::processSample (int channel,
                                     float input,
                                     float drive,
                                     float tapeAmount,
                                     float saturation,
                                     float bias) noexcept
{
    if (states.empty())
        return input;

    auto& state = states[static_cast<size_t> (juce::jlimit (0, static_cast<int> (states.size()) - 1, channel))];

    const auto driveAmount = PortastudioDsp::clamp01 (drive);
    const auto tape = PortastudioDsp::clamp01 (tapeAmount);
    const auto sat = PortastudioDsp::clamp01 (saturation);
    const auto biasOffset = (PortastudioDsp::clamp01 (bias) - 0.5f) * 0.10f;

    const auto firstDrive = 1.05f + driveAmount * 2.2f + sat * 1.6f;
    const auto secondDrive = 1.15f + driveAmount * 1.5f + sat * 2.4f + tape * 0.8f;
    const auto trim = 1.0f / (1.0f + driveAmount * 0.35f + sat * 0.25f);

    auto driven = input * (1.0f + driveAmount * 0.75f + tape * 0.25f);
    auto first = PortastudioDsp::asymmetricalSaturation (driven, firstDrive, biasOffset);
    first = state.firstStageTone.process (first);

    const auto interstage = first * 0.82f + driven * 0.18f;
    auto second = PortastudioDsp::asymmetricalSaturation (interstage, secondDrive, -biasOffset * 0.45f);
    second = state.secondStageTone.process (second);

    const auto shaped = second * trim;
    return input + (shaped - input) * tape;
}
