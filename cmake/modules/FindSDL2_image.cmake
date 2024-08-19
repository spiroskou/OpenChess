# Look for the header file.
find_path(SDL2_IMAGE_INCLUDE_DIR SDL_image.h
          HINTS
          $ENV{SDL2_IMAGE_DIR}
          PATH_SUFFIXES SDL2 include/SDL2 include
          PATHS ${SDL2_PATH})

# Look for the library.
find_library(SDL2_IMAGE_LIBRARY
             NAMES SDL2_image
             HINTS
             $ENV{SDL2_IMAGE_DIR}
             PATH_SUFFIXES lib64 lib32 lib
             PATHS ${SDL2_PATH})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(SDL2_image REQUIRED_VARS SDL2_IMAGE_LIBRARY SDL2_IMAGE_INCLUDE_DIR)

if(SDL2_IMAGE_FOUND AND NOT TARGET SDL2_image::SDL2_image)
  add_library(SDL2_image::SDL2_image UNKNOWN IMPORTED)
  set_target_properties(SDL2_image::SDL2_image PROPERTIES
    IMPORTED_LOCATION "${SDL2_IMAGE_LIBRARY}"
    INTERFACE_INCLUDE_DIRECTORIES "${SDL2_IMAGE_INCLUDE_DIR}")
endif()

mark_as_advanced(SDL2_IMAGE_INCLUDE_DIR SDL2_IMAGE_LIBRARY)