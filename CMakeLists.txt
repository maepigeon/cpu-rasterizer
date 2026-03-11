cmake_minimum_required(VERSION 3.10)
set(ProjectName 
    rasten)

project($(ProjectName))
# Use the C++23 standard.
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(SDL3 REQUIRED)
find_package(glm CONFIG REQUIRED)

# sources variable contains a link to all cpp files to compile
set(SOURCES
    src/main.cpp
    src/Camera.cpp
    src/Model.cpp
    src/ModelLoader.cpp
    src/Mesh.cpp
    src/ResourceManager.cpp
    src/CPURender/RasterizerGeometry.cpp
    src/CPURender/SutherlandHodgmanClipping.cpp
    src/CPURender/Rasterizer.cpp
    src/CPURender/PixelProcessor.cpp
    src/CPURender/FrameBuffer.cpp
    src/CPURender/RenderManager.cpp
    src/CPURender/VertexProcessor.cpp
)
add_executable(rasten ${SOURCES})

target_link_libraries(${ProjectName} SDL3::SDL3)
target_link_libraries(${ProjectName} glm::glm)

set(TINYGLTF_HEADER_ONLY ON CACHE INTERNAL "" FORCE)
set(TINYGLTF_INSTALL OFF CACHE INTERNAL "" FORCE)

# Set the directories that should be included in the build command for this target
# when running g++ these will be included as -I/directory/path/
target_include_directories(${ProjectName}
    PRIVATE 
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/tinygltf
)