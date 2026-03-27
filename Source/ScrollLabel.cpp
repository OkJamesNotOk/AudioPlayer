/*
  ==============================================================================

    ScrollLabel.cpp
    Created: 18 Feb 2025 8:53:05pm
    Author:  PhanKien

  ==============================================================================
*/

#include "ScrollLabel.h"

ScrollLabel::ScrollLabel()
{
    startTimerHz(30);
}

void ScrollLabel::setText(const juce::String& str, juce::NotificationType notif)
{
    juce::Label::setText(str, notif);
    width = getFont().getStringWidth(str);
}

void ScrollLabel::timerCallback()
{
    distTravelled -= 2;
    if (-distTravelled > width) {
        distTravelled = getWidth();
    }
}

void ScrollLabel::paint(juce::Graphics& g)
{
    g.setColour(juce::Colours::grey);
    g.drawRect(getLocalBounds(), 1);

    if (width > getWidth()) {
        g.setColour(findColour(juce::Label::textColourId));
        g.setFont(getFont());
        g.drawText(getText(), distTravelled, 0, width, getHeight(), getJustificationType());
        repaint();
    }
    else {
        juce::Label::paint(g);
    }
}
