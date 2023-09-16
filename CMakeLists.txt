cmake_minimum_required(VERSION 3.26)

# project name, version and language
project("Poker Reviewer Modulaire" VERSION 0.0.1 LANGUAGES CXX)

# the project language is C++ 23
set(CMAKE_CXX_STANDARD 23)

# if the compiler doesn't know C++ 23, fail
set(CMAKE_CXX_STANDARD_REQUIRED 23)

# no language extension to be as portable as possible
set(CMAKE_CXX_EXTENSIONS OFF)

# experimental use of C++ 20 modules
# need to update at each new CMake version
# see https://github.com/Kitware/CMake/blob/v<current cmake version>/Help/dev/experimental.rst
set(CMAKE_EXPERIMENTAL_CXX_MODULE_CMAKE_API "aa1f7df0-828a-4fcd-9afc-2dc80491aca7")

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

# the main executable is created from main.cpp and modules
target_sources(prm
    PUBLIC
    src/main/cpp/main.cpp
)
target_sources(prm
  PUBLIC
    FILE_SET all_my_modules TYPE CXX_MODULES FILES
    ${sourceFiles}
)

# copy images from source to target directory
configure_file(src/main/resources/images/aceClub.gif ${EXECUTABLE_OUTPUT_PATH}/resources/images/aceClub.gif COPYONLY)
configure_file(src/main/resources/images/aceDiamond.gif ${EXECUTABLE_OUTPUT_PATH}/resources/images/aceDiamond.gif COPYONLY)
configure_file(src/main/resources/images/aceHeart.gif ${EXECUTABLE_OUTPUT_PATH}/resources/images/aceHeart.gif COPYONLY)
configure_file(src/main/resources/images/aceSpade.gif ${EXECUTABLE_OUTPUT_PATH}/resources/images/aceSpade.gif COPYONLY)
configure_file(src/main/resources/images/back.gif ${EXECUTABLE_OUTPUT_PATH}/resources/images/back.gif COPYONLY)
configure_file(src/main/resources/images/eightClub.gif ${EXECUTABLE_OUTPUT_PATH}/resources/images/eightClub.gif COPYONLY)
configure_file(src/main/resources/images/eightDiamond.gif ${EXECUTABLE_OUTPUT_PATH}/resources/images/eightDiamond.gif COPYONLY)
configure_file(src/main/resources/images/eightHeart.gif ${EXECUTABLE_OUTPUT_PATH}/resources/images/eightHeart.gif COPYONLY)
configure_file(src/main/resources/images/eightSpade.gif ${EXECUTABLE_OUTPUT_PATH}/resources/images/eightSpade.gif COPYONLY)
configure_file(src/main/resources/images/fiveClub.gif ${EXECUTABLE_OUTPUT_PATH}/resources/images/fiveClub.gif COPYONLY)
configure_file(src/main/resources/images/fiveDiamond.gif ${EXECUTABLE_OUTPUT_PATH}/resources/images/fiveDiamond.gif COPYONLY)
configure_file(src/main/resources/images/fiveHeart.gif ${EXECUTABLE_OUTPUT_PATH}/resources/images/fiveHeart.gif COPYONLY)
configure_file(src/main/resources/images/fiveSpade.gif ${EXECUTABLE_OUTPUT_PATH}/resources/images/fiveSpade.gif COPYONLY)
configure_file(src/main/resources/images/fourClub.gif ${EXECUTABLE_OUTPUT_PATH}/resources/images/fourClub.gif COPYONLY)
configure_file(src/main/resources/images/fourDiamond.gif ${EXECUTABLE_OUTPUT_PATH}/resources/images/fourDiamond.gif COPYONLY)
configure_file(src/main/resources/images/fourHeart.gif ${EXECUTABLE_OUTPUT_PATH}/resources/images/fourHeart.gif COPYONLY)
configure_file(src/main/resources/images/fourSpade.gif ${EXECUTABLE_OUTPUT_PATH}/resources/images/fourSpade.gif COPYONLY)
configure_file(src/main/resources/images/jackClub.gif ${EXECUTABLE_OUTPUT_PATH}/resources/images/jackClub.gif COPYONLY)
configure_file(src/main/resources/images/jackDiamond.gif ${EXECUTABLE_OUTPUT_PATH}/resources/images/jackDiamond.gif COPYONLY)
configure_file(src/main/resources/images/jackHeart.gif ${EXECUTABLE_OUTPUT_PATH}/resources/images/jackHeart.gif COPYONLY)
configure_file(src/main/resources/images/jackSpade.gif ${EXECUTABLE_OUTPUT_PATH}/resources/images/jackSpade.gif COPYONLY)
configure_file(src/main/resources/images/kingClub.gif ${EXECUTABLE_OUTPUT_PATH}/resources/images/kingClub.gif COPYONLY)
configure_file(src/main/resources/images/kingDiamond.gif ${EXECUTABLE_OUTPUT_PATH}/resources/images/kingDiamond.gif COPYONLY)
configure_file(src/main/resources/images/kingHeart.gif ${EXECUTABLE_OUTPUT_PATH}/resources/images/kingHeart.gif COPYONLY)
configure_file(src/main/resources/images/kingSpade.gif ${EXECUTABLE_OUTPUT_PATH}/resources/images/kingSpade.gif COPYONLY)
configure_file(src/main/resources/images/nineClub.gif ${EXECUTABLE_OUTPUT_PATH}/resources/images/nineClub.gif COPYONLY)
configure_file(src/main/resources/images/nineDiamond.gif ${EXECUTABLE_OUTPUT_PATH}/resources/images/nineDiamond.gif COPYONLY)
configure_file(src/main/resources/images/nineHeart.gif ${EXECUTABLE_OUTPUT_PATH}/resources/images/nineHeart.gif COPYONLY)
configure_file(src/main/resources/images/nineSpade.gif ${EXECUTABLE_OUTPUT_PATH}/resources/images/nineSpade.gif COPYONLY)
configure_file(src/main/resources/images/queenClub.gif ${EXECUTABLE_OUTPUT_PATH}/resources/images/queenClub.gif COPYONLY)
configure_file(src/main/resources/images/queenDiamond.gif ${EXECUTABLE_OUTPUT_PATH}/resources/images/queenDiamond.gif COPYONLY)
configure_file(src/main/resources/images/queenHeart.gif ${EXECUTABLE_OUTPUT_PATH}/resources/images/queenHeart.gif COPYONLY)
configure_file(src/main/resources/images/queenSpade.gif ${EXECUTABLE_OUTPUT_PATH}/resources/images/queenSpade.gif COPYONLY)
configure_file(src/main/resources/images/sevenClub.gif ${EXECUTABLE_OUTPUT_PATH}/resources/images/sevenClub.gif COPYONLY)
configure_file(src/main/resources/images/sevenDiamond.gif ${EXECUTABLE_OUTPUT_PATH}/resources/images/sevenDiamond.gif COPYONLY)
configure_file(src/main/resources/images/sevenHeart.gif ${EXECUTABLE_OUTPUT_PATH}/resources/images/sevenHeart.gif COPYONLY)
configure_file(src/main/resources/images/sevenSpade.gif ${EXECUTABLE_OUTPUT_PATH}/resources/images/sevenSpade.gif COPYONLY)
configure_file(src/main/resources/images/sixClub.gif ${EXECUTABLE_OUTPUT_PATH}/resources/images/sixClub.gif COPYONLY)
configure_file(src/main/resources/images/sixDiamond.gif ${EXECUTABLE_OUTPUT_PATH}/resources/images/sixDiamond.gif COPYONLY)
configure_file(src/main/resources/images/sixHeart.gif ${EXECUTABLE_OUTPUT_PATH}/resources/images/sixHeart.gif COPYONLY)
configure_file(src/main/resources/images/sixSpade.gif ${EXECUTABLE_OUTPUT_PATH}/resources/images/sixSpade.gif COPYONLY)
configure_file(src/main/resources/images/table.gif ${EXECUTABLE_OUTPUT_PATH}/resources/images/table.gif COPYONLY)
configure_file(src/main/resources/images/table.jpg ${EXECUTABLE_OUTPUT_PATH}/resources/images/table.jpg COPYONLY)
configure_file(src/main/resources/images/table.png ${EXECUTABLE_OUTPUT_PATH}/resources/images/table.png COPYONLY)
configure_file(src/main/resources/images/tenClub.gif ${EXECUTABLE_OUTPUT_PATH}/resources/images/tenClub.gif COPYONLY)
configure_file(src/main/resources/images/tenDiamond.gif ${EXECUTABLE_OUTPUT_PATH}/resources/images/tenDiamond.gif COPYONLY)
configure_file(src/main/resources/images/tenHeart.gif ${EXECUTABLE_OUTPUT_PATH}/resources/images/tenHeart.gif COPYONLY)
configure_file(src/main/resources/images/tenSpade.gif ${EXECUTABLE_OUTPUT_PATH}/resources/images/tenSpade.gif COPYONLY)
configure_file(src/main/resources/images/threeClub.gif ${EXECUTABLE_OUTPUT_PATH}/resources/images/threeClub.gif COPYONLY)
configure_file(src/main/resources/images/threeDiamond.gif ${EXECUTABLE_OUTPUT_PATH}/resources/images/threeDiamond.gif COPYONLY)
configure_file(src/main/resources/images/threeHeart.gif ${EXECUTABLE_OUTPUT_PATH}/resources/images/threeHeart.gif COPYONLY)
configure_file(src/main/resources/images/threeSpade.gif ${EXECUTABLE_OUTPUT_PATH}/resources/images/threeSpade.gif COPYONLY)
configure_file(src/main/resources/images/twoClub.gif ${EXECUTABLE_OUTPUT_PATH}/resources/images/twoClub.gif COPYONLY)
configure_file(src/main/resources/images/twoDiamond.gif ${EXECUTABLE_OUTPUT_PATH}/resources/images/twoDiamond.gif COPYONLY)
configure_file(src/main/resources/images/twoHeart.gif ${EXECUTABLE_OUTPUT_PATH}/resources/images/twoHeart.gif COPYONLY)
configure_file(src/main/resources/images/twoSpade.gif ${EXECUTABLE_OUTPUT_PATH}/resources/images/twoSpade.gif COPYONLY)

# pass informations to the source code
target_compile_definitions(prm PUBLIC APP_VERSION="${CMAKE_PROJECT_VERSION}")
target_compile_definitions(prm PUBLIC APP_NAME_SHORT="Poker Reviewer Modulaire")
target_compile_definitions(prm PUBLIC IMAGES_DIR="${EXECUTABLE_OUTPUT_PATH}/resources/images/")

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

################################################################################
# will use the frozen library https://github.com/serge-sans-paille/frozen, so
# configure the project for it
# need to have frozen_DIR as an environment variable pointing to the dir where
# the library has been installed
################################################################################
find_package(frozen 1.1.0 REQUIRED)
target_link_libraries(prm PRIVATE frozen::frozen-headers)

################################################################################
# will use the GSL library https://github.com/Microsoft/GSL, so configure the
# project for it.
# need to have Microsoft.GSL_DIR as an environment variable pointing to the dir
# where the library has been installed
################################################################################
find_package(Microsoft.GSL CONFIG REQUIRED)
target_link_libraries(prm PRIVATE Microsoft.GSL::GSL)

################################################################################
# will use the stlab library https://github.com/stlab/libraries, so configure
# the project for it
# need to have stlab_DIR as an environment variable pointing to the dir where
# the library has been installed
# 
################################################################################
find_package(stlab 1.7.1 REQUIRED)
target_link_libraries(prm PRIVATE stlab::stlab)

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
  # select the prm project when opening Visual Studio
  set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT prm)
  
  # use utf-8 without byte order mark, compile with multiple threads
  # note : /INCREMENTAL is is implied for debug build
  # /fsanitize=address, /fsanitize=undefined, /fsanitize=memory, /fsanitize=thread, /fsanitize=leak, /fsanitize=hwaddress
  #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /utf-8 /MP /ZI /Gy /EHsc /Zc:__cplusplus /fsanitize=leak" CACHE STRING "" FORCE)
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /ignore:4099" CACHE STRING "" FORCE)
  # compile with max warning level
  target_compile_options(prm PRIVATE /JMC # debug just mt code
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
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++2a -Wall -Wextra -Wpedantic -Werror=unused-function -pedantic-errors -Wvla -Wextra-semi -Wnull-dereference -Wswitch-enum -Wno-deprecated -fno-elide-type -Wduplicated-cond -Wduplicated-branches -Wsuggest-override -Wshadow -Wnon-virtual-dtor -Wold-style-cast -Wcast-align -Wunused -Woverloaded-virtual -pedantic -Wconversion -Wsign-conversion -Wmisleading-indentation -Wduplicated-cond -Wduplicated-branches -Wlogical-op -Wnull-dereference -Wuseless-cast -Wdouble-promotion -Wformat=2 -Weffc++")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static")
  target_link_libraries(prm PRIVATE "-lpsapi -pthread")
  #target_link_libraries(prm PRIVATE -mwindows)
  add_definitions(-DWIN32)
endif(MSVC)