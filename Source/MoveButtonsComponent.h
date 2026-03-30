/*
  ==============================================================================

    MoveButtonsComponent.h
    Created: 29 Mar 2026 9:28:57pm
    Author:  OkJames

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PlaylistComponent.h"

//==============================================================================
/*
*/
class PlaylistComponent;

class MoveButtonsComponent : public juce::Component
{
public:
    MoveButtonsComponent(PlaylistComponent& owner,
        juce::Drawable* upImage,
        juce::Drawable* downImage);
    ~MoveButtonsComponent() override;

    void paint(juce::Graphics&) override;
    void resized() override;
    void setRowFile(const juce::File& file);

private:
    PlaylistComponent& parent;
    juce::DrawableButton upBtn;
    juce::DrawableButton downBtn;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MoveButtonsComponent)
};
