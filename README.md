To compile
cmake CMakeLists.txt

To rebuild:
cmake --build ./build

To run:
.build/cpu-rasterizer [width:(int)] [height:(int)] [model:(String path)] [scale:(float)]


Note: This has 2 dependencies, which the project discovers using its CMakeLists.txt. SDL3 and glm must first be installed to build.
