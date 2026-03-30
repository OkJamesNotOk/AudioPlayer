/*
  ==============================================================================

    SettingsComponent.cpp
    Created: 30 Mar 2026 2:24:40pm
    Author:  OkJames

  ==============================================================================
*/

#include <JuceHeader.h>
#include "SettingsComponent.h"

//==============================================================================
SettingsComponent::SettingsComponent()
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.

    initialiseDefinitions();
    initialiseRows();
    loadSettings();
    syncButtonsFromSettings();
    attachCallbacks();
}

SettingsComponent::~SettingsComponent()
{
    saveSettings();
}

void SettingsComponent::paint (juce::Graphics& g)
{

    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));   // clear the background

    g.setColour (juce::Colours::grey);
    g.drawRect (getLocalBounds(), 1);   // draw an outline around the component
}

void SettingsComponent::resized()
{
    auto area = getLocalBounds().reduced(12);
    layoutRows(area);
}

juce::File SettingsComponent::getSettingsFile() const
{
    return juce::File::getSpecialLocation(juce::File::userApplicationDataDirectory)
        .getChildFile("OtoDecksSettings.json");
}

void SettingsComponent::initialiseDefinitions()
{
    settingDefinitions =
    {
        {
            "autoPlay",
            "Auto Play",
            "Automatically start playback when a track is dropped or loaded.",
            true
        },
        {
            "playlistWin",
            "Open Playlist Window",
            "Show the playlist in a separate window instead of the main app.",
            false
        }
    };

    settings.clear();

    for (const auto& definition : settingDefinitions)
        settings[definition.key] = SettingState{ definition.defaultValue };
}

void SettingsComponent::initialiseRows()
{
    settingRows.clear();

    for (const auto& definition : settingDefinitions)
        createSettingRow(definition);
}

void SettingsComponent::createSettingRow(const SettingDefinition& definition)
{
    SettingRow row;
    row.key = definition.key;

    row.titleLabel = std::make_unique<juce::Label>();
    row.titleLabel->setComponentID(definition.key + "_title");
    row.titleLabel->setText(definition.title, juce::dontSendNotification);
    row.titleLabel->setJustificationType(juce::Justification::centredLeft);
    row.titleLabel->setTooltip(definition.description);

    row.descriptionLabel = std::make_unique<juce::Label>();
    row.descriptionLabel->setComponentID(definition.key + "_description");
    row.descriptionLabel->setText(definition.description, juce::dontSendNotification);
    row.descriptionLabel->setJustificationType(juce::Justification::centredLeft);
    row.descriptionLabel->setColour(juce::Label::textColourId, juce::Colours::lightgrey);

    row.button = std::make_unique<juce::DrawableButton>(
        definition.key,
        juce::DrawableButton::ImageOnButtonBackground
    );
    row.button->setComponentID(definition.key);
    row.button->setClickingTogglesState(true);
    row.button->setTooltip(definition.description);

    addAndMakeVisible(*row.titleLabel);
    addAndMakeVisible(*row.descriptionLabel);
    addAndMakeVisible(*row.button);

    settingRows.push_back(std::move(row));
}

void SettingsComponent::layoutRows(juce::Rectangle<int> area)
{
    const int rowHeight = 56;
    const int rowGap = 10;
    const int buttonSize = 28;
    const int buttonGap = 10;

    for (auto& row : settingRows)
    {
        auto rowArea = area.removeFromTop(rowHeight);

        auto buttonArea = rowArea.removeFromRight(buttonSize);
        rowArea.removeFromRight(buttonGap);

        auto titleArea = rowArea.removeFromTop(24);
        auto descriptionArea = rowArea;

        row.titleLabel->setBounds(titleArea);
        row.descriptionLabel->setBounds(descriptionArea);
        row.button->setBounds(buttonArea.withSizeKeepingCentre(buttonSize, buttonSize));

        area.removeFromTop(rowGap);
    }
}

void SettingsComponent::attachCallbacks()
{
    for (auto& row : settingRows)
    {
        auto key = row.key;
        auto* buttonPtr = row.button.get();

        row.button->onClick = [this, key, buttonPtr]()
            {
                const bool value = buttonPtr->getToggleState();
                settings[key].value = value;

                if (auto* rowPtr = findRow(key))
                    updateRowVisuals(*rowPtr, value);

                saveSettings();

                if (onSettingChanged)
                    onSettingChanged(key, value);
            };
    }
}

void SettingsComponent::syncButtonsFromSettings()
{
    for (auto& row : settingRows)
    {
        const bool value = getSettingValue(row.key);
        row.button->setToggleState(value, juce::dontSendNotification);
        updateRowVisuals(row, value);
    }
}

void SettingsComponent::loadSettings()
{
    auto file = getSettingsFile();

    if (!file.existsAsFile())
        return;

    auto parsed = juce::JSON::parse(file);
    auto* obj = parsed.getDynamicObject();

    if (obj == nullptr)
        return;

    for (const auto& definition : settingDefinitions)
    {
        if (obj->hasProperty(definition.key))
            settings[definition.key].value = static_cast<bool>(obj->getProperty(definition.key));
    }
}

void SettingsComponent::saveSettings() const
{
    juce::DynamicObject::Ptr root = new juce::DynamicObject();

    for (const auto& definition : settingDefinitions)
    {
        auto it = settings.find(definition.key);
        const bool value = (it != settings.end()) ? it->second.value : definition.defaultValue;
        root->setProperty(definition.key, value);
    }

    getSettingsFile().replaceWithText(
        juce::JSON::toString(juce::var(root.get()), true)
    );
}

bool SettingsComponent::getSettingValue(const juce::String& key) const
{
    auto it = settings.find(key);
    return it != settings.end() ? it->second.value : false;
}

void SettingsComponent::setSettingValue(const juce::String& key, bool value)
{
    auto it = settings.find(key);
    if (it == settings.end())
        return;

    it->second.value = value;

    if (auto* row = findRow(key))
    {
        row->button->setToggleState(value, juce::dontSendNotification);
        updateRowVisuals(*row, value);
    }

    saveSettings();

    if (onSettingChanged)
        onSettingChanged(key, value);
}

void SettingsComponent::updateButtonStyle(juce::DrawableButton& button, bool isOn)
{
    button.setColour(juce::TextButton::buttonColourId,
        isOn ? juce::Colours::green : juce::Colours::red);

    button.setColour(juce::TextButton::buttonOnColourId,
        juce::Colours::green);

    button.setColour(juce::TextButton::textColourOffId, juce::Colours::black);
    button.setColour(juce::TextButton::textColourOnId, juce::Colours::black);

    button.setAlpha(1.0f);
    button.repaint();
}

void SettingsComponent::updateRowVisuals(SettingRow& row, bool isOn)
{
    updateButtonStyle(*row.button, isOn);

    row.titleLabel->setColour(juce::Label::textColourId,
        isOn ? juce::Colours::white : juce::Colours::lightgrey);

    row.descriptionLabel->setColour(juce::Label::textColourId,
        isOn ? juce::Colours::lightgreen : juce::Colours::grey);
}

const SettingsComponent::SettingDefinition* SettingsComponent::findDefinition(const juce::String& key) const
{
    for (const auto& definition : settingDefinitions)
    {
        if (definition.key == key)
            return &definition;
    }

    return nullptr;
}

SettingsComponent::SettingRow* SettingsComponent::findRow(const juce::String& key)
{
    for (auto& row : settingRows)
    {
        if (row.key == key)
            return &row;
    }

    return nullptr;
}

int SettingsComponent::getSettingsCount() const
{
    return static_cast<int>(settingDefinitions.size());
}

int SettingsComponent::getSettingsHeight() const
{
    const int topBottomPadding = 24;
    const int rowHeight = 56;
    const int rowGap = 10;

    const int count = getSettingsCount();

    if (count <= 0)
        return topBottomPadding;

    return topBottomPadding + (count * rowHeight) + ((count - 1) * rowGap);
}