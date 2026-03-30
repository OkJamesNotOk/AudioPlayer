/*
  ==============================================================================

    ScrollLabel.h
    Created: 18 Feb 2025 8:53:05pm
    Author:  OkJames

  ==============================================================================
*/

// Make text moves from right to left if lenghth of text is longer than width of text container
#pragma once

#include "JuceHeader.h"

class ScrollLabel : public juce::Label, private juce::Timer
{
public:
    ScrollLabel();

    // Write text onto the label
    void setText(const juce::String& str, juce::NotificationType notif);
    void setScrollingEnabled(bool shouldScroll);

    void paint(juce::Graphics& g) override;
private:
    // width of text
    int width = 0;
    // distance travelled by text
    int distTravelled = 0;
    bool scrollingEnabled = false;
    void timerCallback() override;
};