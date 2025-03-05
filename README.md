# snatchbot 🤖
See. Solve. Snatch. A real-time computer vision system that recognizes letter tiles and suggests the best possible moves on the fly.

TODO: Insert GIF here

## Features ✨
- 🎥 **Real-time Tile Recognition** – Uses contour detection to instantly identify letter tiles.
- 🧠 **AI-Powered Letter Recognition** – Extracts characters from detected tiles with OCR technology.
- ⚡ **Smart Word Suggestions** – Finds playable words with graph-based algorithms and updates them dynamically as new tiles appear.

## How It Works ⚙️
The system uses **contour detection** to identify letter tiles, followed by **OCR technology** (Tesseract) for character recognition. Once the tiles are detected, a **graph-based algorithm** forms connected words, and a **brute-force solver** finds the best possible words to play. Here’s a quick breakdown:
- Contour detection finds the edges of tiles.
- OCR (Tesseract) extracts the letters from those tiles.
- Graph algorithms link letters to form valid words.
- The system uses a brute-force solver to generate the best snatchable words.

TODO: Insert Screenshots/Visuals here (showing the recognition process and word suggestions)

## Rules of Snatch
There are many variations of "snatch" (sometimes known as [Anagrams](https://en.wikipedia.org/wiki/Anagrams_(game))), but the version we play at home is as follows.
1. **Tile Pool Setup**: All tiles are placed face down in a pool at the center of the table. Players take turns flipping over tiles.
2. **Word Formation**: When a player spots a word of three or more letters, they can form it in two ways:
   - **Using face-up tiles** from the pool to create a valid word.
   - **Stealing words** by combining one or more face-up tiles from the pool with a player's existing word. For example, combining the word **CART** with a **K** from the pool to form **TRACK**. Players can also extend their own words with tiles from the pool.
3. **Calling "Snatch!"**: As soon as a player spots a word, they **call it immediately** and place the word in front of them. The game continues with players flipping additional tiles.
4. **Minimum Word Length**: All words must be at **least three letters long**.
5. **Game End & Scoring**: The game ends when all tiles are face up and no further words can be formed. Players score based on the number of letter tiles they have.

## Quick Start 🚀

TODO: Improve this section, perhaps make it more concise

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

## License

TODO: Add MIT license

## Tests

TODO: Fill in this section
