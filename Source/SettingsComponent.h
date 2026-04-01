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

    juce::var getSettingValue(const juce::String& key) const;
    void setSettingValue(const juce::String& key, const juce::var& value);

    bool getBoolSettingValue(const juce::String& key) const;
    void setBoolSettingValue(const juce::String& key, bool value);

    std::function<void(const juce::String&, const juce::var&)> onSettingChanged;

    int getSettingsCount() const;
    int getSettingsHeight() const;

private:
    juce::File getSettingsFile() const;

    enum class SettingType
    {
        boolean,
        integer,
        floating,
        text
    };

    struct SettingDefinition
    {
        juce::String key;
        juce::String title;
        juce::String description;
        SettingType type = SettingType::boolean;
        juce::var defaultValue = false;
    };

    struct SettingState
    {
        SettingType type = SettingType::boolean;
        juce::var value = false;
    };

    struct SettingRow
    {
        juce::String key;
        SettingType type = SettingType::boolean;

        std::unique_ptr<juce::Label> titleLabel;
        std::unique_ptr<juce::Label> descriptionLabel;

        std::unique_ptr<juce::DrawableButton> drawableButton;
        std::unique_ptr<juce::Slider> slider;
        std::unique_ptr<juce::TextEditor> textEditor;
    };

    void initialiseDefinitions();
    void initialiseRows();
    void createSettingRow(const SettingDefinition& definition);
    void layoutRows(juce::Rectangle<int> area);
    void syncRowsFromSettings();
    void attachCallbacks();

    void updateToggleStyle(juce::DrawableButton& button, bool isOn);

    void updateRowVisuals(SettingRow& row);

    int getIntSettingValue(const juce::String& key) const;
    double getFloatSettingValue(const juce::String& key) const;
    juce::String getTextSettingValue(const juce::String& key) const;

    const SettingDefinition* findDefinition(const juce::String& key) const;
    SettingRow* findRow(const juce::String& key);

    std::vector<SettingDefinition> settingDefinitions;
    std::map<juce::String, SettingState> settings;
    std::vector<SettingRow> settingRows;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SettingsComponent)
};
