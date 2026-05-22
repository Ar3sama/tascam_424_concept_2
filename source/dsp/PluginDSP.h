#pragma once

#include <juce_audio_basics/juce_audio_basics.h>

class PluginDSP
{
public:
    void prepare (double sampleRate, int samplesPerBlock, int numChannels);
    void reset();

    void processBlock (juce::AudioBuffer<float>& buffer);
};
