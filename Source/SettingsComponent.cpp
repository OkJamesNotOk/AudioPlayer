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
    syncRowsFromSettings();
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
    auto exeFolder = juce::File::getSpecialLocation(juce::File::currentExecutableFile)
        .getParentDirectory();

    auto dataFolder = exeFolder.getChildFile("data");

    if (!dataFolder.exists())
        dataFolder.createDirectory();

    return dataFolder.getChildFile("settings.json");
}

void SettingsComponent::initialiseDefinitions()
{
    settingDefinitions =
    {
        {
            "autoPlay",
            "Auto Play",
            "Automatically start playback when a track is dropped or loaded.",
            SettingType::boolean,
            true
        },
        {
            "playlistWin",
            "Open Playlist Window",
            "Show the playlist in a separate window instead of the main app.",
            SettingType::boolean,
            false
        },

        {
            "playerMargin",
            "Player Margin",
            "Set Margin for components of the player",
            SettingType::integer,
            5
        }
    };

    settings.clear();

    for (const auto& definition : settingDefinitions)
    {
        settings[definition.key] = SettingState
        {
            definition.type,
            definition.defaultValue
        };
    }
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
    row.type = definition.type;

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

    switch (definition.type)
    {
    case SettingType::boolean:
    {
        row.drawableButton = std::make_unique<juce::DrawableButton>(
            definition.key,
            juce::DrawableButton::ImageOnButtonBackground
        );

        row.drawableButton->setComponentID(definition.key);
        row.drawableButton->setClickingTogglesState(true);
        row.drawableButton->setTooltip(definition.description);

        addAndMakeVisible(*row.drawableButton);
        break;
    }

    case SettingType::integer:
    {
        row.slider = std::make_unique<juce::Slider>();
        row.slider->setComponentID(definition.key);
        row.slider->setSliderStyle(juce::Slider::IncDecButtons);
        row.slider->setTextBoxStyle(juce::Slider::TextBoxAbove, false, 60, 24);
        row.slider->setRange(0, 30, 1);
        row.slider->setTooltip(definition.description);
        addAndMakeVisible(*row.slider);
        break;
    }

    case SettingType::floating:
    {
        row.slider = std::make_unique<juce::Slider>();
        row.slider->setComponentID(definition.key);
        row.slider->setSliderStyle(juce::Slider::LinearHorizontal);
        row.slider->setTextBoxStyle(juce::Slider::TextBoxLeft, false, 60, 24);
        row.slider->setRange(0.0, 100.0, 0.1);
        row.slider->setTooltip(definition.description);
        addAndMakeVisible(*row.slider);
        break;
    }

    case SettingType::text:
    {
        row.textEditor = std::make_unique<juce::TextEditor>();
        row.textEditor->setComponentID(definition.key);
        row.textEditor->setTooltip(definition.description);
        addAndMakeVisible(*row.textEditor);
        break;
    }
    }

    addAndMakeVisible(*row.titleLabel);
    addAndMakeVisible(*row.descriptionLabel);

    settingRows.push_back(std::move(row));
}

void SettingsComponent::layoutRows(juce::Rectangle<int> area)
{
    const int rowHeight = 56;
    const int rowGap = 10;
    const int buttonSize = 40;
    const int buttonGap = 10;
    const int controlWidth = 100;

    for (auto& row : settingRows)
    {
        auto rowArea = area.removeFromTop(rowHeight);

        auto controlArea = rowArea.removeFromRight(controlWidth);

        auto titleArea = rowArea.removeFromTop(24);
        auto descriptionArea = rowArea;

        row.titleLabel->setBounds(titleArea);
        row.descriptionLabel->setBounds(descriptionArea);

        auto fullRowArea = rowArea;
        if (row.drawableButton != nullptr)
        {
            //auto buttonArea = controlArea.removeFromRight(buttonSize);
            //controlArea.removeFromRight(buttonGap);
            //row.drawableButton->setBounds(buttonArea.withSizeKeepingCentre(buttonSize, buttonSize));

            auto buttonBounds = juce::Rectangle<int>(buttonSize, buttonSize)
                .withCentre({ controlArea.getCentreX(), fullRowArea.getCentreY() });
            row.drawableButton->setBounds(buttonBounds);
        }

        if (row.slider != nullptr)
            row.slider->setBounds(controlArea);

        if (row.textEditor != nullptr)
            row.textEditor->setBounds(controlArea);

        area.removeFromTop(rowGap);
    }
}

void SettingsComponent::attachCallbacks()
{
    for (auto& row : settingRows)
    {
        auto key = row.key;
        auto type = row.type;

        if (row.drawableButton != nullptr)
        {
            row.drawableButton->onClick = [this, key, button = row.drawableButton.get()]()
                {
                    const bool value = button->getToggleState();

                    settings[key].value = value;

                    if (auto* rowPtr = findRow(key))
                        updateRowVisuals(*rowPtr);

                    saveSettings();

                    if (onSettingChanged)
                        onSettingChanged(key, value);
                };
        }

        if (row.slider != nullptr)
        {
            row.slider->onValueChange = [this, key, type, slider = row.slider.get()]()
                {
                    if (type == SettingType::integer)
                        settings[key].value = static_cast<int>(slider->getValue());
                    else if (type == SettingType::floating)
                        settings[key].value = slider->getValue();

                    saveSettings();

                    if (onSettingChanged)
                        onSettingChanged(key, settings[key].value);
                };
        }

        if (row.textEditor != nullptr)
        {
            row.textEditor->onTextChange = [this, key, editor = row.textEditor.get()]()
                {
                    settings[key].value = editor->getText();

                    saveSettings();

                    if (onSettingChanged)
                        onSettingChanged(key, settings[key].value);
                };
        }
    }
}

void SettingsComponent::syncRowsFromSettings()
{
    for (auto& row : settingRows)
    {
        switch (row.type)
        {
        case SettingType::boolean:
        {
            const bool value = getBoolSettingValue(row.key);

            if (row.drawableButton != nullptr) {
                row.drawableButton->setToggleState(value, juce::dontSendNotification);
                updateToggleStyle(*row.drawableButton, value);
            }

            updateRowVisuals(row);
            break;
        }

        case SettingType::integer:
        {
            if (row.slider != nullptr)
                row.slider->setValue(static_cast<double>(getIntSettingValue(row.key)),
                    juce::dontSendNotification);
            break;
        }

        case SettingType::floating:
        {
            if (row.slider != nullptr)
                row.slider->setValue(getFloatSettingValue(row.key),
                    juce::dontSendNotification);
            break;
        }

        case SettingType::text:
        {
            if (row.textEditor != nullptr)
                row.textEditor->setText(getTextSettingValue(row.key),
                    juce::dontSendNotification);
            break;
        }
        }
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
            settings[definition.key].value = obj->getProperty(definition.key);
    }
}

void SettingsComponent::saveSettings() const
{
    juce::DynamicObject::Ptr root = new juce::DynamicObject();

    for (const auto& definition : settingDefinitions)
    {
        auto it = settings.find(definition.key);
        const juce::var value = (it != settings.end()) ? it->second.value : definition.defaultValue;
        root->setProperty(definition.key, value);
    }

    getSettingsFile().replaceWithText(
        juce::JSON::toString(juce::var(root.get()), true)
    );
}

juce::var SettingsComponent::getSettingValue(const juce::String& key) const
{
    auto it = settings.find(key);
    return it != settings.end() ? it->second.value : juce::var();
}

bool SettingsComponent::getBoolSettingValue(const juce::String& key) const
{
    return static_cast<bool>(getSettingValue(key));
}

void SettingsComponent::setSettingValue(const juce::String& key, const juce::var& value)
{
    auto it = settings.find(key);
    if (it == settings.end())
        return;

    it->second.value = value;

    if (auto* row = findRow(key))
    {
        switch (row->type)
        {
        case SettingType::boolean:
            if (row->drawableButton != nullptr)
            {
                row->drawableButton->setToggleState(static_cast<bool>(value),
                    juce::dontSendNotification);
                updateToggleStyle(*row->drawableButton, static_cast<bool>(value));
            }

            updateRowVisuals(*row);
            break;

        case SettingType::integer:
            if (row->slider != nullptr)
                row->slider->setValue(static_cast<int>(value),
                    juce::dontSendNotification);
            break;

        case SettingType::floating:
            if (row->slider != nullptr)
                row->slider->setValue(static_cast<double>(value),
                    juce::dontSendNotification);
            break;

        case SettingType::text:
            if (row->textEditor != nullptr)
                row->textEditor->setText(value.toString(),
                    juce::dontSendNotification);
            break;
        }
    }

    saveSettings();

    if (onSettingChanged)
        onSettingChanged(key, value);
}

void SettingsComponent::setBoolSettingValue(const juce::String& key, bool value)
{
    setSettingValue(key, value);
}

void SettingsComponent::updateToggleStyle(juce::DrawableButton& button, bool isOn)
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

void SettingsComponent::updateRowVisuals(SettingRow& row)
{
    if (row.type != SettingType::boolean)
        return;

    const bool isOn = getBoolSettingValue(row.key);

    if (row.drawableButton != nullptr)
        updateToggleStyle(*row.drawableButton, isOn);

    row.titleLabel->setColour(
        juce::Label::textColourId,
        isOn ? juce::Colours::white : juce::Colours::lightgrey);

    row.descriptionLabel->setColour(
        juce::Label::textColourId,
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

int SettingsComponent::getIntSettingValue(const juce::String& key) const
{
    return static_cast<int>(getSettingValue(key));
}

double SettingsComponent::getFloatSettingValue(const juce::String& key) const
{
    return static_cast<double>(getSettingValue(key));
}

juce::String SettingsComponent::getTextSettingValue(const juce::String& key) const
{
    return getSettingValue(key).toString();
}