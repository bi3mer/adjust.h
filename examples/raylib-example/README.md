# Bouncing Ball Example with CMake

This shows a working example of Adjust using the [bouncing ball example](https://www.raylib.com/examples/shapes/loader.html?name=shapes_bouncing_ball) from [raylib](https://www.raylib.com/index.html) that is built with [CMake](https://cmake.org/).

## Building and Running
### Debug Mode
```bash
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Debug ..
cmakeke --build .
./bouncing-ball
```

### Production Mode

```bash
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
cmakeke --build .
./bouncing-ball
```

## Demo

There are three *adjustable* variables in the [main.c](./main.c): `ballRadius`, `gravity`, and `spaceString`. Modify their values in the code and save. You'll see updates live! You can also find all the modifications in [main.c](./main.c) by searching for `// m`.
