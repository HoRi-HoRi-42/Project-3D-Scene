cmake_minimum_required(VERSION 3.23)
project(untitled)

set(CMAKE_CXX_STANDARD 14)
include_directories(I:/codaer/an3/devLib/include)
if (CMAKE_BUILD_TYPE MATCHES Debug)
    link_directories(I:/codaer/an3/devLib/lib/Debug)
elseif (CMAKE_BUILD_TYPE MATCHES Release)
    link_directories(I:/codaer/an3/devLib/lib/Release)
endif ()


add_executable(untitled main.cpp Shader.cpp Camera.cpp Mesh.cpp Model3D.cpp stb_image.cpp tiny_obj_loader.cpp Window.cpp SkyBox.cpp )

if (CMAKE_BUILD_TYPE MATCHES Debug)
    target_link_libraries(untitled glfw3.lib libglew32d.lib opengl32.lib WinMM.Lib)
elseif (CMAKE_BUILD_TYPE MATCHES Release)
    target_link_libraries(untitled glfw3.lib libglew32.lib opengl32.lib WinMM.Lib)


endif ()
