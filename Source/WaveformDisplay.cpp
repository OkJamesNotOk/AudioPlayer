/*
  ==============================================================================

    WaveformDisplay.cpp
    Created: 24 Jan 2025 7:54:09pm
    Author:  PhanKien

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

void WaveformDisplay::paint (juce::Graphics& g)
{
    /* This demo code just fills the component's background and
       draws some placeholder text to get you started.

       You should replace everything in this method with your own
       drawing code..
    */

    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));   // clear the background
    g.fillAll(juce::Colours::darkgrey);

    g.setColour (juce::Colours::grey);
    g.drawRect (getLocalBounds(), 1);   // draw an outline around the component
    g.setColour(juce::Colours::orange);

    if (fileLoaded) {
        audioThumb.drawChannel(g, getLocalBounds(), 0, audioThumb.getTotalLength(), 0, 1.0f);
        g.setColour(Colours::purple);
        g.fillRect(position * getWidth() - getWidth() / 250 / 2, 0, 0.5f, getHeight());
        juce::Path path;
        path.addTriangle(
            position * getWidth() - getWidth() / 45 / 2, 0,
            position * getWidth() + getWidth() / 45 / 2, 0,
            position * getWidth(), 10
        );
        path.addTriangle(
            position * getWidth() - getWidth() / 45 / 2, getHeight(),
            position * getWidth() + getWidth() / 45 / 2, getHeight(),
            position * getWidth(), getHeight() - 10
        );
        g.fillPath(path);
    }
    else {
        g.setFont(20.0f);
        g.drawText("File not loaded", getLocalBounds(),
            juce::Justification::centred, true);   // draw some placeholder text
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
