# glfw
#set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "Build the GLFW example programs")
#set(GLFW_BUILD_TESTS OFF CACHE BOOL "Build the GLFW test programs")
#set(GLFW_BUILD_DOCS OFF CACHE BOOL "Build the GLFW documentation")
#set(GLFW_INSTALL OFF CACHE BOOL "Generate installation target")
#add_subdirectory(glfw)

# SDL
include(SDL.cmake)

# glad
include(glad.cmake)

# glm
include(glm.cmake)

# imgui
include(imgui.cmake)

# stb
include(stb.cmake)

# spdlog
add_subdirectory(spdlog)

# assimp
set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build package with shared libraries." FORCE)
set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "If the supplementary tools for Assimp are built in addition to the library." FORCE)
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "If the test suite for Assimp is built in addition to the library." FORCE)
set(ASSIMP_WARNINGS_AS_ERRORS OFF CACHE BOOL "Do not treat Assimp warnings as errors." FORCE)
add_subdirectory(assimp)