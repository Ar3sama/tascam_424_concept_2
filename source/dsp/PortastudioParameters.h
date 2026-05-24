#pragma once

struct PortastudioParameters
{
    float inputGainDb = 0.0f;
    float outputGainDb = 0.0f;
    float drive = 0.35f;
    float tapeAmount = 0.65f;
    float saturation = 0.45f;
    float compression = 0.35f;
    float mix = 1.0f;

    float wow = 0.18f;
    float flutter = 0.12f;
    int tapeSpeedIndex = 1;
    float bandwidth = 0.65f;
    float hysteresis = 0.35f;
    float bias = 0.5f;

    float noiseAmount = 0.12f;
    float crosstalk = 0.18f;
    float dropouts = 0.05f;

    bool dbxEnabled = true;
    float dbxAmount = 0.55f;

    int oversamplingIndex = 0;
};
