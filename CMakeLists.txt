cmake_minimum_required(VERSION 3.14)
project(Hana LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 23)
include(FetchContent)

file(GLOB_RECURSE HANA_SOURCES "src/*.cpp")
file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/Resource-Files/" DESTINATION "${CMAKE_BINARY_DIR}/Resource-Files/")

add_executable(HanaAI ${HANA_SOURCES})

#SFML
FetchContent_Declare(sfml GIT_REPOSITORY https://github.com/SFML/SFML.git GIT_TAG 2.6.1)
FetchContent_MakeAvailable(sfml)
target_link_libraries(HanaAI PRIVATE sfml-graphics)

#Box2D
FetchContent_Declare(box2d GIT_REPOSITORY https://github.com/erincatto/box2d.git GIT_TAG v3.1.1)
FetchContent_MakeAvailable(box2d)
target_link_libraries(HanaAI PRIVATE box2d)