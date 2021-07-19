cmake_minimum_required(VERSION 3.0)
project(COMP3320_External LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 14)

if(UNIX)
  # Make sure X11 is found and components are available
  find_package(X11 REQUIRED)

  if(NOT X11_Xinerama_FOUND)
    message(
      FATAL_ERROR "Unable to find X11 Xinerama. You must install this first.")
  endif(NOT X11_Xinerama_FOUND)

  if(NOT X11_Xi_FOUND)
    message(FATAL_ERROR "Unable to find X11 Xi. You must install this first.")
  endif(NOT X11_Xi_FOUND)

  if(NOT X11_Xcursor_FOUND)
    message(
      FATAL_ERROR "Unable to find X11 Xcursor. You must install this first.")
  endif(NOT X11_Xcursor_FOUND)

  if(NOT X11_Xmu_FOUND)
    message(FATAL_ERROR "Unable to find X11 Xmu. You must install this first.")
  endif(NOT X11_Xmu_FOUND)
endif(UNIX)

# Download and build all of our dependencies
include("${CMAKE_ROOT}/Modules/ExternalProject.cmake")

ExternalProject_Add(
  GLFW3
  GIT_REPOSITORY "https://github.com/glfw/glfw.git"
  GIT_TAG "master"
  UPDATE_COMMAND ""
  PATCH_COMMAND ""
  SOURCE_DIR "${CMAKE_SOURCE_DIR}/GLFW3"
  CMAKE_ARGS -DCMAKE_INSTALL_LIBDIR=lib
             -DBUILD_SHARED_LIBS=ON
             -DGLFW_BUILD_EXAMPLES=ON
             -DGLFW_BUILD_TESTS=ON
             -DGLFW_BUILD_DOCS=ON
             -DCMAKE_BUILD_TYPE=Release
             -DGLFW_INSTALL=ON
             -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}
             -G
             "Unix Makefiles"
  BUILD_COMMAND make
  INSTALL_DIR "${CMAKE_BINARY_DIR}"
  INSTALL_COMMAND make install
  TEST_COMMAND "")

ExternalProject_Add(
  GLM
  GIT_REPOSITORY "https://github.com/g-truc/glm.git"
  GIT_TAG "master"
  UPDATE_COMMAND ""
  PATCH_COMMAND ""
  SOURCE_DIR "${CMAKE_SOURCE_DIR}/GLM"
  CMAKE_ARGS -DCMAKE_INSTALL_LIBDIR=lib
             -DBUILD_SHARED_LIBS=ON
             -DBUILD_STATIC_LIBS=ON
             -DGLM_TEST_ENABLE_CXX_14=ON
             -DGLM_TEST_ENABLE_LANG_EXTENSIONS=ON
             -DGLM_TEST_ENABLE_FAST_MATH=ON
             -DGLM_TEST_ENABLE=ON
             -DCMAKE_BUILD_TYPE=Release
             -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}
             -G
             "Unix Makefiles"
  BUILD_COMMAND make
  INSTALL_DIR "${CMAKE_BINARY_DIR}"
  INSTALL_COMMAND make install
  TEST_COMMAND "")

ExternalProject_Add(
  GLUT
  SVN_REPOSITORY
    "http://svn.code.sf.net/p/freeglut/code/trunk/freeglut/freeglut"
  UPDATE_COMMAND ""
  PATCH_COMMAND ""
  SOURCE_DIR "${CMAKE_SOURCE_DIR}/GLUT"
  CMAKE_ARGS -DCMAKE_INSTALL_LIBDIR=lib
             -DFREEGLUT_BUILD_SHARED_LIBS=ON
             -DFREEGLUT_BUILD_STATIC_LIBS=ON
             -DCMAKE_BUILD_TYPE=Release
             -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}
             -G
             "Unix Makefiles"
  BUILD_COMMAND make
  INSTALL_DIR "${CMAKE_BINARY_DIR}"
  INSTALL_COMMAND make install
  TEST_COMMAND "")

ExternalProject_Add(
  ASSIMP
  DEPENDS GLUT
  GIT_REPOSITORY "https://github.com/assimp/assimp.git"
  GIT_TAG "v.5.0.0.rc1"
  UPDATE_COMMAND ""
  PATCH_COMMAND ""
  SOURCE_DIR "${CMAKE_SOURCE_DIR}/ASSIMP"
  CMAKE_ARGS -DCMAKE_INSTALL_LIBDIR=lib
             -DBUILD_SHARED_LIBS=ON
             -DASSIMP_DOUBLE_PRECISION=ON
             -DASSIMP_BUILD_ASSIMP_TOOLS=ON
             -DASSIMP_BUILD_SAMPLES=ON
             -DASSIMP_BUILD_TESTS=ON
             -DBUILD_DOCS=ON
             -DCMAKE_BUILD_TYPE=Release
             -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}
             -DGLUT_INCLUDE_DIR=${CMAKE_BINARY_DIR}/include
             -DGLUT_glut_LIBRARY=${CMAKE_BINARY_DIR}/lib/libglut.so
             -G
             "Unix Makefiles"
  BUILD_COMMAND make
  INSTALL_DIR "${CMAKE_BINARY_DIR}"
  INSTALL_COMMAND make install
  TEST_COMMAND "")

ExternalProject_Add(
  GLEW
  GIT_REPOSITORY "https://github.com/nigels-com/glew.git"
  GIT_TAG "master"
  UPDATE_COMMAND ""
  PATCH_COMMAND make extensions
  SOURCE_DIR "${CMAKE_SOURCE_DIR}/GLEW"
  SOURCE_SUBDIR "build/cmake"
  CMAKE_ARGS -DCMAKE_INSTALL_LIBDIR=lib
             -DOpenGL_GL_PREFERENCE=GLVND
             -DBUILD_UTILS=OFF
             -DCMAKE_BUILD_TYPE=Release
             -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}
             -G
             "Unix Makefiles"
  BUILD_COMMAND make
  INSTALL_DIR "${CMAKE_BINARY_DIR}"
  INSTALL_COMMAND make install
  TEST_COMMAND "")

ExternalProject_Add(
  SOIL
  GIT_REPOSITORY "https://github.com/kbranigan/Simple-OpenGL-Image-Library.git"
  GIT_TAG "master"
  UPDATE_COMMAND ""
  PATCH_COMMAND ""
  SOURCE_DIR "${CMAKE_SOURCE_DIR}/SOIL"
  CMAKE_ARGS -DOpenGL_GL_PREFERENCE=GLVND -DCMAKE_BUILD_TYPE=Release
             -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR} -G "Unix Makefiles"
  BUILD_COMMAND make
  INSTALL_DIR "${CMAKE_BINARY_DIR}"
  INSTALL_COMMAND make install
  TEST_COMMAND "")

ExternalProject_Add(
  FMT
  GIT_REPOSITORY "https://github.com/fmtlib/fmt.git"
  GIT_TAG "master"
  UPDATE_COMMAND ""
  PATCH_COMMAND ""
  SOURCE_DIR "${CMAKE_SOURCE_DIR}/FMT"
  CMAKE_ARGS -DCMAKE_INSTALL_LIBDIR=lib
             -DFMT_DOC=ON
             -DFMT_INSTALL=ON
             -DBUILD_SHARED_LIBS=ON
             -DCMAKE_BUILD_TYPE=Release
             -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}
             -G
             "Unix Makefiles"
  INSTALL_DIR "${CMAKE_BINARY_DIR}"
  INSTALL_COMMAND make install
  TEST_COMMAND "")

ExternalProject_Add(
  OPENAL
  GIT_REPOSITORY "https://github.com/kcat/openal-soft.git"
  GIT_TAG "1.21.1"
  UPDATE_COMMAND ""
  PATCH_COMMAND ""
  SOURCE_DIR "${CMAKE_SOURCE_DIR}/OPENAL"
  CMAKE_ARGS -DALSOFT_UTILS=ON
             -DALSOFT_EXAMPLES=OFF
             -DALSOFT_TESTS=ON
             -DALSOFT_INSTALL=ON
             -DCMAKE_INSTALL_LIBDIR=lib
             -DCMAKE_BUILD_TYPE=Release
             -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}
             -G
             "Unix Makefiles"
  INSTALL_DIR "${CMAKE_BINARY_DIR}"
  INSTALL_COMMAND make install
  TEST_COMMAND "")

ExternalProject_Add(
  OPUS
  GIT_REPOSITORY "https://gitlab.xiph.org/xiph/opus.git"
  GIT_TAG "master"
  UPDATE_COMMAND ""
  PATCH_COMMAND ""
  SOURCE_DIR "${CMAKE_SOURCE_DIR}/OPUS"
  CMAKE_ARGS -DCMAKE_INSTALL_LIBDIR=lib
             -DCMAKE_BUILD_TYPE=Release
             -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}
             -DBUILD_SHARED_LIBS=ON
             -DBUILD_PROGRAMS=ON
             -DBUILD_TESTING=ON
             -G
             "Unix Makefiles"
  INSTALL_DIR "${CMAKE_BINARY_DIR}"
  INSTALL_COMMAND make install
  TEST_COMMAND "")

ExternalProject_Add(
  OGG
  GIT_REPOSITORY "https://gitlab.xiph.org/xiph/ogg.git"
  GIT_TAG "master"
  UPDATE_COMMAND ""
  PATCH_COMMAND ""
  SOURCE_DIR "${CMAKE_SOURCE_DIR}/OGG"
  CMAKE_ARGS -DCMAKE_INSTALL_LIBDIR=lib
             -DCMAKE_BUILD_TYPE=Release
             -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}
             -DBUILD_SHARED_LIBS=ON
             -DBUILD_TESTING=ON
             -G
             "Unix Makefiles"
  INSTALL_DIR "${CMAKE_BINARY_DIR}"
  INSTALL_COMMAND make install
  TEST_COMMAND "")

ExternalProject_Add(
  FLAC
  DEPENDS OGG
  GIT_REPOSITORY "https://gitlab.xiph.org/xiph/flac.git"
  GIT_TAG "master"
  UPDATE_COMMAND ""
  PATCH_COMMAND ""
  SOURCE_DIR "${CMAKE_SOURCE_DIR}/FLAC"
  CMAKE_ARGS -DCMAKE_INSTALL_LIBDIR=lib
             -DCMAKE_BUILD_TYPE=Release
             -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}
             -DWITH_OGG=ON
             -DBUILD_CXXLIBS=ON
             -DBUILD_EXAMPLES=ON
             -DBUILD_TESTING=ON
             -G
             "Unix Makefiles"
  INSTALL_DIR "${CMAKE_BINARY_DIR}"
  INSTALL_COMMAND make install
  TEST_COMMAND "")

ExternalProject_Add(
  VORBIS
  DEPENDS OGG
  GIT_REPOSITORY "https://gitlab.xiph.org/xiph/vorbis.git"
  GIT_TAG "master"
  UPDATE_COMMAND ""
  PATCH_COMMAND ""
  SOURCE_DIR "${CMAKE_SOURCE_DIR}/VORBIS"
  CMAKE_ARGS -DCMAKE_INSTALL_LIBDIR=lib -DCMAKE_BUILD_TYPE=Release
             -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR} -DBUILD_SHARED_LIBS=ON
             -G "Unix Makefiles"
  INSTALL_DIR "${CMAKE_BINARY_DIR}"
  INSTALL_COMMAND make install
  TEST_COMMAND "")

ExternalProject_Add(
  MPG123
  SVN_REPOSITORY "svn://scm.orgis.org/mpg123/trunk"
  UPDATE_COMMAND ""
  PATCH_COMMAND "autoreconf" "-iv"
  SOURCE_DIR "${CMAKE_SOURCE_DIR}/MPG123"
  CONFIGURE_COMMAND "${CMAKE_SOURCE_DIR}/MPG123/configure"
                    "--prefix=${CMAKE_BINARY_DIR}" "--with-optimization=3"
  BUILD_COMMAND make
  INSTALL_DIR "${CMAKE_BINARY_DIR}"
  INSTALL_COMMAND make install
  TEST_COMMAND "")

ExternalProject_Add(
  LAME
  DEPENDS VORBIS
  SVN_REPOSITORY "https://svn.code.sf.net/p/lame/svn/trunk/lame"
  PATCH_COMMAND ""
  SOURCE_DIR "${CMAKE_SOURCE_DIR}/LAME"
  CONFIGURE_COMMAND
    "${CMAKE_SOURCE_DIR}/LAME/configure" "--prefix=${CMAKE_BINARY_DIR}"
    "--with-vorbis" "--with-vorbis-prefix=${CMAKE_BINARY_DIR}"
  BUILD_COMMAND make
  INSTALL_DIR "${CMAKE_BINARY_DIR}"
  INSTALL_COMMAND make install
  TEST_COMMAND "")

ExternalProject_Add(
  LIBSNDFILE
  DEPENDS OGG VORBIS FLAC OPUS MPG123 LAME
  GIT_REPOSITORY "https://github.com/erikd/libsndfile.git"
  GIT_TAG "master"
  UPDATE_COMMAND ""
  PATCH_COMMAND ""
  SOURCE_DIR "${CMAKE_SOURCE_DIR}/LIBSNDFILE"
  CMAKE_ARGS -DCMAKE_INSTALL_LIBDIR=lib
             -DCMAKE_BUILD_TYPE=Release
             -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}
             -DENABLE_EXTERNAL_LIBS=OFF
             -DENABLE_EXPERIMENTAL=OFF
             -G
             "Unix Makefiles"
  INSTALL_DIR "${CMAKE_BINARY_DIR}"
  INSTALL_COMMAND make install
  TEST_COMMAND "")

# Build the GLAD library This depends on sources that need to be manually
# downloaded
if(EXISTS "${CMAKE_SOURCE_DIR}/GLAD")
  add_library(GLAD ${CMAKE_SOURCE_DIR}/GLAD/src/glad.c)
  target_include_directories(GLAD PUBLIC ${CMAKE_SOURCE_DIR}/GLAD/include)
else()
  message(FATAL_ERROR "Unable to find GLAD. You must install this first.")
  message(FATAL_ERROR "Go to https://glad.dav1d.de/ and generate GLAD files.")
endif(EXISTS "${CMAKE_SOURCE_DIR}/GLAD")
