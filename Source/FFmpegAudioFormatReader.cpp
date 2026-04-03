/*
  ==============================================================================

    FFmpegAudioFormatReader.cpp
    Created: 2 Apr 2026 11:33:56pm
    Author:  OkJames

  ==============================================================================
*/

#include "FFmpegAudioFormatReader.h"

//==============================================================================
FFmpegAudioFormatReader::FFmpegAudioFormatReader(juce::InputStream* in,
    const juce::File& sourceFile)
    : juce::AudioFormatReader(in, "FFmpeg"),
    file(sourceFile)
{
    bitsPerSample = 32;
    usesFloatingPointData = true;
    sampleRate = 0.0;
    numChannels = 0;
    lengthInSamples = 0;

    if (!file.existsAsFile())
        return;

    auto path = file.getFullPathName().toRawUTF8();

    if (avformat_open_input(&formatContext, path, nullptr, nullptr) < 0)
        return;

    if (avformat_find_stream_info(formatContext, nullptr) < 0)
        return;

    audioStreamIndex = av_find_best_stream(formatContext, AVMEDIA_TYPE_AUDIO, -1, -1, &codec, 0);
    if (audioStreamIndex < 0 || codec == nullptr)
        return;

    codecContext = avcodec_alloc_context3(codec);
    if (codecContext == nullptr)
        return;

    auto* stream = formatContext->streams[audioStreamIndex];

    if (avcodec_parameters_to_context(codecContext, stream->codecpar) < 0)
        return;

    if (avcodec_open2(codecContext, codec, nullptr) < 0)
        return;

    sampleRate = static_cast<double>(codecContext->sample_rate);
    numChannels = static_cast<unsigned int>(codecContext->ch_layout.nb_channels > 0
        ? codecContext->ch_layout.nb_channels
        : 2);

    if (stream->duration > 0 && stream->time_base.den != 0)
    {
        double durationSeconds = stream->duration * av_q2d(stream->time_base);
        lengthInSamples = static_cast<juce::int64>(durationSeconds * sampleRate);
    }
    else if (formatContext->duration > 0)
    {
        double durationSeconds = static_cast<double>(formatContext->duration) / AV_TIME_BASE;
        lengthInSamples = static_cast<juce::int64>(durationSeconds * sampleRate);
    }

    packet = av_packet_alloc();
    frame = av_frame_alloc();

    if (packet == nullptr || frame == nullptr)
        return;

    AVChannelLayout outLayout;
    av_channel_layout_default(&outLayout, (int)numChannels);

    if (swr_alloc_set_opts2(
        &swrContext,
        &outLayout,
        AV_SAMPLE_FMT_FLTP,
        codecContext->sample_rate,
        &codecContext->ch_layout,
        codecContext->sample_fmt,
        codecContext->sample_rate,
        0,
        nullptr) < 0)
    {
        av_channel_layout_uninit(&outLayout);
        return;
    }

    av_channel_layout_uninit(&outLayout);

    if (swrContext == nullptr)
        return;

    if (swr_init(swrContext) < 0)
        return;

    ok = true;
}

FFmpegAudioFormatReader::~FFmpegAudioFormatReader()
{
    if (frame != nullptr)
        av_frame_free(&frame);

    if (packet != nullptr)
        av_packet_free(&packet);

    if (swrContext != nullptr)
        swr_free(&swrContext);

    if (codecContext != nullptr)
        avcodec_free_context(&codecContext);

    if (formatContext != nullptr)
        avformat_close_input(&formatContext);
}

bool FFmpegAudioFormatReader::readSamples(int* const* destSamples,
    int numDestChannels,
    int startOffsetInDestBuffer,
    juce::int64 startSampleInFile,
    int numSamples)
{
    if (startSampleInFile != currentSamplePosition) {
        if (!seekToSample(startSampleInFile)) return false;
    }

    int samplesWritten = 0;
    while (samplesWritten < numSamples) {
        if (decodedBufferStart >= decodedBuffer.getNumSamples()) {
            if (!decodeNextFrame()) break;
            decodedBufferStart = 0;
        }

        const int available = decodedBuffer.getNumSamples() - decodedBufferStart;
        const int toCopy = juce::jmin(numSamples - samplesWritten, available);

        //for (int ch = 0; ch < numDestChannels; ++ch) {
        //    if (destSamples[ch] == nullptr) continue;
        //    int* dst = destSamples[ch] + startOffsetInDestBuffer + samplesWritten;
        //    const int srcCh = juce::jmin(ch, (int)numChannels - 1);
        //    const float* src = decodedBuffer.getReadPointer(srcCh, decodedBufferStart);

        //    for (int i = 0; i < toCopy; ++i) {
        //        //dst[i] = (int)(juce::jlimit(-1.0f, 1.0f, src[i]) * 2147483647.0f);
        //        float* floatDest = reinterpret_cast<float*>(destSamples[ch] + startOffsetInDestBuffer + samplesWritten);
        //        floatDest[i] = src[i];
        //    }
        //}

        for (int ch = 0; ch < numDestChannels; ++ch) {
            if (destSamples[ch] == nullptr) continue;

            // 1. Cast the base pointer to float
            float* floatDest = reinterpret_cast<float*>(destSamples[ch]);

            // 2. Calculate the single correct write position
            const int destWritePos = startOffsetInDestBuffer + samplesWritten;

            const int srcCh = juce::jmin(ch, (int)numChannels - 1);
            const float* src = decodedBuffer.getReadPointer(srcCh, decodedBufferStart);

            for (int i = 0; i < toCopy; ++i) {
                floatDest[destWritePos + i] = src[i]; // Fixed indexing
            }
        }

        decodedBufferStart += toCopy;
        samplesWritten += toCopy;
        currentSamplePosition += toCopy;
    }
    return true;
}

bool FFmpegAudioFormatReader::openedOk() const noexcept
{
    return ok;
}

bool FFmpegAudioFormatReader::decodeNextFrame()
{
    //decodedBuffer.setSize((int)numChannels, 0);

    while (av_read_frame(formatContext, packet) >= 0)
    {
        if (packet->stream_index != audioStreamIndex)
        {
            av_packet_unref(packet);
            continue;
        }

        if (avcodec_send_packet(codecContext, packet) < 0)
        {
            av_packet_unref(packet);
            return false;
        }

        av_packet_unref(packet);

        const int ret = avcodec_receive_frame(codecContext, frame);

        if (ret == AVERROR(EAGAIN))
            continue;

        if (ret < 0)
            return false;

        const int outSamples = swr_get_out_samples(swrContext, frame->nb_samples);
        if (outSamples <= 0)
            return false;

        decodedBuffer.setSize((int)numChannels, outSamples);

        uint8_t* outData[AV_NUM_DATA_POINTERS] = {};
        for (unsigned int ch = 0; ch < numChannels; ++ch)
            outData[ch] = reinterpret_cast<uint8_t*>(decodedBuffer.getWritePointer((int)ch));

        const int converted = swr_convert(
            swrContext,
            outData,
            outSamples,
            (const uint8_t**)frame->extended_data,
            frame->nb_samples);

        if (converted < 0)
            return false;

        decodedBuffer.setSize((int)numChannels, converted, true, true, true);
        return true;
    }

    return false;
}

bool FFmpegAudioFormatReader::seekToSample(juce::int64 samplePosition)
{
    int64_t timestamp = av_rescale_q(
        samplePosition,
        AVRational{ 1, (int)sampleRate },
        formatContext->streams[audioStreamIndex]->time_base);

    if (av_seek_frame(formatContext, audioStreamIndex, timestamp, AVSEEK_FLAG_BACKWARD) < 0)
        return false;

    avcodec_flush_buffers(codecContext);
    if (swrContext != nullptr)
        swr_init(swrContext);

    decodedBuffer.setSize((int)numChannels, 0);
    decodedBufferStart = 0;
    currentSamplePosition = samplePosition;
    return true;
}