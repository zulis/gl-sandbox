# glfw
#set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "Build the GLFW example programs")
#set(GLFW_BUILD_TESTS OFF CACHE BOOL "Build the GLFW test programs")
#set(GLFW_BUILD_DOCS OFF CACHE BOOL "Build the GLFW documentation")
#set(GLFW_INSTALL OFF CACHE BOOL "Generate installation target")
#add_subdirectory(glfw)

# SDL
add_subdirectory(SDL)

# glad
include(glad.cmake)

# glm
include(glm.cmake)

# imgui
include(imgui.cmake)

# stb
include(stb.cmake)

# spdlog
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "Build the GLFW example programs")
add_subdirectory(spdlog)