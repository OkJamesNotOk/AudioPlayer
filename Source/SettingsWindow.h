/*
  ==============================================================================

    SettingsWindow.h
    Created: 30 Mar 2026 2:24:54pm
    Author:  OkJames

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "SettingsComponent.h"

//==============================================================================
/*
*/
class SettingsWindow  : public juce::DocumentWindow
{
public:
    SettingsWindow(SettingsComponent& settingsComponent);
    ~SettingsWindow() override;

    void closeButtonPressed() override
    {
        setVisible(false);
    }

    bool keyPressed(const KeyPress& key);
    void moved() override;
    void visibilityChanged() override;

private:
    void keepWindowOnScreen();
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SettingsWindow)
};
