<img width="2560" height="1920" alt="2026-06-30 17 03 37" src="https://github.com/user-attachments/assets/057e8da9-9f4f-4b80-8142-37c3c044cf7a" />
To compile
cmake CMakeLists.txt

To rebuild:
cmake --build ./build

To run:
.build/cpu-rasterizer [width:(int)] [height:(int)] [model:(String path)] [scale:(float)]


Note: This has 2 dependencies, which the project discovers using its CMakeLists.txt. SDL3 and glm must first be installed to build.
