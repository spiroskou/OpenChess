cmake_minimum_required(VERSION 3.8)
project("OpenChess")

# Include the vcpkg toolchain for building
set(CMAKE_TOOLCHAIN_FILE "${CMAKE_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "Vcpkg toolchain file")

find_package(SDL2 CONFIG REQUIRED)
find_package(SDL2_image CONFIG REQUIRED)

add_executable(OpenChess "main.cpp"  "Pieces/Piece.h" "Pieces/King.h" "Pieces/King.cpp" "Pieces/Rook.h" "Pieces/Rook.cpp" "Pieces/Queen.h" "Pieces/Queen.cpp" "Pieces/Pawn.h" "Pieces/Pawn.cpp" "Pieces/Bishop.h" "Pieces/Bishop.cpp" "Pieces/Knight.h" "Pieces/Knight.cpp" "Board/Board.cpp" "Board/Board.h" "ChessSDL.cpp" "ChessSDL.h" "Pieces/Piece.cpp")

# Using generator expressions to link the correct SDL2 and SDL2_image libraries
target_link_libraries(OpenChess
    PRIVATE
    $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>
    $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
    $<IF:$<TARGET_EXISTS:SDL2_image::SDL2_image>,SDL2_image::SDL2_image,SDL2_image::SDL2_image-static>
)

if(CMAKE_VERSION VERSION_GREATER 3.12)
    set_property(TARGET OpenChess PROPERTY CXX_STANDARD 20)
endif()

if(WIN32)
    set_target_properties(OpenChess PROPERTIES
        WIN32_EXECUTABLE TRUE
    )
endif()
