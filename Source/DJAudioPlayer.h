/*
  ==============================================================================

    DJAudioPlayer.h
    Created: 18 Jan 2025 3:02:48pm
    Author:  OkJames

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "VolumeLimiter.h"

class DJAudioPlayer : public AudioSource {
public:
    DJAudioPlayer(AudioFormatManager& _formatManager);
    ~DJAudioPlayer();

    //==============================================================================
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock(const AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    void loadURL(URL audioURL);
    void setGain(double gain);
    void setSpeed(double ratio);
    void setPosition(double posInSec);
    void setPositionRelative(double pos);

    void start();
    void stop();

    double getPositionRelative();

    void setLimiterEnabled(bool enabled);

    double getMaxVolume();

private:
    AudioFormatManager& formatManager;
    std::unique_ptr<AudioFormatReaderSource> readerSource;
    AudioTransportSource transportSource;
    ResamplingAudioSource resampleSource{ &transportSource, false, 2 };

    VolumeLimiter volumeLimiter;

    VolumeLimiter::LimiterParameters limiterParams;

    double maxVolume = 4.0;
};

