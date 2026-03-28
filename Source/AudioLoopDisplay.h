/*
  ==============================================================================

    AudioLoopDisplay.h
    Created: 26 Feb 2025 9:17:42pm
    Author:  OkJames

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
*/
class AudioLoopDisplay  : public juce::Component
{
public:
    AudioLoopDisplay();
    ~AudioLoopDisplay() override;

    void paint (juce::Graphics&) override;
    void resized() override;

    // Draw a marker for the start point of the loop with the input number
    void setMarkerStart(double start);

    // Draw a marker for the end point of the loop with the input number
    void setMarkerEnd(double end);

    // intitial values for the markers
    double loopMarkerStart = 0.0;
    double loopMarkerEnd = 1.0;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioLoopDisplay)
};
