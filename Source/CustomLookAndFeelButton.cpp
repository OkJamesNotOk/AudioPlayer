/*
  ==============================================================================

    CustomLookAndFeelButton.cpp
    Created: 27 Mar 2026 2:51:06am
    Author:  OkJames

  ==============================================================================
*/

#include <JuceHeader.h>
#include "CustomLookAndFeelButton.h"

CustomLookAndFeelButton::CustomLookAndFeelButton()
{
    setColour(juce::TextButton::buttonColourId, juce::Colours::darkgrey);
    setColour(juce::TextButton::textColourOnId, juce::Colours::white);
    setColour(juce::TextButton::textColourOffId, juce::Colours::lightgrey);
}

CustomLookAndFeelButton::~CustomLookAndFeelButton() = default;

// set dynamic scaling using width and height of buttons
juce::Font CustomLookAndFeelButton::getTextButtonFont(juce::TextButton& button, int buttonHeight)
{
    const float w = (float)button.getWidth();
    const float h = (float)buttonHeight;

    const float hybridSize = (w * 0.12f) + (h * 0.22f);
    const float clampedSize = juce::jlimit(12.0f, 24.0f, hybridSize);

    return juce::Font(clampedSize);
}

// set dynamic scaling using the higher value between width and height for sliders
juce::Font CustomLookAndFeelButton::getLabelFont(juce::Label& label)
{
    const float w = (float)label.getWidth();
    const float h = (float)label.getHeight();

    const float base = std::max(w, h) * 0.07f;

    return juce::Font(juce::jlimit(3.0f, 24.0f, base));
}

juce::Font CustomLookAndFeelButton::getSliderPopupFont(juce::Slider& slider)
{
    const float w = (float)slider.getWidth();
    const float h = (float)slider.getHeight();

    const float hybridSize = w * 0.1f + h * 0.25f;
    const float clampedSize = juce::jlimit(11.0f, 20.0f, hybridSize);

    return juce::Font(clampedSize);
}

void CustomLookAndFeelButton::drawButtonBackground(juce::Graphics& g,
    juce::Button& button,
    const juce::Colour& backgroundColour,
    bool shouldDrawButtonAsHighlighted,
    bool shouldDrawButtonAsDown)
{
    auto bounds = button.getLocalBounds().toFloat().reduced(1.0f);

    float cornerSize = 18.0f;

    auto colour = backgroundColour;

    if (shouldDrawButtonAsDown)
        colour = colour.darker(0.2f);
    else if (shouldDrawButtonAsHighlighted)
        colour = colour.brighter(0.1f);

    g.setColour(colour);
    g.fillRoundedRectangle(bounds, cornerSize);

    g.setColour(juce::Colours::black);
    g.drawRoundedRectangle(bounds, cornerSize, 0.5f);
}
