/*
  ==============================================================================

    MainComponent.cpp
    Created: 27 Mar 2026 2:51:06am
    Author:  OkJames

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "DJAudioPlayer.h"
#include "FFmpegAudioFormat.h"
#include "PlaylistComponent.h"
#include "PlaylistPlayer.h"
#include "PlaylistWindow.h"
#include "SettingsWindow.h"
#include "ImageHelper.h"
#include "VolumeLimiter.h"

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/


class MainComponent : 
    public AudioAppComponent
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

    bool taskbarPlayPause();
    void taskbarPrevious();
    void taskbarNext();
    bool isPlaying() const;

    std::function<void(bool)> onPlaybackStateChanged;

    void openPlaylist();
    void openSettings();

    void settingsManagement(const juce::String& key, const juce::var& value);
    void updatePlaylistPresentation();

    void closePlaylistOnShutDown();

    // set keyboard shortcut
    bool keyPressed(const KeyPress& key) override;

    void parentHierarchyChanged() override;

private:

    AudioFormatManager formatManager;
    AudioThumbnailCache thumbCache{ 100 };

    MixerAudioSource mixerSource;

    PlaylistComponent playlistComponent;
    DJAudioPlayer looperPlayer{ formatManager };
    PlaylistPlayer PlaylistPlayer{ &looperPlayer, formatManager, thumbCache, playlistComponent };

    juce::DrawableButton settingsButton{ "SettingsButton", juce::DrawableButton::ImageOnButtonBackground };
    juce::DrawableButton playlistButton{ "PlaylistButton", juce::DrawableButton::ImageOnButtonBackground };
    std::vector<juce::DrawableButton*> topBarButtons
    {
        &settingsButton,
        &playlistButton
    };

    std::unique_ptr<PlaylistWindow> playlistWindow;

    SettingsComponent settingsComponent;
    std::unique_ptr<SettingsWindow> settingsWindow;

    bool internalPlaylistVisible = false;
    int internalCollapsedWindowHeight = 0;

    void updateInternalWindowSize();
    int getInternalPlaylistHeight() const;
    bool wouldInternalPlaylistExceedScreen() const;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};
