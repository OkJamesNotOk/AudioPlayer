/*
  ==============================================================================

    MainComponent.cpp
    Created: 27 Mar 2026 2:51:06am
    Author:  OkJames

  ==============================================================================
*/

#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent()
{
    // Make sure you set the size of the component after
    // you add any child components.
    setSize(400, 600);

    // Some platforms require permissions to open input channels so request that here
    if (RuntimePermissions::isRequired(RuntimePermissions::recordAudio)
        && !RuntimePermissions::isGranted(RuntimePermissions::recordAudio))
    {
        RuntimePermissions::request(RuntimePermissions::recordAudio,
            [&](bool granted) { if (granted)  setAudioChannels(2, 2); });
    }
    else
    {
        // Specify the number of input and output channels that we want to open
        setAudioChannels(0, 2);
    }

    addAndMakeVisible(playlistComponent);
    addAndMakeVisible(PlaylistPlayer);

    for (auto* button : topBarButtons)
        addAndMakeVisible(button);

    settingsButton.onClick = [this]()
        {
            openSettings();
        };

    playlistButton.onClick = [this]()
        {
            openPlaylist();
        };
    auto playlistImage = ImageHelpers::loadPngFromBinaryData(BinaryData::playlistplus_png, BinaryData::playlistplus_pngSize);
    auto playlistDrawable = ImageHelpers::makeDrawableFromImage(playlistImage);
    playlistButton.setImages(playlistDrawable.get());

    auto settingImage = ImageHelpers::loadPngFromBinaryData(BinaryData::settings3_5_png, BinaryData::settings3_5_pngSize);
    auto settingDrawable = ImageHelpers::makeDrawableFromImage(settingImage);
    settingsButton.setImages(settingDrawable.get());

    settingsButton.setTooltip("Settings");
    playlistButton.setTooltip("Playlist");

    formatManager.registerFormat(new FFmpegAudioFormat(), false);
    formatManager.registerBasicFormats();

    PlaylistPlayer.setAutoPlayEnabled(settingsComponent.getSettingValue("autoPlay"));
    PlaylistPlayer.setComponentsMargin(settingsComponent.getIntSettingValue("playerMargin"));
    settingsComponent.onSettingChanged = [this](const juce::String& key, const juce::var& value)
        {
            settingsManagement(key, value);
        };

    PlaylistPlayer.restoreSavedState();

    PlaylistPlayer.onPlaybackStateChanged = [this](bool playing)
        {
            if (onPlaybackStateChanged)
                onPlaybackStateChanged(playing);
        };

    updatePlaylistPresentation();
}

MainComponent::~MainComponent()
{
    // This shuts down the audio device and clears the audio source.
    shutdownAudio();
}

//==============================================================================
void MainComponent::prepareToPlay(int samplesPerBlockExpected, double sampleRate) {
    looperPlayer.prepareToPlay(samplesPerBlockExpected, sampleRate);

    mixerSource.addInputSource(&looperPlayer, false);

}
void MainComponent::getNextAudioBlock(const AudioSourceChannelInfo& bufferToFill) {
    mixerSource.getNextAudioBlock(bufferToFill);
}

void MainComponent::releaseResources() {
    looperPlayer.releaseResources();
    mixerSource.releaseResources();
}

//==============================================================================
void MainComponent::paint(Graphics& g)
{
    g.fillAll(getLookAndFeel().findColour(ResizableWindow::backgroundColourId));
    g.setColour(juce::Colours::grey);
    g.drawRect(getLocalBounds(), 1);
}

int MainComponent::getInternalPlaylistHeight() const
{
    auto area = getLocalBounds().reduced(5);

    const int gap = 5;
    const int topBarH = 40;

    area.removeFromTop(topBarH);
    area.removeFromTop(gap);

    const int availableHeight = area.getHeight();

    return juce::jlimit(300, 500, availableHeight / 2);
}

void MainComponent::updateInternalWindowSize()
{
    if (settingsComponent.getSettingValue("playlistWin"))
        return;

    if (auto* window = findParentComponentOfClass<juce::DocumentWindow>())
    {
        auto bounds = window->getBounds();

        if (internalPlaylistVisible)
        {
            if (internalCollapsedWindowHeight <= 0)
                internalCollapsedWindowHeight = bounds.getHeight();

            const int targetHeight = internalCollapsedWindowHeight + getInternalPlaylistHeight() + 5;
            window->setSize(bounds.getWidth(), targetHeight);
        }
        else
        {
            if (internalCollapsedWindowHeight > 0)
                window->setSize(bounds.getWidth(), internalCollapsedWindowHeight);
        }
    }
}

void MainComponent::resized()
{
    auto area = getLocalBounds().reduced(5);
    const int gap = 5;
    const int topBarH = 40;

    auto topBar = area.removeFromTop(topBarH);

    const int buttonCount = static_cast<int>(topBarButtons.size());
    const int totalGapSpace = gap * (buttonCount - 1);
    const int availableWidth = topBar.getWidth() - totalGapSpace;
    const int buttonSize = availableWidth / buttonCount;

    for (int i = 0; i < buttonCount; ++i)
    {
        topBarButtons[i]->setBounds(topBar.removeFromLeft(buttonSize));
        if (i < buttonCount - 1)
            topBar.removeFromLeft(gap);
    }

    area.removeFromTop(gap);

    const bool isExternal = settingsComponent.getSettingValue("playlistWin");

    if (isExternal)
    {
        PlaylistPlayer.setBounds(area);
        PlaylistPlayer.setVisible(true);

        if (playlistComponent.getParentComponent() == this)
            playlistComponent.setVisible(false);

        return;
    }

    if (playlistComponent.getParentComponent() == this && internalPlaylistVisible)
    {
        const int playlistHeight = getInternalPlaylistHeight();

        auto playlistArea = area.removeFromBottom(playlistHeight);
        area.removeFromBottom(gap);

        PlaylistPlayer.setBounds(area);
        PlaylistPlayer.setVisible(true);

        playlistComponent.setBounds(playlistArea);
        playlistComponent.setVisible(true);
    }
    else
    {
        PlaylistPlayer.setBounds(area);
        PlaylistPlayer.setVisible(true);

        if (playlistComponent.getParentComponent() == this)
            playlistComponent.setVisible(false);
    }
}

bool MainComponent::taskbarPlayPause() {
    return PlaylistPlayer.taskbarPlayPause();
}

void MainComponent::taskbarPrevious() {
    PlaylistPlayer.taskbarPrevious();
}

void MainComponent::taskbarNext() {
    PlaylistPlayer.taskbarNext();
}

bool MainComponent::isPlaying() const
{
    return PlaylistPlayer.isPlaying();
}

void MainComponent::openPlaylist()
{
    if (!settingsComponent.getSettingValue("playlistWin") && !internalPlaylistVisible)
    {
        if (auto* window = findParentComponentOfClass<juce::DocumentWindow>())
            internalCollapsedWindowHeight = window->getHeight();

        if (wouldInternalPlaylistExceedScreen())
            settingsComponent.setSettingValue("playlistWin", true);
    }

    const bool isExternal = settingsComponent.getSettingValue("playlistWin");

    if (isExternal)
    {
        if (playlistWindow == nullptr)
        {
            juce::Logger::writeToLog("openPlaylist: playlistWindow was null, creating it");
            playlistWindow = std::make_unique<PlaylistWindow>(playlistComponent);
        }

        const bool shouldShow = !playlistWindow->isVisible();
        juce::Logger::writeToLog("openPlaylist: external mode, shouldShow=" + juce::String(shouldShow ? "true" : "false"));

        if (shouldShow)
        {
            if (auto* mainWindow = findParentComponentOfClass<juce::DocumentWindow>())
            {
                auto mainBounds = mainWindow->getBounds();
                const int gap = 10;
                playlistWindow->setTopLeftPosition(mainBounds.getRight() + gap, mainBounds.getY());
            }

            playlistComponent.setVisible(true);
            playlistWindow->setVisible(true);
            playlistWindow->toFront(true);
        }
        else
        {
            playlistWindow->setVisible(false);
        }
        return;
    }

    if (!internalPlaylistVisible)
    {
        if (auto* window = findParentComponentOfClass<juce::DocumentWindow>())
            internalCollapsedWindowHeight = window->getHeight();
    }

    internalPlaylistVisible = !internalPlaylistVisible;
    updateInternalWindowSize();
    resized();
    repaint();
}

void MainComponent::openSettings()
{
    if (settingsWindow == nullptr)
        settingsWindow = std::make_unique<SettingsWindow>(settingsComponent);

    if (auto* mainWindow = findParentComponentOfClass<juce::DocumentWindow>())
    {
        auto mainBounds = mainWindow->getBounds();

        int gap = 10;
        int newX = mainBounds.getX() - settingsWindow->getWidth() - gap;
        int newY = mainBounds.getY();

        settingsWindow->setTopLeftPosition(newX, newY);
    }

    settingsWindow->setVisible(true);
    settingsWindow->toFront(true);
}


void MainComponent::settingsManagement(const juce::String& key, const juce::var& value)
{
    if (key == "autoPlay")
    {
        PlaylistPlayer.setAutoPlayEnabled((bool)value);
    }
    else if (key == "playlistWin")
    {
        const bool ValBool = (bool)value;
        if (auto* window = findParentComponentOfClass<juce::DocumentWindow>())
        {
            if (!ValBool)
            {
                internalCollapsedWindowHeight = window->getHeight();
            }
            else if (internalPlaylistVisible && internalCollapsedWindowHeight > 0)
            {
                window->setSize(window->getWidth(), internalCollapsedWindowHeight);
            }
        }

        internalPlaylistVisible = false;
        updatePlaylistPresentation();
    }
    else if (key == "playerMargin") {
        PlaylistPlayer.setComponentsMargin((int)value);
    }
}

namespace
{
    juce::String getComponentParentName(juce::Component* comp)
    {
        if (comp == nullptr)
            return "nullptr";

        if (dynamic_cast<MainComponent*>(comp) != nullptr)
            return "MainComponent";

        if (dynamic_cast<PlaylistWindow*>(comp) != nullptr)
            return "PlaylistWindow";

        return comp->getName().isNotEmpty() ? comp->getName() : "OtherComponent";
    }
}

void MainComponent::updatePlaylistPresentation()
{
    const bool isExternal = settingsComponent.getSettingValue("playlistWin");

    if (isExternal)
    {
        if (playlistComponent.getParentComponent() == this)
        {
            removeChildComponent(&playlistComponent);
        }

        if (playlistWindow != nullptr)
        {
            playlistWindow->setVisible(false);
            playlistWindow.reset();
        }
        playlistWindow = std::make_unique<PlaylistWindow>(playlistComponent);

        playlistComponent.setVisible(true);
        playlistWindow->setVisible(false);
    }
    else
    {
        if (playlistWindow != nullptr)
        {
            playlistWindow->setVisible(false);
            playlistWindow.reset();
        }

        if (playlistComponent.getParentComponent() != this)
        {
            addAndMakeVisible(playlistComponent);
        }

        playlistComponent.setVisible(internalPlaylistVisible);
    }

    resized();
    repaint();
}

bool MainComponent::wouldInternalPlaylistExceedScreen() const
{
    if (settingsComponent.getSettingValue("playlistWin"))
        return false;

    auto* window = const_cast<MainComponent*>(this)
        ->findParentComponentOfClass<juce::DocumentWindow>();

    if (window == nullptr)
        return false;

    auto* display = juce::Desktop::getInstance()
        .getDisplays()
        .getDisplayForRect(window->getScreenBounds());

    if (display == nullptr)
        return false;

    const int gap = 5;
    const int baseHeight = (internalCollapsedWindowHeight > 0)
        ? internalCollapsedWindowHeight
        : window->getHeight();

    const int targetHeight = baseHeight + getInternalPlaylistHeight() + gap;

    return targetHeight > display->userArea.getHeight();
}

void MainComponent::closePlaylistOnShutDown()
{
    if (!settingsComponent.getSettingValue("playlistWin") && internalPlaylistVisible)
    {
        internalPlaylistVisible = false;
        updateInternalWindowSize();
        resized();
        repaint();
    }
}