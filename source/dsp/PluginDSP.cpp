#include "PluginDSP.h"

void PluginDSP::prepare (double sampleRate, int samplesPerBlock, int numChannels)
{
    currentSampleRate = sampleRate;
    preparedNumChannels = juce::jmax (1, numChannels);
    preparedBlockSize = juce::jmax (samplesPerBlock, 4096);

    dryBuffer.setSize (preparedNumChannels, preparedBlockSize, false, false, true);

    inputGain.reset (currentSampleRate, 0.020);
    outputGain.reset (currentSampleRate, 0.020);
    mix.reset (currentSampleRate, 0.030);

    inputGain.setCurrentAndTargetValue (1.0f);
    outputGain.setCurrentAndTargetValue (1.0f);
    mix.setCurrentAndTargetValue (1.0f);

    oversampling.prepare (currentSampleRate, preparedBlockSize, preparedNumChannels);
    tapeProcessor4x.prepare (currentSampleRate * 4.0, preparedBlockSize * 4, preparedNumChannels);
    tapeProcessor8x.prepare (currentSampleRate * 8.0, preparedBlockSize * 8, preparedNumChannels);
}

void PluginDSP::reset()
{
    dryBuffer.clear();
    oversampling.reset();
    tapeProcessor4x.reset();
    tapeProcessor8x.reset();
    inputGain.setCurrentAndTargetValue (1.0f);
    outputGain.setCurrentAndTargetValue (1.0f);
    mix.setCurrentAndTargetValue (1.0f);
}

void PluginDSP::processBlock (juce::AudioBuffer<float>& buffer, const PortastudioParameters& parameters)
{
    const auto numSamples = buffer.getNumSamples();

    if (numSamples <= 0)
        return;

    currentParameters = parameters;

    const auto numChannels = juce::jmin (buffer.getNumChannels(), preparedNumChannels);
    const auto hasDryBuffer = dryBuffer.getNumChannels() >= numChannels
                           && dryBuffer.getNumSamples() >= numSamples;

    if (hasDryBuffer)
    {
        for (int channel = 0; channel < numChannels; ++channel)
            dryBuffer.copyFrom (channel, 0, buffer, channel, 0, numSamples);
    }
    else
    {
        jassertfalse;
    }

    inputGain.setTargetValue (PortastudioDsp::dbToGain (currentParameters.inputGainDb));
    outputGain.setTargetValue (PortastudioDsp::dbToGain (currentParameters.outputGainDb));
    mix.setTargetValue (PortastudioDsp::clamp01 (currentParameters.mix));

    applyInputGain (buffer, numChannels, numSamples);

    if (currentParameters.oversamplingIndex > 0)
    {
        oversampling.process (buffer,
                              OversamplingProcessor::Mode::eightTimes,
                              tapeProcessor8x,
                              currentParameters);
    }
    else
    {
        oversampling.process (buffer,
                              OversamplingProcessor::Mode::fourTimes,
                              tapeProcessor4x,
                              currentParameters);
    }

    applyMixAndOutputGain (buffer, numChannels, numSamples, hasDryBuffer);
}

void PluginDSP::applyInputGain (juce::AudioBuffer<float>& buffer, int numChannels, int numSamples)
{
    for (int sample = 0; sample < numSamples; ++sample)
    {
        const auto gain = inputGain.getNextValue();

        for (int channel = 0; channel < numChannels; ++channel)
            buffer.getWritePointer (channel)[sample] *= gain;
    }
}

void PluginDSP::applyMixAndOutputGain (juce::AudioBuffer<float>& buffer,
                                       int numChannels,
                                       int numSamples,
                                       bool hasDryBuffer)
{
    for (int sample = 0; sample < numSamples; ++sample)
    {
        const auto wetAmount = mix.getNextValue();
        const auto gain = outputGain.getNextValue();

        for (int channel = 0; channel < numChannels; ++channel)
        {
            auto* data = buffer.getWritePointer (channel);
            const auto wet = data[sample];
            const auto mixed = hasDryBuffer
                                 ? dryBuffer.getSample (channel, sample) + (wet - dryBuffer.getSample (channel, sample)) * wetAmount
                                 : wet;

            data[sample] = PortastudioDsp::softHeadroom (mixed * gain);
        }
    }
}
