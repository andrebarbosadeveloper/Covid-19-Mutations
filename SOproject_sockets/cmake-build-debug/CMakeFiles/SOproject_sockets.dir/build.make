# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.16

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
CMAKE_COMMAND = /Applications/CLion.app/Contents/bin/cmake/mac/bin/cmake

# The command to remove a file.
RM = /Applications/CLion.app/Contents/bin/cmake/mac/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Users/andrebarbosa/CLionProjects/SOproject_sockets

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/andrebarbosa/CLionProjects/SOproject_sockets/cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/SOproject_sockets.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/SOproject_sockets.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/SOproject_sockets.dir/flags.make

CMakeFiles/SOproject_sockets.dir/main.c.o: CMakeFiles/SOproject_sockets.dir/flags.make
CMakeFiles/SOproject_sockets.dir/main.c.o: ../main.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/andrebarbosa/CLionProjects/SOproject_sockets/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/SOproject_sockets.dir/main.c.o"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/SOproject_sockets.dir/main.c.o   -c /Users/andrebarbosa/CLionProjects/SOproject_sockets/main.c

CMakeFiles/SOproject_sockets.dir/main.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/SOproject_sockets.dir/main.c.i"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /Users/andrebarbosa/CLionProjects/SOproject_sockets/main.c > CMakeFiles/SOproject_sockets.dir/main.c.i

CMakeFiles/SOproject_sockets.dir/main.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/SOproject_sockets.dir/main.c.s"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /Users/andrebarbosa/CLionProjects/SOproject_sockets/main.c -o CMakeFiles/SOproject_sockets.dir/main.c.s

# Object files for target SOproject_sockets
SOproject_sockets_OBJECTS = \
"CMakeFiles/SOproject_sockets.dir/main.c.o"

# External object files for target SOproject_sockets
SOproject_sockets_EXTERNAL_OBJECTS =

SOproject_sockets: CMakeFiles/SOproject_sockets.dir/main.c.o
SOproject_sockets: CMakeFiles/SOproject_sockets.dir/build.make
SOproject_sockets: CMakeFiles/SOproject_sockets.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/Users/andrebarbosa/CLionProjects/SOproject_sockets/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C executable SOproject_sockets"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/SOproject_sockets.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/SOproject_sockets.dir/build: SOproject_sockets

.PHONY : CMakeFiles/SOproject_sockets.dir/build

CMakeFiles/SOproject_sockets.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/SOproject_sockets.dir/cmake_clean.cmake
.PHONY : CMakeFiles/SOproject_sockets.dir/clean

CMakeFiles/SOproject_sockets.dir/depend:
	cd /Users/andrebarbosa/CLionProjects/SOproject_sockets/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/andrebarbosa/CLionProjects/SOproject_sockets /Users/andrebarbosa/CLionProjects/SOproject_sockets /Users/andrebarbosa/CLionProjects/SOproject_sockets/cmake-build-debug /Users/andrebarbosa/CLionProjects/SOproject_sockets/cmake-build-debug /Users/andrebarbosa/CLionProjects/SOproject_sockets/cmake-build-debug/CMakeFiles/SOproject_sockets.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/SOproject_sockets.dir/depend
