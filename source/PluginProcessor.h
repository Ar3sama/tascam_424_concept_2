#pragma once

#include <juce_audio_processors/juce_audio_processors.h>

#include <atomic>

#include "dsp/PluginDSP.h"
#include "parameters/ParameterLayout.h"


//==============================================================================
class AudioPluginAudioProcessor final : public juce::AudioProcessor
{
public:
    //==============================================================================
    AudioPluginAudioProcessor();
    ~AudioPluginAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    using AudioProcessor::processBlock;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    juce::AudioProcessorValueTreeState parameters;

    float getInputMeterLevel() const noexcept;
    float getOutputMeterLevel() const noexcept;

private:
    //==============================================================================
    struct ParameterPointers
    {
        std::atomic<float>* inputGain = nullptr;
        std::atomic<float>* outputGain = nullptr;
        std::atomic<float>* drive = nullptr;
        std::atomic<float>* tapeAmount = nullptr;
        std::atomic<float>* saturation = nullptr;
        std::atomic<float>* compression = nullptr;
        std::atomic<float>* mix = nullptr;
        std::atomic<float>* wow = nullptr;
        std::atomic<float>* flutter = nullptr;
        std::atomic<float>* tapeSpeed = nullptr;
        std::atomic<float>* bandwidth = nullptr;
        std::atomic<float>* hysteresis = nullptr;
        std::atomic<float>* bias = nullptr;
        std::atomic<float>* noiseAmount = nullptr;
        std::atomic<float>* crosstalk = nullptr;
        std::atomic<float>* dropouts = nullptr;
        std::atomic<float>* dbxEnabled = nullptr;
        std::atomic<float>* dbxAmount = nullptr;
        std::atomic<float>* oversampling = nullptr;
    };

    void cacheParameterPointers();
    PortastudioParameters loadPortastudioParameters() const noexcept;
    static float calculateRmsLevel (const juce::AudioBuffer<float>& buffer,
                                    int numChannels,
                                    int numSamples) noexcept;

    PluginDSP dsp;
    ParameterPointers parameterPointers;
    std::atomic<float> inputMeterLevel { 0.0f };
    std::atomic<float> outputMeterLevel { 0.0f };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioPluginAudioProcessor)
};
