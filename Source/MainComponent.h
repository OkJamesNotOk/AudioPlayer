/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "DJAudioPlayer.h"
#include "PlaylistComponent.h"
#include "PlaylistLooper.h"

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent : public AudioAppComponent
{
public:
    //==============================================================================
    MainComponent();
    ~MainComponent();

    //==============================================================================
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock(const AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    //==============================================================================
    void paint(Graphics& g) override;
    void resized() override;

    void taskbarPlayPause();
    void taskbarPrevious();
    void taskbarNext();

private:

    AudioFormatManager formatManager;
    AudioThumbnailCache thumbCache{ 100 };

    MixerAudioSource mixerSource;

    PlaylistComponent playlistComponent;
    DJAudioPlayer looperPlayer{ formatManager };
    PlaylistLooper playlistLooper{ &looperPlayer, formatManager, thumbCache, playlistComponent };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};
