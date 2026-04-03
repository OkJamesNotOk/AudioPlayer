/*
  ==============================================================================

    FFmpegAudioFormat.cpp
    Created: 2 Apr 2026 11:33:43pm
    Author:  OkJames

  ==============================================================================
*/

#include "FFmpegAudioFormat.h"

//==============================================================================
FFmpegAudioFormat::FFmpegAudioFormat()
    : juce::AudioFormat(
        "FFmpeg",
        {
            ".aac",
            ".m4a",
            ".mp4",
            ".mov",
            ".flac",
            ".ogg",
            ".opus",
            ".wma",
            ".aiff",
            ".aif",
            ".alac",
            ".ac3",
            ".caf",
            ".amr",
            ".ape",
            ".mp3",
            ".wav"
        })
{
}

FFmpegAudioFormat::~FFmpegAudioFormat() {

}

juce::Array<int> FFmpegAudioFormat::getPossibleSampleRates()
{
    return {};
}

juce::Array<int> FFmpegAudioFormat::getPossibleBitDepths()
{
    return {};
}

bool FFmpegAudioFormat::canDoStereo()
{
    return true;
}

bool FFmpegAudioFormat::canDoMono()
{
    return true;
}

bool FFmpegAudioFormat::isCompressed()
{
    return true;
}

juce::AudioFormatReader* FFmpegAudioFormat::createReaderFor(juce::InputStream* sourceStream,
    bool deleteStreamIfOpeningFails)
{
    if (sourceStream == nullptr)
        return nullptr;

    std::unique_ptr<juce::InputStream> streamOwner(sourceStream);

    auto* fileStream = dynamic_cast<juce::FileInputStream*>(sourceStream);

    if (fileStream == nullptr)
    {
        if (!deleteStreamIfOpeningFails)
            streamOwner.release();

        return nullptr;
    }

    const juce::File audioFile = fileStream->getFile();

    if (!audioFile.existsAsFile())
    {
        if (!deleteStreamIfOpeningFails)
            streamOwner.release();

        return nullptr;
    }

    const auto extension = audioFile.getFileExtension().toLowerCase();

    if (!isFileExtensionSupported(extension))
    {
        if (!deleteStreamIfOpeningFails)
            streamOwner.release();

        return nullptr;
    }

    auto* reader = new FFmpegAudioFormatReader(streamOwner.release(), audioFile);

    if (!reader->openedOk())
    {
        delete reader;
        return nullptr;
    }

    return reader;
}

juce::StringArray FFmpegAudioFormat::getQualityOptions()
{
    return {};
}

bool FFmpegAudioFormat::isFileExtensionSupported(const juce::String& extension) const
{
    static const juce::StringArray supportedExtensions
    {
        ".aac",
        ".m4a",
        ".mp4",
        ".mov",
        ".flac",
        ".ogg",
        ".opus",
        ".wma",
        ".aiff",
        ".aif",
        ".alac",
        ".ac3",
        ".caf",
        ".amr",
        ".ape",
        ".mp3",
        ".wav"
    };

    return supportedExtensions.contains(extension.toLowerCase());
}

std::unique_ptr<juce::AudioFormatWriter> FFmpegAudioFormat::createWriterFor(
    std::unique_ptr<juce::OutputStream>& streamToWriteTo,
    const juce::AudioFormatWriterOptions& options)
{
    juce::ignoreUnused(streamToWriteTo, options);
    return nullptr;
}