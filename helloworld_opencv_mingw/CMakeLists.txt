cmake_minimum_required(VERSION 2.8)


INCLUDE_DIRECTORIES(C:/MinGW/include)
INCLUDE_DIRECTORIES(C:/opencv/build/include)
LINK_DIRECTORIES(C:/opencv/build/x86/mingw/lib)


ADD_EXECUTABLE( test
                main.cpp
                )

TARGET_LINK_LIBRARIES(test opencv_core246 opencv_highgui246)

#file(MAKE_DIRECTORY bin)
#set(EXECUTABLE_OUTPUT_PATH bin)