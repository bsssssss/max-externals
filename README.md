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

To generate the projects on osx
```bash
cmake -G Xcode ..
cmake --build .
```

For building on windows or if facing problems check: https://github.com/Cycling74/max-sdk
