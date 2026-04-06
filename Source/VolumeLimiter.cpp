/*
  ==============================================================================

    VolumeLimiter.cpp
    Created: 5 Apr 2026 9:43:51pm
    Author:  OkJames
    simple limiter for high volume during playback.

  ==============================================================================
*/

#include <JuceHeader.h>
#include "VolumeLimiter.h"

//==============================================================================
VolumeLimiter::VolumeLimiter()
{

}

void VolumeLimiter::prepare(double newSampleRate, int samplesPerBlock)
{
    sampleRate = newSampleRate;
    blockSize = samplesPerBlock;

    lookAheadSamples = (int)std::round((limiterParams.lookAheadMs * 0.001f) * sampleRate);

    const int delayBufferSize = juce::jmax(1, lookAheadSamples + samplesPerBlock + 1);
    delayBuffer.setSize(2, delayBufferSize);
    delayBuffer.clear();

    delayWritePosition = 0;

    reset();
    updateCoefficients();
}

void VolumeLimiter::reset()
{
    envelope = 0.0f;
    currentGain = 1.0f;
    targetGain = 1.0f;
}

void VolumeLimiter::setThresholdDb(float newThresholdDb)
{
    limiterParams.thresholdDb = newThresholdDb;
}

void VolumeLimiter::setAttackMs(float newAttackMs)
{
    limiterParams.attackMs = juce::jmax(0.1f, newAttackMs);
    updateCoefficients();
}

void VolumeLimiter::setReleaseMs(float newReleaseMs)
{
    limiterParams.releaseMs = juce::jmax(1.0f, newReleaseMs);
    updateCoefficients();
}

void VolumeLimiter::processBlock(juce::AudioBuffer<float>& buffer, int startSample, int numSamples)
{
    if (!isEnabled) {
        reset();
        return;
    }

    const int numChannels = juce::jmin(buffer.getNumChannels(), delayBuffer.getNumChannels());
    const int delayBufferSize = delayBuffer.getNumSamples();

    if (numChannels <= 0 || numSamples <= 0 || delayBufferSize <= 0)
        return;

    for (int sample = 0; sample < numSamples; ++sample)
    {
        const int writePos = delayWritePosition;
        const int readPos = (writePos - lookAheadSamples + delayBufferSize) % delayBufferSize;

        float peak = 0.0f;

        for (int ch = 0; ch < numChannels; ++ch)
        {
            float in = buffer.getReadPointer(ch)[startSample + sample];
            delayBuffer.getWritePointer(ch)[writePos] = in;

            float absIn = std::abs(in);
            if (absIn > peak)
                peak = absIn;
        }

        if (peak > envelope)
            envelope = attackCoeff * envelope + (1.0f - attackCoeff) * peak;
        else
            envelope = releaseCoeff * envelope + (1.0f - releaseCoeff) * peak;

        const float envelopeDb = juce::Decibels::gainToDecibels(envelope, -100.0f);

        if (envelopeDb > limiterParams.thresholdDb)
        {
            const float gainReductionDb = limiterParams.thresholdDb - envelopeDb;
            targetGain = juce::Decibels::decibelsToGain(gainReductionDb);
        }
        else
        {
            targetGain = 1.0f;
        }

        currentGain = gainSmoothCoeff * currentGain + (1.0f - gainSmoothCoeff) * targetGain;

        for (int ch = 0; ch < numChannels; ++ch)
        {
            float delayedSample = delayBuffer.getReadPointer(ch)[readPos];
            buffer.getWritePointer(ch)[startSample + sample] = delayedSample * currentGain;
        }

        delayWritePosition = (delayWritePosition + 1) % delayBufferSize;
    }
}

float VolumeLimiter::getCurrentGainDb() const
{
    return juce::Decibels::gainToDecibels(currentGain, -100.0f);
}

float VolumeLimiter::getEnvelopeDb() const
{
    return juce::Decibels::gainToDecibels(envelope, -100.0f);
}

void VolumeLimiter::updateCoefficients()
{
    if (sampleRate <= 0.0)
        return;

    attackCoeff = std::exp(-1.0f / (0.001f * limiterParams.attackMs * (float)sampleRate));
    releaseCoeff = std::exp(-1.0f / (0.001f * limiterParams.releaseMs * (float)sampleRate));
    gainSmoothCoeff = std::exp(-1.0f / (0.001f * limiterParams.gainSmoothMs * (float)sampleRate));
}

void VolumeLimiter::setLookAheadMs(float newLookAheadMs)
{
    limiterParams.lookAheadMs = juce::jmax(0.0f, newLookAheadMs);
}

void VolumeLimiter::setLimiterParameters(const LimiterParameters& newParams)
{
    limiterParams = newParams;
    updateCoefficients();
}

void VolumeLimiter::setEnabled(bool shouldBeEnabled)
{
    isEnabled = shouldBeEnabled;
}

bool VolumeLimiter::getEnabled() const
{
    return isEnabled;
}