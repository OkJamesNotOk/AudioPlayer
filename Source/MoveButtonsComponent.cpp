/*
  ==============================================================================

    MoveButtonsComponent.cpp
    Created: 29 Mar 2026 9:28:57pm
    Author:  OkJames

  ==============================================================================
*/

#include <JuceHeader.h>
#include "MoveButtonsComponent.h"
#include "PlaylistComponent.h"

//==============================================================================
MoveButtonsComponent::MoveButtonsComponent(PlaylistComponent& owner,
    juce::Drawable* upImage,
    juce::Drawable* downImage)
    : parent(owner),
    upBtn("Up", juce::DrawableButton::ImageOnButtonBackground),
    downBtn("Down", juce::DrawableButton::ImageOnButtonBackground)
{
    upBtn.setImages(upImage);
    downBtn.setImages(downImage);

    upBtn.addListener(&parent);
    downBtn.addListener(&parent);

    upBtn.setColour(juce::TextButton::buttonColourId, juce::Colour::fromString("#ffa0dcff"));

    downBtn.setColour(juce::TextButton::buttonColourId, juce::Colour::fromString("#ffffc3a0"));

    addAndMakeVisible(upBtn);
    addAndMakeVisible(downBtn);
}

MoveButtonsComponent::~MoveButtonsComponent()
{
}

void MoveButtonsComponent::paint (juce::Graphics& g)
{

}

void MoveButtonsComponent::resized()
{
    auto area = getLocalBounds();
    auto topHalf = area.removeFromTop(area.getHeight() / 2);

    upBtn.setBounds(topHalf);
    downBtn.setBounds(area);
}

void MoveButtonsComponent::setRowFile(const juce::File& file)
{
    upBtn.setComponentID("Up:" + file.getFullPathName());
    downBtn.setComponentID("Down:" + file.getFullPathName());
}