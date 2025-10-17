# max-externals

Source code and mxo's for my Max objects

### Build from source

Clone the repository with submodules

```bash
git clone --recurse-submodules https://github.com/bsssssss/max-externals.git
cd max-externals
```

Then create a build directory and cd into it

```bash
mkdir build
cd build
```

To generate the projects I use Ninja so compile_commands.json can be generated

```bash
cmake -G "Ninja" ..
cmake --build .
cmake --build . --target <project_name> # To build a specific project
```

If the CMakeCache.txt file is messed up (for example when directories have
changed), delete everything in the build dir.
