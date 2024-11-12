CXX = g++

DEBUG_CXXFLAGS =	-O0 \
			-fbounds-check \
			-fdiagnostics-color=always \
			-fdiagnostics-show-template-tree \
			-fsanitize=address \
			-fsanitize=leak \
			-fsanitize=undefined \
			-ggdb3 \
			-mavx \
			-pedantic \
			-std=c++23
			# -Wfloat-equal \

DEBUG_CXXFLAGS_GCC =	-O0 \
			-W \
			-Wall \
			-Wconversion \
			-Wdangling-else \
			-Wdouble-promotion \
			-Wduplicated-branches \
			-Wduplicated-cond \
			-Werror \
			-Wextra \
			-Wfloat-equal \
			-Wformat \
			-Winvalid-pch \
			-Wlogical-op \
			-Wmisleading-indentation \
			-Wnull-dereference \
			-Wodr \
			-Wpointer-arith \
			-Wrestrict \
			-Wshadow \
			-Wswitch-default \
			-Wswitch-enum \
			-Wvla \
			-fbounds-check \
			-fconcepts-diagnostics-depth=3 \
			-fdiagnostics-color=always \
			-fdiagnostics-path-format=inline-events \
			-fdiagnostics-show-caret \
			-fdiagnostics-show-template-tree \
			-fmax-errors=5 \
			-fsanitize=address \
			-fsanitize=leak \
			-fsanitize=undefined \
			-ggdb3 \
			-mavx \
			-pedantic \
			-std=c++23
			# -Wfloat-equal \

RELEASE_CXXFLAGS =	-fdiagnostics-color=always \
			-fdiagnostics-show-template-tree \
			-fdiagnostics-path-format=inline-events \
			-fdiagnostics-show-caret \
			-ggdb3 \
			-O2 \
			-Wall \
			-Wextra \
			-Wshadow \
			-Wconversion \
			-ffinite-math-only \
			-Wmisleading-indentation \
			-Werror \
			-pedantic \
			-mavx \
			-fstrength-reduce \
			-fbounds-check \
			-fconcepts-diagnostics-depth=3 \
			-std=c++23
			
FULL_RELEASE_CXXFLAGS = -fdiagnostics-color=always \
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
			-fstrength-reduce \
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
#UNIT_SYSTEM_CORE_DIR	    =	mp-units/src/core/include
#UNIT_SYSTEM_SYSTEMS_DIR	    =	mp-units/src/systems/include

UTILITY_INCL			=
GENERAL_INCL			=	-I./$(UTILITY_DIR) $(UTILITY_INCL)
PHYSICAL_MODEL_INCL		=	-I./$(PHYSICAL_MODEL_DIR)
NDTREE_INCL			=	-I./$(CONTAINERS_DIR)
#UNIT_SYSTEM_INCL		=	-I./$(UNIT_SYSTEM_CORE_DIR) -I./$(UNIT_SYSTEM_SYSTEMS_DIR)
MAIN_SIMULATION_INCL		=	$(GENERAL_INCL) $(PHYSICAL_MODEL_INCL) $(NDTREE_INCL)

ROOT_LIB			= `root-config --libs`
LOG_LIB				= -lboost_log -lboost_thread -lboost_system -lpthread
MAIN_SIMULATION_LIB		= $(LOG_LIB)

ROOT_FLAGS			= `root-config --cflags` -Wno-cpp


#=================================================================================================
main: ${OUT_DIR}/main.o

${OUT_DIR}/main.o: $(SRC_DIR)/*.cpp
	@echo -e Building $@..."\n"
	@mkdir -p ${OUT_DIR}
	$(CXX) $(CXXFLAGS) $(MAIN_SIMULATION_INCL) $(MAIN_SIMULATION_LIB) $(SRC_DIR)/main.cpp -o $@
	@echo -e Built $@ successfully."\n"
#================================================================================================
