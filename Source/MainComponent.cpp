/*
  ==============================================================================

    This file was auto-generated!

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
    addAndMakeVisible(playlistLooper);

    formatManager.registerBasicFormats();
    playlistLooper.restoreSavedState();
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
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll(getLookAndFeel().findColour(ResizableWindow::backgroundColourId));

    // You can add your drawing code here!

    g.setColour(Colours::white);
    g.setFont(14.0f);
    g.drawText("PlaylistComponent", getLocalBounds(), Justification::centred, true);
}

void MainComponent::resized()
{
    juce::FlexBox flexBox;
    flexBox.flexDirection = juce::FlexBox::Direction::column;
    flexBox.justifyContent = juce::FlexBox::JustifyContent::flexStart;
    flexBox.alignItems = juce::FlexBox::AlignItems::stretch;

    flexBox.items.add(juce::FlexItem(playlistComponent).withFlex(0.7f));

    flexBox.items.add(juce::FlexItem(playlistLooper).withFlex(0.3f));
    flexBox.performLayout(getLocalBounds());
}

void MainComponent::taskbarPlayPause()
{
    playlistLooper.taskbarPlayPause();
}

void MainComponent::taskbarPrevious()
{
    playlistLooper.taskbarPrevious();
}

void MainComponent::taskbarNext()
{
    playlistLooper.taskbarNext();
}


