#pragma once

#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_dsp/juce_dsp.h>

#include "OversamplingProcessor.h"
#include "PortastudioParameters.h"
#include "TapeProcessor.h"

class PluginDSP
{
public:
    void prepare (double sampleRate, int samplesPerBlock, int numChannels);
    void reset();

    void processBlock (juce::AudioBuffer<float>& buffer, const PortastudioParameters& parameters);

private:
    void applyInputGain (juce::AudioBuffer<float>& buffer, int numChannels, int numSamples);
    void applyMixAndOutputGain (juce::AudioBuffer<float>& buffer,
                                int numChannels,
                                int numSamples,
                                bool hasDryBuffer);

    double currentSampleRate = 44100.0;
    int preparedNumChannels = 0;
    int preparedBlockSize = 0;

    juce::AudioBuffer<float> dryBuffer;
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Multiplicative> inputGain;
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Multiplicative> outputGain;
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> mix;

    PortastudioParameters currentParameters;
    OversamplingProcessor oversampling;
    TapeProcessor tapeProcessor4x;
    TapeProcessor tapeProcessor8x;
};
