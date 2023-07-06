cmake_minimum_required (VERSION 3.15)

project(NiceAeromatic++)

# set general stuff
set(PROJECT_VERSION_MAJOR "1")
set(PROJECT_VERSION_MINOR "2")
set(PROJECT_VERSION_PATCH "0.dev1")
set(PROJECT_CONTACT_EMAIL "genimotion@ya.ru")
set(PROJECT_CONTACT_VENDOR "Dream team")
set(PROJECT_VERSION "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}")
set(LIBRARY_VERSION ${PROJECT_VERSION})
set(LIBRARY_SOVERSION ${PROJECT_VERSION_MAJOR})

set(CMAKE_DEBUG_POSTFIX d)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED)

# set imgui stuff
set(IMGUI_DIR ${CMAKE_SOURCE_DIR}/imgui)
set(SOURCE_DIR ${CMAKE_SOURCE_DIR}/app)
set(ROOT ${IMGUI_DIR})

# set aeromatic stuff
set(AEROMATIC_DIR ${CMAKE_SOURCE_DIR}/aeromatic++)


# imgui build
find_package(glfw3 REQUIRED)
find_package(OpenGL REQUIRED)

add_library(imgui STATIC
	${ROOT}/imgui.cpp
	${ROOT}/imgui.h
	${ROOT}/imstb_rectpack.h
	${ROOT}/imstb_textedit.h
	${ROOT}/imstb_truetype.h
    ${ROOT}/imgui_demo.cpp
    ${ROOT}/imgui_draw.cpp
    ${ROOT}/imgui_internal.h
    ${ROOT}/imgui_tables.cpp
    ${ROOT}/imgui_widgets.cpp
    ${AEROMATIC_DIR}/Systems/Systems.h
    ${AEROMATIC_DIR}/types.h
    ${AEROMATIC_DIR}/Aircraft.h
)
target_include_directories(imgui PUBLIC
    $<BUILD_INTERFACE:${ROOT}>
    $<INSTALL_INTERFACE:include>)
	
set(INSTALL_TARGETS imgui)
set(INSTALL_HEADERS ${ROOT}/imgui.h ${ROOT}/imconfig.h)

foreach(BACKEND glfw opengl3)
	set(NAME imgui_impl_${BACKEND})
	set(HEADER ${ROOT}/backends/${NAME}.h)
	add_library(${NAME} STATIC ${ROOT}/backends/${NAME}.cpp ${HEADER})
	target_link_libraries(${NAME} PUBLIC imgui)
	target_include_directories(${NAME} PUBLIC
	    $<BUILD_INTERFACE:${ROOT}/backends>
	    $<INSTALL_INTERFACE:include>)
	LIST(APPEND INSTALL_TARGETS ${NAME})
	LIST(APPEND INSTALL_HEADERS ${HEADER})
endforeach()

target_link_libraries(imgui_impl_glfw PRIVATE glfw)
target_sources(imgui_impl_opengl3 PRIVATE ${ROOT}/backends/imgui_impl_opengl3_loader.h)
target_link_libraries(imgui_impl_opengl3 PRIVATE OpenGL::GL)

install(TARGETS ${INSTALL_TARGETS}
    EXPORT imgui-targets DESTINATION lib)
install(EXPORT imgui-targets
    FILE imgui-config.cmake
    NAMESPACE imgui::
    DESTINATION lib/cmake/imgui)
install(FILES ${INSTALL_HEADERS}
    DESTINATION include)

# aeromatic build
add_library(aeromatic STATIC
    ${AEROMATIC_DIR}/types.cpp
    ${AEROMATIC_DIR}/Aircraft.cpp
    ${AEROMATIC_DIR}/AeroLightGA.cpp
    ${AEROMATIC_DIR}/AeroFighterJet.cpp
    ${AEROMATIC_DIR}/AeroHighPerformance.cpp
    ${AEROMATIC_DIR}/AeroJetTransport.cpp
    ${AEROMATIC_DIR}/AeroPropTransport.cpp
    ${AEROMATIC_DIR}/Systems/Controls.cpp
    ${AEROMATIC_DIR}/Systems/Flaps.cpp
    ${AEROMATIC_DIR}/Systems/LandingGear.cpp
    ${AEROMATIC_DIR}/Systems/Propulsion.cpp
    ${AEROMATIC_DIR}/Systems/Speedbrake.cpp
    ${AEROMATIC_DIR}/Systems/Chute.cpp
    ${AEROMATIC_DIR}/Systems/ArrestorHook.cpp
    ${AEROMATIC_DIR}/Systems/Spoilers.cpp
    ${AEROMATIC_DIR}/Systems/Thruster.cpp
    ${AEROMATIC_DIR}/Systems/ThrustReverse.cpp
    ${AEROMATIC_DIR}/Systems/Catapult.cpp
    ${AEROMATIC_DIR}/types.h
    ${AEROMATIC_DIR}/Aircraft.h
	${AEROMATIC_DIR}/config.h
    ${AEROMATIC_DIR}/Systems/Controls.h
    ${AEROMATIC_DIR}/Systems/Propulsion.h
    ${AEROMATIC_DIR}/Systems/Systems.h
    ${AEROMATIC_DIR}/Systems/Thruster.h
)

target_include_directories(aeromatic PUBLIC
    $<BUILD_INTERFACE:${AEROMATIC_DIR}>
    $<INSTALL_INTERFACE:include>)


IF(WIN32)
   SET(EXTRA_LIBS "shlwapi.lib")
   IF(MSVC)
     SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_SCL_SECURE_NO_WARNINGS")
   ENDIF(MSVC)
ENDIF(WIN32)

# make target
add_executable(aeromatic_app ${SOURCE_DIR}/aeromatic_app.cpp)
target_link_libraries(aeromatic_app
	PRIVATE imgui imgui_impl_glfw imgui_impl_opengl3 aeromatic glfw
)