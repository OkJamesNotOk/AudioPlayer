/*
  ==============================================================================

    CustomLookAndFeelButton.h
    Created: 27 Mar 2026 2:51:06am
    Author:  PhanKien

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
};
