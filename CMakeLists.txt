cmake_minimum_required(VERSION 3.10)
project(ChessGame)

# ADD THESE TWO LINES:
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(/opt/homebrew/include)
link_directories(/opt/homebrew/lib)

add_executable(ChessGame main.cpp)
target_link_libraries(ChessGame sfml-graphics sfml-window sfml-system)
