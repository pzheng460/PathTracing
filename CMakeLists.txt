#---------------------------------------------------------------------------------------------------
# CMake Build Configuration for the Ray Tracing Weekend Series
#
# See README.md for guidance.
#---------------------------------------------------------------------------------------------------

cmake_minimum_required ( VERSION 3.1.0...3.27.0 )

project ( PathTracing LANGUAGES CXX )

# Set to C++11
set ( CMAKE_CXX_STANDARD          17 )
set ( CMAKE_CXX_STANDARD_REQUIRED ON )
set ( CMAKE_CXX_EXTENSIONS        OFF )

# Source

set ( EXTERNAL
        src/external/stb_image.h
)

set ( SOURCE_PATH_TRACER
        src/PathTracer/main.cpp
        src/PathTracer/AABB.h
        src/PathTracer/Camera.h
        src/PathTracer/color.h
        src/PathTracer/ConstantMedium.h
        src/PathTracer/Hittable.h
        src/PathTracer/HittableList.h
        src/PathTracer/Interval.h
        src/PathTracer/Material.h
        src/PathTracer/ONB.h
        src/PathTracer/PDF.h
        src/PathTracer/Perlin.h
        src/PathTracer/Quad.h
        src/PathTracer/Ray.h
        src/PathTracer/rtw_stb_image.h
        src/PathTracer/utils.h
        src/PathTracer/Sphere.h
        src/PathTracer/Texture.h
        src/PathTracer/Vec3.h
        src/PathTracer/Renderer.h
        src/PathTracer/Renderer.cpp
        src/PathTracer/rtw_stb_image.cpp
        src/PathTracer/Vertex.h
        src/PathTracer/Vertex.cpp
        src/PathTracer/BVH.h
)

include_directories(src)

# Specific compiler flags below. We're not going to add options for all possible compilers, but if
# you're new to CMake (like we are), the following may be a helpful example if you're using a
# different compiler or want to set different compiler options.

message (STATUS "Compiler ID: " ${CMAKE_CXX_COMPILER_ID})
message (STATUS "Release flags: " ${CMAKE_CXX_FLAGS_RELEASE})
message (STATUS "Debug flags: " ${CMAKE_CXX_FLAGS_DEBUG})

if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    # /wd #### - Disable warning
    # /we #### - treat warning as error
    add_compile_options("/W4")      # Enable level-4 warnings
    add_compile_options("/we 4265") # Class has virtual functions, but its non-trivial destructor is not virtual
    add_compile_options("/we 5204") # Class has virtual functions, but its trivial destructor is not virtual
    add_compile_options("/wd 4100") # unreferenced formal parameter
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    add_compile_options(-Wnon-virtual-dtor) # Class has virtual functions, but its destructor is not virtual
    add_compile_options(-Wreorder) # Data member will be initialized after [other] data member
    add_compile_options(-Wmaybe-uninitialized) # Variable improperly initialized
    add_compile_options(-Wunused-variable) # Variable is defined but unused
elseif (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    add_compile_options(-Wnon-virtual-dtor) # Class has virtual functions, but its destructor is not virtual
    add_compile_options(-Wreorder) # Data member will be initialized after [other] data member
    add_compile_options(-Wsometimes-uninitialized) # Variable improperly initialized
    add_compile_options(-Wunused-variable) # Variable is defined but unused
endif()

# Executables
add_executable(PathTracing ${EXTERNAL} ${SOURCE_PATH_TRACER})
add_executable(cos_cubed src/PathTracer/cos_cubed.cpp)
add_executable(cos_density src/PathTracer/cos_density.cpp)
add_executable(integrate_x_sq src/PathTracer/integrate_x_sq.cpp)
add_executable(pi src/PathTracer/pi.cpp)
add_executable(estimate_halfway src/PathTracer/estimate_halfway.cpp)
add_executable(sphere_importance src/PathTracer/sphere_importance.cpp)
add_executable(sphere_plot src/PathTracer/sphere_plot.cpp)
