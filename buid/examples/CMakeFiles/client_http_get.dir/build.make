# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.10

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/free/Downloads/neat

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/free/Downloads/neat/buid

# Include any dependencies generated for this target.
include examples/CMakeFiles/client_http_get.dir/depend.make

# Include the progress variables for this target.
include examples/CMakeFiles/client_http_get.dir/progress.make

# Include the compile flags for this target's objects.
include examples/CMakeFiles/client_http_get.dir/flags.make

examples/CMakeFiles/client_http_get.dir/client_http_get.c.o: examples/CMakeFiles/client_http_get.dir/flags.make
examples/CMakeFiles/client_http_get.dir/client_http_get.c.o: ../examples/client_http_get.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/free/Downloads/neat/buid/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object examples/CMakeFiles/client_http_get.dir/client_http_get.c.o"
	cd /home/free/Downloads/neat/buid/examples && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/client_http_get.dir/client_http_get.c.o   -c /home/free/Downloads/neat/examples/client_http_get.c

examples/CMakeFiles/client_http_get.dir/client_http_get.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/client_http_get.dir/client_http_get.c.i"
	cd /home/free/Downloads/neat/buid/examples && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/free/Downloads/neat/examples/client_http_get.c > CMakeFiles/client_http_get.dir/client_http_get.c.i

examples/CMakeFiles/client_http_get.dir/client_http_get.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/client_http_get.dir/client_http_get.c.s"
	cd /home/free/Downloads/neat/buid/examples && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/free/Downloads/neat/examples/client_http_get.c -o CMakeFiles/client_http_get.dir/client_http_get.c.s

examples/CMakeFiles/client_http_get.dir/client_http_get.c.o.requires:

.PHONY : examples/CMakeFiles/client_http_get.dir/client_http_get.c.o.requires

examples/CMakeFiles/client_http_get.dir/client_http_get.c.o.provides: examples/CMakeFiles/client_http_get.dir/client_http_get.c.o.requires
	$(MAKE) -f examples/CMakeFiles/client_http_get.dir/build.make examples/CMakeFiles/client_http_get.dir/client_http_get.c.o.provides.build
.PHONY : examples/CMakeFiles/client_http_get.dir/client_http_get.c.o.provides

examples/CMakeFiles/client_http_get.dir/client_http_get.c.o.provides.build: examples/CMakeFiles/client_http_get.dir/client_http_get.c.o


examples/CMakeFiles/client_http_get.dir/util.c.o: examples/CMakeFiles/client_http_get.dir/flags.make
examples/CMakeFiles/client_http_get.dir/util.c.o: ../examples/util.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/free/Downloads/neat/buid/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building C object examples/CMakeFiles/client_http_get.dir/util.c.o"
	cd /home/free/Downloads/neat/buid/examples && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/client_http_get.dir/util.c.o   -c /home/free/Downloads/neat/examples/util.c

examples/CMakeFiles/client_http_get.dir/util.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/client_http_get.dir/util.c.i"
	cd /home/free/Downloads/neat/buid/examples && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/free/Downloads/neat/examples/util.c > CMakeFiles/client_http_get.dir/util.c.i

examples/CMakeFiles/client_http_get.dir/util.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/client_http_get.dir/util.c.s"
	cd /home/free/Downloads/neat/buid/examples && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/free/Downloads/neat/examples/util.c -o CMakeFiles/client_http_get.dir/util.c.s

examples/CMakeFiles/client_http_get.dir/util.c.o.requires:

.PHONY : examples/CMakeFiles/client_http_get.dir/util.c.o.requires

examples/CMakeFiles/client_http_get.dir/util.c.o.provides: examples/CMakeFiles/client_http_get.dir/util.c.o.requires
	$(MAKE) -f examples/CMakeFiles/client_http_get.dir/build.make examples/CMakeFiles/client_http_get.dir/util.c.o.provides.build
.PHONY : examples/CMakeFiles/client_http_get.dir/util.c.o.provides

examples/CMakeFiles/client_http_get.dir/util.c.o.provides.build: examples/CMakeFiles/client_http_get.dir/util.c.o


examples/CMakeFiles/client_http_get.dir/picohttpparser.c.o: examples/CMakeFiles/client_http_get.dir/flags.make
examples/CMakeFiles/client_http_get.dir/picohttpparser.c.o: ../examples/picohttpparser.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/free/Downloads/neat/buid/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building C object examples/CMakeFiles/client_http_get.dir/picohttpparser.c.o"
	cd /home/free/Downloads/neat/buid/examples && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/client_http_get.dir/picohttpparser.c.o   -c /home/free/Downloads/neat/examples/picohttpparser.c

examples/CMakeFiles/client_http_get.dir/picohttpparser.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/client_http_get.dir/picohttpparser.c.i"
	cd /home/free/Downloads/neat/buid/examples && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/free/Downloads/neat/examples/picohttpparser.c > CMakeFiles/client_http_get.dir/picohttpparser.c.i

examples/CMakeFiles/client_http_get.dir/picohttpparser.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/client_http_get.dir/picohttpparser.c.s"
	cd /home/free/Downloads/neat/buid/examples && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/free/Downloads/neat/examples/picohttpparser.c -o CMakeFiles/client_http_get.dir/picohttpparser.c.s

examples/CMakeFiles/client_http_get.dir/picohttpparser.c.o.requires:

.PHONY : examples/CMakeFiles/client_http_get.dir/picohttpparser.c.o.requires

examples/CMakeFiles/client_http_get.dir/picohttpparser.c.o.provides: examples/CMakeFiles/client_http_get.dir/picohttpparser.c.o.requires
	$(MAKE) -f examples/CMakeFiles/client_http_get.dir/build.make examples/CMakeFiles/client_http_get.dir/picohttpparser.c.o.provides.build
.PHONY : examples/CMakeFiles/client_http_get.dir/picohttpparser.c.o.provides

examples/CMakeFiles/client_http_get.dir/picohttpparser.c.o.provides.build: examples/CMakeFiles/client_http_get.dir/picohttpparser.c.o


examples/CMakeFiles/client_http_get.dir/webrtc_signaling.c.o: examples/CMakeFiles/client_http_get.dir/flags.make
examples/CMakeFiles/client_http_get.dir/webrtc_signaling.c.o: ../examples/webrtc_signaling.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/free/Downloads/neat/buid/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building C object examples/CMakeFiles/client_http_get.dir/webrtc_signaling.c.o"
	cd /home/free/Downloads/neat/buid/examples && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/client_http_get.dir/webrtc_signaling.c.o   -c /home/free/Downloads/neat/examples/webrtc_signaling.c

examples/CMakeFiles/client_http_get.dir/webrtc_signaling.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/client_http_get.dir/webrtc_signaling.c.i"
	cd /home/free/Downloads/neat/buid/examples && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/free/Downloads/neat/examples/webrtc_signaling.c > CMakeFiles/client_http_get.dir/webrtc_signaling.c.i

examples/CMakeFiles/client_http_get.dir/webrtc_signaling.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/client_http_get.dir/webrtc_signaling.c.s"
	cd /home/free/Downloads/neat/buid/examples && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/free/Downloads/neat/examples/webrtc_signaling.c -o CMakeFiles/client_http_get.dir/webrtc_signaling.c.s

examples/CMakeFiles/client_http_get.dir/webrtc_signaling.c.o.requires:

.PHONY : examples/CMakeFiles/client_http_get.dir/webrtc_signaling.c.o.requires

examples/CMakeFiles/client_http_get.dir/webrtc_signaling.c.o.provides: examples/CMakeFiles/client_http_get.dir/webrtc_signaling.c.o.requires
	$(MAKE) -f examples/CMakeFiles/client_http_get.dir/build.make examples/CMakeFiles/client_http_get.dir/webrtc_signaling.c.o.provides.build
.PHONY : examples/CMakeFiles/client_http_get.dir/webrtc_signaling.c.o.provides

examples/CMakeFiles/client_http_get.dir/webrtc_signaling.c.o.provides.build: examples/CMakeFiles/client_http_get.dir/webrtc_signaling.c.o


examples/CMakeFiles/client_http_get.dir/gyropi.cpp.o: examples/CMakeFiles/client_http_get.dir/flags.make
examples/CMakeFiles/client_http_get.dir/gyropi.cpp.o: ../examples/gyropi.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/free/Downloads/neat/buid/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Building CXX object examples/CMakeFiles/client_http_get.dir/gyropi.cpp.o"
	cd /home/free/Downloads/neat/buid/examples && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/client_http_get.dir/gyropi.cpp.o -c /home/free/Downloads/neat/examples/gyropi.cpp

examples/CMakeFiles/client_http_get.dir/gyropi.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/client_http_get.dir/gyropi.cpp.i"
	cd /home/free/Downloads/neat/buid/examples && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/free/Downloads/neat/examples/gyropi.cpp > CMakeFiles/client_http_get.dir/gyropi.cpp.i

examples/CMakeFiles/client_http_get.dir/gyropi.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/client_http_get.dir/gyropi.cpp.s"
	cd /home/free/Downloads/neat/buid/examples && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/free/Downloads/neat/examples/gyropi.cpp -o CMakeFiles/client_http_get.dir/gyropi.cpp.s

examples/CMakeFiles/client_http_get.dir/gyropi.cpp.o.requires:

.PHONY : examples/CMakeFiles/client_http_get.dir/gyropi.cpp.o.requires

examples/CMakeFiles/client_http_get.dir/gyropi.cpp.o.provides: examples/CMakeFiles/client_http_get.dir/gyropi.cpp.o.requires
	$(MAKE) -f examples/CMakeFiles/client_http_get.dir/build.make examples/CMakeFiles/client_http_get.dir/gyropi.cpp.o.provides.build
.PHONY : examples/CMakeFiles/client_http_get.dir/gyropi.cpp.o.provides

examples/CMakeFiles/client_http_get.dir/gyropi.cpp.o.provides.build: examples/CMakeFiles/client_http_get.dir/gyropi.cpp.o


# Object files for target client_http_get
client_http_get_OBJECTS = \
"CMakeFiles/client_http_get.dir/client_http_get.c.o" \
"CMakeFiles/client_http_get.dir/util.c.o" \
"CMakeFiles/client_http_get.dir/picohttpparser.c.o" \
"CMakeFiles/client_http_get.dir/webrtc_signaling.c.o" \
"CMakeFiles/client_http_get.dir/gyropi.cpp.o"

# External object files for target client_http_get
client_http_get_EXTERNAL_OBJECTS =

examples/client_http_get: examples/CMakeFiles/client_http_get.dir/client_http_get.c.o
examples/client_http_get: examples/CMakeFiles/client_http_get.dir/util.c.o
examples/client_http_get: examples/CMakeFiles/client_http_get.dir/picohttpparser.c.o
examples/client_http_get: examples/CMakeFiles/client_http_get.dir/webrtc_signaling.c.o
examples/client_http_get: examples/CMakeFiles/client_http_get.dir/gyropi.cpp.o
examples/client_http_get: examples/CMakeFiles/client_http_get.dir/build.make
examples/client_http_get: libneat.so.0.0.1~td161
examples/client_http_get: /usr/lib/x86_64-linux-gnu/libuv.so
examples/client_http_get: /usr/lib/x86_64-linux-gnu/libldns.so
examples/client_http_get: /usr/lib/x86_64-linux-gnu/libjansson.so
examples/client_http_get: /usr/lib/x86_64-linux-gnu/libssl.so
examples/client_http_get: /usr/lib/x86_64-linux-gnu/libcrypto.so
examples/client_http_get: /usr/lib/x86_64-linux-gnu/libmnl.so
examples/client_http_get: /usr/lib/x86_64-linux-gnu/libsctp.so
examples/client_http_get: examples/CMakeFiles/client_http_get.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/free/Downloads/neat/buid/CMakeFiles --progress-num=$(CMAKE_PROGRESS_6) "Linking CXX executable client_http_get"
	cd /home/free/Downloads/neat/buid/examples && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/client_http_get.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
examples/CMakeFiles/client_http_get.dir/build: examples/client_http_get

.PHONY : examples/CMakeFiles/client_http_get.dir/build

examples/CMakeFiles/client_http_get.dir/requires: examples/CMakeFiles/client_http_get.dir/client_http_get.c.o.requires
examples/CMakeFiles/client_http_get.dir/requires: examples/CMakeFiles/client_http_get.dir/util.c.o.requires
examples/CMakeFiles/client_http_get.dir/requires: examples/CMakeFiles/client_http_get.dir/picohttpparser.c.o.requires
examples/CMakeFiles/client_http_get.dir/requires: examples/CMakeFiles/client_http_get.dir/webrtc_signaling.c.o.requires
examples/CMakeFiles/client_http_get.dir/requires: examples/CMakeFiles/client_http_get.dir/gyropi.cpp.o.requires

.PHONY : examples/CMakeFiles/client_http_get.dir/requires

examples/CMakeFiles/client_http_get.dir/clean:
	cd /home/free/Downloads/neat/buid/examples && $(CMAKE_COMMAND) -P CMakeFiles/client_http_get.dir/cmake_clean.cmake
.PHONY : examples/CMakeFiles/client_http_get.dir/clean

examples/CMakeFiles/client_http_get.dir/depend:
	cd /home/free/Downloads/neat/buid && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/free/Downloads/neat /home/free/Downloads/neat/examples /home/free/Downloads/neat/buid /home/free/Downloads/neat/buid/examples /home/free/Downloads/neat/buid/examples/CMakeFiles/client_http_get.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : examples/CMakeFiles/client_http_get.dir/depend

