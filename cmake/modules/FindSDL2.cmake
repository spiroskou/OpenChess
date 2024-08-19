# Look for the header file.
find_path(SDL2_INCLUDE_DIR SDL.h
          HINTS
          $ENV{SDL2_DIR}
          PATH_SUFFIXES SDL2 include/SDL2 include
          PATHS ${SDL2_PATH})

# Look for the library.
find_library(SDL2_LIBRARY
             NAMES SDL2
             HINTS
             $ENV{SDL2_DIR}
             PATH_SUFFIXES lib64 lib32 lib
             PATHS ${SDL2_PATH})

find_library(SDL2MAIN_LIBRARY NAMES SDL2main
             HINTS ${SDL2_DIR}
             PATH_SUFFIXES lib64 lib32 lib)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(SDL2 REQUIRED_VARS SDL2_LIBRARY SDL2_INCLUDE_DIR)

if(SDL2_FOUND AND NOT TARGET SDL2::SDL2)
  add_library(SDL2::SDL2 UNKNOWN IMPORTED)
  set_target_properties(SDL2::SDL2 PROPERTIES
    IMPORTED_LOCATION "${SDL2_LIBRARY}"
    INTERFACE_INCLUDE_DIRECTORIES "${SDL2_INCLUDE_DIR}")
endif()

if(NOT TARGET SDL2::SDL2main AND SDL2MAIN_LIBRARY)
    add_library(SDL2::SDL2main UNKNOWN IMPORTED)
    set_target_properties(SDL2::SDL2main PROPERTIES
        IMPORTED_LOCATION "${SDL2MAIN_LIBRARY}")
endif()

mark_as_advanced(SDL2_INCLUDE_DIR SDL2_LIBRARY SDL2MAIN_LIBRARY)