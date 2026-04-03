/*
  ==============================================================================

    FFmpegAudioFormat.h
    Created: 2 Apr 2026 11:33:43pm
    Author:  OkJames

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "FFmpegAudioFormatReader.h"

class FFmpegAudioFormat : public juce::AudioFormat
{
public:
    FFmpegAudioFormat();
    ~FFmpegAudioFormat() override;

    juce::Array<int> getPossibleSampleRates() override;
    juce::Array<int> getPossibleBitDepths() override;

    bool canDoStereo() override;
    bool canDoMono() override;
    bool isCompressed() override;

    juce::AudioFormatReader* createReaderFor(juce::InputStream* sourceStream,
        bool deleteStreamIfOpeningFails) override;

    std::unique_ptr<juce::AudioFormatWriter> createWriterFor(
        std::unique_ptr<juce::OutputStream>& streamToWriteTo,
        const juce::AudioFormatWriterOptions& options) override;

    juce::StringArray getQualityOptions() override;

private:
    bool isFileExtensionSupported(const juce::String& extension) const;
};