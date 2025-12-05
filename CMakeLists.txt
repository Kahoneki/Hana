cmake_minimum_required(VERSION 3.14)
project(Hana LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 23)
include(FetchContent)

file(GLOB_RECURSE HANA_SOURCES "src/*.cpp")
file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/Resource-Files/" DESTINATION "${CMAKE_BINARY_DIR}/Resource-Files/")

add_executable(HanaAI ${HANA_SOURCES})

#SFML
FetchContent_Declare(sfml GIT_REPOSITORY https://github.com/SFML/SFML.git GIT_TAG 3.0.2)
FetchContent_MakeAvailable(sfml)
target_link_libraries(HanaAI PRIVATE sfml-graphics)

#Box2D
FetchContent_Declare(box2d GIT_REPOSITORY https://github.com/erincatto/box2d.git GIT_TAG v3.1.1)
FetchContent_MakeAvailable(box2d)
target_link_libraries(HanaAI PRIVATE box2d)

##Google Highway
#FetchContent_Declare(hwy GIT_REPOSITORY https://github.com/google/highway.git GIT_TAG 1.3.0)
#FetchContent_MakeAvailable(hwy)
#target_link_libraries(HanaAI PRIVATE hwy)

FetchContent_Declare(imgui GIT_REPOSITORY https://github.com/ocornut/imgui GIT_TAG v1.92.5)
FetchContent_MakeAvailable(imgui)

#ImGui-SFML
FetchContent_Declare(imgui-sfml GIT_REPOSITORY https://github.com/SFML/imgui-sfml.git GIT_TAG master)
set(IMGUI_DIR ${imgui_SOURCE_DIR})
set(IMGUI_SFML_FIND_SFML OFF CACHE BOOL "Disable finding SFML" FORCE)
set(IMGUI_SFML_IMGUI_DEMO OFF CACHE BOOL "Disable demo" FORCE)
FetchContent_MakeAvailable(imgui-sfml)
target_link_libraries(HanaAI PRIVATE ImGui-SFML::ImGui-SFML)