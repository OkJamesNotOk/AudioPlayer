/*
  ==============================================================================

    ImageHelper.h
    Created: 29 Mar 2026 8:31:25pm
    Author:  OkJames

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
*/

namespace ImageHelpers
{
    inline juce::Image loadPngFromBinaryData(const void* data, int size)
    {
        juce::MemoryInputStream stream(data, static_cast<size_t>(size), false);
        return juce::PNGImageFormat::loadFrom(stream);
    }

    inline std::unique_ptr<juce::Drawable> makeDrawableFromImage(const juce::Image& image)
    {
        auto drawable = std::make_unique<juce::DrawableImage>();
        drawable->setImage(image);
        return drawable;
    }
}
