/*
  ==============================================================================

    SettingsWindow.cpp
    Created: 30 Mar 2026 2:24:54pm
    Author:  OkJames

  ==============================================================================
*/

#include <JuceHeader.h>
#include "SettingsWindow.h"

//==============================================================================
SettingsWindow::SettingsWindow(SettingsComponent& settingsComponent)
    : juce::DocumentWindow("Settings",
        juce::Colours::lightgrey,
        juce::DocumentWindow::allButtons)
{
    setUsingNativeTitleBar(true);
    setResizable(true, true);
    setResizeLimits(260, 140, 600, 500);
    setContentNonOwned(&settingsComponent, false);

    const int windowWidth = 300;
    const int windowHeight = settingsComponent.getSettingsHeight();
    centreWithSize(windowWidth, windowHeight);

    keepWindowOnScreen();
}

SettingsWindow::~SettingsWindow()
{
}

bool SettingsWindow::keyPressed(const juce::KeyPress& key)
{
    if (key == juce::KeyPress::escapeKey)
    {
        setVisible(false);
        return true;
    }

    return false;
}

void SettingsWindow::keepWindowOnScreen()
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

void SettingsWindow::moved()
{
    keepWindowOnScreen();
}

void SettingsWindow::visibilityChanged()
{
    if (isVisible())
        keepWindowOnScreen();
}
