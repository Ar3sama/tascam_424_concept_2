#include "ParameterLayout.h"
#include "ParameterIDs.h"

namespace Parameters
{
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout()
    {
        std::vector<std::unique_ptr<juce::RangedAudioParameter>> parameters;

        parameters.push_back (std::make_unique<juce::AudioParameterBool> (
            ParameterIDs::bypass,
            "Bypass",
            false));

        // Register plugin parameters here.
        // Keep IDs in ParameterIDs.h and connect UI controls with APVTS attachments.

        return { parameters.begin(), parameters.end() };
    }
}
