This repo has a lot of moving parts, once I achieve something I will come back to clean the readme

TODO: Also do make a doc for this




These two will build your build/debug & build/release

To run release:
cmake -S . -B build/release -DCMAKE_BUILD_TYPE=Release
cmake --build build/release -j

To run debugging:
cmake -S . -B build/debug -DCMAKE_BUILD_TYPE=Debug
cmake --build build/debug -j

Release for checking how fast and debug is for, obvouisly, debugging

and then just run cmake --build build/ to reconfig
and then run ./build/debug/replay or ./build/debug/test

