/*
  ==============================================================================

    AudioLoopDisplay.cpp
    Created: 26 Feb 2025 9:17:42pm
    Author:  OkJames

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
    //g.drawRect (getLocalBounds(), 1);   // draw an outline around the component

    g.setColour (juce::Colours::white);
    g.setFont (juce::FontOptions (14.0f));

    auto area = getLocalBounds().reduced(5);
    auto drawX = (float)area.getX();
    auto drawY = (float)area.getY();
    auto drawHeight = (float)area.getHeight();
    auto drawWidth = (float)area.getWidth();

    if (loopMarkerStart > 0) {
        // Draw start point of the loop in red
        g.setColour(juce::Colours::red);

        // draw a thin vertical line
        float lineWidth = juce::jmax(1.0f, drawWidth / 250.0f / 2);
        float x = drawX + (float)loopMarkerStart * drawWidth;
        g.fillRect(x - lineWidth / 2.0f, drawY, lineWidth, drawHeight);

        // Draw triangles to increase visibility of the marker
        juce::Path path;
        path.addTriangle(
            x - drawWidth / 45.0f / 2.0f, drawY,
            x + drawWidth / 45.0f / 2.0f, drawY,
            x, drawY + 10.0f
        );
        path.addTriangle(
            x - drawWidth / 45.0f / 2.0f, drawY + drawHeight,
            x + drawWidth / 45.0f / 2.0f, drawY + drawHeight,
            x, drawY + drawHeight - 10.0f
        );
        g.fillPath(path);
    }
    if (loopMarkerEnd < 1) {
        // Draw start point of the loop in colour dark blue
        g.setColour(juce::Colours::darkblue);

        // draw a thin vertical line to indicate the end point of the loop 
        float lineWidth = juce::jmax(1.0f, drawWidth / 250.0f / 2);
        float x = drawX + (float)loopMarkerEnd * drawWidth;
        g.fillRect(x - lineWidth / 2.0f, drawY, lineWidth, drawHeight);

        // Added triangles to increase visibility of the end point marker
        juce::Path path;
        path.addTriangle(
            x - drawWidth / 45.0f / 2.0f, drawY,
            x + drawWidth / 45.0f / 2.0f, drawY,
            x, drawY + 10.0f
        );
        path.addTriangle(
            x - drawWidth / 45.0f / 2.0f, drawY + drawHeight,
            x + drawWidth / 45.0f / 2.0f, drawY + drawHeight,
            x, drawY + drawHeight - 10.0f
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
