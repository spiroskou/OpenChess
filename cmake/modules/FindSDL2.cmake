find_path(SDL2_INCLUDE_DIR NAME SDL.h
          HINTS $ENV{SDL2DIR}
          PATH_SUFFIXES SDL2 include/SDL2 include)

find_library(SDL2_LIBRARY NAME SDL2
             HINTS $ENV{SDL2DIR}
             PATH_SUFFIXES lib64 lib32 lib
             PATHS ${SDL2_PATH})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(SDL2 REQUIRED_VARS SDL2_LIBRARY SDL2_INCLUDE_DIR)

if(SDL2_FOUND AND NOT TARGET SDL2::SDL2)
    add_library(SDL2::SDL2 SHARED IMPORTED)
    set_target_properties(SDL2::SDL2 PROPERTIES
        IMPORTED_LOCATION "${SDL2_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${SDL2_INCLUDE_DIR}")
endif()

if(EXISTS "${SDL2_LIBRARY}" AND EXISTS "${SDL2_INCLUDE_DIR}")
    set(SDL2_SDL2_FOUND TRUE)
else()
    set(SDL2_SDL2_FOUND FALSE)
endif()

mark_as_advanced(SDL2_INCLUDE_DIR SDL2_LIBRARY)
