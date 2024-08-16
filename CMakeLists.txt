# CMakeList.txt : CMake project for OpenChess, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

project ("OpenChess")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/modules/")

find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)

# Print the variables to see their values
include_directories(${SDL2_INCLUDE_DIRS})
include_directories(Board Pieces)

# Add source to this project's executable.
add_executable (OpenChess "main.cpp"  "Pieces/Piece.h" "Pieces/King.h" "Pieces/King.cpp" "Pieces/Rook.h" "Pieces/Rook.cpp" "Pieces/Queen.h" "Pieces/Queen.cpp" "Pieces/Pawn.h" "Pieces/Pawn.cpp" "Pieces/Bishop.h" "Pieces/Bishop.cpp" "Pieces/Knight.h" "Pieces/Knight.cpp" "Board/Board.cpp" "Board/Board.h" "ChessSDL.cpp" "ChessSDL.h" "Pieces/Piece.cpp")

target_link_libraries(OpenChess ${SDL2_LIBRARIES} SDL2_image::SDL2_image)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET OpenChess PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
