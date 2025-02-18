# vcpkg-cmake-template
Template for building a project with `vcpkg` and `cmake`.

1. Create `vcpkg.exe`.
```cmd
git clone https://github.com/microsoft/vcpkg.git
vcpkg\bootstrap-vcpkg.bat
```
2. Run `cmake` step, specifying whichever generator or build system you have on your machine.
   ```cmd
   cmake -B build -S . -G "Visual Studio 17 2022" -A x64
   ```
   Alternatively, if using Visual Studio with the `cmake` tools extension, you can just open the root folder in Visual Studio. `CMakeSettings.json` tells Visual Studio which build system you want to use, which configurations etc.
3. Build the project
   ```cmd
   cmake --build build --config Debug
   ```
