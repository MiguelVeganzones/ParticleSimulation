cmake_minimum_required(VERSION 3.30)
project(NBodySim CXX)

# Set C++ Standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set verbose output
set(CMAKE_VERBOSE_MAKEFILE ON)

# Build type configurations
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

# Compiler flags
set(DEBUG_CXX_FLAGS
	-O0
	-W
	-Wall
	-Wconversion
	-Wdangling-else
	-Wdouble-promotion
	-Wduplicated-branches
	-Wduplicated-cond
	-Werror
	-Wextra
	-Wfloat-equal
	-Wformat
	-Winvalid-pch
	-Wlogical-op
	-Wmisleading-indentation
	-Wnull-dereference
	-Wnrvo
	-Wodr
	-Wpointer-arith
	-Wrestrict
	-Wshadow
	-Wswitch-default
	-Wswitch-enum
	-Wuninitialized
	-Wvla
	-fbounds-check
	-fconcepts-diagnostics-depth=3
	-fdiagnostics-color=always
	-fdiagnostics-path-format=inline-events
	-fdiagnostics-show-caret
	-fdiagnostics-show-template-tree
	-ffinite-math-only
	-fmax-errors=5
	-fsanitize=address
	-fsanitize=bounds
	-fsanitize=float-cast-overflow
	-fsanitize=float-divide-by-zero
	-fsanitize=integer-divide-by-zero
	-fsanitize=leak
	-fsanitize=null
	-fsanitize=signed-integer-overflow
	-fsanitize=undefined
	-ggdb3
	-mavx
	-march=native
	-pedantic
)

set(DEBUG_SANITIZERS
	-fsanitize=address
	-fsanitize=bounds
	-fsanitize=float-cast-overflow
	-fsanitize=float-divide-by-zero
	-fsanitize=integer-divide-by-zero
	-fsanitize=leak
	-fsanitize=null
	-fsanitize=signed-integer-overflow
	-fsanitize=undefined
)

set(RELEASE_CXX_FLAGS
	-fdiagnostics-color=always
	-fdiagnostics-show-template-tree
	-fdiagnostics-path-format=inline-events
	-fdiagnostics-show-caret
	-ggdb3
	-O2
	-Wall
	-Wextra
	-Wshadow
	-ffinite-math-only
	-Wconversion
	-Wnrvo
	-Wuninitialized
	-Wmisleading-indentation
	-Werror
	-pedantic
	-mavx
	-march=native
	-fstrength-reduce
	-fsanitize=address
	-fsanitize=bounds
	-fsanitize=float-cast-overflow
	-fsanitize=float-divide-by-zero
	-fsanitize=integer-divide-by-zero
	-fsanitize=leak
	-fsanitize=null
	-fsanitize=signed-integer-overflow
	-fsanitize=undefined
	-fbounds-check
	-fconcepts-diagnostics-depth=3
)

set(RELEASE_SANITIZERS
	-fsanitize=address
	-fsanitize=bounds
	-fsanitize=float-cast-overflow
	-fsanitize=float-divide-by-zero
	-fsanitize=integer-divide-by-zero
	-fsanitize=leak
	-fsanitize=null
	-fsanitize=signed-integer-overflow
	-fsanitize=undefined
)

set(FULL_RELEASE_CXX_FLAGS
	-fdiagnostics-color=always
	-fdiagnostics-show-template-tree
	-fdiagnostics-path-format=inline-events
	-fdiagnostics-show-caret
	-O3
	-Wall
	-Wextra
	-Wshadow
	-Wconversion
	-Wmisleading-indentation
	-Werror
	-fstrength-reduce
	-pedantic
	-mavx
	-march=native
	-fconcepts-diagnostics-depth=3
	-fno-math-errno
	-fno-trapping-math
	-fno-exceptions
)

set(FULL_RELEASE_SANITIZERS
)

# Set output directory based on build configuration
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CXX_FLAGS ${DEBUG_CXX_FLAGS} ${DEBUG_SANITIZERS})
    set(LINK_FLAGS ${DEBUG_SANITIZERS})
    set(OUT_TAIL_DIR "debug")
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(CXX_FLAGS ${RELEASE_CXX_FLAGS} ${RELEASE_SANITIZERS})
    set(LINK_FLAGS ${RELEASE_SANITIZERS})
    set(OUT_TAIL_DIR "release")
elseif(CMAKE_BUILD_TYPE STREQUAL "FullRelease")
    set(CXX_FLAGS ${FULL_RELEASE_CXX_FLAGS} ${FULL_RELEASE_SANITIZERS})
    set(LINK_FLAGS ${FULL_RELEASE_SANITIZERS})
    set(OUT_TAIL_DIR "full_release")
else()
    message(ERROR "Unknown build type!")
endif()
message(STATUS "CXX Flags: ${CXX_FLAGS}")

# Output directories
set(OUTPUT_DIR ${CMAKE_BINARY_DIR}/bin/${OUT_TAIL_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_DIR})

# Include directories
include_directories(
    include
    include/Utility
    include/Plotting
    include/Containers
    include/Timing
    include/PhysicalModel
    include/Solvers
    include/Simulation
    include/DataLoggers
)

# Source files
file(GLOB_RECURSE SRC_FILES src/*.cpp)
file(GLOB_RECURSE TEST_FILES tests/*.cpp)
file(GLOB_RECURSE PLOTTING_FILES include/Plotting/*.cpp)

# Optional features
option(UNIT_SYSTEM "Enable unit system" OFF)
option(ROOT_PLOTTING "Enable ROOT plotting" OFF)
option(BOOST_LOGGING "Enable Boost logging" OFF)
option(FFAST_MATH "Enable fast math optimizations" OFF)

if(UNIT_SYSTEM)
    add_compile_definitions(USE_UNIT_SYSTEM)
endif()

if(BOOST_LOGGING)
    find_package(Boost REQUIRED COMPONENTS log thread system)
    include_directories(${Boost_INCLUDE_DIRS})
    add_compile_definitions(USE_BOOST_LOGGING)
    set(LOG_LIBS ${Boost_LIBRARIES})
    set(CXX_FLAGS "${CXX_FLAGS};-fexceptions")
endif()

if(FFAST_MATH)
    add_compile_options(-ffast-math)
endif()

# Plotting library
add_library(plotting STATIC ${PLOTTING_FILES})
target_include_directories(plotting PUBLIC ${CMAKE_SOURCE_DIR}/include/Plotting)
target_compile_options(plotting PRIVATE ${CXX_FLAGS})
if(ROOT_PLOTTING)
    find_package(ROOT REQUIRED)
    include_directories(${ROOT_INCLUDE_DIRS})
    add_compile_definitions(USE_ROOT_PLOTTING)
    set(CXX_FLAGS "${CXX_FLAGS};-Wno-cpp")
    target_compile_options(plotting PRIVATE -pthread -m64 -Wno-cpp)
    target_link_libraries(plotting PRIVATE ${ROOT_LIBRARIES})
endif()

# Main executable
add_executable(main ${SRC_FILES})
# Link libraries for main
set(MAIN_LIBS plotting ${LOG_LIBS} ${LINK_FLAGS} -lboost_program_options)
target_compile_options(main PRIVATE ${CXX_FLAGS})
target_link_libraries(main PRIVATE ${MAIN_LIBS})

# Tests executable
add_executable(tests ${TEST_FILES})
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})
target_link_libraries(tests PRIVATE plotting ${LINK_FLAGS} ${GTEST_BOTH_LIBRARIES} pthread)

target_compile_options(tests PRIVATE ${CXX_FLAGS})

# Build messages
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Output directory: ${OUTPUT_DIR}")
message(STATUS "Unit system: ${UNIT_SYSTEM}")
message(STATUS "ROOT plotting: ${ROOT_PLOTTING}")
message(STATUS "Boost logging: ${BOOST_LOGGING}")
message(STATUS "Fast math: ${FFAST_MATH}")
