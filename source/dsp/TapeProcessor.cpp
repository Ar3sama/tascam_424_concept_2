#include "TapeProcessor.h"

void TapeProcessor::prepare (double sampleRate, int maximumBlockSize, int numChannels)
{
    const auto channels = juce::jmax (1, numChannels);
    preampStates.resize (static_cast<size_t> (channels));

    for (auto& state : preampStates)
    {
        state.lowBand.prepare (sampleRate, 360.0f);
        state.highBand.prepare (sampleRate, 2850.0f);
        state.toneLoss.prepare (sampleRate, 20500.0f);
    }

    dbxEncoder.prepare (sampleRate, channels);
    dbxDecoder.prepare (sampleRate, channels);
    tapeSaturation.prepare (sampleRate, channels);
    tapeCompression.prepare (sampleRate, channels);
    hysteresis.prepare (channels);
    dynamicBandwidth.prepare (sampleRate, channels);
    wowFlutter.prepare (sampleRate, maximumBlockSize, channels);
    noise.prepare (sampleRate, channels);
    crosstalk.prepare (sampleRate);
    reset();
}

void TapeProcessor::reset()
{
    for (auto& state : preampStates)
    {
        state.lowBand.reset();
        state.highBand.reset();
        state.toneLoss.reset();
    }

    dbxEncoder.reset();
    dbxDecoder.reset();
    tapeSaturation.reset();
    tapeCompression.reset();
    hysteresis.reset();
    dynamicBandwidth.reset();
    wowFlutter.reset();
    noise.reset();
    crosstalk.reset();
}

void TapeProcessor::processBlock (juce::dsp::AudioBlock<float>& block,
                                  const PortastudioParameters& parameters) noexcept
{
    if (preampStates.empty())
        return;

    const auto numChannels = static_cast<int> (juce::jmin (block.getNumChannels(), preampStates.size()));
    const auto numSamples = static_cast<int> (block.getNumSamples());

    for (int sample = 0; sample < numSamples; ++sample)
    {
        for (int channel = 0; channel < numChannels; ++channel)
        {
            auto* data = block.getChannelPointer (static_cast<size_t> (channel));
            auto value = data[sample];

            value = processPreampSample (channel, value, parameters);

            if (parameters.dbxEnabled)
                value = dbxEncoder.processEncoderSample (channel, value, parameters.dbxAmount);

            value = tapeSaturation.processSample (channel,
                                                  value,
                                                  parameters.drive,
                                                  parameters.tapeAmount,
                                                  parameters.saturation,
                                                  parameters.bias);
            value = tapeCompression.processSample (channel,
                                                   value,
                                                   parameters.compression,
                                                   parameters.drive);
            value = hysteresis.processSample (channel,
                                              value,
                                              parameters.hysteresis,
                                              parameters.bias);
            value = dynamicBandwidth.processSample (channel,
                                                    value,
                                                    parameters.bandwidth,
                                                    parameters.tapeAmount,
                                                    parameters.drive + parameters.saturation,
                                                    parameters.tapeSpeedIndex);

            data[sample] = PortastudioDsp::softHeadroom (value);
        }
    }

    wowFlutter.processBlock (block, parameters.wow, parameters.flutter, parameters.tapeSpeedIndex);
    noise.processBlock (block, parameters.noiseAmount, parameters.dropouts, parameters.tapeSpeedIndex);
    crosstalk.processBlock (block, parameters.crosstalk);

    if (parameters.dbxEnabled)
    {
        for (int sample = 0; sample < numSamples; ++sample)
        {
            for (int channel = 0; channel < numChannels; ++channel)
            {
                auto* data = block.getChannelPointer (static_cast<size_t> (channel));
                data[sample] = dbxDecoder.processDecoderSample (channel, data[sample], parameters.dbxAmount);
            }
        }
    }
}

float TapeProcessor::processPreampSample (int channel,
                                          float input,
                                          const PortastudioParameters& parameters) noexcept
{
    auto& state = preampStates[static_cast<size_t> (juce::jlimit (0, static_cast<int> (preampStates.size()) - 1, channel))];

    // Two gentle one-poles create a broad mid push without a resonant EQ stage.
    const auto low = state.lowBand.process (input);
    const auto high = state.highBand.process (input);
    const auto midBand = high - low;
    const auto midPush = 0.035f + parameters.drive * 0.060f + parameters.tapeAmount * 0.035f;
    const auto preampDrive = 1.0f + parameters.drive * 0.75f;
    const auto asymmetry = (parameters.bias - 0.5f) * 0.045f + parameters.drive * 0.012f;

    auto output = input + midBand * midPush;
    output = PortastudioDsp::asymmetricalSaturation (output * preampDrive, 1.0f + parameters.drive * 0.35f, asymmetry);
    output = state.toneLoss.process (output);

    return input + (output - input) * (0.55f + parameters.drive * 0.25f);
}
