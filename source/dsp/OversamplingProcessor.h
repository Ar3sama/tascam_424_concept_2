#pragma once

#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_dsp/juce_dsp.h>

#include <memory>

#include "PortastudioParameters.h"

class OversamplingProcessor
{
public:
    enum class Mode
    {
        fourTimes,
        eightTimes
    };

    void prepare (double sampleRate, int maximumBlockSize, int numChannels);
    void reset();

    template <typename Processor>
    void process (juce::AudioBuffer<float>& buffer,
                  Mode mode,
                  Processor& processor,
                  const PortastudioParameters& parameters) noexcept
    {
        auto* oversampling = mode == Mode::eightTimes ? oversampling8x.get() : oversampling4x.get();

        if (oversampling == nullptr)
        {
            juce::dsp::AudioBlock<float> block { buffer };
            processor.processBlock (block, parameters);
            return;
        }

        juce::dsp::AudioBlock<float> inputBlock { buffer };
        auto oversampledBlock = oversampling->processSamplesUp (inputBlock);
        processor.processBlock (oversampledBlock, parameters);
        oversampling->processSamplesDown (inputBlock);
    }

private:
    std::unique_ptr<juce::dsp::Oversampling<float>> oversampling4x;
    std::unique_ptr<juce::dsp::Oversampling<float>> oversampling8x;
};
