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
}
void DJAudioPlayer::getNextAudioBlock(const AudioSourceChannelInfo& bufferToFill) {
    resampleSource.getNextAudioBlock(bufferToFill);

}
void DJAudioPlayer::releaseResources() {
    transportSource.releaseResources();
    resampleSource.releaseResources();
}

double DJAudioPlayer::getPositionRelative() {
    //return transportSource.getCurrentPosition() / transportSource.getLengthInSeconds();

    double length = transportSource.getLengthInSeconds();
    if (length <= 0.0)
        return 0.0;

    return transportSource.getCurrentPosition() / length;
}

void DJAudioPlayer::loadURL(URL audioURL) {
    auto* reader = formatManager.createReaderFor(audioURL.createInputStream(false));
    if (reader != nullptr) // good file!
    {
        std::unique_ptr<AudioFormatReaderSource> newSource(new AudioFormatReaderSource(reader, true));
        transportSource.setSource(newSource.get(), 0, nullptr, reader->sampleRate);
        readerSource.reset(newSource.release());
    }
}

void DJAudioPlayer::setGain(double gain) {
    if (gain < 0 || gain > 2.0) {
        std::cout << "DJAudioPlayer::setGain: gain should be between 0 and 2" << std::endl;
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


