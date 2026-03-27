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

## Disclaimer

The Projucer and Visual Studio workflow has been used for this project.

The CMake version is included, but it has not been personally tested, so additional setup or adjustments may be required depending on the system and development environment.