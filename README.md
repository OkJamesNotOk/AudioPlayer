Personal project built from a school project and further improved, app not really resource efficient though.

# Building the JUCE Project

This project includes both `.jucer` and CMake versions.

## Option 1: Build with Projucer and Visual Studio

1. Install **Visual Studio** and **Projucer**.
2. Open the `.jucer` file in **Projucer**.
3. Export or open the project in **Visual Studio**.
4. Build the project in either **Debug** or **Release** mode.

## Option 2: Build with CMake

You can also build the project from the command line using CMake:

```bash
cmake -B build .
cmake --build build --config Debug
cmake --build build --config Release
```

The executable should appear in:

```text
build/OtoDecks_artefacts
```

## Using CMake in Visual Studio Code

Another option is to open the folder containing the `CMakeLists.txt` file in **Visual Studio Code** and use the CMake extension to manage the build.

Useful links:

- https://code.visualstudio.com/
- https://github.com/microsoft/vscode-cmake-tools

## Generating IDE Projects with CMake

CMake can also generate IDE project files.

### Visual Studio 2022

```bash
cmake -G "Visual Studio 17 2022" -B build .
```

Then open the generated `.sln` file in the `build` folder.

### Xcode

```bash
cmake -G "Xcode" -B build .
```

Then open the generated Xcode project in the `build` folder.

## Option 3: Download from Release

A prebuilt version can be downloaded from the **Releases** section of this repository.

1. Go to the **Releases** page.
2. Download the latest available build.
3. Extract the downloaded file `.zip` file. It should be a `.zip` file.
4. Run the `.exe` from the extracted folder.

## Features

- Load and play audio files
- Drag and drop tracks from the playlist into the player
- Save and restore playback state between sessions
- Remember previously loaded tracks and playlist state
- Store user settings such as volume and playback speed
- Set loop start and end points for repeated playback
- Filter playlist and search for specific track

## Disclaimer

The Projucer and Visual Studio workflow has been used for this project The CMake version is included, but it has not been personally tested, so additional setup or adjustments may be required depending on the system and development environment.

This project was originally developed as a school project and later improved as a personal project. The released build may still have performance limitations and is not fully resource efficient.
