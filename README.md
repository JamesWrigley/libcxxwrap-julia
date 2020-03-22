# JlCxx

[![Build Status](https://travis-ci.org/JuliaInterop/libcxxwrap-julia.svg?branch=master)](https://travis-ci.org/JuliaInterop/libcxxwrap-julia)
[![Build status](https://ci.appveyor.com/api/projects/status/96h6josegra2ct2d?svg=true)](https://ci.appveyor.com/project/barche/libcxxwrap-julia)

This is the C++ library component of the [CxxWrap.jl](https://github.com/JuliaInterop/CxxWrap.jl) package, distributed as a regular CMake library
for use in other C++ projects. To build a Julia interface to a C++ library, you need to build against this library and supply the resulting library as a binary dependency to your Julia package. The `testlib-builder` directory contains a complete example of how to build and distribute these binaries, or you can use the [BinaryBuilder.jl](https://github.com/JuliaPackaging/BinaryBuilder.jl) wizard to generate the builder repository.

## Building libcxxwrap-julia

The main CMake option of interest is `Julia_PREFIX`, which should point to the Julia installation prefix you want to use, i.e. the directory containing the `bin` and `lib` directories and so on. On Linux or Mac, the sequence of commands to follow is:

```bash
git clone https://github.com/JuliaInterop/libcxxwrap-julia.git
mkdir libcxxwrap-julia-build
cd libcxxwrap-julia-build
cmake -DJulia_PREFIX=/home/user/julia-1.4.0 ../libcxxwrap-julia
cmake --build . --config Release
```

Instead of specifying the prefix, it is also possible to directly set the Julia executable, using:

```bash
cmake -DJulia_EXECUTABLE=/home/user/julia-1.4.0/bin/julia ../libcxxwrap-julia
```

Next, you can build your own code against this by setting the `JlCxx_DIR` CMake variable to the build directory (`libcxxwrap-julia-build`) used above, or add it to the `CMAKE_PREFIX_PATH` CMake variable.

### Using the compiled libcxxwrap-julia in CxxWrap

Since CxxWrap v0.10, binaries are managed through the `libcxxwrap_julia_jll` JLL package. To use your own binaries, you need to set up an overrides file, by default at `~/.julia/artifacts/Overrides.toml`or `%HOMEDRIVE%\%HOMEPATH%\.julia\artifacts\Overrides.toml`, with the following contents:

```toml
[3eaa8342-bff7-56a5-9981-c04077f7cee7]
libcxxwrap_julia = "/path/to/libcxxwrap-julia-build"
```

The file can be generated automatically using the `OVERRIDES_PATH`, `OVERRIDE_ROOT` and `APPEND_OVERRIDES_TOML` Cmake options, with the caveat that each CMake run will append again to the file and make it invalid, i.e. this is mostly intended for use on CI (see the appveyor and travis files for examples).

### Building on Windows

On Windows, building is easiest with [Visual Studio 2019](https://visualstudio.microsoft.com/vs/), for which the Community Edition with C++ support is a free download. You can clone the `https://github.com/JuliaInterop/libcxxwrap-julia.git` repository using the [built-in git support](https://docs.microsoft.com/en-us/visualstudio/get-started/tutorial-open-project-from-repo?view=vs-2019), and configure the `Julia_PREFIX` option from the built-in CMake support. See the [Visual Studio docs](https://docs.microsoft.com/en-us/cpp/build/customize-cmake-settings?view=vs-2019) for more info.

See the [CxxWrap.jl](https://github.com/JuliaInterop/CxxWrap.jl) README for more info on the API.

## Binaries for the master version

Binaries for the master version are usually available at https://github.com/barche/libcxxwrap_julia_jll.jl, you can install them (in Pkg mode, hit `]`) using:

```
add https://github.com/barche/libcxxwrap_julia_jll.jl.git
```