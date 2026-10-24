<img width="759" height="687" alt="Screenshot 2026-07-09 at 1 13 35 AM" src="https://github.com/user-attachments/assets/5018dc04-101e-4884-95f2-33aba56cba60" />

First, make sure a build dir exists
mkdir build

To compile
cmake -S . -B build      # configure (once)
cmake --build build      # build (after each change)

To run:
.build/cpu-rasterizer [width:(int)] [height:(int)] [model:(String path)] [scale:(float)]

./build/cpu-rasterizer w:500 h:500 model:./demo-scene/assets/Tetrahedron.gltf
./build/cpu-rasterizer w:500 h:500 model:./demo-scene/SheenChair.glb   

Note: This has 2 dependencies, which the project discovers using its CMakeLists.txt. SDL3 and glm must first be installed to build.

