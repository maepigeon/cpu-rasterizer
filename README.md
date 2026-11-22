To BUILD:
First, make sure a build dir exists
mkdir build

To compile
cmake -S . -B build
cmake --build build
To run independently:
.build/cpu-rasterizer [width:(int)] [height:(int)] [model:(String path)] [scale:(float)]

./build/cpu-rasterizer w:500 h:500 model:./demo-scene/assets/Tetrahedron.gltf
./build/cpu-rasterizer w:500 h:500 model:./demo-scene/SheenChair.glb   

To connect to a parent application:
    More info, eventually!

Note: This has 2 dependencies, which the project discovers using its CMakeLists.txt. SDL3 and glm must first be installed to build.
// SUPPORTED DATA: TRIANGLE MESHES (vertices, triangles, textures, and normals);


