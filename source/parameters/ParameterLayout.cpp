#include "ParameterLayout.h"
#include "ParameterIDs.h"

namespace Parameters
{
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout()
    {
        std::vector<std::unique_ptr<juce::RangedAudioParameter>> parameters;

        const auto addDbParameter = [&parameters] (const char* id,
                                                   const juce::String& name,
                                                   float defaultValue)
        {
            parameters.push_back (std::make_unique<juce::AudioParameterFloat> (
                id,
                name,
                juce::NormalisableRange<float> { -24.0f, 24.0f, 0.01f },
                defaultValue));
        };
        

        const auto addPercentParameter = [&parameters] (const char* id,
                                                        const juce::String& name,
                                                        float defaultValue)
        {
            parameters.push_back (std::make_unique<juce::AudioParameterFloat> (
                id,
                name,
                juce::NormalisableRange<float> { 0.0f, 100.0f, 0.01f },
                defaultValue));
        };

        addDbParameter (ParameterIDs::inputGain, "Input Gain", 0.0f);
        addDbParameter (ParameterIDs::outputGain, "Output Gain", 0.0f);
        addPercentParameter (ParameterIDs::drive, "Drive", 35.0f);
        addPercentParameter (ParameterIDs::tapeAmount, "Tape Amount", 65.0f);
        addPercentParameter (ParameterIDs::saturation, "Saturation", 45.0f);
        addPercentParameter (ParameterIDs::compression, "Compression", 35.0f);
        addPercentParameter (ParameterIDs::mix, "Mix", 100.0f);

        addPercentParameter (ParameterIDs::wow, "Wow", 18.0f);
        addPercentParameter (ParameterIDs::flutter, "Flutter", 12.0f);
        parameters.push_back (std::make_unique<juce::AudioParameterChoice> (
            ParameterIDs::tapeSpeed,
            "Tape Speed",
            juce::StringArray { "Low", "Normal", "High" },
            1));
        addPercentParameter (ParameterIDs::bandwidth, "Bandwidth", 65.0f);
        addPercentParameter (ParameterIDs::hysteresis, "Hysteresis", 35.0f);
        addPercentParameter (ParameterIDs::bias, "Bias", 50.0f);

        addPercentParameter (ParameterIDs::noiseAmount, "Noise Amount", 12.0f);
        addPercentParameter (ParameterIDs::crosstalk, "Crosstalk", 18.0f);
        addPercentParameter (ParameterIDs::dropouts, "Dropouts", 5.0f);

        parameters.push_back (std::make_unique<juce::AudioParameterBool> (
            ParameterIDs::dbxEnabled,
            "dbx Enable",
            true));
        addPercentParameter (ParameterIDs::dbxAmount, "dbx Amount", 55.0f);

        parameters.push_back (std::make_unique<juce::AudioParameterChoice> (
            ParameterIDs::oversampling,
            "Oversampling",
            juce::StringArray { "4x", "8x" },
            0));

        parameters.push_back (std::make_unique<juce::AudioParameterBool> (
            ParameterIDs::bypass,
            "Bypass",
            false));

        return { parameters.begin(), parameters.end() };
    }
}
