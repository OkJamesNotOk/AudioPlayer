/*
  ==============================================================================

    PlaylistPlayer.h
    Created: 18 Jan 2025 4:07:59pm
    Author:  OkJames

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "MainComponent.h"

#if JUCE_WINDOWS
#include <windows.h>
#include <shobjidl.h>
#include <commctrl.h>
#include <wrl/client.h>
#pragma comment(lib, "Ole32.lib")
#endif

//==============================================================================
class OtoDecksApplication : public JUCEApplication
{
public:
    //==============================================================================
    OtoDecksApplication() {}
    const String getApplicationName() override { return ProjectInfo::projectName; }
    const String getApplicationVersion() override { return ProjectInfo::versionString; }
    bool moreThanOneInstanceAllowed() override { return true; }

    //==============================================================================
    void initialise(const String& commandLine) override
    {
        mainWindow.reset(new MainWindow(getApplicationName()));
    }

    void shutdown() override
    {
        if (mainWindow != nullptr)
            mainWindow->saveWindowState();

        mainWindow = nullptr;
    }

    //==============================================================================
    void systemRequestedQuit() override
    {
        quit();
    }

    void anotherInstanceStarted(const String& commandLine) override
    {
    }

    //==============================================================================
    class MainWindow : public DocumentWindow
    {
    public:
        MainWindow(String name)
            : DocumentWindow(name,
                Desktop::getInstance().getDefaultLookAndFeel()
                .findColour(ResizableWindow::backgroundColourId),
                DocumentWindow::allButtons)
        {
            setUsingNativeTitleBar(true);
            setContentOwned(new MainComponent(), true);

            if (auto* main = getMainComponent())
            {
                main->onPlaybackStateChanged = [this](bool playing)
                    {
                        updateTaskbarPlayPauseButton(playing);
                    };
            }

#if JUCE_IOS || JUCE_ANDROID
            setFullScreen(true);
#else
            setResizable(true, true);
            setResizeLimits(320, 280, 2000, 2000);
            restoreWindowState();

            if (getWidth() <= 0 || getHeight() <= 0)
                centreWithSize(400, 600);
#endif

            setVisible(true);

#if JUCE_WINDOWS
            initialiseTaskbarButtons();
#endif
        }

        ~MainWindow() override
        {
#if JUCE_WINDOWS
            shutdownTaskbarButtons();
#endif
        }

        void closeButtonPressed() override
        {
            if (auto* main = getMainComponent())
                main->closePlaylistOnShutDown();

            saveWindowState();
            JUCEApplication::getInstance()->systemRequestedQuit();
        }

        juce::File getWindowStateFile() const
        {
            return juce::File::getSpecialLocation(juce::File::userApplicationDataDirectory)
                .getChildFile("OtoDecksWindowState.txt");
        }

        void saveWindowState()
        {
            getWindowStateFile().replaceWithText(getWindowStateAsString());
        }

        void restoreWindowState()
        {
            auto file = getWindowStateFile();

            if (file.existsAsFile())
                restoreWindowStateFromString(file.loadFileAsString());
        }

    private:
#if JUCE_WINDOWS
        enum ThumbButtonIds
        {
            thumbPrev = 1001,
            thumbPlayPause = 1002,
            thumbNext = 1003
        };

        Microsoft::WRL::ComPtr<ITaskbarList3> taskbarList;
        HWND hwnd = nullptr;
        UINT taskbarButtonCreatedMessage = 0;

        HICON hIconPrev = nullptr;
        HICON hIconPlay = nullptr;
        HICON hIconPause = nullptr;
        HICON hIconNext = nullptr;

        static constexpr UINT_PTR taskbarSubclassId = 1;


        HICON loadIconFromMemory(const void* data, int dataSize)
        {
            if (data == nullptr || dataSize <= 0)
                return nullptr;

            auto* bytes = static_cast<const unsigned char*>(data);

            struct IconDir
            {
                uint16_t reserved;
                uint16_t type;
                uint16_t count;
            };

            struct IconDirEntry
            {
                uint8_t  width;
                uint8_t  height;
                uint8_t  colourCount;
                uint8_t  reserved;
                uint16_t planes;
                uint16_t bitCount;
                uint32_t bytesInRes;
                uint32_t imageOffset;
            };

            if (dataSize < (int)sizeof(IconDir))
                return nullptr;

            auto* dir = reinterpret_cast<const IconDir*>(bytes);

            if (dir->reserved != 0 || dir->type != 1 || dir->count == 0)
                return nullptr;

            auto entriesOffset = sizeof(IconDir);
            auto entriesSize = dir->count * sizeof(IconDirEntry);

            if (dataSize < (int)(entriesOffset + entriesSize))
                return nullptr;

            auto* entries = reinterpret_cast<const IconDirEntry*>(bytes + entriesOffset);

            const IconDirEntry* bestEntry = nullptr;

            for (int i = 0; i < dir->count; ++i)
            {
                const auto& entry = entries[i];

                if (entry.imageOffset + entry.bytesInRes > (uint32_t)dataSize)
                    continue;

                if (bestEntry == nullptr)
                    bestEntry = &entry;
                else
                {
                    int bestScore = std::abs((bestEntry->width == 0 ? 256 : bestEntry->width) - 16)
                        + std::abs((bestEntry->height == 0 ? 256 : bestEntry->height) - 16);

                    int newScore = std::abs((entry.width == 0 ? 256 : entry.width) - 16)
                        + std::abs((entry.height == 0 ? 256 : entry.height) - 16);

                    if (newScore < bestScore)
                        bestEntry = &entry;
                }
            }

            if (bestEntry == nullptr)
                return nullptr;

            return CreateIconFromResourceEx(
                const_cast<PBYTE>(bytes + bestEntry->imageOffset),
                bestEntry->bytesInRes,
                TRUE,
                0x00030000,
                16,
                16,
                LR_DEFAULTCOLOR
            );
        }

        void initialiseTaskbarButtons()
        {
            if (auto* peer = getPeer())
                hwnd = static_cast<HWND>(peer->getNativeHandle());

            if (hwnd == nullptr)
                return;

            taskbarButtonCreatedMessage = RegisterWindowMessage(TEXT("TaskbarButtonCreated"));

            hIconPrev = loadIconFromMemory(BinaryData::skipbackcircle_ico,
                BinaryData::skipbackcircle_icoSize);

            hIconPlay = loadIconFromMemory(BinaryData::playcircle_ico,
                BinaryData::playcircle_icoSize);

            hIconPause = loadIconFromMemory(BinaryData::pausecircle_ico,
                BinaryData::pausecircle_icoSize);

            hIconNext = loadIconFromMemory(BinaryData::skipforwardcircle_ico,
                BinaryData::skipforwardcircle_icoSize);

            HRESULT hr = CoCreateInstance(CLSID_TaskbarList, nullptr,
                CLSCTX_INPROC_SERVER,
                IID_PPV_ARGS(&taskbarList));

            if (SUCCEEDED(hr) && taskbarList != nullptr)
            {
                taskbarList->HrInit();
                SetWindowSubclass(hwnd, taskbarSubclassProc, taskbarSubclassId,
                    reinterpret_cast<DWORD_PTR>(this));
            }
        }

        void addTaskbarThumbButtons()
        {
            if (taskbarList == nullptr || hwnd == nullptr)
                return;

            THUMBBUTTON buttons[3] = {};

            buttons[0].dwMask = THB_FLAGS | THB_TOOLTIP | THB_ICON;
            buttons[0].iId = thumbPrev;
            buttons[0].dwFlags = THBF_ENABLED;
            buttons[0].hIcon = hIconPrev;
            wcscpy_s(buttons[0].szTip, L"Previous");

            bool playing = false;
            if (auto* main = getMainComponent())
                playing = main->isPlaying();
            buttons[1].dwMask = THB_FLAGS | THB_TOOLTIP | THB_ICON;
            buttons[1].iId = thumbPlayPause;
            buttons[1].dwFlags = THBF_ENABLED;
            buttons[1].hIcon = playing ? hIconPause : hIconPlay;
            wcscpy_s(buttons[1].szTip, playing ? L"Pause" : L"Play");

            buttons[2].dwMask = THB_FLAGS | THB_TOOLTIP | THB_ICON;
            buttons[2].iId = thumbNext;
            buttons[2].dwFlags = THBF_ENABLED;
            buttons[2].hIcon = hIconNext;
            wcscpy_s(buttons[2].szTip, L"Next");

            taskbarList->ThumbBarAddButtons(hwnd, 3, buttons);
        }

        void shutdownTaskbarButtons()
        {
            if (hwnd != nullptr)
                RemoveWindowSubclass(hwnd, taskbarSubclassProc, taskbarSubclassId);

            if (hIconPrev) DestroyIcon(hIconPrev);
            if (hIconPlay) DestroyIcon(hIconPlay);
            if (hIconPause) DestroyIcon(hIconPause);
            if (hIconNext) DestroyIcon(hIconNext);

            hIconPrev = nullptr;
            hIconPlay = nullptr;
            hIconPause = nullptr;
            hIconNext = nullptr;

            taskbarList.Reset();
            hwnd = nullptr;
        }

        static LRESULT CALLBACK taskbarSubclassProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam,
            UINT_PTR, DWORD_PTR refData)
        {
            auto* window = reinterpret_cast<MainWindow*>(refData);

            if (window != nullptr)
            {
                if (message == window->taskbarButtonCreatedMessage)
                {
                    window->addTaskbarThumbButtons();
                    return 0;
                }

                if (message == WM_COMMAND && HIWORD(wParam) == THBN_CLICKED)
                {
                    if (auto* main = window->getMainComponent())
                    {
                        switch (LOWORD(wParam))
                        {
                        case thumbPrev:
                            main->taskbarPrevious();
                            return 0;

                        case thumbPlayPause:
                        {
                            bool playing = main->taskbarPlayPause();
                            window->updateTaskbarPlayPauseButton(playing);
                            return 0;
                        }

                        case thumbNext:
                            main->taskbarNext();
                            return 0;

                        default:
                            break;
                        }
                    }
                }
            }

            return DefSubclassProc(hWnd, message, wParam, lParam);
        }

        void updateTaskbarPlayPauseButton(bool playing)
        {
            if (taskbarList == nullptr || hwnd == nullptr)
                return;

            THUMBBUTTON button = {};
            button.dwMask = THB_ICON | THB_TOOLTIP;
            button.iId = thumbPlayPause;
            button.hIcon = playing ? hIconPause : hIconPlay;
            wcscpy_s(button.szTip, playing ? L"Pause" : L"Play");

            taskbarList->ThumbBarUpdateButtons(hwnd, 1, &button);
        }
#endif
        MainComponent* getMainComponent() const
        {
            return dynamic_cast<MainComponent*> (getContentComponent());
        }

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainWindow)
    };

private:
    std::unique_ptr<MainWindow> mainWindow;
};

//==============================================================================
// This macro generates the main() routine that launches the app.
START_JUCE_APPLICATION(OtoDecksApplication)