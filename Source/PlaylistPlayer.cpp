/*
  ==============================================================================

    PlaylistPlayer.cpp
    Created: 26 Mar 2026 11:02:59pm
    Author:  OkJames

  ==============================================================================
*/

#include <JuceHeader.h>
#include "PlaylistPlayer.h"

PlaylistPlayer::PlaylistPlayer(DJAudioPlayer* _player,
    AudioFormatManager& formatManagerToUse,
    AudioThumbnailCache& cacheToUse,
    PlaylistComponent& _playlistComponent) : player(_player),
    waveformDisplay(formatManagerToUse, cacheToUse),
    playlistComponent(_playlistComponent)
{
    initialiseButtonGroups();
    initialiseButtons();
    initialiseSliders();
    addAndMakeVisible(songName);
    songName.setText("No File Loaded!", juce::dontSendNotification);
    songName.setScrollingEnabled(true);


    addAndMakeVisible(waveformDisplay);

    songName.setLookAndFeel(&customButton);


    volSliderColour();

    startTimer(500);

    // enable keyboards events
    setWantsKeyboardFocus(true);

    addAndMakeVisible(loopDisplay);

    // add mouse listener to loopDisplay to draw markers
    loopDisplay.addMouseListener(this, true);
}

PlaylistPlayer::~PlaylistPlayer()
{
    stopTimer();

    playButton.setLookAndFeel(nullptr);
    //loadButton.setLookAndFeel(nullptr);
    loopButton.setLookAndFeel(nullptr);
    prevButton.setLookAndFeel(nullptr);
    nextButton.setLookAndFeel(nullptr);
    loopStartButton.setLookAndFeel(nullptr);
    loopEndButton.setLookAndFeel(nullptr);

    volSlider.setLookAndFeel(nullptr);
    speedSlider.setLookAndFeel(nullptr);
    posSlider.setLookAndFeel(nullptr);

    songName.setLookAndFeel(nullptr);
}

void PlaylistPlayer::initialiseButtonGroups()
{
    //arrays of buttons
    transportButtons = { &playButton, &prevButton, &nextButton };
    loopButtons = { &loopButton, &loopStartButton, &loopEndButton };
    rowButtons =
    {
        &prevButton,
        &playButton,
        &nextButton,
        &loopButton,
        &loopStartButton,
        &loopEndButton
    };
}

void PlaylistPlayer::initialiseButtons()
{
    std::vector<juce::Button*> allButtons =
    {
        &playButton, &prevButton, &nextButton,
        &loopButton, &loopStartButton, &loopEndButton
    };

    for (auto* button : allButtons)
    {
        addAndMakeVisible(button);
        button->setLookAndFeel(&customButton);
        button->addListener(this);
    }

    auto playImage = ImageHelpers::loadPngFromBinaryData(BinaryData::playsolid_png, BinaryData::playsolid_pngSize);
    auto pauseImage = ImageHelpers::loadPngFromBinaryData(BinaryData::pausesolid_png, BinaryData::pausesolid_pngSize);
    auto prevImage = ImageHelpers::loadPngFromBinaryData(BinaryData::skipprevsolid_png, BinaryData::skipprevsolid_pngSize);
    auto nextImage = ImageHelpers::loadPngFromBinaryData(BinaryData::skipnextsolid_png, BinaryData::skipnextsolid_pngSize);

    auto playDrawable = ImageHelpers::makeDrawableFromImage(playImage);
    auto pauseDrawable = ImageHelpers::makeDrawableFromImage(pauseImage);
    auto prevDrawable = ImageHelpers::makeDrawableFromImage(prevImage);
    auto nextDrawable = ImageHelpers::makeDrawableFromImage(nextImage);

    playButton.setClickingTogglesState(true);
    playButton.setColour(juce::TextButton::buttonColourId, juce::Colour::fromString("#FFB48B9E"));
    playButton.setColour(juce::TextButton::textColourOffId, juce::Colours::black);
    playButton.setColour(juce::TextButton::buttonOnColourId, juce::Colour::fromString("#FFFF7E3A"));
    playButton.setColour(juce::TextButton::textColourOnId, juce::Colours::black);

    prevButton.setColour(juce::TextButton::buttonColourId, juce::Colour::fromString("#FFFFBE48"));
    prevButton.setColour(juce::TextButton::textColourOffId, juce::Colours::black);

    nextButton.setColour(juce::TextButton::buttonColourId, juce::Colour::fromString("#FFFFBE48"));
    nextButton.setColour(juce::TextButton::textColourOffId, juce::Colours::black);
    playButton.setImages(playDrawable.get(), nullptr, nullptr, nullptr,
        pauseDrawable.get(), nullptr, nullptr, nullptr);

    prevButton.setImages(prevDrawable.get());
    nextButton.setImages(nextDrawable.get());

    for (auto* button : loopButtons)
    {
        button->setClickingTogglesState(true);
        button->setColour(juce::TextButton::buttonColourId, juce::Colour::fromString("#FF9fc5e8"));
        button->setColour(juce::TextButton::buttonOnColourId, juce::Colour::fromString("#FFFF5722"));
        button->setColour(juce::TextButton::textColourOffId, juce::Colours::black);
        button->setColour(juce::TextButton::textColourOnId, juce::Colours::black);
    }
}
void PlaylistPlayer::initialiseSliders()
{
    std::vector<juce::Slider*> sliders = { &volSlider, &speedSlider, &posSlider };

    for (auto* slider : sliders)
    {
        addAndMakeVisible(slider);
        //slider->setLookAndFeel(&customButton);
        slider->addListener(this);
    }
    posSlider.setLookAndFeel(&customButton);

    // Set range, initial value, and precision to the speed slider
    // Set velocity based mode on for better control in the change of speed
    volSlider.setSliderStyle(juce::Slider::LinearVertical);
    // add mouse listener to change slider colour
    volSlider.addMouseListener(this, true);
    volSlider.setRange(0.0, 2.0);
    volSlider.setValue(0.7);
    volSlider.setNumDecimalPlacesToDisplay(3);
    volSlider.setTextBoxStyle(juce::Slider::TextBoxAbove, false, 60, 50);
    volSlider.setSliderSnapsToMousePosition(true);
    // Add description of what the slider is for before the slider value
    volSlider.textFromValueFunction = [](double value)
        {
            return juce::String(value, 2);
        };


    // Change speed slider for more surface area to click on
    speedSlider.setSliderStyle(juce::Slider::LinearVertical);
    speedSlider.setRange(0.1, 4.0);
    speedSlider.setValue(1.0);
    speedSlider.setNumDecimalPlacesToDisplay(3);
    speedSlider.setTextBoxStyle(juce::Slider::TextBoxAbove, false, 60, 50);
    speedSlider.setVelocityBasedMode(true);
    // Add description of what the slider is for before the slider value
    speedSlider.textFromValueFunction = [](double value)
        {
            return juce::String(value, 2) + "x";
        };

    // set range and precision for position slider
    // CHange slider style for more surface area to click on
    posSlider.setSliderStyle(juce::Slider::LinearBar);
    posSlider.setTextBoxStyle(juce::Slider::TextBoxAbove, true, 0, 0);
    posSlider.setSliderSnapsToMousePosition(true);
    posSlider.setRange(0.00, 1.00);
    posSlider.setNumDecimalPlacesToDisplay(3);

    posSlider.textFromValueFunction = [this](double value) {
        if (!currentFile.existsAsFile())
            return juce::String("00:00");

        double duration = playlistComponent.getOrCacheTrackDuration(currentFile);
        double currentSeconds = value * duration;

        if (duration >= 3600)
        {
            return juce::String::formatted("%02d:%02d:%02d",
                (int)(currentSeconds / 3600),
                ((int)(currentSeconds / 60)) % 60,
                (int)currentSeconds % 60);
        }

        return juce::String::formatted("%02d:%02d",
            (int)(currentSeconds / 60),
            (int)currentSeconds % 60);
    };
}

void PlaylistPlayer::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::darkgrey);
    g.setColour(juce::Colours::white);
    g.drawRect(getLocalBounds(), 1);   // draw an outline around the component
}

void PlaylistPlayer::resized()
{
    auto area = getLocalBounds().reduced(1);

    auto rightPanel = area.removeFromRight(area.getWidth() * 10 / 32).reduced(0, componentsMargin).withTrimmedRight(componentsMargin);
    auto leftPanel = area;
    auto rowH = leftPanel.getHeight() / 9;

    // Song name display and position slider 
    songName.setBounds(leftPanel.removeFromTop(rowH).reduced(componentsMargin,0));
    posSlider.setBounds(leftPanel.removeFromTop(rowH).reduced(componentsMargin, 0));

    // Waveform display 
    auto displayArea = leftPanel.removeFromTop(rowH * 4);
    waveformDisplay.setBounds(displayArea);

    // Loop display
    loopDisplay.setBounds(displayArea);

    // Buttons area
    auto buttonArea = leftPanel.withTrimmedBottom(1);

    const int buttonsPerRow = 3;
    const int buttonCount = static_cast<int>(rowButtons.size());
    const int numRows = (buttonCount + buttonsPerRow - 1) / buttonsPerRow;

    const int rowHeight = (numRows > 0) ? buttonArea.getHeight() / numRows : 0;

    for (int i = 0; i < buttonCount; ++i)
    {
        const int row = i / buttonsPerRow;
        const int col = i % buttonsPerRow;

        auto currentRowArea = buttonArea.withTrimmedTop(row * rowHeight).removeFromTop(rowHeight).reduced(componentsMargin, 0);

        const int buttonsInThisRow = juce::jmin(buttonsPerRow, buttonCount - row * buttonsPerRow);
        const int buttonWidth = (buttonsInThisRow > 0) ? currentRowArea.getWidth() / buttonsInThisRow : 0;

        rowButtons[i]->setBounds(
            currentRowArea.withTrimmedLeft(col * buttonWidth)
            .removeFromLeft(buttonWidth)
        );
    }

    // Volume slider and speed slider
    auto sliderW = rightPanel.getWidth() / 2;
    volSlider.setBounds(rightPanel.removeFromLeft(sliderW));
    rightPanel.removeFromLeft(2);
    speedSlider.setBounds(rightPanel);
}

void PlaylistPlayer::persistPlayerState()
{
    if (!currentFile.existsAsFile())
        return;
    PlaylistComponent::PlayerState state;
    state.currentFile = currentFile;
    state.position = posSlider.getValue();
    state.volume = volSlider.getValue();
    state.speed = speedSlider.getValue();
    state.loopStart = loopStart;
    state.loopEnd = loopEnd;
    state.loopEnabled = loop;

    playlistComponent.savePlayerState(state);
}

void PlaylistPlayer::buttonClicked(Button* button)
{
    if (button == &playButton)
    {
        setPlaybackState(playButton.getToggleState());
    }
    if (button == &prevButton)
    {
        juce::File previousTrack = playlistComponent.getPreviousTrack(currentFile);
        if (previousTrack.existsAsFile())
        {
            currentFile = previousTrack;
            playlistComponent.setCurrentPlayingFile(currentFile);
            player->loadURL(juce::URL{ previousTrack });
            waveformDisplay.loadURL(juce::URL{ previousTrack });
            scrollLabelTextSet(previousTrack.getFileName());
            player->start();
            playButton.setToggleState(true, juce::NotificationType::dontSendNotification);
            playButton.setButtonText("| |");
            persistPlayerState();
        }
    }
    if (button == &nextButton)
    {
        juce::File nextTrack = playlistComponent.getNextTrack(currentFile);
        if (nextTrack.existsAsFile())
        {
            currentFile = nextTrack;
            playlistComponent.setCurrentPlayingFile(currentFile);
            player->loadURL(juce::URL{ nextTrack });
            waveformDisplay.loadURL(juce::URL{ nextTrack });
            scrollLabelTextSet(nextTrack.getFileName());
            player->start();
            playButton.setToggleState(true, juce::NotificationType::dontSendNotification);
            playButton.setButtonText("| |");
            persistPlayerState();
        }
    }
    if (button == &loadButton)
    {
        // file chooser that allow multiple file selection
        auto fileChooserFlags = FileBrowserComponent::canSelectMultipleItems;
        fChooser.launchAsync(fileChooserFlags, [this](const FileChooser& chooser)
            {
                auto chosenFiles = chooser.getResults();

                if (chosenFiles.size() > 0) {
                    for (int i = 0; i < chosenFiles.size(); i++) {
                        File file = chosenFiles[i];

                        // add file to playlist if they have the extensions ".mp3;.wav;.flac"
                        if (file.hasFileExtension(".mp3;.wav;.flac")) {
                            playlistComponent.addFileToPlaylist(file);
                        }
                    }

                    // The first file of the selected files is loaded into the player 
                    // if it has the correct extension
                    File file = chosenFiles[0];
                    if (PlaylistComponent::isSupportedAudioFile(file)) {
                        player->loadURL(URL{ file });
                        currentFile = file;
                        playlistComponent.setCurrentPlayingFile(currentFile);
                        waveformDisplay.loadURL(URL{ file });
                        playButton.setToggleState(true, juce::NotificationType::dontSendNotification);
                        playButton.triggerClick();
                        scrollLabelTextSet(String(file.getFileName()));
                    }
                    persistPlayerState();
                }
            });
    }
    if (button == &loopButton) {
        loop = button->getToggleState();
        persistPlayerState();
    }
    if (button == &loopStartButton) {
        // set boolean to start creating the start point for the loop
        isStartLoop = true;
        setStart = true;

        // Remove start point of the loop
        if (!button->getToggleState()) {
            isStartLoop = false;
            setStart = false;
            loopStart = 0.0;
            loopDisplay.setMarkerStart(loopStart);
        }

        persistPlayerState();
    }
    if (button == &loopEndButton) {
        // set booleans to true to start creating the end point for the loop
        isEndLoop = true;
        setEnd = true;

        // Remove the end point for the loop
        if (!button->getToggleState()) {
            isEndLoop = false;
            setEnd = false;
            loopEnd = 1.0;
            loopDisplay.setMarkerEnd(loopEnd);
        }

        persistPlayerState();
    }
}

void PlaylistPlayer::sliderValueChanged(Slider* slider)
{
    if (slider == &volSlider)
    {
        double vol = slider->getValue();
        player->setGain(vol);

        volSliderColour();
        volSlider.setColour(juce::Slider::trackColourId, volColour);
        volSlider.repaint();

        persistPlayerState();
    }

    if (slider == &speedSlider)
    {
        player->setSpeed(slider->getValue());
        persistPlayerState();
    }
    if (slider == &posSlider)
    {
        player->setPositionRelative(slider->getValue());
        //persistPlayerState();
    }
}

void PlaylistPlayer::volSliderColour() {
    double maxVol = volSlider.getMaximum();
    double vol = volSlider.getValue();
    double greenSpot = (maxVol * 0.35);

    // If slider value is 35% of the vol slider range, colour it green
    if (vol <= greenSpot) {
        float green = static_cast<float>(vol / greenSpot);
        volColour = Colour::fromFloatRGBA(0, 1, 0, 1);
    }
    // else colour the slider, the higher the value the redder the colour
    else {
        float red = static_cast<float>((vol - greenSpot / 2) / (maxVol - greenSpot / 2));
        volColour = Colour::fromFloatRGBA(red, 1 - red, 0, 1);
    }
}

void PlaylistPlayer::scrollLabelTextSet(const juce::String& str)
{
    songName.setText(str, juce::dontSendNotification);
}

void PlaylistPlayer::updateMarkers(const MouseEvent& e) {
    // get mouse position on the component
    MouseEvent event = e.getEventRelativeTo(&loopDisplay);
    auto waveformMouPos = event.getPosition();
    // convert to a number from 0 - 1 for the position of the loop markers
    double clickX = (double)waveformMouPos.x / loopDisplay.getWidth();

    if (setStart) {
        if (clickX < 0) {
            clickX = 0;
        }
        loopStart = clickX;
        // Set boolean to false so subsequent clickes do not set a new marker 
        // and start point of the loop
        setStart = false;
        if (isStartLoop && loopDisplay.loopMarkerStart != loopStart) {
            loopDisplay.setMarkerStart(loopStart);
            persistPlayerState();
        }
        // Turn off and remove start point of the loop if start point is more than end point
        if (isEndLoop) {
            if (loopStart > loopEnd) {
                loopStartButton.setToggleState(false, juce::NotificationType::dontSendNotification);
                PlaylistPlayer::buttonClicked(&loopStartButton);
            }
        }
    }
    else if (setEnd) {
        if (clickX > 1.0) {
            clickX = 1;
        }
        loopEnd = clickX;
        // Set boolean to false so subsequent clickes do not set a new marker 
        // and end point of the loop
        setEnd = false;
        if (isEndLoop && loopDisplay.loopMarkerEnd != loopEnd) {
            loopDisplay.setMarkerEnd(loopEnd);
            persistPlayerState();
        }
        // Turn off and remove end point of the loop if end point is less than start point
        if (isStartLoop) {
            if (loopEnd < loopStart) {
                loopEndButton.setToggleState(false, juce::NotificationType::dontSendNotification);
                PlaylistPlayer::buttonClicked(&loopEndButton);
            }
        }
    }
}

// Draw preview of the markers (Does not set the markers)
void PlaylistPlayer::previewMarkers(const MouseEvent& e) {
    MouseEvent event = e.getEventRelativeTo(&loopDisplay);
    auto waveformMouPos = event.getPosition();
    double clickX = (double)waveformMouPos.x / loopDisplay.getWidth();

    // Only draw preview on first click and drag
    if (isStartLoop && setStart) {
        if (clickX < 0) {
            clickX = 0;
        }
        loopDisplay.setMarkerStart(clickX);
    }
    else if (isEndLoop && setEnd) {
        if (clickX > 1.0) {
            clickX = 1;
        }
        loopDisplay.setMarkerEnd(clickX);
    }
}

void PlaylistPlayer::mouseDown(const MouseEvent& e) {

}

void PlaylistPlayer::mouseUp(const MouseEvent& e) {
    if (e.eventComponent == &loopDisplay) {
        updateMarkers(e);
    }
}

void PlaylistPlayer::mouseDrag(const MouseEvent& e) {
    if (e.eventComponent == &loopDisplay) {
        previewMarkers(e);
    }
}

bool PlaylistPlayer::isInterestedInFileDrag(const StringArray& files)
{
    return true;
}

void PlaylistPlayer::filesDropped(const StringArray& files, int x, int y)
{
    if (files.size() == 1)
    {
        currentFile = juce::File{ files[0] };
        playlistComponent.setCurrentPlayingFile(currentFile);

        juce::URL fileURL{ currentFile };
        player->loadURL(fileURL);
        waveformDisplay.loadURL(fileURL);

        playlistComponent.addFileToPlaylist(currentFile);
        playlistComponent.setCurrentPlayingFile(currentFile);

        // changed to File instead of URL, for proper unicode filename display
        scrollLabelTextSet(currentFile.getFileName());

        setPlaybackState(autoplay);
        persistPlayerState();
    }
}

void PlaylistPlayer::timerCallback() {
    double g = player->getPositionRelative();
    waveformDisplay.setPositionRelative(g);
    if (!isnan(g)) {
        posSlider.setValue(g, juce::dontSendNotification);

        ++saveCounter;
        // saveinterval * 500ms = interval in seconds (e.g 20 * 500 = 10000ms = 10s)
        if (saveCounter >= saveInterval) 
        {
            persistPlayerState();
            saveCounter = 0;
        }


        // check if track ended or passed the end point of the loop
        if (posSlider.getValue() >= loopEnd) {
            // if loop is not on, stop the player
            if (!loop) {
                //playButton.setToggleState(true, juce::NotificationType::dontSendNotification);
                //playButton.triggerClick();
                juce::File nextTrack = playlistComponent.getNextTrack(currentFile);
                if (nextTrack.existsAsFile()) {
                    currentFile = nextTrack;
                    playlistComponent.setCurrentPlayingFile(currentFile);
                    player->loadURL(juce::URL{ nextTrack });
                    waveformDisplay.loadURL(juce::URL{ nextTrack });
                    scrollLabelTextSet(nextTrack.getFileName());
                    player->start();
                    playButton.setToggleState(true, juce::NotificationType::dontSendNotification);
                }
                else {
                    playButton.setToggleState(true, juce::NotificationType::dontSendNotification);
                    playButton.triggerClick();
                }
            }
            //posSlider.setValue(loopStart);
            player->setPositionRelative(loopStart);
            posSlider.setValue(loopStart, juce::dontSendNotification);
            // if loop is on, play the track
            if (loop) {
                playButton.setToggleState(false, juce::NotificationType::dontSendNotification);
                playButton.triggerClick();
            }
        }
    }
}

// Check if the description of the drag item contain a path name 
// that end with extensions ".mp3;.wav;.flac"
bool PlaylistPlayer::isInterestedInDragSource(const SourceDetails& sourceDescriptions) {
    String file = sourceDescriptions.description.toString();
    if (PlaylistComponent::isSupportedAudioFile(juce::File(file))) {
        return true;
    }
    return false;
}

// Get file path from dragged item and load into player
void PlaylistPlayer::itemDropped(const SourceDetails& filename) {
    String filePath = filename.description.toString();

    File file(filePath);
    if (file.existsAsFile()) {
        currentFile = file;
        playlistComponent.setCurrentPlayingFile(currentFile);
        player->loadURL(URL{ file });
        waveformDisplay.loadURL(URL{ file });
        setPlaybackState(autoplay);
        scrollLabelTextSet(String(file.getFileName()));
        persistPlayerState();
    }
}

// Bind the space key to the function of the play button
bool PlaylistPlayer::keyPressed(const KeyPress& key) {
    if (key == juce::KeyPress::spaceKey) {
        playButton.triggerClick();
        return true;
    }

    return false;
}

void PlaylistPlayer::restoreSavedState()
{
    PlaylistComponent::PlayerState savedState;

    if (playlistComponent.loadPlayerState(savedState))
    {
        if (savedState.currentFile.existsAsFile())
        {
            currentFile = savedState.currentFile;
            playlistComponent.setCurrentPlayingFile(currentFile);
            player->loadURL(juce::URL{ currentFile });
            waveformDisplay.loadURL(juce::URL{ currentFile });
            scrollLabelTextSet(currentFile.getFileName());

            posSlider.setValue(savedState.position, juce::dontSendNotification);
            volSlider.setValue(savedState.volume, juce::dontSendNotification);
            speedSlider.setValue(savedState.speed, juce::dontSendNotification);

            player->setGain(savedState.volume);
            player->setSpeed(savedState.speed);
            player->setPositionRelative(savedState.position);

            // restore stored values
            loop = savedState.loopEnabled;
            loopStart = savedState.loopStart;
            loopEnd = savedState.loopEnd;

            // restore main loop button
            loopButton.setToggleState(loop, juce::dontSendNotification);

            // restore loop start marker
            if (savedState.loopStart != 0.0)
            {
                isStartLoop = true;
                setStart = false;
                loopStartButton.setToggleState(true, juce::dontSendNotification);
                loopDisplay.setMarkerStart(loopStart);
            }
            else
            {
                isStartLoop = false;
                setStart = false;
                loopStart = 0.0;
                loopStartButton.setToggleState(false, juce::dontSendNotification);
                loopDisplay.setMarkerStart(loopStart);
            }

            // restore loop end marker
            if (savedState.loopEnd != 1.0)
            {
                isEndLoop = true;
                setEnd = false; // important: do NOT wait for next click
                loopEndButton.setToggleState(true, juce::dontSendNotification);
                loopDisplay.setMarkerEnd(loopEnd);
            }
            else
            {
                isEndLoop = false;
                setEnd = false;
                loopEnd = 1.0;
                loopEndButton.setToggleState(false, juce::dontSendNotification);
                loopDisplay.setMarkerEnd(loopEnd);
            }
        }
    }
}

bool PlaylistPlayer::taskbarPlayPause()
{
    return setPlaybackState(!playButton.getToggleState());
}

bool PlaylistPlayer::setPlaybackState(bool shouldPlay)
{
    playButton.setToggleState(shouldPlay, juce::dontSendNotification);

    if (shouldPlay)
        player->start();
    else
        player->stop();

    if (onPlaybackStateChanged)
        onPlaybackStateChanged(shouldPlay);

    return shouldPlay;
}

void PlaylistPlayer::taskbarPrevious()
{
    prevButton.triggerClick();
}

void PlaylistPlayer::taskbarNext()
{
    nextButton.triggerClick();
}

bool PlaylistPlayer::isPlaying() const
{
    return playButton.getToggleState();
}

void PlaylistPlayer::setAutoPlayEnabled(bool enabled)
{
    autoplay = enabled;
}