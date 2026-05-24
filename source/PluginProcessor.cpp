#include "PluginProcessor.h"
#include "PluginEditor.h"

#include "parameters/ParameterIDs.h"

//==============================================================================
AudioPluginAudioProcessor::AudioPluginAudioProcessor()
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ),
       parameters (*this, nullptr, "Parameters", Parameters::createParameterLayout())
{
    cacheParameterPointers();
}


AudioPluginAudioProcessor::~AudioPluginAudioProcessor()
{
}

//==============================================================================
const juce::String AudioPluginAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool AudioPluginAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool AudioPluginAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool AudioPluginAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double AudioPluginAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int AudioPluginAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int AudioPluginAudioProcessor::getCurrentProgram()
{
    return 0;
}

void AudioPluginAudioProcessor::setCurrentProgram (int index)
{
    juce::ignoreUnused (index);
}

const juce::String AudioPluginAudioProcessor::getProgramName (int index)
{
    juce::ignoreUnused (index);
    return {};
}

void AudioPluginAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
    juce::ignoreUnused (index, newName);
}

//==============================================================================
void AudioPluginAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    dsp.prepare (sampleRate, samplesPerBlock, getTotalNumOutputChannels());
}

void AudioPluginAudioProcessor::releaseResources()
{
    dsp.reset();
}

bool AudioPluginAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}

void AudioPluginAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer,
                                              juce::MidiBuffer& midiMessages)
{
    juce::ignoreUnused (midiMessages);

    juce::ScopedNoDenormals noDenormals;
    const auto totalNumInputChannels  = getTotalNumInputChannels();
    const auto totalNumOutputChannels = getTotalNumOutputChannels();
    const auto numSamples = buffer.getNumSamples();

    inputMeterLevel.store (calculateRmsLevel (buffer, totalNumInputChannels, numSamples),
                           std::memory_order_relaxed);

    
    // clears any output channels that didn't contain input data
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, numSamples);

    const auto bypass = parameters.getRawParameterValue (ParameterIDs::bypass)->load() > 0.5f;

    if (bypass)
    {
        outputMeterLevel.store (inputMeterLevel.load (std::memory_order_relaxed), std::memory_order_relaxed);
        return;
    }

    dsp.processBlock (buffer, loadPortastudioParameters());
    outputMeterLevel.store (calculateRmsLevel (buffer, totalNumOutputChannels, numSamples),
                            std::memory_order_relaxed);
}

float AudioPluginAudioProcessor::getInputMeterLevel() const noexcept
{
    return inputMeterLevel.load (std::memory_order_relaxed);
}

float AudioPluginAudioProcessor::getOutputMeterLevel() const noexcept
{
    return outputMeterLevel.load (std::memory_order_relaxed);
}

void AudioPluginAudioProcessor::cacheParameterPointers()
{
    const auto getParameter = [this] (const char* parameterID)
    {
        return parameters.getRawParameterValue (parameterID);
    };

    parameterPointers.inputGain = getParameter (ParameterIDs::inputGain);
    parameterPointers.outputGain = getParameter (ParameterIDs::outputGain);
    parameterPointers.drive = getParameter (ParameterIDs::drive);
    parameterPointers.tapeAmount = getParameter (ParameterIDs::tapeAmount);
    parameterPointers.saturation = getParameter (ParameterIDs::saturation);
    parameterPointers.compression = getParameter (ParameterIDs::compression);
    parameterPointers.mix = getParameter (ParameterIDs::mix);
    parameterPointers.wow = getParameter (ParameterIDs::wow);
    parameterPointers.flutter = getParameter (ParameterIDs::flutter);
    parameterPointers.tapeSpeed = getParameter (ParameterIDs::tapeSpeed);
    parameterPointers.bandwidth = getParameter (ParameterIDs::bandwidth);
    parameterPointers.hysteresis = getParameter (ParameterIDs::hysteresis);
    parameterPointers.bias = getParameter (ParameterIDs::bias);
    parameterPointers.noiseAmount = getParameter (ParameterIDs::noiseAmount);
    parameterPointers.crosstalk = getParameter (ParameterIDs::crosstalk);
    parameterPointers.dropouts = getParameter (ParameterIDs::dropouts);
    parameterPointers.dbxEnabled = getParameter (ParameterIDs::dbxEnabled);
    parameterPointers.dbxAmount = getParameter (ParameterIDs::dbxAmount);
    parameterPointers.oversampling = getParameter (ParameterIDs::oversampling);
}

PortastudioParameters AudioPluginAudioProcessor::loadPortastudioParameters() const noexcept
{
    const auto load = [] (const std::atomic<float>* parameter, float fallback) noexcept
    {
        return parameter != nullptr ? parameter->load (std::memory_order_relaxed) : fallback;
    };

    const auto loadPercent = [&load] (const std::atomic<float>* parameter, float fallback) noexcept
    {
        return PortastudioDsp::clamp01 (load (parameter, fallback) * 0.01f);
    };

    PortastudioParameters result;
    result.inputGainDb = load (parameterPointers.inputGain, 0.0f);
    result.outputGainDb = load (parameterPointers.outputGain, 0.0f);
    result.drive = loadPercent (parameterPointers.drive, 35.0f);
    result.tapeAmount = loadPercent (parameterPointers.tapeAmount, 65.0f);
    result.saturation = loadPercent (parameterPointers.saturation, 45.0f);
    result.compression = loadPercent (parameterPointers.compression, 35.0f);
    result.mix = loadPercent (parameterPointers.mix, 100.0f);
    result.wow = loadPercent (parameterPointers.wow, 18.0f);
    result.flutter = loadPercent (parameterPointers.flutter, 12.0f);
    result.tapeSpeedIndex = static_cast<int> (load (parameterPointers.tapeSpeed, 1.0f) + 0.5f);
    result.bandwidth = loadPercent (parameterPointers.bandwidth, 65.0f);
    result.hysteresis = loadPercent (parameterPointers.hysteresis, 35.0f);
    result.bias = loadPercent (parameterPointers.bias, 50.0f);
    result.noiseAmount = loadPercent (parameterPointers.noiseAmount, 12.0f);
    result.crosstalk = loadPercent (parameterPointers.crosstalk, 18.0f);
    result.dropouts = loadPercent (parameterPointers.dropouts, 5.0f);
    result.dbxEnabled = load (parameterPointers.dbxEnabled, 1.0f) > 0.5f;
    result.dbxAmount = loadPercent (parameterPointers.dbxAmount, 55.0f);
    result.oversamplingIndex = static_cast<int> (load (parameterPointers.oversampling, 0.0f) + 0.5f);

    return result;
}

float AudioPluginAudioProcessor::calculateRmsLevel (const juce::AudioBuffer<float>& buffer,
                                                    int numChannels,
                                                    int numSamples) noexcept
{
    const auto channelsToMeasure = juce::jmin (numChannels, buffer.getNumChannels());

    if (channelsToMeasure <= 0 || numSamples <= 0)
        return 0.0f;

    double sum = 0.0;

    for (int channel = 0; channel < channelsToMeasure; ++channel)
    {
        const auto* data = buffer.getReadPointer (channel);

        for (int sample = 0; sample < numSamples; ++sample)
        {
            const auto value = data[sample];
            sum += static_cast<double> (value * value);
        }
    }

    return static_cast<float> (std::sqrt (sum / static_cast<double> (channelsToMeasure * numSamples)));
}

//==============================================================================
bool AudioPluginAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* AudioPluginAudioProcessor::createEditor()
{
    return new AudioPluginAudioProcessorEditor (*this);
}

//==============================================================================
void AudioPluginAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = parameters.copyState();
    std::unique_ptr<juce::XmlElement> xml (state.createXml());

    if (xml != nullptr)
        copyXmlToBinary (*xml, destData);
}

void AudioPluginAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));

    if (xmlState != nullptr && xmlState->hasTagName (parameters.state.getType()))
        parameters.replaceState (juce::ValueTree::fromXml (*xmlState));
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new AudioPluginAudioProcessor();
}
