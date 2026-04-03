/*
  ==============================================================================

    PlaylistComponent.h
    Created: 8 Feb 2025 8:40:10am
    Author:  OkJames

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include <vector>
#include <string>
#include "ImageHelper.h"
#include "MoveButtonsComponent.h"
#include "ScrollLabel.h"
#include "FFmpegAudioFormat.h"

//==============================================================================
/*
*/
class PlaylistComponent  : 
    public juce::Component, 
    public TableListBoxModel,
    public Button::Listener,
    public FileDragAndDropTarget,
    public DragAndDropContainer
{
public:
    PlaylistComponent();
    ~PlaylistComponent() override;

    void paint (juce::Graphics&) override;
    void resized() override;

    int getNumRows() override;

    void paintRowBackground(Graphics& g, int rowNumber, int width, int height, bool rowIsSelected) override;

    void paintCell(Graphics& g, int rowNumber, int columnId, int width, int height, bool rowIsSelected) override;

    Component* refreshComponentForCell(int rowNumber, int columnId, bool isRowSelected, Component* existingComponentToUpdate) override;

    void buttonClicked(Button* button) override;

    bool isInterestedInFileDrag(const StringArray& files) override;

    void filesDropped(const StringArray& files, int x, int y) override;

    // Function used to enable dragging of playlist items to the DeckGUIs
    void mouseDrag(const MouseEvent& event) override;

    // used to change selected row of the playlist
    void listBoxItemClicked(int row, const MouseEvent& e);

    // return the path name of the dragged item from the playlist
    var getDragSourceDescription(const SparseSet<int>& selectedRow) override;

    // return duration of input file
    double getTrackDuration(const File& file);

    double getOrCacheTrackDuration(const juce::File& file);

    // function used to add input file into the vector trackTitles
    void addFileToPlaylist(const File& file);

    // function used to check if input file already existed in vector trackTitles
    bool checkFileDuplicate(const File& file);

    // next track on the full playlist
    juce::File getNextTrack(juce::File currentFile);

    // previous track on full playlist
    juce::File getPreviousTrack(juce::File currentFile);

    static bool isSupportedAudioFile(const juce::File& file);

    // player state
    struct PlayerState
    {
        juce::File currentFile;
        double position = 0.0;
        double volume = 0.7;
        double speed = 1.0;
        double loopStart = 0.0;
        double loopEnd = 1.0;
        bool loopEnabled = false;
    };

    void savePlayerState(const PlayerState& state);
    bool loadPlayerState(PlayerState& state) const;

    // Functions to save and load and clear playlist
    void savePlaylist();
    void loadPlaylist();
    void clearAllPlaylist();

    void setCurrentPlayingFile(const juce::File& file);

    bool moveTrackUp(const juce::File& file);
    bool moveTrackDown(const juce::File& file);

    std::unique_ptr<juce::Drawable> trashDrawable;
    std::unique_ptr<juce::Drawable> upDrawable;
    std::unique_ptr<juce::Drawable> downDrawable;

    void cellClicked(int rowNumber, int columnId, const juce::MouseEvent& event) override;
    int currentRow = -1;

private:
    TableListBox tableComponent;

    struct TrackInfo
    {
        juce::File file;
        double duration = 0.0;
    };

    // vector containing files which makes up the playlist
    std::vector<TrackInfo> trackTitles;

    // vector containing the tracks that match the filter
    std::vector<TrackInfo> filteredTrackTitles;

    // Search bar for looking up specific track in the playlist
    juce::TextEditor searchBar;

    // filter the tracks, update the vector filteredTrackTitles
    void updateFilter();

    // file chooser for selecting and loading files into playlist
    FileChooser fChooser{ "Select a file..." };
    juce::TextButton addFile{ "Add File" };

    juce::TextButton saveButton{ "Save Playlist" };

    juce::TextButton clearPlaylist{ "Clear Playlist" };

    juce::File currentPlayingFile;

    int minColW = 40;
    int minDurationColW = 50;
    int minOrderColW = 15;
    int minDelColW = 25;

    juce::File getDataFile(const juce::String& fileName) const;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PlaylistComponent)
};
