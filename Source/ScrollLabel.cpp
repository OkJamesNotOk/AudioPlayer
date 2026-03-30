/*
  ==============================================================================

    ScrollLabel.cpp
    Created: 18 Feb 2025 8:53:05pm
    Author:  OkJames
    auto scrolling label

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
    distTravelled = 2;
}

void ScrollLabel::timerCallback()
{
    if (!scrollingEnabled || width <= getWidth())
        return;

    distTravelled -= 2;
    if (-distTravelled > width) {
        distTravelled = getWidth();
    }

    repaint();
}

void ScrollLabel::paint(juce::Graphics& g)
{
    g.setColour(juce::Colours::grey);
    //g.drawRect(getLocalBounds().reduced(1), 1);

    if (width > getWidth()) {
        g.setColour(findColour(juce::Label::textColourId));
        g.setFont(getFont());
        g.drawText(getText(), distTravelled, 0, width, getHeight(), getJustificationType());
        //repaint();
    }
    else {
        juce::Label::paint(g);
    }
}

void ScrollLabel::setScrollingEnabled(bool shouldScroll)
{
    scrollingEnabled = shouldScroll;
    repaint();
}