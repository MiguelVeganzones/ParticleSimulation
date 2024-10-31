CXX = g++

DEBUG_CXXFLAGS =  	-fdiagnostics-color=always \
					-fdiagnostics-show-template-tree \
					-fdiagnostics-path-format=inline-events \
					-fdiagnostics-show-caret \
					-ggdb3 \
					-O0 \
					-Wall \
					-Wextra \
					-Wshadow \
					-Wconversion \
					-Wmisleading-indentation \
					-Wduplicated-cond \
					-Wduplicated-branches \
					-Wlogical-op \
					-Wnull-dereference \
					-Wpointer-arith \
					-Wdangling-else \
					-Wrestrict \
					-Wdouble-promotion \
					-Wvla \
					-Wswitch-enum \
					-Wswitch-default \
					-Winvalid-pch \
					-Wodr \
					-Wstack-usage=$(shell ulimit -s) \
					-Wformat \
					-fsanitize=leak \
					-fsanitize=address \
					-fsanitize=undefined \
					-Werror \
					-pedantic \
					-mavx \
					-fbounds-check \
					-fconcepts-diagnostics-depth=3 \
					-std=c++23 

RELEASE_CXXFLAGS =  -fdiagnostics-color=always \
					-fdiagnostics-show-template-tree \
					-fdiagnostics-path-format=inline-events \
					-fdiagnostics-show-caret \
					-ggdb3 \
					-O2 \
					-Wall \
					-Wextra \
					-Wshadow \
					-Wconversion \
					-Wmisleading-indentation \
					-Werror \
					-pedantic \
					-mavx \
					-fbounds-check \
					-fconcepts-diagnostics-depth=3 \
					-std=c++23
					
FULL_RELEASE_CXXFLAGS =  -fdiagnostics-color=always \
					-fdiagnostics-show-template-tree \
					-fdiagnostics-path-format=inline-events \
					-fdiagnostics-show-caret \
					-O3 \
					-Wall \
					-Wextra \
					-Wshadow \
					-Wconversion \
					-Wmisleading-indentation \
					-Werror \
					-pedantic \
					-mavx \
					-fconcepts-diagnostics-depth=3 \
					-std=c++23
					#-fno-exceptions
					

RELEASE ?= 0
ifeq (${RELEASE}, 1)
    CXXFLAGS = ${RELEASE_CXXFLAGS}
	OUT_DIR = bin/release
else ifeq (${RELEASE}, 2)
    CXXFLAGS = ${FULL_RELEASE_CXXFLAGS}
	OUT_DIR = bin/full_release
else
    CXXFLAGS = ${DEBUG_CXXFLAGS}
	OUT_DIR = bin/debug
endif


SRC_DIR			    =	src
INCLUDE_DIR		    =	include
UTILITY_DIR		    =	$(INCLUDE_DIR)/Utility
ROOT_DIR		    =	$(INCLUDE_DIR)/Plotting
CONTAINERS_DIR		    =	$(INCLUDE_DIR)/Containers
PHYSICAL_MODEL_DIR	    =	$(INCLUDE_DIR)/PhysicalModel

UTILITY_INCL			=
GENERAL_INCL			=	-I./$(UTILITY_DIR) $(UTILITY_INCL)
PHYSICAL_MODEL_INCL		=	-I./$(PHYSICAL_MODEL_DIR)
NDTREE_INCL			=	-I./$(CONTAINERS_DIR)
MAIN_SIMULATION_INCL		=	$(GENERAL_INCL) $(PHYSICAL_MODEL_INCL) $(NDTREE_INCL)

ROOT_LIB			= `root-config --libs`

ROOT_FLAGS			= `root-config --cflags` -Wno-cpp


#=================================================================================================
main: ${OUT_DIR}/main.o

${OUT_DIR}/main.o: $(SRC_DIR)/*.cpp
	@echo -e Building $@..."\n"
	@mkdir -p ${OUT_DIR}
	$(CXX) $(CXXFLAGS) $(MAIN_SIMULATION_INCL) $(SRC_DIR)/main.cpp -o $@
	@echo -e Built $@ successfully."\n"
#================================================================================================
