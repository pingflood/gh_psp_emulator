project(yabause)

include(CheckFunctionExists)
include(CheckIncludeFile)

set(yabause_SOURCES
	bios.c
	cdbase.c cheat.c coffelf.c cs0.c cs1.c cs2.c
	debug.c
	error.c
	m68kcore.c m68kd.c memory.c movie.c
	netlink.c
	peripheral.c profile.c
	scu.c sh2core.c sh2d.c sh2idle.c sh2int.c sh2trace.c smpc.c snddummy.c
	vdp1.c vdp2.c vdp2debug.c vidogl.c vidshared.c vidsoft.c
	yabause.c ygl.c)

# new SCSP
option(YAB_USE_SCSP2 "Use the new SCSP implementation.")
if (YAB_USE_SCSP2)
	add_definitions(-DUSE_SCSP2=1)
	set(yabause_SOURCES ${yabause_SOURCES} scsp2.c)
else()
	set(yabause_SOURCES ${yabause_SOURCES} scsp.c)
endif()

# math library
if (UNIX)
    set(YABAUSE_LIBRARIES ${YABAUSE_LIBRARIES} "m")
endif()

# Bigendian
include(TestBigEndian)
test_big_endian(WORDS_BIGENDIAN)
if (WORDS_BIGENDIAN)
	add_definitions(-DWORDS_BIGENDIAN=1)
endif (WORDS_BIGENDIAN)

include(CheckCSourceCompiles)

# variadic macros
check_c_source_compiles("#define MACRO(...) puts(__VA_ARGS__)
	int main(int argc, char ** argv) { MACRO(\"foo\"); }"
	VARIADIC_MACROS_OK)
if (VARIADIC_MACROS_OK)
	add_definitions(-DHAVE_C99_VARIADIC_MACROS=1)
endif (VARIADIC_MACROS_OK)

# gettimeofday
check_function_exists(gettimeofday GETTIMEOFDAY_OK)
if (GETTIMEOFDAY_OK)
    add_definitions(-DHAVE_GETTIMEOFDAY=1)
endif ()

# floorf
set(CMAKE_REQUIRED_LIBRARIES ${CMAKE_REQUIRED_LIBRARIES} "-lm")
check_function_exists(floorf FLOORF_OK)
if (FLOORF_OK)
	add_definitions(-DHAVE_FLOORF=1)
endif ()

# sys/time.h
check_include_file("sys/time.h" SYSTIME_OK)
if (SYSTIME_OK)
	add_definitions(-DHAVE_SYS_TIME_H=1)
endif()

# Find stdint.h
check_include_file("stdint.h" STDINT_H_FOUND)
if (STDINT_H_FOUND)
	add_definitions(-DHAVE_STDINT_H=1)
endif()

# OpenGL
option(YAB_WANT_OPENGL "use OpenGL for video output (most ports require it)" ON)
if (YAB_WANT_OPENGL)
	include(FindOpenGL)
	if (OPENGL_FOUND)
		add_definitions(-DHAVE_LIBGL=1)
		set(YABAUSE_LIBRARIES ${YABAUSE_LIBRARIES} ${OPENGL_LIBRARIES})

		include(FindGLUT)
		if (GLUT_FOUND)
			include_directories(${GLUT_INCLUDE_DIR})
			add_definitions(-DHAVE_LIBGLUT=1)
			set(YABAUSE_LIBRARIES ${YABAUSE_LIBRARIES} ${GLUT_LIBRARIES})
		endif()

		# glXGetProcAddress
		set(CMAKE_REQUIRED_LIBRARIES ${CMAKE_REQUIRED_LIBRARIES} ${OPENGL_LIBRARIES})
		check_function_exists(glXGetProcAddress GLXGETPROCADDRESS_OK)
		if (GLXGETPROCADDRESS_OK)
			add_definitions(-DHAVE_GLXGETPROCADDRESS=1)
		endif()
	endif(OPENGL_FOUND)
endif (YAB_WANT_OPENGL)

# SDL
option(YAB_WANT_SDL "use SDL cores if available" ON)
if (YAB_WANT_SDL)
	include(FindSDL)
	if (SDL_FOUND)
		add_definitions(-DHAVE_LIBSDL=1)
		include_directories(${SDL_INCLUDE_DIR})
		set(yabause_SOURCES ${yabause_SOURCES} persdljoy.c sndsdl.c)
		set(YABAUSE_LIBRARIES ${YABAUSE_LIBRARIES} ${SDL_LIBRARY})
	endif (SDL_FOUND)
endif (YAB_WANT_SDL)

# OpenAL
option(YAB_WANT_OPENAL "use OpenAL sound core if available" ON)
if (YAB_WANT_OPENAL)
	include(FindOpenAL)
	if (OPENAL_FOUND)
		add_definitions(-DHAVE_LIBAL=1)
		include_directories(${OPENAL_INCLUDE_DIR})
		set(yabause_SOURCES ${yabause_SOURCES} sndal.c)
		set(YABAUSE_LIBRARIES ${YABAUSE_LIBRARIES} ${OPENAL_LIBRARY})
	endif (OPENAL_FOUND)
endif (YAB_WANT_OPENAL)

# mini18n
find_path(MINI18N_INCLUDE_DIR mini18n.h)
find_library(MINI18N_LIBRARY mini18n)
if (NOT MINI18N_INCLUDE_DIR STREQUAL "MINI18N_INCLUDE_DIR-NOTFOUND" AND NOT MINI18N_LIBRARY STREQUAL "MINI18N_LIBRARY-NOTFOUND")
	set(MINI18N_FOUND TRUE)
	include_directories(${MINI18N_INCLUDE_DIR})
	add_definitions(-DHAVE_LIBMINI18N=1)
	set(YABAUSE_LIBRARIES ${YABAUSE_LIBRARIES} ${MINI18N_LIBRARY})
endif (NOT MINI18N_INCLUDE_DIR STREQUAL "MINI18N_INCLUDE_DIR-NOTFOUND" AND NOT MINI18N_LIBRARY STREQUAL "MINI18N_LIBRARY-NOTFOUND")

if (MINI18N_FOUND)
	if (UNIX)
		add_definitions(-DYTSDIR=\"${CMAKE_INSTALL_PREFIX}/share/${YAB_PACKAGE}/yts\")
	elseif (WIN32)
		add_definitions(-DYTSDIR=\"trans\")
	endif()
endif()

# APPLE // not necessary mac os x, but i don't care ;)
if (APPLE)
	FIND_LIBRARY(COREFOUNDATION_LIBRARY NAMES CoreFoundation )
	FIND_LIBRARY(IOKIT_LIBRARY NAMES IOKit )
	set(yabause_SOURCES ${yabause_SOURCES} macjoy.c permacjoy.c cd-macosx.c)
	set(YABAUSE_LIBRARIES ${YABAUSE_LIBRARIES} ${COREFOUNDATION_LIBRARY} ${IOKIT_LIBRARY})
endif (APPLE)

# Visual Studio
if (MSVC)
        # Find DDK
        if (EXISTS "$ENV{SYSTEMDRIVE}/WINDDK/3790.1830/")
		set(DDK_DIR "$ENV{SYSTEMDRIVE}/WINDDK/3790.1830/")
        elseif (EXISTS "$ENV{SYSTEMDRIVE}/WINDDK/6000/")
		set(DDK_DIR "$ENV{SYSTEMDRIVE}/WINDDK/6000/")
        elseif (EXISTS "$ENV{SYSTEMDRIVE}/WINDDK/7600.16385.0/")
		set(DDK_DIR "$ENV{SYSTEMDRIVE}/WINDDK/7600.16385.0/")
        endif (EXISTS "$ENV{SYSTEMDRIVE}/WINDDK/3790.1830/")

	add_definitions(-DHAVE_C99_VARIADIC_MACROS -D_CRT_SECURE_NO_WARNINGS -DC68K_NO_JUMP_TABLE 
			-D_UNICODE -DUNICODE)
endif (MSVC)

# Windows ddk
if (WIN32)
	# Find ntddcdrm.h
	find_path(ntddcdrm_INCLUDE_DIR ntddcdrm.h
		PATHS "${DDK_DIR}" "${DDK_DIR}/inc" PATH_SUFFIXES ddk api)

	if (ntddcdrm_INCLUDE_DIR)
		include_directories(${ntddcdrm_INCLUDE_DIR})
		message(STATUS "Found ntddcdrm.h: ${ntddcdrm_INCLUDE_DIR}")
	else (ntddcdrm_INCLUDE_DIR)
		message(STATUS "Could not find ntddcdrm.h")
	endif (ntddcdrm_INCLUDE_DIR)

	set(yabause_SOURCES ${yabause_SOURCES} cd-windows.c)
endif (WIN32)

if (WII)
	set(CMAKE_C_FLAGS "-mrvl -mcpu=750 -meabi -mhard-float")
	add_definitions(-DGEKKO=1)
	# that shouldn't be hardcoded, either use an ENV variable or try to detect it...
	include_directories(/opt/devkitpro/libogc/include/)
endif()

# SH2 dynamic recompiler
message(STATUS "CMAKE_SYSTEM_NAME ${CMAKE_SYSTEM_NAME}")
message(STATUS "CMAKE_SYSTEM_PROCESSOR ${CMAKE_SYSTEM_PROCESSOR}")
option(YAB_SH2_DYNAREC "SH2 dynamic recompiler" ON)
if (YAB_SH2_DYNAREC)
	if("${CMAKE_SYSTEM_NAME}" STREQUAL "Linux")
		if("${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "i686")
			enable_language(ASM-ATT)
			set(yabause_SOURCES ${yabause_SOURCES}
				sh2_dynarec/sh2_dynarec.c sh2_dynarec/linkage_x86.s)
			add_definitions(-DSH2_DYNAREC=1)
		endif("${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "i686")
		if("${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "x86_64")
			enable_language(ASM-ATT)
			set(yabause_SOURCES ${yabause_SOURCES}
				sh2_dynarec/sh2_dynarec.c sh2_dynarec/linkage_x64.s)
			add_definitions(-DSH2_DYNAREC=1)
		endif("${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "x86_64")
		if("${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "armv7l")
			enable_language(ASM-ATT)
			set(yabause_SOURCES ${yabause_SOURCES}
				sh2_dynarec/sh2_dynarec.c sh2_dynarec/linkage_arm.s)
			add_definitions(-DSH2_DYNAREC=1)
		endif("${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "armv7l")
	endif("${CMAKE_SYSTEM_NAME}" STREQUAL "Linux")
endif (YAB_SH2_DYNAREC)

# c68k
option(YAB_WANT_C68K "enable c68k compilation" ON)
if (YAB_WANT_C68K)
	include(ExternalProject)
	ExternalProject_Add(c68kinc
		DOWNLOAD_COMMAND ""
		SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/c68k
		CMAKE_GENERATOR "${CMAKE_GENERATOR}"
		INSTALL_COMMAND ""
		BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/c68k
	)

	add_definitions(-DHAVE_C68K=1)
	include_directories(${CMAKE_CURRENT_BINARY_DIR}/c68k)
	set(yabause_SOURCES ${yabause_SOURCES} c68k/c68kexec.c c68k/c68k.c c68k/gen68k.c m68kc68k.c)
	if (MSVC)
		set_source_files_properties(c68k/c68kexec.c PROPERTIES COMPILE_FLAGS "/Od")
	else()
		set_source_files_properties(c68k/c68kexec.c PROPERTIES COMPILE_FLAGS "-O0")
	endif()
endif(YAB_WANT_C68K)

# q68
option(YAB_WANT_Q68 "enable q68 compilation" OFF)
if (YAB_WANT_Q68)
	add_definitions(-DHAVE_Q68=1)
	set(yabause_SOURCES ${yabause_SOURCES}
		m68kq68.c q68/q68.c q68/q68-core.c q68/q68-disasm.c
		q68/q68-const.h q68/q68.h q68/q68-internal.h q68/q68-jit.h q68/q68-jit-psp.h q68/q68-jit-x86.h)
endif()

# *DEBUG
set(YAB_DEBUG "" CACHE STRING "List of enabled debug information") 
foreach(DEBUG IN LISTS YAB_DEBUG)
	if (${DEBUG} STREQUAL "main")
		add_definitions(-DDEBUG=1)
	elseif (${DEBUG} STREQUAL "cd")
		add_definitions(-DCDDEBUG=1)
	elseif (${DEBUG} STREQUAL "idle")
		add_definitions(-DIDLE_DETECT_VERBOSE=1)
	else (${DEBUG} STREQUAL "main")
		string(TOUPPER ${DEBUG} UPDEBUG)
		add_definitions(-D${UPDEBUG}_DEBUG=1)
	endif (${DEBUG} STREQUAL "main")
endforeach(DEBUG)

# Mic shaders
option(YAB_SHADERS "Enable OpenGL shaders for gouraud and mesh")
if (GLXGETPROCADDRESS_OK AND YAB_SHADERS)
	add_definitions(-DUSEMICSHADERS=1)
endif()

# Network
option(YAB_NETWORK "Enable network")
if (YAB_NETWORK)
	add_definitions(-DUSESOCKET=1)
endif()

# Peripheral key name
option(YAB_PERKEYNAME "Try to display key names instead of cryptic values" OFF)
if (YAB_PERKEYNAME)
	add_definitions(-DPERKEYNAME=1)
endif()

# Exec from cache
option(YAB_EXEC_FROM_CACHE "Allow code execution from 0xC0000000" OFF)
if (YAB_EXEC_FROM_CACHE)
	add_definitions(-DEXEC_FROM_CACHE=1)
endif()

# Optimized DMA
option(YAB_OPTIMIZED_DMA "Use optimized DMA when possible" OFF)
if (YAB_OPTIMIZED_DMA)
	add_definitions(-DOPTIMIZED_DMA=1)
endif()

# Yabause Arch
if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
	add_definitions(-DARCH_IS_MACOSX=1)
	set(yabause_SOURCES ${yabause_SOURCES} thr-dummy.c)
elseif (${CMAKE_SYSTEM_NAME} MATCHES "FreeBSD")
	add_definitions(-DARCH_IS_FREEBSD=1)
	set(yabause_SOURCES ${yabause_SOURCES} thr-dummy.c cd-freebsd.c)
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
	add_definitions(-DARCH_IS_LINUX=1)
	set(yabause_SOURCES ${yabause_SOURCES} thr-linux.c cd-linux.c)

	check_include_file("linux/joystick.h" LINUX_HAS_JOYSTICK)
	if (LINUX_HAS_JOYSTICK)
		set(yabause_SOURCES ${yabause_SOURCES} perlinuxjoy.c)
	endif()

	check_c_source_compiles("
		#include <linux/cdrom.h>
		int main(int argc, char ** argv) { int i = CDSL_CURRENT; }
	" LINUX_CDROM_H_OK)
	if (NOT LINUX_CDROM_H_OK)
       		add_definitions(-DLINUX_CDROM_H_IS_BROKEN)
	endif (NOT LINUX_CDROM_H_OK)
elseif (${CMAKE_SYSTEM_NAME} MATCHES "NetBSD")
	add_definitions(-DARCH_IS_NETBSD=1)
	set(yabause_SOURCES ${yabause_SOURCES} thr-dummy.c cd-netbsd.c)
elseif (${CMAKE_SYSTEM_NAME} MATCHES "OpenBSD")
	add_definitions(-DARCH_IS_NETBSD=1)
	set(yabause_SOURCES ${yabause_SOURCES} thr-dummy.c cd-netbsd.c)
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
	add_definitions(-DARCH_IS_WINDOWS=1)
	set(yabause_SOURCES ${yabause_SOURCES} thr-dummy.c)
else ()
	add_definitions(-DUNKNOWN_ARCH=1)
	set(yabause_SOURCES ${yabause_SOURCES} thr-dummy.c)
endif ()

if("${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "i686")
	set(CMAKE_C_FLAGS "-O3 -march=i686 -msse")
endif("${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "i686")
if("${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "x86_64")
	set(CMAKE_C_FLAGS "-O3")
endif("${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "x86_64")
if("${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "armv7l")
	set(CMAKE_C_FLAGS "-O3 -mcpu=cortex-a8 -mfpu=vfp -mfloat-abi=softfp")
endif("${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "armv7l")

add_definitions(-DPACKAGE=\"${YAB_PACKAGE}\")
add_definitions(-DVERSION=\"${YAB_VERSION}\")

add_library(yabause ${yabause_SOURCES})

if (YAB_WANT_C68K)
	add_dependencies(yabause c68kinc)
endif(YAB_WANT_C68K)

macro(yab_port_start)
	if (YAB_PORT_BUILT AND NOT YAB_MULTIBUILD)
		return()
	endif ()
endmacro(yab_port_start)

macro(yab_port_success YAB_TARGET)
	if (NOT YAB_MULTIBUILD)
		set_target_properties(${YAB_TARGET} PROPERTIES OUTPUT_NAME yabause)
		set(YAB_PORT_NAME "yabause")
	else ()
		set(YAB_PORT_NAME ${YAB_TARGET})
	endif ()
	set(YAB_PORT_BUILT TRUE PARENT_SCOPE)
endmacro(yab_port_success)

set(YAB_MAN_DIR "share/man")
if (NOT $ENV{PKGMANDIR} STREQUAL "")
    set(YAB_MAN_DIR $ENV{PKGMANDIR})
endif ()

option(YAB_MULTIBUILD "Choose wether to build all ports or only a single one")
set(YAB_PORT_BUILT FALSE)
set(YAB_PORTS "gtk;qt;windows;dreamcast;wii" CACHE STRING "List of ports to build")
foreach(PORT IN LISTS YAB_PORTS)
	add_subdirectory(${PORT})
endforeach(PORT)
