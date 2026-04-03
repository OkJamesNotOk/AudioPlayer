/*
  ==============================================================================

    FFmpegAudioFormatReader.h
    Created: 2 Apr 2026 11:33:56pm
    Author:  OkJames

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
extern "C"
{
    #include <libavformat/avformat.h>
    #include <libavcodec/avcodec.h>
    #include <libavutil/avutil.h>
    #include <libavutil/channel_layout.h>
    #include <libswresample/swresample.h>
}

//==============================================================================
/*
*/
class FFmpegAudioFormatReader : public juce::AudioFormatReader
{
public:
    FFmpegAudioFormatReader(juce::InputStream* in, const juce::File& sourceFile);
    ~FFmpegAudioFormatReader() override;

    bool openedOk() const noexcept;

    bool readSamples(int* const* destSamples,
        int numDestChannels,
        int startOffsetInDestBuffer,
        juce::int64 startSampleInFile,
        int numSamples) override;

private:
    juce::File file;
    bool ok = false;

    struct AVFormatContext* formatContext = nullptr;
    struct AVCodecContext* codecContext = nullptr;
    const struct AVCodec* codec = nullptr;
    int audioStreamIndex = -1;

    struct SwrContext* swrContext = nullptr;
    struct AVPacket* packet = nullptr;
    struct AVFrame* frame = nullptr;

    juce::AudioBuffer<float> decodedBuffer;
    juce::int64 currentSamplePosition = 0;
    juce::int64 decodedSamples = 0;
    int decodedBufferStart = 0;

    bool decodeNextFrame();
    bool seekToSample(juce::int64 samplePosition);
};
