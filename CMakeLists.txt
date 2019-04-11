cmake_minimum_required(VERSION 3.7)
project(tgrftool)

include_directories(include)

file(GLOB SOURCES "src/Ct*.cpp")
file(GLOB HEADERS "include/*.h")

add_library(libconstel ${SOURCES} ${HEADERS})

#add_executable(tgrftool "src/process.cpp")
add_executable(net2d "src/net2d.cpp")
add_executable(checklinks "src/checklinks.cpp")

find_package(CGAL REQUIRED)
find_package(OpenGL REQUIRED)
find_package(libLAS REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt5Core REQUIRED)
find_package(Qt5Widgets REQUIRED)

include_directories(${OPENGL_INCLUDE_DIRS}
										${GLUT_INCLUDE_DIRS}
				    		    ${LIBLAS_INCLUDE_DIRS}
										${Qt5Gui_INCLUDE_DIRS}
										${Qt5Core_INCLUDE_DIRS}
										${Qt5Widgets_INCLUDE_DIRS})

add_definitions(${Qt5Gui_DEFINITIONS})
add_definitions(${Qt5Core_DEFINITIONS})
add_definitions(${Qt5Widgets_DEFINITIONS})

set(CMAKE_CXX_FLAGS "${Qt5Gui_EXECUTABLE_COMPILE_FLAGS} ${Qt5Core_EXECUTABLE_COMPILE_FLAGS} ${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS}  -fPIC")

#target_link_libraries(tgrftool libconstel ${CGAL_LIBRARIES} ${OPENGL_LIBRARIES} ${GLUT_LIBRARY} ${libLAS_LIBRARIES}
#	${Qt5Gui_LIBRARIES} ${Qt5Core_LIBRARIES} ${Qt5Widgets_LIBRARIES} stdc++fs CGAL gmp )

target_link_libraries(net2d libconstel ${CGAL_LIBRARIES} ${OPENGL_LIBRARIES} ${GLUT_LIBRARY} ${libLAS_LIBRARIES}
	${Qt5Gui_LIBRARIES} ${Qt5Core_LIBRARIES} ${Qt5Widgets_LIBRARIES} stdc++fs CGAL gmp)

target_link_libraries(checklinks libconstel ${CGAL_LIBRARIES} ${OPENGL_LIBRARIES} ${GLUT_LIBRARY} ${libLAS_LIBRARIES}
	${Qt5Gui_LIBRARIES} ${Qt5Core_LIBRARIES} ${Qt5Widgets_LIBRARIES} stdc++fs CGAL gmp)
