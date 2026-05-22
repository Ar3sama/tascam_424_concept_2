#include "PluginDSP.h"

void PluginDSP::prepare (double sampleRate, int samplesPerBlock, int numChannels)
{
    //TODO : Remove This Statement and Prepare ressources here.
    juce::ignoreUnused (sampleRate, samplesPerBlock, numChannels);
}

void PluginDSP::reset()
{
}

void PluginDSP::processBlock (juce::AudioBuffer<float>& buffer)
{
    // Neutral template: audio passes through unchanged until plugin-specific DSP is added.
    //TODO : add plugin DSP here.
    juce::ignoreUnused (buffer);
}
