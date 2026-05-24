#include "OversamplingProcessor.h"

void OversamplingProcessor::prepare (double sampleRate, int maximumBlockSize, int numChannels)
{
    juce::ignoreUnused (sampleRate);

    const auto channels = static_cast<size_t> (juce::jmax (1, numChannels));
    const auto blockSize = static_cast<size_t> (juce::jmax (1, maximumBlockSize));

    oversampling4x = std::make_unique<juce::dsp::Oversampling<float>> (
        channels,
        2,
        juce::dsp::Oversampling<float>::filterHalfBandPolyphaseIIR,
        true);

    oversampling8x = std::make_unique<juce::dsp::Oversampling<float>> (
        channels,
        3,
        juce::dsp::Oversampling<float>::filterHalfBandPolyphaseIIR,
        true);

    oversampling4x->initProcessing (blockSize);
    oversampling8x->initProcessing (blockSize);
    reset();
}

void OversamplingProcessor::reset()
{
    if (oversampling4x != nullptr)
        oversampling4x->reset();

    if (oversampling8x != nullptr)
        oversampling8x->reset();
}
