/*
  ==============================================================================

    ScrollLabel.cpp
    Created: 18 Feb 2025 8:53:05pm
    Author:  OkJames
    auto scrolling label

  ==============================================================================
*/

#include "ScrollLabel.h"

namespace
{
    static bool isAllowedWhitespace(juce::juce_wchar c)
    {
        return c == '\n' || c == '\r' || c == '\t' || c == ' ';
    }

    static bool isInvisibleFormattingChar(juce::juce_wchar c)
    {
        return c == 0x200B || c == 0x200C || c == 0x200D ||
            c == 0x200E || c == 0x200F ||
            c == 0x202A || c == 0x202B || c == 0x202C ||
            c == 0x202D || c == 0x202E ||
            c == 0x2060 || c == 0xFEFF ||
            (c >= 0xFE00 && c <= 0xFE0F);
    }

    static bool isLikelyProblemChar(juce::juce_wchar c)
    {
        const auto uc = static_cast<uint32_t>(c);

        if (uc == 0xFFFD)
            return true;

        if (uc < 32 && !isAllowedWhitespace(c))
            return true;

        if (isInvisibleFormattingChar(c))
            return true;

        if (uc > 0xFFFF)
            return true;

        if ((uc >= 0x2600 && uc <= 0x27BF) ||
            (uc >= 0x2300 && uc <= 0x23FF))
            return true;

        return false;
    }

    static juce::String sanitiseForLabel(const juce::String& input)
    {
        juce::String cleaned;
        constexpr juce::juce_wchar square = 0x25A1;

        for (auto it = input.getCharPointer(); !it.isEmpty(); ++it)
        {
            auto c = *it;

            if (isLikelyProblemChar(c))
            {
                cleaned << square;
                continue;
            }

            cleaned << c;
        }

        return cleaned;
    }
}

ScrollLabel::ScrollLabel()
{
    setFont(juce::Font(juce::Font::getDefaultSansSerifFontName(), 15.0f, juce::Font::plain));
    updateTimerState();
}

//void ScrollLabel::setText(const juce::String& str, juce::NotificationType notif)
//{
//    juce::String safeStr = str.removeCharacters(juce::String::charToString((juce::juce_wchar)0xfffd));
//
//    juce::String cleaned;
//    for (auto it = safeStr.getCharPointer(); !it.isEmpty(); ++it)
//    {
//        auto c = *it;
//        if (c >= 32 || c == '\n' || c == '\r' || c == '\t')
//            cleaned << c;
//    }
//
//    juce::Label::setText(cleaned, notif);
//    width = getFont().getStringWidth(cleaned);
//    distTravelled = 0;
//}

//void ScrollLabel::setText(const juce::String& str, juce::NotificationType notif)
//{
//    juce::String cleaned;
//
//    for (auto it = str.getCharPointer(); !it.isEmpty(); ++it)
//    {
//        auto c = *it;
//
//        // remove invalid replacement char
//        if (c == (juce::juce_wchar)0xFFFD)
//            continue;
//
//        // remove control chars except common whitespace
//        if (c < 32 && c != '\n' && c != '\r' && c != '\t')
//            continue;
//
//        // remove invisible/problem formatting chars
//        if (c == 0x200B || c == 0x200C || c == 0x200D ||
//            c == 0x200E || c == 0x200F ||
//            c == 0x202A || c == 0x202B || c == 0x202C || c == 0x202D || c == 0x202E ||
//            c == 0x2060 || c == 0xFEFF)
//            continue;
//
//        // replace likely emoji / unsupported supplementary-plane chars
//        if ((uint32_t)c > 0xFFFF)
//        {
//            cleaned << juce::String("[?]");
//            continue;
//        }
//
//        cleaned << c;
//    }
//
//    juce::Label::setText(cleaned, notif);
//    width = getFont().getStringWidth(cleaned);
//    distTravelled = 0;
//    updateTimerState();
//}

void ScrollLabel::setText(const juce::String& str, juce::NotificationType notif)
{
    const auto cleaned = sanitiseForLabel(str);

    juce::Label::setText(cleaned, notif);
    width = getFont().getStringWidth(cleaned);
    distTravelled = 0;
    updateTimerState();
}

void ScrollLabel::timerCallback()
{
    if (!scrollingEnabled || width <= getWidth())
        return;

    distTravelled -= 2;
    if (-distTravelled > width) {
        distTravelled = getWidth();
    }

    repaint();
}

void ScrollLabel::paint(juce::Graphics& g)
{
    g.setColour(juce::Colours::grey);
    //g.drawRect(getLocalBounds().reduced(1), 1);

    if (width > getWidth()) {
        g.setColour(findColour(juce::Label::textColourId));
        g.setFont(getFont());
        g.drawText(getText(), distTravelled, 0, width, getHeight(), getJustificationType());
        //repaint();
    }
    else {
        juce::Label::paint(g);
    }
}

void ScrollLabel::setScrollingEnabled(bool shouldScroll)
{
    scrollingEnabled = shouldScroll;
    updateTimerState();
    repaint();
}

void ScrollLabel::updateTimerState()
{
    if (scrollingEnabled && width > getWidth())
        startTimerHz(30);
    else
        stopTimer();
}

void ScrollLabel::resized()
{
    updateTimerState();
}