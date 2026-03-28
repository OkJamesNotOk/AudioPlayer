/*
  ==============================================================================

    PlaylistComponent.cpp
    Created: 8 Feb 2025 8:40:10am
    Author:  OkJames

  ==============================================================================
*/

#include <JuceHeader.h>
#include "PlaylistComponent.h"

//==============================================================================
PlaylistComponent::PlaylistComponent()
{
    tableComponent.getHeader().addColumn("Track Title", 1, 200, 80, -1, juce::TableHeaderComponent::defaultFlags);
    tableComponent.getHeader().addColumn("Duration", 2, 80, minDurationColW, -1, juce::TableHeaderComponent::defaultFlags);
    tableComponent.getHeader().addColumn("Up", 3, 60, minOrderColW, -1, juce::TableHeaderComponent::defaultFlags);
    tableComponent.getHeader().addColumn("Dn", 4, 60, minOrderColW, -1, juce::TableHeaderComponent::defaultFlags);
    tableComponent.getHeader().addColumn("", 5, 100, minColW, -1, juce::TableHeaderComponent::defaultFlags);

    tableComponent.setModel(this);
    tableComponent.setColour(juce::TableListBox::backgroundColourId, juce::Colour::fromString("#FF444444"));

    tableComponent.setInterceptsMouseClicks(false, true);
    setInterceptsMouseClicks(true, true);

    addAndMakeVisible(tableComponent);

    searchBar.setMultiLine(false);
    searchBar.setJustification(juce::Justification::centredLeft);
    searchBar.setTextToShowWhenEmpty("Enter track name...", Colours::black);
    searchBar.onTextChange = [this] { updateFilter(); };
    searchBar.setColour(juce::TextEditor::backgroundColourId, juce::Colour::fromString("#FF9fc5e8"));
    searchBar.setColour(juce::TextEditor::textColourId, juce::Colours::black);

    addAndMakeVisible(searchBar);

    filteredTrackTitles = trackTitles;

    addAndMakeVisible(addFile);
    addFile.addListener(this);
    addFile.setColour(juce::TextButton::buttonColourId, juce::Colour::fromString("#FFFFBE48"));
    addFile.setColour(juce::TextButton::textColourOffId, juce::Colours::black);

    addAndMakeVisible(saveButton);
    saveButton.addListener(this);
    saveButton.setColour(juce::TextButton::buttonColourId, juce::Colour::fromString("#FFFFBE48"));
    saveButton.setColour(juce::TextButton::textColourOffId, juce::Colours::black);

    addAndMakeVisible(clearPlaylist);
    clearPlaylist.addListener(this);
    clearPlaylist.setColour(juce::TextButton::buttonColourId, juce::Colour::fromString("#FF960404"));
    clearPlaylist.setColour(juce::TextButton::textColourOffId, juce::Colours::black);

    loadPlaylist();
}

int PlaylistComponent::getNumRows() {
    return filteredTrackTitles.size();
}

void PlaylistComponent::paintRowBackground(Graphics& g, int rowNumber, int width, int height, bool rowIsSelected) {
    auto alternateColour = getLookAndFeel().findColour(ListBox::backgroundColourId)
        .interpolatedWith(getLookAndFeel().findColour(ListBox::textColourId), 0.03f);
    if (rowNumber >= 0 && rowNumber < filteredTrackTitles.size()
        && filteredTrackTitles[rowNumber] == currentPlayingFile)
    {
        g.fillAll(juce::Colours::orange); // current playing row colour
    }
    else if (rowIsSelected)
        g.fillAll(Colours::lightblue);
    else if (rowNumber % 2)
        g.fillAll(alternateColour);
    else 
    {
        g.fillAll(Colours::darkgrey);
    }
}

void PlaylistComponent::setCurrentPlayingFile(const juce::File& file)
{
    currentPlayingFile = file;
    tableComponent.repaint();
}

void PlaylistComponent::paintCell(Graphics& g, int rowNumber, int columnId, int width, int height, bool rowIsSelected) {
    if(columnId ==1){
        g.setColour(juce::Colour::fromString("#FF457B9D"));
        g.drawText(filteredTrackTitles[rowNumber].getFileName(), 2, 0, width - 4, height, Justification::centredLeft, true);
    }
}

PlaylistComponent::~PlaylistComponent()
{
    tableComponent.setModel(nullptr);
}

void PlaylistComponent::paint (juce::Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));   // clear the background

    g.setColour (juce::Colours::grey);
    g.drawRect (getLocalBounds(), 1);   // draw an outline around the component

    g.setColour (juce::Colours::white);
    g.setFont (juce::FontOptions (14.0f));
    g.drawText ("PlaylistComponent", getLocalBounds(),
                juce::Justification::centred, true);   // draw some placeholder text
}

void PlaylistComponent::resized()
{
    auto area = getLocalBounds().reduced(4);

    const int topHeight = getHeight() / 12;
    const int gap = 4;

    searchBar.setBounds(area.removeFromTop(topHeight));
    area.removeFromTop(gap);

    auto buttonArea = area.removeFromTop(topHeight);
    auto thirdW = (buttonArea.getWidth() - gap) / 3;

    addFile.setBounds(buttonArea.removeFromLeft(thirdW));
    buttonArea.removeFromLeft(gap);
    saveButton.setBounds(buttonArea.removeFromLeft(thirdW));
    buttonArea.removeFromLeft(gap);
    clearPlaylist.setBounds(buttonArea);

    area.removeFromTop(gap);

    tableComponent.setBounds(area);

    int scrollbarWidth = 18;

    if (auto* vp = tableComponent.getViewport())
    {
        vp->setScrollBarsShown(true, false);
        scrollbarWidth = vp->getScrollBarThickness();
    }

    const int tableWidth = tableComponent.getWidth() - scrollbarWidth - 4;
    const int segment = 20;    

    const int colW = juce::jmax((tableWidth / segment), minDurationColW);
    const int colWhalf = juce::jmax((colW / 2), minOrderColW);
    const int titleWidth = juce::jmax(50, tableWidth - colW * 2 - colWhalf * 2);

    tableComponent.getHeader().setColumnWidth(1, titleWidth);
    tableComponent.getHeader().setColumnWidth(2, colW);
    tableComponent.getHeader().setColumnWidth(3, colWhalf);
    tableComponent.getHeader().setColumnWidth(4, colWhalf);
    tableComponent.getHeader().setColumnWidth(5, colW);
}

Component* PlaylistComponent::refreshComponentForCell(int rowNumber, int columnId, bool isRowSelected, Component* existingComponentToUpdate) {
    if (columnId == 2) {
        juce::Label* durationLabel = dynamic_cast<juce::Label*>(existingComponentToUpdate);
        if (existingComponentToUpdate == nullptr) {
            durationLabel = new juce::Label();
            juce::File file(filteredTrackTitles[rowNumber]);

            double duration = getTrackDuration(file);
            juce::String durationText = duration >= 3600
                ? juce::String::formatted("%02d:%02d:%02d", (int)(duration / 3600), (int)(duration / 60) % 60, (int)duration % 60)
                : juce::String::formatted("%02d:%02d", (int)(duration / 60), (int)(duration) % 60);

            durationLabel->setText(durationText, juce::NotificationType::dontSendNotification);
            durationLabel->setColour(juce::Label::textColourId, juce::Colour::fromString("#FFAFAFDC"));
            durationLabel->setJustificationType(juce::Justification::centredLeft);
            durationLabel->setFont(juce::Font(14.0f));

            existingComponentToUpdate = durationLabel;
        }
        else {
            juce::File file(filteredTrackTitles[rowNumber]);
            double duration = getTrackDuration(file);
            juce::String durationText = duration >= 3600
                ? juce::String::formatted("%02d:%02d:%02d", (int)(duration / 3600), (int)(duration / 60) % 60, (int)duration % 60)
                : juce::String::formatted("%02d:%02d", (int)(duration / 60), (int)(duration) % 60);

            durationLabel->setText(durationText, juce::NotificationType::dontSendNotification);
        }
    }
    if (columnId == 3) {
        auto* upBtn = dynamic_cast<juce::TextButton*>(existingComponentToUpdate);
        if (existingComponentToUpdate == nullptr) {
            upBtn = new juce::TextButton("/\\");
            upBtn->addListener(this);
            upBtn->setColour(juce::TextButton::buttonColourId, juce::Colour::fromString("#FFFFBE48"));
            upBtn->setColour(juce::TextButton::textColourOffId, juce::Colours::black);
            existingComponentToUpdate = upBtn;
        }

        // append full path name to ID because they are all unique
        upBtn->setComponentID("Up:" + filteredTrackTitles[rowNumber].getFullPathName());
    }
    if (columnId == 4) {
        auto* downBtn = dynamic_cast<juce::TextButton*>(existingComponentToUpdate);
        if (existingComponentToUpdate == nullptr) {
            downBtn = new juce::TextButton("\\/");
            downBtn->addListener(this);
            downBtn->setColour(juce::TextButton::buttonColourId, juce::Colour::fromString("#FFFFBE48"));
            downBtn->setColour(juce::TextButton::textColourOffId, juce::Colours::black);
            existingComponentToUpdate = downBtn;
        }

        // append full path name to ID because they are all unique
        downBtn->setComponentID("Down:" + filteredTrackTitles[rowNumber].getFullPathName());
    }
    if (columnId == 5) {
        auto* deleteBtn = dynamic_cast<juce::TextButton*>(existingComponentToUpdate);
        if (existingComponentToUpdate == nullptr) {
            deleteBtn = new TextButton("Delete");
            deleteBtn->addListener(this);
            deleteBtn->setColour(juce::TextButton::buttonColourId, juce::Colour::fromString("#FFAFAFDC"));
            deleteBtn->setColour(juce::TextButton::textColourOffId, juce::Colours::black);
            existingComponentToUpdate = deleteBtn;
        }
        // using full path name as ID because they are all unique
        deleteBtn->setComponentID(filteredTrackTitles[rowNumber].getFullPathName());
    }
    return existingComponentToUpdate;
}

bool PlaylistComponent::moveTrackUp(const juce::File& file)
{
    for (size_t i = 1; i < trackTitles.size(); ++i)
    {
        if (trackTitles[i] == file)
        {
            std::swap(trackTitles[i], trackTitles[i - 1]);
            updateFilter();
            tableComponent.updateContent();
            tableComponent.repaint();
            return true;
        }
    }
    return false;
}

bool PlaylistComponent::moveTrackDown(const juce::File& file)
{
    for (size_t i = 0; i + 1 < trackTitles.size(); ++i)
    {
        if (trackTitles[i] == file)
        {
            std::swap(trackTitles[i], trackTitles[i + 1]);
            updateFilter();
            tableComponent.updateContent();
            tableComponent.repaint();
            return true;
        }
    }
    return false;
}

void PlaylistComponent::buttonClicked(Button* button) {
    if (button == &addFile) {
        auto fileChooserFlags =
            FileBrowserComponent::canSelectMultipleItems;
        fChooser.launchAsync(fileChooserFlags, [this](const FileChooser& chooser)
            {
                auto chosenFiles = chooser.getResults();

                if (chosenFiles.size() > 0) {
                    for (int i = 0; i < chosenFiles.size(); i++) {
                        File file = chosenFiles[i];
                        if (file.hasFileExtension(".mp3;.wav;.flac")) {
                            addFileToPlaylist(file);
                        }
                    }
                }
            });
    }
    if (button == &saveButton) {
        savePlaylist();
    }
    if (button == &clearPlaylist) {
        clearAllPlaylist();
    }
    else {
        //juce::String toBeDelete = button->getComponentID();
        //for(auto i = trackTitles.begin(); i != trackTitles.end(); ++i){
        //    if (i->getFullPathName() == toBeDelete) {
        //        trackTitles.erase(i);
        //        updateFilter();
        //        tableComponent.updateContent();
        //        tableComponent.repaint();
        //        break;
        //    }

        juce::String id = button->getComponentID();

        if (id.startsWith("Up:"))
        {
            juce::File file(id.fromFirstOccurrenceOf("Up:", false, false));
            moveTrackUp(file);
        }
        else if (id.startsWith("Down:"))
        {
            juce::File file(id.fromFirstOccurrenceOf("Down:", false, false));
            moveTrackDown(file);
        }
        else{
            for (auto i = trackTitles.begin(); i != trackTitles.end(); ++i)
            {
                if (i->getFullPathName() == id)
                {
                    trackTitles.erase(i);
                    updateFilter();
                    tableComponent.updateContent();
                    tableComponent.repaint();
                    break;
                }
            }
        }
    }
}

// text based save file
//void PlaylistComponent::savePlaylist() {
//    juce::File save =
//        juce::File::getSpecialLocation(juce::File::currentExecutableFile)
//        .getParentDirectory()
//        .getChildFile("playlist.txt");
//
//    juce::FileOutputStream output(save);
//
//    if (output.openedOk()) {
//        output.setPosition(0);
//        output.truncate();
//
//        for (auto& file : trackTitles) {
//            output.writeText(file.getFullPathName() + "\n", false, false, nullptr);
//        }
//        output.flush();
//    }
//}

//void PlaylistComponent::loadPlaylist() {
//    juce::File load = 
//        juce::File::getSpecialLocation(juce::File::currentExecutableFile)
//        .getParentDirectory()
//        .getChildFile("playlist.txt");
//
//    if (load.existsAsFile()) {
//        trackTitles.clear();
//
//        juce::FileInputStream input(load);
//        if (input.openedOk()) {
//            while (!input.isExhausted()) {
//                juce::String file = input.readNextLine();
//                juce::File track(file);
//
//                if (track.existsAsFile()) {
//                    if (!checkFileDuplicate(track)) {
//                        trackTitles.push_back(file);
//                    }
//                }
//            }
//        }
//        updateFilter();
//        tableComponent.updateContent();
//    }
//}

// json based save file
void PlaylistComponent::savePlaylist()
{
    juce::Array<juce::var> playlistArray;

    for (const auto& file : trackTitles)
        playlistArray.add(file.getFullPathName());

    juce::File saveFile =
        juce::File::getSpecialLocation(juce::File::currentExecutableFile)
        .getParentDirectory()
        .getChildFile("playlist.json");

    saveFile.replaceWithText(juce::JSON::toString(juce::var(playlistArray), true));
}

void PlaylistComponent::loadPlaylist()
{
    juce::File loadFile =
        juce::File::getSpecialLocation(juce::File::currentExecutableFile)
        .getParentDirectory()
        .getChildFile("playlist.json");

    if (!loadFile.existsAsFile())
        return;

    juce::var parsed = juce::JSON::parse(loadFile);

    if (!parsed.isArray())
        return;

    trackTitles.clear();

    auto* arr = parsed.getArray();
    for (const auto& item : *arr)
    {
        juce::File track(item.toString());

        if (track.existsAsFile() && !checkFileDuplicate(track))
            trackTitles.push_back(track);
    }

    updateFilter();
    tableComponent.updateContent();
    tableComponent.repaint();
}

void PlaylistComponent::clearAllPlaylist() {
    trackTitles.clear();
    filteredTrackTitles.clear();
    tableComponent.deselectAllRows();
    tableComponent.updateContent();
    tableComponent.repaint();
}

void PlaylistComponent::savePlayerState(const PlayerState& state)
{
    juce::DynamicObject::Ptr root = new juce::DynamicObject();

    root->setProperty("currentFile", state.currentFile.getFullPathName());
    root->setProperty("position", state.position);
    root->setProperty("volume", state.volume);
    root->setProperty("speed", state.speed);
    root->setProperty("loopStart", state.loopStart);
    root->setProperty("loopEnd", state.loopEnd);
    root->setProperty("loopEnabled", state.loopEnabled);

    juce::File saveFile =
        juce::File::getSpecialLocation(juce::File::currentExecutableFile)
        .getParentDirectory()
        .getChildFile("playlistplayer.json");

    saveFile.replaceWithText(juce::JSON::toString(juce::var(root.get()), true));
}

//save player state
bool PlaylistComponent::loadPlayerState(PlayerState& state) const
{
    juce::File loadFile =
        juce::File::getSpecialLocation(juce::File::currentExecutableFile)
        .getParentDirectory()
        .getChildFile("playlistplayer.json");

    if (!loadFile.existsAsFile())
        return false;

    juce::var parsed = juce::JSON::parse(loadFile);
    auto* obj = parsed.getDynamicObject();

    if (obj == nullptr)
        return false;

    state.currentFile = juce::File(obj->getProperty("currentFile").toString());
    state.position = static_cast<double>(obj->getProperty("position"));
    state.volume = static_cast<double>(obj->getProperty("volume"));
    state.speed = static_cast<double>(obj->getProperty("speed"));
    state.loopStart = static_cast<double>(obj->getProperty("loopStart"));
    state.loopEnd = static_cast<double>(obj->getProperty("loopEnd"));
    state.loopEnabled = static_cast<bool>(obj->getProperty("loopEnabled"));

    return true;
}

juce::File PlaylistComponent::getNextTrack(juce::File currentFile)
{
    for (size_t i = 0; i < trackTitles.size(); ++i)
    {
        if (trackTitles[i] == currentFile)
        {
            // Calculate next index (wrap around to 0 if at the end)
            int nextIndex = (i + 1) % trackTitles.size();
            return trackTitles[nextIndex];
        }
    }
    // If file not found or playlist empty, return the first track if available
    return trackTitles.size() > 0 ? trackTitles[0] : juce::File{};
}

juce::File PlaylistComponent::getPreviousTrack(juce::File currentFile)
{
    for (size_t i = 0; i < trackTitles.size(); ++i)
    {
        if (trackTitles[i] == currentFile)
        {
            // Calculate next index (wrap around to 0 if at the end)
            int previousIndex = (int(i) - 1 + (int)trackTitles.size()) % (int)trackTitles.size();
            return trackTitles[previousIndex];
        }
    }
    // If file not found or playlist empty, return the first track if available
    return trackTitles.size() > 0 ? trackTitles[0] : juce::File{};
}

bool PlaylistComponent::isInterestedInFileDrag(const StringArray& files) {
    for (auto& file : files) {
        if (file.endsWith(".mp3") || file.endsWith(".wav") || file.endsWith(".flac")) {
            return true;
        }
    }
    return false;
}

void PlaylistComponent::filesDropped(const StringArray& files, int x, int y) {
    for (auto& file : files) {
        if (file.endsWith(".mp3") || file.endsWith(".wav") || file.endsWith(".flac")) {
            if (!checkFileDuplicate(file)) {
                trackTitles.push_back(file);
                updateFilter();
            }
        }
    }
    tableComponent.updateContent();
}

void PlaylistComponent::addFileToPlaylist(const File& file) {
    if (!checkFileDuplicate(file)) {
        trackTitles.push_back(file);
        updateFilter();
        tableComponent.updateContent();
    }
}

bool PlaylistComponent::checkFileDuplicate(const File& file) {
    // empty track list always return false
    if (trackTitles.empty()) {
        return false;
    }

    //check if there is a matching track already in the list
    for (int i = 0; i < trackTitles.size(); i++) {
        if (file.getFullPathName() == trackTitles[i].getFullPathName()) {
            return true;
        }
    }
    
    return false;
}

void PlaylistComponent::listBoxItemClicked(int row, const MouseEvent& e) {
    if (row >= 0 && row < filteredTrackTitles.size()) {
        tableComponent.selectRow(row);
    }
}

var PlaylistComponent::getDragSourceDescription(const SparseSet<int>& selectedRow) {
    if (selectedRow.size() > 0) {
        int row = selectedRow[0];
        return filteredTrackTitles[row].getFullPathName();
    }
    return {};
}

void PlaylistComponent::mouseDrag(const MouseEvent& event) {
    int row = tableComponent.getSelectedRow();
    if (row >= 0 && row < filteredTrackTitles.size()) {
        var filename = filteredTrackTitles[row].getFullPathName();

        if (DragAndDropContainer* container = findParentComponentOfClass<DragAndDropContainer>()) {
            container->startDragging(filename, this);
        }
    }
}

double PlaylistComponent::getTrackDuration(const File& file) {
    AudioFormatManager fManager;
    fManager.registerBasicFormats();

    std::unique_ptr<AudioFormatReader> r(fManager.createReaderFor(file));

    if (r != nullptr) {
        return r->lengthInSamples / r->sampleRate;
    }
    return 0.0;
}

void PlaylistComponent::updateFilter() {
    juce::String searchText = searchBar.getText().toLowerCase();
    filteredTrackTitles.clear();

    for (const auto& file : trackTitles) {
        if (file.getFileNameWithoutExtension().toLowerCase().contains(searchText)) {
            filteredTrackTitles.push_back(file);
        }
    }
    tableComponent.updateContent();
}