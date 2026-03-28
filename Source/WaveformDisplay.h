/*
  ==============================================================================

    WaveformDisplay.h
    Created: 24 Jan 2025 7:54:09pm
    Author:  OkJames

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
*/
class WaveformDisplay  : 
    public juce::Component,
    public ChangeListener
{
public:
    WaveformDisplay(
        AudioFormatManager& formatManagerToUse,
        AudioThumbnailCache& cacheToUse);
    ~WaveformDisplay() override;

    void paint (juce::Graphics&) override;
    void resized() override;

    void changeListenerCallback(ChangeBroadcaster* source) override;

    void loadURL(URL audioURL);
    void setPositionRelative(double pos);

private:
    double position;
    AudioThumbnail audioThumb;
    bool fileLoaded;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WaveformDisplay)
};
