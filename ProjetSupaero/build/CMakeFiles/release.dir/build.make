# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 2.8

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list

# Produce verbose output by default.
VERBOSE = 1

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

# The program to use to edit the cache.
CMAKE_EDIT_COMMAND = /usr/bin/ccmake

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = "/home/maxime/Documents/Study/Projets Ingenieur/Acado_Exs/ProjetSupaero"

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = "/home/maxime/Documents/Study/Projets Ingenieur/Acado_Exs/ProjetSupaero/build"

# Utility rule file for release.

# Include the progress variables for this target.
include CMakeFiles/release.dir/progress.make

CMakeFiles/release:
	! test x$$VERSION = x || ( echo Please\ set\ a\ version\ for\ this\ release && false ) && cd /home/maxime/Documents/Study/Projets\ Ingenieur/Acado_Exs/ProjetSupaero && /home/maxime/Documents/Study/Projets\ Ingenieur/Acado_Exs/ProjetSupaero/install/bin/git tag -s v$$VERSION -m Release\ of\ version\ $$VERSION. && cd /home/maxime/Documents/Study/Projets\ Ingenieur/Acado_Exs/ProjetSupaero/build && cmake /home/maxime/Documents/Study/Projets\ Ingenieur/Acado_Exs/ProjetSupaero && make distcheck || ( echo Please\ fix\ distcheck\ first. && /home/maxime/Documents/Study/Projets\ Ingenieur/Acado_Exs/ProjetSupaero/install/bin/git tag -d v$$VERSION && cmake /home/maxime/Documents/Study/Projets\ Ingenieur/Acado_Exs/ProjetSupaero && false ) && echo Please,\ run\ 'git\ push\ --tags'\ to\ finalize\ this\ release.

release: CMakeFiles/release
release: CMakeFiles/release.dir/build.make
.PHONY : release

# Rule to build all files generated by this target.
CMakeFiles/release.dir/build: release
.PHONY : CMakeFiles/release.dir/build

CMakeFiles/release.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/release.dir/cmake_clean.cmake
.PHONY : CMakeFiles/release.dir/clean

CMakeFiles/release.dir/depend:
	cd "/home/maxime/Documents/Study/Projets Ingenieur/Acado_Exs/ProjetSupaero/build" && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" "/home/maxime/Documents/Study/Projets Ingenieur/Acado_Exs/ProjetSupaero" "/home/maxime/Documents/Study/Projets Ingenieur/Acado_Exs/ProjetSupaero" "/home/maxime/Documents/Study/Projets Ingenieur/Acado_Exs/ProjetSupaero/build" "/home/maxime/Documents/Study/Projets Ingenieur/Acado_Exs/ProjetSupaero/build" "/home/maxime/Documents/Study/Projets Ingenieur/Acado_Exs/ProjetSupaero/build/CMakeFiles/release.dir/DependInfo.cmake" --color=$(COLOR)
.PHONY : CMakeFiles/release.dir/depend
