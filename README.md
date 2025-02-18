# snatchbot 🤖
Computer vision application which plays the game "Snatch".


## Features

- 🎥 **Real-time Tile Recognition**: Accurately recognize and interpret Bananagrams tiles in real-time using computer vision techniques.
- ⚡ **Instant Word Suggestions**: Generate a list of possible words to play based on the recognized tiles, updated in real-time as new tiles appear.


## Installation

1. Clone the repository
   ```cmd
   git clone 
   cd snatchbot
   ```
2. Create `vcpkg.exe`.
  ```cmd
  git clone https://github.com/microsoft/vcpkg.git
  vcpkg\bootstrap-vcpkg.bat
  ```
3. Run `cmake` step, specifying whichever generator or build system you have on your machine.
   ```cmd
   cmake -B build -S . -G "Visual Studio 17 2022" -A x64
   ```
   Alternatively, if using Visual Studio with the `cmake` tools extension, you can just open the root folder in Visual Studio. `CMakeSettings.json` tells Visual Studio which build system you want to use, which configurations etc.
4. Build the project
   ```cmd
   cmake --build build --config Debug
   ```


## Basic Usage

## License

## Tests
