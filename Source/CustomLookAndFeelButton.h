/*
  ==============================================================================

    CustomLookAndFeelButton.h
    Created: 27 Mar 2026 2:51:06am
    Author:  OkJames

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class CustomLookAndFeelButton : public juce::LookAndFeel_V4
{
public:
    CustomLookAndFeelButton();
    ~CustomLookAndFeelButton() override; 

    juce::Font getTextButtonFont(juce::TextButton& button, int buttonHeight) override;
    juce::Font getLabelFont(juce::Label& label) override;
    juce::Font getSliderPopupFont(juce::Slider& slider) override;

    void drawButtonBackground(juce::Graphics& g,
        juce::Button& button,
        const juce::Colour& backgroundColour,
        bool shouldDrawButtonAsHighlighted,
        bool shouldDrawButtonAsDown) override;
};
