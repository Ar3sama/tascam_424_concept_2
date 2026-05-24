#pragma once

#include <juce_dsp/juce_dsp.h>

#include <vector>

#include "CrosstalkProcessor.h"
#include "DbxProcessor.h"
#include "DynamicBandwidthProcessor.h"
#include "HysteresisProcessor.h"
#include "NoiseProcessor.h"
#include "PortastudioParameters.h"
#include "TapeCompression.h"
#include "TapeSaturation.h"
#include "WowFlutterProcessor.h"

class TapeProcessor
{
public:
    void prepare (double sampleRate, int maximumBlockSize, int numChannels);
    void reset();

    void processBlock (juce::dsp::AudioBlock<float>& block,
                       const PortastudioParameters& parameters) noexcept;

private:
    float processPreampSample (int channel,
                               float input,
                               const PortastudioParameters& parameters) noexcept;

    struct PreampState
    {
        PortastudioDsp::OnePoleLowpass lowBand;
        PortastudioDsp::OnePoleLowpass highBand;
        PortastudioDsp::OnePoleLowpass toneLoss;
    };

    std::vector<PreampState> preampStates;
    DbxProcessor dbxEncoder;
    DbxProcessor dbxDecoder;
    TapeSaturation tapeSaturation;
    TapeCompression tapeCompression;
    HysteresisProcessor hysteresis;
    DynamicBandwidthProcessor dynamicBandwidth;
    WowFlutterProcessor wowFlutter;
    NoiseProcessor noise;
    CrosstalkProcessor crosstalk;
};
