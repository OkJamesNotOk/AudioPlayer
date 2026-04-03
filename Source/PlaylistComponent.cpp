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
    tableComponent.getHeader().addColumn("Move", 3, 60, minOrderColW, -1, juce::TableHeaderComponent::defaultFlags);
    tableComponent.getHeader().addColumn("Del", 4, 100, minDelColW, -1, juce::TableHeaderComponent::defaultFlags);

    tableComponent.setModel(this);
    tableComponent.setRowHeight(32);
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

    auto trashImage = ImageHelpers::loadPngFromBinaryData(
        BinaryData::trashsolid2_png,
        BinaryData::trashsolid2_pngSize
    );
    trashDrawable = ImageHelpers::makeDrawableFromImage(trashImage);

    auto downImage = ImageHelpers::loadPngFromBinaryData(
        BinaryData::navarrowdownsolid2_png,
        BinaryData::navarrowdownsolid2_pngSize
    );
    downDrawable = ImageHelpers::makeDrawableFromImage(downImage);

    auto upImage = ImageHelpers::loadPngFromBinaryData(
        BinaryData::navarrowupsolid2_png,
        BinaryData::navarrowupsolid2_pngSize
    );
    upDrawable = ImageHelpers::makeDrawableFromImage(upImage);

    loadPlaylist();

}

int PlaylistComponent::getNumRows() {
    return filteredTrackTitles.size();
}

void PlaylistComponent::paintRowBackground(Graphics& g, int rowNumber, int width, int height, bool rowIsSelected) {
    auto alternateColour = getLookAndFeel().findColour(ListBox::backgroundColourId)
        .interpolatedWith(getLookAndFeel().findColour(ListBox::textColourId), 0.03f);
    if (rowNumber >= 0 && rowNumber < filteredTrackTitles.size()
        && filteredTrackTitles[rowNumber].file == currentPlayingFile)
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
    tableComponent.updateContent();
    tableComponent.repaint();
}

void PlaylistComponent::paintCell(Graphics& g, int rowNumber, int columnId, int width, int height, bool rowIsSelected) {

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
}

void PlaylistComponent::resized() {
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
    const int colMove = juce::jmax(colW / 2, minOrderColW);
    const int colDel = juce::jmax((colW / 2), minDelColW);
    const int titleWidth = juce::jmax(50, tableWidth - colW - colMove - colDel);

    tableComponent.getHeader().setColumnWidth(1, titleWidth);
    tableComponent.getHeader().setColumnWidth(2, colW);
    tableComponent.getHeader().setColumnWidth(3, colMove);
    tableComponent.getHeader().setColumnWidth(4, colDel);
}

Component* PlaylistComponent::refreshComponentForCell(int rowNumber, int columnId, bool isRowSelected, Component* existingComponentToUpdate) {
    if (rowNumber < 0 || rowNumber >= (int)filteredTrackTitles.size())
        return nullptr;
    if (columnId == 1)
    {
        auto* titleLabel = dynamic_cast<ScrollLabel*>(existingComponentToUpdate);

        if (titleLabel == nullptr)
        {
            titleLabel = new ScrollLabel();
            titleLabel->setInterceptsMouseClicks(false, false);
            titleLabel->setColour(juce::Label::textColourId, juce::Colour::fromString("#FF457B9D"));
            titleLabel->setJustificationType(juce::Justification::centredLeft);
            titleLabel->setBorderSize(juce::BorderSize<int>(0, 2, 0, 2));
            titleLabel->setFont(juce::Font(14.0f));
            existingComponentToUpdate = titleLabel;
        }

        bool shouldScroll = isRowSelected;
        titleLabel->setText(filteredTrackTitles[rowNumber].file.getFileName(), juce::dontSendNotification);
        titleLabel->setScrollingEnabled(shouldScroll);
        return existingComponentToUpdate;
    }
    if (columnId == 2)
    {
        auto* durationLabel = dynamic_cast<juce::Label*>(existingComponentToUpdate);
        if (durationLabel == nullptr)
            durationLabel = new juce::Label();

        double duration = filteredTrackTitles[rowNumber].duration;

        juce::String durationText = duration >= 3600
            ? juce::String::formatted("%02d:%02d:%02d",
                (int)(duration / 3600), (int)(duration / 60) % 60, (int)duration % 60)
            : juce::String::formatted("%02d:%02d",
                (int)(duration / 60), (int)duration % 60);

        durationLabel->setText(durationText, juce::dontSendNotification);
        durationLabel->setColour(juce::Label::textColourId, juce::Colour::fromString("#FFAFAFDC"));
        durationLabel->setJustificationType(juce::Justification::centredLeft);
        durationLabel->setFont(juce::Font(14.0f));

        return durationLabel;
    }
    if (columnId == 3)
    {
        auto* moveComp = dynamic_cast<MoveButtonsComponent*>(existingComponentToUpdate);

        if (moveComp == nullptr)
        {
            moveComp = new MoveButtonsComponent(*this, upDrawable.get(), downDrawable.get());
            existingComponentToUpdate = moveComp;
        }

        moveComp->setRowFile(filteredTrackTitles[rowNumber].file);
    }
    if (columnId == 4) {
        auto* deleteBtn = dynamic_cast<juce::DrawableButton*>(existingComponentToUpdate);
        if (existingComponentToUpdate == nullptr)
        {
            deleteBtn = new juce::DrawableButton("Delete", juce::DrawableButton::ImageOnButtonBackground);
            deleteBtn->setImages(trashDrawable.get());
            deleteBtn->addListener(this);
            existingComponentToUpdate = deleteBtn;
        }
        // using full path name as ID because they are all unique
        deleteBtn->setComponentID(filteredTrackTitles[rowNumber].file.getFullPathName());
    }
    return existingComponentToUpdate;
}

bool PlaylistComponent::moveTrackUp(const juce::File& file)
{
    for (size_t i = 1; i < trackTitles.size(); ++i)
    {
        if (trackTitles[i].file == file)
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
        if (trackTitles[i].file == file)
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
                        if (isSupportedAudioFile(file)) {
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
                if (i->file.getFullPathName() == id)
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
        playlistArray.add(file.file.getFullPathName());

    juce::File saveFile = getDataFile("playlist.json");

    saveFile.replaceWithText(juce::JSON::toString(juce::var(playlistArray), true));
}

void PlaylistComponent::loadPlaylist()
{
    juce::File loadFile = getDataFile("playlist.json");

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
            trackTitles.push_back({ track, getTrackDuration(track) });
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

//save player state
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

    juce::File saveFile = getDataFile("playlistplayer.json");

    saveFile.replaceWithText(juce::JSON::toString(juce::var(root.get()), true));
}

// load player state
bool PlaylistComponent::loadPlayerState(PlayerState& state) const
{
    juce::File loadFile = getDataFile("playlistplayer.json");

    if (!loadFile.existsAsFile())
        return false;

    juce::var parsed = juce::JSON::parse(loadFile);
    auto* obj = parsed.getDynamicObject();

    if (obj == nullptr)
        return false;

    juce::String filepath = obj->getProperty("currentFile").toString();

    if (filepath.isEmpty())
        return false;

    if (!juce::File::isAbsolutePath(filepath))
        return false;

    juce::File fileToLoadPlayer(filepath);

    if (!fileToLoadPlayer.existsAsFile())
        return false;

    state.currentFile = fileToLoadPlayer;
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
        if (trackTitles[i].file == currentFile)
        {
            // Calculate next index (wrap around to 0 if at the end)
            int nextIndex = (i + 1) % trackTitles.size();
            return trackTitles[nextIndex].file;
        }
    }
    // If file not found or playlist empty, return the first track if available
    return trackTitles.size() > 0 ? trackTitles[0].file : juce::File{};
}

juce::File PlaylistComponent::getPreviousTrack(juce::File currentFile)
{
    for (size_t i = 0; i < trackTitles.size(); ++i)
    {
        if (trackTitles[i].file == currentFile)
        {
            // Calculate next index (wrap around to 0 if at the end)
            int previousIndex = (int(i) - 1 + (int)trackTitles.size()) % (int)trackTitles.size();
            return trackTitles[previousIndex].file;
        }
    }
    // If file not found or playlist empty, return the first track if available
    return trackTitles.size() > 0 ? trackTitles[0].file : juce::File{};
}

bool PlaylistComponent::isInterestedInFileDrag(const StringArray& files) {
    for (auto& file : files) {
        if (isSupportedAudioFile(juce::File(file))) {
            return true;
        }
    }
    return false;
}

void PlaylistComponent::filesDropped(const StringArray& files, int x, int y) {
    for (auto& file : files) {
        if (isSupportedAudioFile(juce::File(file))) {
            if (!checkFileDuplicate(file)) {
                trackTitles.push_back({ juce::File(file), getTrackDuration(juce::File(file)) });
                updateFilter();
            }
        }
    }
    tableComponent.updateContent();
}

void PlaylistComponent::addFileToPlaylist(const File& file) {
    if (!checkFileDuplicate(file))
    {
        trackTitles.push_back({ file, getTrackDuration(file) }); // cache once
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
        if (file.getFullPathName() == trackTitles[i].file.getFullPathName()) {
            return true;
        }
    }
    
    return false;
}

void PlaylistComponent::listBoxItemClicked(int row, const MouseEvent& e) {
    //if (row >= 0 && row < filteredTrackTitles.size()) {
    //    tableComponent.selectRow(row);
    //}
}

void PlaylistComponent::cellClicked(int rowNumber, int columnId, const juce::MouseEvent&) {
    if (rowNumber < 0 || rowNumber >= filteredTrackTitles.size())
        return;

    if (currentRow == rowNumber)
    {
        tableComponent.deselectAllRows();
        currentRow = -1;
    }
    else
    {
        tableComponent.selectRow(rowNumber);
        currentRow = rowNumber;
    }

    tableComponent.repaint();
}

var PlaylistComponent::getDragSourceDescription(const SparseSet<int>& selectedRow) {
    if (selectedRow.size() > 0) {
        int row = selectedRow[0];
        return filteredTrackTitles[row].file.getFullPathName();
    }
    return {};
}

void PlaylistComponent::mouseDrag(const MouseEvent& event) {
    int row = tableComponent.getSelectedRow();
    if (row >= 0 && row < filteredTrackTitles.size()) {
        var filename = filteredTrackTitles[row].file.getFullPathName();

        if (DragAndDropContainer* container = findParentComponentOfClass<DragAndDropContainer>()) {
            container->startDragging(filename, this);
        }
    }
}

double PlaylistComponent::getTrackDuration(const File& file)
{
    AudioFormatManager fManager;
    fManager.registerFormat(new FFmpegAudioFormat(), false);
    fManager.registerBasicFormats();

    std::unique_ptr<AudioFormatReader> r(fManager.createReaderFor(file));

    if (r != nullptr && r->sampleRate > 0.0)
        return r->lengthInSamples / r->sampleRate;

    return 0.0;
}

void PlaylistComponent::updateFilter()
{
    juce::String searchText = searchBar.getText().toLowerCase();
    filteredTrackTitles.clear();

    for (const auto& track : trackTitles)
    {
        if (track.file.getFileNameWithoutExtension().toLowerCase().contains(searchText))
            filteredTrackTitles.push_back(track);
    }

    tableComponent.updateContent();
}

juce::File PlaylistComponent::getDataFile(const juce::String& fileName) const
{
    auto exeFolder = juce::File::getSpecialLocation(juce::File::currentExecutableFile)
        .getParentDirectory();

    auto dataFolder = exeFolder.getChildFile("data");

    if (!dataFolder.exists())
        dataFolder.createDirectory();

    return dataFolder.getChildFile(fileName);
}

bool PlaylistComponent::isSupportedAudioFile(const juce::File& file)
{
    return file.hasFileExtension(".mp3;.wav;.flac;.aac;.m4a;.mp4;.mov;.ogg;.opus;.wma;.aiff;.aif;.alac;.ac3;.caf;.amr;.ape");
}

double PlaylistComponent::getOrCacheTrackDuration(const juce::File& file)
{
    for (auto& track : trackTitles)
    {
        if (track.file == file)
        {
            if (track.duration <= 0.0)
                track.duration = getTrackDuration(file);

            return track.duration;
        }
    }

    double duration = getTrackDuration(file);
    trackTitles.push_back({ file, duration });
    return duration;
}