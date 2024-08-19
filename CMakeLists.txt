# CMakeList.txt : CMake project for OpenChess, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

project ("OpenChess")

# SDL2 paths
set(SDL2_DIR "${CMAKE_SOURCE_DIR}/SDL/SDL2-devel-2.30.4-VC/SDL2-2.30.4")
set(SDL2_INCLUDE_DIR "${SDL2_DIR}/include")
set(SDL2_LIBRARY "${SDL2_DIR}/lib/x64/SDL2.lib")  # Adjust if you have a different lib name
set(SDL2MAIN_LIBRARY "${SDL2_DIR}/lib/x64/SDL2main.lib")

# SDL2_image paths
set(SDL2_IMAGE_DIR "${CMAKE_SOURCE_DIR}/SDL/SDL2_image-2.8.2")
set(SDL2_IMAGE_INCLUDE_DIR "${SDL2_IMAGE_DIR}/include")
set(SDL2_IMAGE_LIBRARY "${SDL2_IMAGE_DIR}/lib/x64/SDL2_image.lib")  # Adjust similarly

# Print the variables to see their values
include_directories(${SDL2_INCLUDE_DIR} ${SDL2_IMAGE_INCLUDE_DIR})
include_directories(Pieces Board)

# Add source to this project's executable.
add_executable (OpenChess "main.cpp"  "Pieces/Piece.h" "Pieces/King.h" "Pieces/King.cpp" "Pieces/Rook.h" "Pieces/Rook.cpp" "Pieces/Queen.h" "Pieces/Queen.cpp" "Pieces/Pawn.h" "Pieces/Pawn.cpp" "Pieces/Bishop.h" "Pieces/Bishop.cpp" "Pieces/Knight.h" "Pieces/Knight.cpp" "Board/Board.cpp" "Board/Board.h" "ChessSDL.cpp" "ChessSDL.h" "Pieces/Piece.cpp")

target_link_libraries(OpenChess SDL2::SDL2 SDL2::SDL2main SDL2_image::SDL2_image)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET OpenChess PROPERTY CXX_STANDARD 20)
endif()

if(WIN32)
    set_target_properties(OpenChess PROPERTIES
        WIN32_EXECUTABLE YES
    )
endif()

add_custom_command(TARGET OpenChess POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${SDL2_DIR}/lib/x64/SDL2.dll"
    $<TARGET_FILE_DIR:OpenChess>)
add_custom_command(TARGET OpenChess POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${SDL2_IMAGE_DIR}/lib/x64/SDL2_image.dll"
    $<TARGET_FILE_DIR:OpenChess>)

# TODO: Add tests and install targets if needed.
