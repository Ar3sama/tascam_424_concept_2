#pragma once

#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_core/juce_core.h>

#include <cmath>
#include <cstdint>

namespace PortastudioDsp
{
    inline constexpr float minimumLevel = 1.0e-8f;

    inline float clamp01 (float value) noexcept
    {
        return juce::jlimit (0.0f, 1.0f, value);
    }

    inline float dbToGain (float decibels) noexcept
    {
        return juce::Decibels::decibelsToGain (decibels);
    }

    inline float gainToDb (float gain) noexcept
    {
        return juce::Decibels::gainToDecibels (juce::jmax (gain, minimumLevel), -120.0f);
    }

    inline float onePoleCoefficient (float cutoffHz, double sampleRate) noexcept
    {
        const auto limitedCutoff = juce::jlimit (5.0f,
                                                 static_cast<float> (sampleRate * 0.45),
                                                 cutoffHz);

        return 1.0f - std::exp (-juce::MathConstants<float>::twoPi
                                * limitedCutoff
                                / static_cast<float> (sampleRate));
    }

    inline float asymmetricalSaturation (float input, float drive, float asymmetry) noexcept
    {
        const auto limitedDrive = juce::jlimit (0.1f, 12.0f, drive);
        const auto limitedAsymmetry = juce::jlimit (-0.35f, 0.35f, asymmetry);
        const auto offset = std::tanh (limitedAsymmetry * limitedDrive);
        const auto shaped = std::tanh ((input + limitedAsymmetry) * limitedDrive) - offset;
        const auto normaliser = 1.0f / juce::jmax (0.35f, 1.0f - std::abs (offset));

        return shaped * normaliser;
    }

    inline float softHeadroom (float input) noexcept
    {
        constexpr auto ceiling = 1.45f;
        return ceiling * std::tanh (input / ceiling);
    }

    class OnePoleLowpass
    {
    public:
        void prepare (double newSampleRate, float cutoffHz)
        {
            sampleRate = newSampleRate;
            setCutoff (cutoffHz);
            reset();
        }

        void reset() noexcept
        {
            state = 0.0f;
        }

        void setCutoff (float cutoffHz) noexcept
        {
            coefficient = onePoleCoefficient (cutoffHz, sampleRate);
        }

        void setCoefficient (float newCoefficient) noexcept
        {
            coefficient = juce::jlimit (0.0f, 1.0f, newCoefficient);
        }

        float getCoefficient() const noexcept
        {
            return coefficient;
        }

        float process (float input) noexcept
        {
            state += coefficient * (input - state);
            return state;
        }

    private:
        double sampleRate = 44100.0;
        float coefficient = 0.1f;
        float state = 0.0f;
    };

    class OnePoleHighpass
    {
    public:
        void prepare (double newSampleRate, float cutoffHz)
        {
            lowpass.prepare (newSampleRate, cutoffHz);
        }

        void reset() noexcept
        {
            lowpass.reset();
        }

        float process (float input) noexcept
        {
            return input - lowpass.process (input);
        }

    private:
        OnePoleLowpass lowpass;
    };

    class FastRandom
    {
    public:
        void setSeed (std::uint32_t newSeed) noexcept
        {
            state = newSeed != 0 ? newSeed : 0x1234567u;
        }

        float nextFloat() noexcept
        {
            state = state * 1664525u + 1013904223u;
            return static_cast<float> ((state >> 8) & 0x00ffffffu) / 16777216.0f;
        }

        float nextBipolar() noexcept
        {
            return nextFloat() * 2.0f - 1.0f;
        }

    private:
        std::uint32_t state = 0x1234567u;
    };
}
