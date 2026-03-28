/*
  ==============================================================================

    PlaylistLooper.h
    Created: 18 Jan 2025 4:07:59pm
    Author:  OkJames

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "DJAudioPlayer.h"
#include "WaveformDisplay.h"
#include "PlaylistComponent.h"
#include "ScrollLabel.h"
#include "AudioLoopDisplay.h"
#include "CustomLookAndFeelButton.h"
//==============================================================================
/*
*/
class PlaylistLooper : 
    public Component, 
    public Button::Listener, 
    public Slider::Listener, 
    public FileDragAndDropTarget,
    public Timer,
    public DragAndDropTarget
{
public:
    PlaylistLooper(
        DJAudioPlayer* player,
        AudioFormatManager& formatManagerToUse,
        AudioThumbnailCache& cacheToUse,
        PlaylistComponent& _playlistComponent);
    ~PlaylistLooper();

    void paint(juce::Graphics&) override;
    void resized() override;

    void buttonClicked(Button*) override;
    void sliderValueChanged(Slider* slider) override;

    bool isInterestedInFileDrag(const StringArray& file) override;
    void filesDropped(const StringArray& files, int x, int y) override;

    void timerCallback() override;

    // Boolean used to determine if the dragged item is of the correct type from the playlist
    bool isInterestedInDragSource(const SourceDetails& sourceDescriptions) override;

    // Function used to collect information of dragged item and load the item into the player 
    void itemDropped(const SourceDetails& sourceDescriptions) override;

    // Function is used to catch where the mouse is lifted up
    void mouseUp(const MouseEvent& e) override;

    // Function is catch where the mouse is clicked
    void mouseDown(const MouseEvent& e) override;

    // Function used to catch where the mouse is dragging
    void mouseDrag(const MouseEvent& e) override;

    // Function used to change the colour of the volume slider
    void volSliderColour();

    // set text for the label, which is used to display the current track name
    void scrollLabelTextSet(const juce::String& str);

    // Used to set space key to trigger the play button.
    bool keyPressed(const KeyPress& key);

    void restoreSavedState();

    // window thumbnail task
    void taskbarPlayPause();
    void taskbarPrevious();
    void taskbarNext();

private:
    // file chooser, used to add files into the player or the playlist
    FileChooser fChooser{ "Select a file..." };

    void initialiseButtons();
    void initialiseSliders();
    void initialiseButtonGroups();

    std::vector<juce::TextButton*> transportButtons;
    std::vector<juce::TextButton*> loopButtons;
    std::vector<juce::TextButton*> rowButtons;

    TextButton playButton{ ">" };
    TextButton loadButton{ "LOAD" };
    TextButton loopButton{ "LOOP" };
    TextButton prevButton{ "|<<"};
    TextButton nextButton{ ">>|"};

    // used to determine whether the player should loop back to the beginning or the start of 
    // the specified loop when the track is finished or the track has reach the end specified loop
    bool loop = false;

    Slider volSlider;
    Slider speedSlider;
    Slider posSlider;

    DJAudioPlayer* player;

    ScrollLabel songName;

    PlaylistComponent& playlistComponent;

    WaveformDisplay waveformDisplay;

    Colour volColour;

    // used to determine whether the volume slider should change colour
    bool vUp = true;

    // these two boolean are used to determine if the start 
    // or end point of a loop is specified
    bool isStartLoop = false;
    bool isEndLoop = false;

    // these two boolean are used to detemine if the next click 
    // should set the start or end point of a loop
    bool setStart = false;
    bool setEnd = false;

    // Start and end point of a loop
    double loopStart = 0.0;
    double loopEnd = 1.0;

    TextButton loopStartButton{ "START LOOP" };
    TextButton loopEndButton{ "END LOOP" };

    // function used to update where the markers for start and end point of a loop is
    void updateMarkers(const MouseEvent& e);

    // function used to draw  markers for start and end point of a loop where the mouse is 
    void previewMarkers(const MouseEvent& e);

    AudioLoopDisplay loopDisplay;
    juce::File currentFile;

    CustomLookAndFeelButton customButton;

    void persistPlayerState();

    int saveCounter = 0;
    // actual save time = saveInterval * 500 ms
    // e.g. 20 * 500 ms = 10000 ms = 10 seconds
    int saveInterval = 20;

    bool autoplay = true;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlaylistLooper)
};
