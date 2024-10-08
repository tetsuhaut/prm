cmake_minimum_required(VERSION 3.28)

# project name, version and language
project("Poker Reviewer Modulaire" VERSION 0.0.1 LANGUAGES CXX)

# the project language is C++ 23
set(CMAKE_CXX_STANDARD 23)

# the compiler must be C++ 23 compatible
set(CMAKE_CXX_STANDARD_REQUIRED 23)

# no language extension to be as portable as possible
set(CMAKE_CXX_EXTENSIONS OFF)

# uncomment in case we want to know where CMake is looking for packages
#set(CMAKE_FIND_DEBUG_MODE 1)

# building in-tree is not allowed
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
  message(FATAL_ERROR "Prevented in-tree built. Remove CMakeCache.txt, the CMakeFiles directory and create a build directory outside of the source code and call cmake from there.")
endif()

# set build type, "Debug" by default
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "build type (one of Debug, RelWithDebInfo, Release, MinSizeRel)" FORCE)
endif()

# use the UTF-8 encoding internally
option(CMAKE_ENCODING_UTF8 "Use UTF-8 encoding internally." ON)

# where to produce executables and libraries
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)

# the main executable source files
file(GLOB_RECURSE sourceFiles
                  src/main/cpp/entities/*
                  src/main/cpp/gui/*
                  src/main/cpp/history/*
                  src/main/cpp/language/*
                  src/main/cpp/system/*)

# this program creates an executable file called 'prm'
add_executable(prm)

# the 'prm' executable is created from main.cpp and modules
target_sources(prm
    PUBLIC
    src/main/cpp/main.cpp
)
target_sources(prm
  PUBLIC
    FILE_SET CXX_MODULES FILES
    ${sourceFiles}
)

# the unitTests executable source files
file(GLOB_RECURSE testSourceFiles src/test/cpp/*)

# this program creates an executable file called 'unitTests'
add_executable(unitTests)

# the 'unitTests' executable is created from main modules and test source file
target_sources(unitTests
    PUBLIC
      FILE_SET CXX_MODULES FILES
      src/main/cpp/language/strings.cpp
      ${testSourceFiles}
)

# the 'unitTests' source files can include prm headers
target_include_directories(unitTests PRIVATE src/main/cpp)

# pass informations to the source code
target_compile_definitions(prm PUBLIC APP_VERSION="${CMAKE_PROJECT_VERSION}")
target_compile_definitions(prm PUBLIC APP_NAME_SHORT="Poker Reviewer Modulaire")
target_compile_definitions(prm PUBLIC IMAGES_DIR="${EXECUTABLE_OUTPUT_PATH}/resources/images/")
target_compile_definitions(unitTests PUBLIC APP_VERSION="${CMAKE_PROJECT_VERSION}")
target_compile_definitions(unitTests PUBLIC APP_NAME_SHORT="Poker Reviewer Modulaire")
target_compile_definitions(unitTests PUBLIC IMAGES_DIR="${EXECUTABLE_OUTPUT_PATH}/resources/images/")

################################################################################
# library configurations
################################################################################

################################################################################
# will use the FLTK libraries, so configure the project for it
# need to have FLTK_DIR as an environment variable pointing to the dir where
# the library has been installed
# notes:
# - CMake's findFltk tool does not point on the installed version, so can't use it
# - FLTK cmake conf does not provide version :(
################################################################################
set(FLTK_SKIP_FORMS true)
set(FLTK_SKIP_FLUID true)
set(FLTK_SKIP_IMAGES false) # use the fltk_images library
set(FLTK_SKIP_OPENGL true)
find_package(FLTK REQUIRED) # dunno how to provide version
target_include_directories(prm PRIVATE ${FLTK_INCLUDE_DIRS})
target_link_libraries(prm PRIVATE ${FLTK_LIBRARIES})
target_include_directories(unitTests PRIVATE ${FLTK_INCLUDE_DIRS})
target_link_libraries(unitTests PRIVATE ${FLTK_LIBRARIES})

################################################################################
# will use the GSL library https://github.com/Microsoft/GSL, so configure the
# project for it.
# need to have Microsoft.GSL_DIR as an environment variable pointing to the dir
# where the library has been installed
################################################################################
find_package(Microsoft.GSL CONFIG REQUIRED)
target_link_libraries(prm PRIVATE Microsoft.GSL::GSL)
target_link_libraries(unitTests PRIVATE Microsoft.GSL::GSL)

################################################################################
# will use the stlab library https://github.com/stlab/libraries, so configure
# the project for it
# need to have stlab_DIR as an environment variable pointing to the dir where
# the library has been installed
# 
################################################################################
find_package(stlab 1.7.1 REQUIRED)
target_link_libraries(prm PRIVATE stlab::stlab)
target_link_libraries(unitTests PRIVATE stlab::stlab)

################################################################################
# will use the Boost libraries, so configure the project for it
# need to have Boost_DIR as an environment variable pointing to the dir where
# the library has been installed
# https://github.com/boostorg/test
################################################################################
# disable auto linking
add_definitions(-DBOOST_ALL_NO_LIB)
SET(Boost_DEBUG ON)

# use static linking, requires the IDE project to use static runtime
set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME ON)

# specify which libraries to link with and corresponding include files
find_package(Boost 1.80.0 CONFIG COMPONENTS unit_test_framework)
include_directories(${Boost_INCLUDE_DIRS})
target_link_libraries(unitTests PRIVATE ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})

################################################################################
# compiler configurations
################################################################################

################################################################################
# if used with Microsoft Visual Studio, link statically, compile with max
# warnings, compile with multiple threads (i.e. faster), use utf-8 without byte
# order mark, use the incremental build.
# see https://github.com/cpp-best-practices/cppbestpractices/blob/master/02-Use_the_Tools_Available.md
################################################################################
if(MSVC)
  # link statically
  set_property(TARGET prm PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
  set_property(TARGET unitTests PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
  # select the prm project when opening Visual Studio
  set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT prm)
  
  # use utf-8 without byte order mark, compile with multiple threads
  # note : /INCREMENTAL is is implied for debug build
  # /fsanitize=address, /fsanitize=undefined, /fsanitize=memory, /fsanitize=thread, /fsanitize=leak, /fsanitize=hwaddress
  #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /utf-8 /MP /ZI /Gy /EHsc /Zc:__cplusplus /fsanitize=leak" CACHE STRING "" FORCE)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /utf-8 /MP /ZI /Gy /EHsc /Zc:__cplusplus " CACHE STRING "" FORCE)
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /ignore:4099" CACHE STRING "" FORCE)
  # compile with max warning level
  target_compile_options(prm PRIVATE /JMC # debug just my code
                                     /utf-8 # Set source and execution character sets to UTF-8.
                                     /MP # Builds multiple source files concurrently.
                                     /Zc:__cplusplus # the __cplusplus macro tells the C++ version is use
                                     /ZI # Edit and Continue debugging features
                                     /permissive- # standards-conforming compiler
                                     /std:c++latest # use the latest C++ version
                                     #/W4 /w35038 /w14640 /w14242 /w14254 /w14263 /w14265 /w14287 /we4289 /w14296 /w14311 /w14545 /w14546 /w14547 /w14549 /w14555 /w14619 /w14640 /w14826 /w14905 /w14906 /w14928)
                                     /Wall /wd4820 /wd4623 /wd4626 /wd5027 /wd4625 /wd5026 /wd5045 /wd5246)
  target_compile_options(unitTests PRIVATE /JMC # debug just my code
                                           /utf-8 # Set source and execution character sets to UTF-8.
                                           /MP # Builds multiple source files concurrently.
                                           /Zc:__cplusplus # the __cplusplus macro tells the C++ version is use
                                           /ZI # Edit and Continue debugging features
                                           /permissive- # standards-conforming compiler
                                           /std:c++latest # use the latest C++ version
                                           #/W4 /w35038 /w14640 /w14242 /w14254 /w14263 /w14265 /w14287 /we4289 /w14296 /w14311 /w14545 /w14546 /w14547 /w14549 /w14555 /w14619 /w14640 /w14826 /w14905 /w14906 /w14928)
                                           /Wall /wd4820 /wd4623 /wd4626 /wd5027 /wd4625 /wd5026 /wd5045 /wd5246)

################################################################################
# if used with MinGW, link statically, compile with max warnings, do not show
# the console window for reviewer.exe, use the Windows API, the thread API
# Note: if using https://gcc-mcf.lhmouse.com/, the executable must be deployed
# aside of the mcfgthread DLL, because it can't be statically linked. See 
# https://sourceforge.net/p/mingw-w64/mailman/mingw-w64-public/thread/575d13dd.430ac20a.33013.1aa0@mx.google.com/
################################################################################
elseif(MINGW)
   # -Werror=conversion breaks compilation because of Boost
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++2a -Wall -Wextra -Wpedantic -Werror=unused-function -pedantic-errors -Wvla -Wextra-semi -Wnull-dereference -Wswitch-enum -Wno-deprecated -fno-elide-type -Wduplicated-cond -Wduplicated-branches -Wsuggest-override -Wshadow -Wnon-virtual-dtor -Wold-style-cast -Wcast-align -Wunused -Woverloaded-virtual -pedantic -Wconversion -Wsign-conversion -Wmisleading-indentation -Wduplicated-cond -Wduplicated-branches -Wlogical-op -Wnull-dereference -Wuseless-cast -Wdouble-promotion -Wformat=2 -Weffc++ -Wtrampolines -Wimplicit-fallthrough")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static")
  target_link_libraries(prm PRIVATE "-lpsapi -pthread")
  #target_link_libraries(prm PRIVATE -mwindows)
  add_definitions(-DWIN32)
endif(MSVC)