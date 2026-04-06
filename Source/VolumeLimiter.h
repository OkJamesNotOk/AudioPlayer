/*
  ==============================================================================

    VolumeLimiter.h
    Created: 5 Apr 2026 9:43:51pm
    Author:  OkJames

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
*/
class VolumeLimiter
{
public:
    VolumeLimiter();

    struct LimiterParameters
    {
        float thresholdDb = -6.0f;
        float attackMs = 1.0f;
        float releaseMs = 80.0f;
        float lookAheadMs = 5.0f;
        float gainSmoothMs = 2.0f;
    };

    void prepare(double newSampleRate, int samplesPerBlock);
    void reset();

    void setThresholdDb(float newThresholdDb);
    void setAttackMs(float newAttackMs);
    void setReleaseMs(float newReleaseMs);

    void processBlock(juce::AudioBuffer<float>& buffer, int startSample, int numSamples);

    float getCurrentGainDb() const;
    float getEnvelopeDb() const;

    void setLookAheadMs(float newLookAheadMs);

    void setLimiterParameters(const LimiterParameters& newParams);

    void setEnabled(bool shouldBeEnabled);
    bool getEnabled() const;

private:
    void updateCoefficients();

    double sampleRate = 44100.0;
    int blockSize = 0;

    LimiterParameters limiterParams;

    float attackCoeff = 0.0f;
    float releaseCoeff = 0.0f;
    float gainSmoothCoeff = 0.0f;

    float envelope = 0.0f;
    float currentGain = 1.0f;
    float targetGain = 1.0f;

    int lookAheadSamples = 0;

    juce::AudioBuffer<float> delayBuffer;
    int delayWritePosition = 0;

    bool isEnabled = true;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VolumeLimiter)
};
