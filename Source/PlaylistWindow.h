/*
  ==============================================================================

    PlaylistWindow.h
    Created: 30 Mar 2026 1:35:21am
    Author:  OkJames

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PlaylistComponent.h"

//==============================================================================
/*
*/
class PlaylistWindow : public juce::DocumentWindow
{
public:
    PlaylistWindow(PlaylistComponent& playlist);
    ~PlaylistWindow() override;

    void closeButtonPressed() override
    {
        setVisible(false);
    }

    bool keyPressed(const KeyPress& key);
    void moved() override;
    void visibilityChanged() override;

private:
    void keepWindowOnScreen();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PlaylistWindow)
};
