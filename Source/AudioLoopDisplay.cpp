/*
  ==============================================================================

    AudioLoopDisplay.cpp
    Created: 26 Feb 2025 9:17:42pm
    Author:  PhanKien

  ==============================================================================
*/

#include <JuceHeader.h>
#include "AudioLoopDisplay.h"

//==============================================================================
AudioLoopDisplay::AudioLoopDisplay()
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.

}

AudioLoopDisplay::~AudioLoopDisplay()
{
}

void AudioLoopDisplay::paint (juce::Graphics& g)
{
    /* This demo code just fills the component's background and
       draws some placeholder text to get you started.

       You should replace everything in this method with your own
       drawing code..
    */

    g.fillAll (juce::Colours::transparentBlack);   // clear the background

    g.setColour (juce::Colours::grey);
    g.drawRect (getLocalBounds(), 1);   // draw an outline around the component

    g.setColour (juce::Colours::white);
    g.setFont (juce::FontOptions (14.0f));

    if (loopMarkerStart > 0) {
        // Draw start point of the loop in red
        g.setColour(juce::Colours::red);

        // draw a thin vertical line
        g.fillRect(loopMarkerStart * getWidth() - getWidth() / 250 / 2, 0, getWidth() / 250, getHeight());

        // Draw triangles to increase visibility of the marker
        juce::Path path;
        path.addTriangle(
            loopMarkerStart * getWidth() - getWidth() / 45 / 2, 0,
            loopMarkerStart * getWidth() + getWidth() / 45 / 2, 0,
            loopMarkerStart * getWidth(), 10
        );
        path.addTriangle(
            loopMarkerStart * getWidth() - getWidth() / 45 / 2, getHeight(),
            loopMarkerStart * getWidth() + getWidth() / 45 / 2, getHeight(),
            loopMarkerStart * getWidth(), getHeight() - 10
        );
        g.fillPath(path);
    }
    if (loopMarkerEnd < 1) {
        // Draw start point of the loop in colour dark blue
        g.setColour(juce::Colours::darkblue);

        // draw a thin vertical line to indicate the end point of the loop 
        g.fillRect(loopMarkerEnd * getWidth() - getWidth() / 250 / 2, 0, getWidth() / 250, getHeight());

        // Added triangles to increase visibility of the end point marker
        juce::Path path;
        path.addTriangle(
            loopMarkerEnd * getWidth() - getWidth() / 45 / 2, 0,
            loopMarkerEnd * getWidth() + getWidth() / 45 / 2, 0,
            loopMarkerEnd * getWidth(), 10
        );
        path.addTriangle(
            loopMarkerEnd * getWidth() - getWidth() / 45 / 2, getHeight(),
            loopMarkerEnd * getWidth() + getWidth() / 45 / 2, getHeight(),
            loopMarkerEnd * getWidth(), getHeight() - 10
        );
        g.fillPath(path);
    }
}

// Get a number from caller and assign it to loopMarkerStart 
void AudioLoopDisplay::setMarkerStart(double start)
{
    loopMarkerStart = start;
    repaint();
}

// Get a number from caller and assign it to loopMarkerEnd 
void AudioLoopDisplay::setMarkerEnd(double end)
{
    loopMarkerEnd = end;
    repaint();
}

void AudioLoopDisplay::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..

}
