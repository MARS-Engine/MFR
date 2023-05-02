find_package(Freetype REQUIRED)
find_package(PkgConfig REQUIRED)

pkg_check_modules(FREETYPE freetype2)

target_include_directories(MARS PUBLIC include/)

target_link_libraries(MARS ${FREETYPE_LIBRARIES})
target_include_directories(MARS PUBLIC ${FREETYPE_INCLUDE_DIRS})
target_compile_options(MARS PUBLIC ${FREETYPE_CGLAGS_OTHER})

add_subdirectory(src)