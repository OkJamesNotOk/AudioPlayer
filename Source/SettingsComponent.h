/*
  ==============================================================================

    SettingsComponent.h
    Created: 30 Mar 2026 2:24:40pm
    Author:  OkJames

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include <map>
#include <vector>

//==============================================================================
/*
*/
class SettingsComponent  : public juce::Component
{
public:
    SettingsComponent();
    ~SettingsComponent() override;

    void paint (juce::Graphics&) override;
    void resized() override;

    void loadSettings();
    void saveSettings() const;

    bool getSettingValue(const juce::String& key) const;
    void setSettingValue(const juce::String& key, bool value);

    std::function<void(const juce::String&, bool)> onSettingChanged;

    int getSettingsCount() const;
    int getSettingsHeight() const;

private:
    juce::File getSettingsFile() const;

    struct SettingDefinition
    {
        juce::String key;
        juce::String title;
        juce::String description;
        bool defaultValue = false;
    };

    struct SettingState
    {
        bool value = false;
    };

    struct SettingRow
    {
        juce::String key;
        std::unique_ptr<juce::Label> titleLabel;
        std::unique_ptr<juce::Label> descriptionLabel;
        std::unique_ptr<juce::DrawableButton> button;
    };

    void initialiseDefinitions();
    void initialiseRows();
    void createSettingRow(const SettingDefinition& definition);
    void layoutRows(juce::Rectangle<int> area);
    void syncButtonsFromSettings();
    void attachCallbacks();

    void updateButtonStyle(juce::DrawableButton& button, bool isOn);
    void updateRowVisuals(SettingRow& row, bool isOn);

    const SettingDefinition* findDefinition(const juce::String& key) const;
    SettingRow* findRow(const juce::String& key);

    std::vector<SettingDefinition> settingDefinitions;
    std::map<juce::String, SettingState> settings;
    std::vector<SettingRow> settingRows;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SettingsComponent)
};
