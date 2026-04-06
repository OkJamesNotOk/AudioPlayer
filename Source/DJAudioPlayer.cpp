/*
  ==============================================================================

    DJAudioPlayer.cpp
    Created: 18 Jan 2025 3:02:48pm
    Author:  OkJames

  ==============================================================================
*/

#include "DJAudioPlayer.h"

DJAudioPlayer::DJAudioPlayer(AudioFormatManager& _formatManager) :formatManager(_formatManager)
{

}
DJAudioPlayer::~DJAudioPlayer() {}


//==============================================================================
void DJAudioPlayer::prepareToPlay(int samplesPerBlockExpected, double sampleRate) {
    transportSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
    resampleSource.prepareToPlay(samplesPerBlockExpected, sampleRate);

    limiterParams.thresholdDb = -12.0f;
    limiterParams.attackMs = 1.0f;
    limiterParams.releaseMs = 80.0f;
    limiterParams.lookAheadMs = 5.0f;
    limiterParams.gainSmoothMs = 2.0f;

    volumeLimiter.setLimiterParameters(limiterParams);
    volumeLimiter.prepare(sampleRate, samplesPerBlockExpected);
}
void DJAudioPlayer::getNextAudioBlock(const AudioSourceChannelInfo& bufferToFill) {
    resampleSource.getNextAudioBlock(bufferToFill);

    if (bufferToFill.buffer != nullptr)
        volumeLimiter.processBlock(*bufferToFill.buffer,
            bufferToFill.startSample,
            bufferToFill.numSamples);
}
void DJAudioPlayer::releaseResources() {
    transportSource.releaseResources();
    resampleSource.releaseResources();
    volumeLimiter.reset();
}

double DJAudioPlayer::getPositionRelative() {
    //return transportSource.getCurrentPosition() / transportSource.getLengthInSeconds();

    double length = transportSource.getLengthInSeconds();
    if (length <= 0.0)
        return 0.0;

    return transportSource.getCurrentPosition() / length;
}

void DJAudioPlayer::loadURL(URL audioURL)
{
    auto* reader = formatManager.createReaderFor(audioURL.createInputStream(false));

    if (reader != nullptr)
    {
        juce::Logger::writeToLog("loadURL success");
        juce::Logger::writeToLog("Reader sample rate: " + juce::String(reader->sampleRate));
        juce::Logger::writeToLog("Reader length: " + juce::String((double)reader->lengthInSamples));
        juce::Logger::writeToLog("Reader channels: " + juce::String((int)reader->numChannels));
        juce::Logger::writeToLog("Reader type: " + juce::String(typeid(*reader).name()));

        std::unique_ptr<AudioFormatReaderSource> newSource(new AudioFormatReaderSource(reader, true));
        transportSource.setSource(newSource.get(), 0, nullptr, reader->sampleRate);
        readerSource.reset(newSource.release());
    }
    else
    {
        juce::Logger::writeToLog("loadURL failed: no reader created");
    }
}

void DJAudioPlayer::setGain(double gain) {
    if (gain < 0 || gain > maxVolume) {
        std::cout << "DJAudioPlayer::setGain: gain should be between 0 and maxVolume" << std::endl;
    }
    else {
        transportSource.setGain(gain);
    }
}
void DJAudioPlayer::setSpeed(double ratio) {
    if (ratio <= 0.1 || ratio >= 4.0) {
        std::cout << "DJAudioPlayer::setSpeed: ratio should be between 0.1 and 4" << std::endl;
    }
    else {
        resampleSource.setResamplingRatio(ratio);
    }
}

void DJAudioPlayer::setPosition(double posInSec) {
    transportSource.setPosition(posInSec);
}

void DJAudioPlayer::setPositionRelative(double pos) {
    if (pos < 0 || pos > 1.0) {
        std::cout << "DJAudioPlayer::setPositionRelative: relative pos should be between 0 and 1." << std::endl;
    }
    else {
        double posInSecs = transportSource.getLengthInSeconds() * pos;
        setPosition(posInSecs);
    }
}

void DJAudioPlayer::start() {
    transportSource.start();
}
void DJAudioPlayer::stop() {
    transportSource.stop();
}

void DJAudioPlayer::setLimiterEnabled(bool enabled)
{
    volumeLimiter.setEnabled(enabled);
}

double DJAudioPlayer::getMaxVolume() {
    return maxVolume;
}