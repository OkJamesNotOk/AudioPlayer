/*
  ==============================================================================

    WaveformDisplay.cpp
    Created: 24 Jan 2025 7:54:09pm
    Author:  OkJames

  ==============================================================================
*/

#include <JuceHeader.h>
#include "WaveformDisplay.h"

//==============================================================================
WaveformDisplay::WaveformDisplay(
    AudioFormatManager& formatManagerToUse,
    AudioThumbnailCache& cacheToUse) 
    :
    audioThumb(1000, formatManagerToUse, cacheToUse), 
    fileLoaded(false),
    position(0)
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.
    audioThumb.addChangeListener(this);

}

WaveformDisplay::~WaveformDisplay()
{
}

void WaveformDisplay::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::darkgrey);

    auto drawArea = getLocalBounds();
    auto bounds = drawArea.toFloat();

    g.setColour(juce::Colours::grey);
    g.drawRect(bounds, 1.0f);

    if (fileLoaded)
    {
        g.setColour(juce::Colours::orange);
        audioThumb.drawChannel(g, drawArea, 0.0, audioThumb.getTotalLength(), 0, 1.0f);

        float x = bounds.getX() + (float)position * bounds.getWidth();
        x = juce::jlimit(bounds.getX(), bounds.getRight(), x);

        g.setColour(juce::Colours::purple);

        float lineWidth = juce::jmax(1.0f, getWidth() / 250.0f / 2);
        g.fillRect(x - lineWidth / 2.0f, bounds.getY(), lineWidth, bounds.getHeight());

        juce::Path path;
        float markerHalfWidth = bounds.getWidth() / 90.0f;
        float markerHeight = 10.0f;

        path.addTriangle(
            x - markerHalfWidth, bounds.getY(),
            x + markerHalfWidth, bounds.getY(),
            x, bounds.getY() + markerHeight
        );

        path.addTriangle(
            x - markerHalfWidth, bounds.getBottom(),
            x + markerHalfWidth, bounds.getBottom(),
            x, bounds.getBottom() - markerHeight
        );

        g.fillPath(path);
    }
    else
    {
        g.setColour(juce::Colours::orange);
        g.setFont(20.0f);
        g.drawText("File not loaded", getLocalBounds(), juce::Justification::centred, true);
    }
}

void WaveformDisplay::setPositionRelative(double pos) {

    if (pos != position) {
        repaint();
    }
    if (!isnan(pos)) {
        position = pos;
    }
}

void WaveformDisplay::loadURL(URL audioURL) {
    audioThumb.clear();
    fileLoaded = audioThumb.setSource(new URLInputSource(audioURL));

    if (fileLoaded) {
        std::cout << "wfd: loaded!" << std::endl;
        repaint();
    }
    else {
        std::cout << "wfd: not loaded!" << std::endl;
    }
}

void WaveformDisplay::changeListenerCallback(ChangeBroadcaster* source) {
    std::cout << "wfd: change received!" << std::endl;
    repaint();
}

void WaveformDisplay::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..
}
