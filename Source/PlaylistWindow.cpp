/*
  ==============================================================================

    PlaylistWindow.cpp
    Created: 30 Mar 2026 1:35:21am
    Author:  OkJames

  ==============================================================================
*/

#include <JuceHeader.h>
#include "PlaylistWindow.h"

//==============================================================================
PlaylistWindow::PlaylistWindow(PlaylistComponent& playlist)
    : juce::DocumentWindow("Playlist",
        juce::Colours::lightgrey,
        juce::DocumentWindow::allButtons)
{
    setUsingNativeTitleBar(true);
    setResizable(true, true);
    setResizeLimits(300, 250, 1000, 800);
    setContentNonOwned(&playlist, false);
    centreWithSize(300, 400);
    keepWindowOnScreen();
}

bool PlaylistWindow::keyPressed(const KeyPress& key) {
    //escape key close the playlist window
    if (key == juce::KeyPress::escapeKey) {
        setVisible(false);
        return true;
    }
    return false;
}

PlaylistWindow::~PlaylistWindow()
{
}

void PlaylistWindow::keepWindowOnScreen()
{
    auto area = juce::Desktop::getInstance()
        .getDisplays()
        .getPrimaryDisplay()->userArea;

    auto bounds = getBounds();

    if (bounds.getWidth() > area.getWidth())
        bounds.setWidth(area.getWidth());

    if (bounds.getHeight() > area.getHeight())
        bounds.setHeight(area.getHeight());

    bounds.setX(juce::jlimit(area.getX(),
        area.getRight() - bounds.getWidth(),
        bounds.getX()));

    bounds.setY(juce::jlimit(area.getY(),
        area.getBottom() - bounds.getHeight(),
        bounds.getY()));

    setBounds(bounds);
}

void PlaylistWindow::moved()
{
    keepWindowOnScreen();
}

void PlaylistWindow::visibilityChanged()
{
    if (isVisible())
        keepWindowOnScreen();
}