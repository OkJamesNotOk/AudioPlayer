/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic startup code for a JUCE application.

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

#if JUCE_IOS || JUCE_ANDROID
            setFullScreen(true);
#else
            setResizable(true, true);
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

        static constexpr UINT_PTR taskbarSubclassId = 1;

        void initialiseTaskbarButtons()
        {
            if (auto* peer = getPeer())
                hwnd = static_cast<HWND>(peer->getNativeHandle());

            if (hwnd == nullptr)
                return;

            taskbarButtonCreatedMessage = RegisterWindowMessage(TEXT("TaskbarButtonCreated"));

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

            buttons[0].dwMask = THB_FLAGS | THB_TOOLTIP;
            buttons[0].iId = thumbPrev;
            buttons[0].dwFlags = THBF_ENABLED;
            wcscpy_s(buttons[0].szTip, L"Previous");

            buttons[1].dwMask = THB_FLAGS | THB_TOOLTIP;
            buttons[1].iId = thumbPlayPause;
            buttons[1].dwFlags = THBF_ENABLED;
            wcscpy_s(buttons[1].szTip, L"Play / Pause");

            buttons[2].dwMask = THB_FLAGS | THB_TOOLTIP;
            buttons[2].iId = thumbNext;
            buttons[2].dwFlags = THBF_ENABLED;
            wcscpy_s(buttons[2].szTip, L"Next");

            taskbarList->ThumbBarAddButtons(hwnd, 3, buttons);
        }

        void shutdownTaskbarButtons()
        {
            if (hwnd != nullptr)
                RemoveWindowSubclass(hwnd, taskbarSubclassProc, taskbarSubclassId);

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
                        case thumbPrev:      main->taskbarPrevious();  return 0;
                        case thumbPlayPause: main->taskbarPlayPause(); return 0;
                        case thumbNext:      main->taskbarNext();      return 0;
                        default: break;
                        }
                    }
                }
            }

            return DefSubclassProc(hWnd, message, wParam, lParam);
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