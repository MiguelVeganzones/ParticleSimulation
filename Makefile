CXX = g++

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
			-Wuninitialized \
			-Wvla \
			-fbounds-check \
			-fconcepts-diagnostics-depth=3 \
			-fdiagnostics-color=always \
			-fdiagnostics-path-format=inline-events \
			-fdiagnostics-show-caret \
			-fdiagnostics-show-template-tree \
			-ffinite-math-only \
			-fmax-errors=5 \
			-fsanitize=address \
			-fsanitize=bounds \
			-fsanitize=float-cast-overflow \
			-fsanitize=float-divide-by-zero \
			-fsanitize=integer-divide-by-zero \
			-fsanitize=leak \
			-fsanitize=null \
			-fsanitize=signed-integer-overflow \
			-fsanitize=undefined \
			-ggdb3 \
			-mavx \
			-march=native \
			-pedantic \
			-std=c++23

RELEASE_CXXFLAGS =	-fdiagnostics-color=always \
			-fdiagnostics-show-template-tree \
			-fdiagnostics-path-format=inline-events \
			-fdiagnostics-show-caret \
			-ggdb3 \
			-O2 \
			-Wall \
			-Wextra \
			-Wshadow \
			-ffinite-math-only \
			-Wconversion \
			-Wuninitialized \
			-Wmisleading-indentation \
			-Werror \
			-pedantic \
			-mavx \
			-march=native \
			-fstrength-reduce \
			-fsanitize=address \
			-fsanitize=bounds \
			-fsanitize=float-cast-overflow \
			-fsanitize=float-divide-by-zero \
			-fsanitize=integer-divide-by-zero \
			-fsanitize=leak \
			-fsanitize=null \
			-fsanitize=signed-integer-overflow \
			-fsanitize=undefined \
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
			-march=native \
			-fconcepts-diagnostics-depth=3 \
			-fno-math-errno \
			-fno-trapping-math \
			-std=c++23
			#-fno-exceptions

OPTIMIZATION_LEVEL ?= 0
ifeq (${OPTIMIZATION_LEVEL}, 1)
    CXXFLAGS = ${RELEASE_CXXFLAGS}
    OUT_DIR = bin/release
else ifeq (${OPTIMIZATION_LEVEL}, 2)
    CXXFLAGS = ${FULL_RELEASE_CXXFLAGS}
    OUT_DIR = bin/full_release
else
    CXXFLAGS = ${DEBUG_CXXFLAGS_GCC}
    OUT_DIR = bin/debug
endif

SRC_DIR			    =	src
INCLUDE_DIR		    =	include
UTILITY_DIR		    =	$(INCLUDE_DIR)/Utility
PLOTTING_DIR		    =	$(INCLUDE_DIR)/Plotting
CONTAINERS_DIR		    =	$(INCLUDE_DIR)/Containers
TIMING_DIR		    =	$(INCLUDE_DIR)/Timing
PHYSICAL_MODEL_DIR	    =	$(INCLUDE_DIR)/PhysicalModel
SOLVERS_DIR		    =	$(INCLUDE_DIR)/Solvers
TEST_DIR 		    =	tests
SIMULATION_DIR		    =	$(INCLUDE_DIR)/Simulation
SIMULATION_LOGGERS_DIR	    =	$(INCLUDE_DIR)/DataLoggers

UTILITY_INCL			=
GENERAL_INCL			=	-I./$(UTILITY_DIR) $(UTILITY_INCL)
PHYSICAL_MODEL_INCL		=	-I./$(PHYSICAL_MODEL_DIR)
NDTREE_INCL			=	-I./$(CONTAINERS_DIR)
TIMING_INCL			=	-I./$(TIMING_DIR)
PLOTTING_INCL			=	-I./$(PLOTTING_DIR)
SOLVERS_INCL			=	-I./$(SOLVERS_DIR)
SIMULATION_INCL			=	-I./$(SIMULATION_DIR)
SIMULATION_LOGGERS_INCL		=	-I./$(SIMULATION_LOGGERS_DIR)
MAIN_SIMULATION_INCL		=	$(GENERAL_INCL) $(PHYSICAL_MODEL_INCL) $(NDTREE_INCL) $(TIMING_INCL) $(PLOTTING_INCL) $(SOLVERS_INCL) $(SIMULATION_INCL) $(SIMULATION_LOGGERS_INCL)
TESTS_INCL				= $(MAIN_SIMULATION_INCL)

ENABLE_UNIT_SYSTEM ?= 0
ifeq (${ENABLE_UNIT_SYSTEM}, 1)
CXXFLAGS			+= -DUSE_UNIT_SYSTEM
endif
ENABLE_ROOT_PLOTTING ?= 0
ifeq (${ENABLE_ROOT_PLOTTING}, 1)
CXXFLAGS			+= -Wno-cpp -DUSE_ROOT_PLOTTNG
PLOTTING_LIB			= `root-config --libs`
ROOT_FLAGS			= `root-config --cflags` -Wno-cpp
endif
ENABLE_BOOST_LOGGING ?= 0
ifeq (${ENABLE_BOOST_LOGGING}, 1)
CXXFLAGS			+= -DUSE_BOOST_LOGGING
LOG_LIB				= -lboost_log -lboost_thread -lboost_system -lpthread
else
LOG_LIB =
endif
ENABLE_FFAST_MATH ?= 0
ifeq (${ENABLE_FFAST_MATH}, 1)
CXXFLAGS			+= -ffast-math
endif

MAIN_SIMULATION_LIB		= $(LOG_LIB) $(PLOTTING_LIB)
TESTS_LIB			= -lgtest -lgtest_main -lpthread


#=================================================================================================
all: main tests
#=============================================================

#=============================================================
main: $(PLOTTING_DIR)/$(OUT_DIR)/plotting.o $(PLOTTING_DIR)/$(OUT_DIR)/time_plotter.o $(PLOTTING_DIR)/$(OUT_DIR)/scatter_plot.o ${OUT_DIR}/main.o

${OUT_DIR}/main.o: $(SRC_DIR)/*.cpp $(INCLUDE_DIR)/*/*.hpp
	@echo -e Building $@..."\n"
	@mkdir -p ${OUT_DIR}
	$(CXX) $(CXXFLAGS) $(MAIN_SIMULATION_INCL) $(MAIN_SIMULATION_LIB) $(PLOTTING_DIR)/$(OUT_DIR)/*.o $(SRC_DIR)/main.cpp -o $@
	@echo -e Built $@ successfully."\n"
#=============================================================

#=============================================================
$(PLOTTING_DIR)/$(OUT_DIR)/plotting.o: $(PLOTTING_DIR)/plotting.cpp $(PLOTTING_DIR)/plotting.hpp
    ifdef ENABLE_ROOT_PLOTTING
	@echo -e Root plotting must be enabled to build $@..."\n"
    endif
	@echo -e Building $@..."\n"
	@mkdir -p $(PLOTTING_DIR)/${OUT_DIR}
	$(CXX) $(ROOT_FLAGS) $(PLOTTING_INCL) $(PLOTTING_LIB) -c $(PLOTTING_DIR)/plotting.cpp -o $@
	@echo -e Built $@ successfully."\n"
#=============================================================

#=============================================================
$(PLOTTING_DIR)/$(OUT_DIR)/time_plotter.o: $(PLOTTING_DIR)/time_plotter.cpp $(PLOTTING_DIR)/time_plotter.hpp
    ifdef ENABLE_ROOT_PLOTTING
	@echo -e Root plotting must be enabled to build $@..."\n"
    endif
	@echo -e Building $@..."\n"
	@mkdir -p $(PLOTTING_DIR)/${OUT_DIR}
	$(CXX) $(ROOT_FLAGS) $(PLOTTING_INCL) $(PLOTTING_LIB) -c $(PLOTTING_DIR)/time_plotter.cpp -o $@
	@echo -e Built $@ successfully."\n"
#=============================================================

#=============================================================
$(PLOTTING_DIR)/$(OUT_DIR)/scatter_plot.o: $(PLOTTING_DIR)/scatter_plot.cpp $(PLOTTING_DIR)/scatter_plot.hpp
    ifdef ENABLE_ROOT_PLOTTING
	@echo -e Root plotting must be enabled to build $@..."\n"
    endif
	@echo -e Building $@..."\n"
	@mkdir -p $(PLOTTING_DIR)/${OUT_DIR}
	$(CXX) $(ROOT_FLAGS) $(PLOTTING_INCL) $(PLOTTING_LIB) -c $(PLOTTING_DIR)/scatter_plot.cpp -o $@
	@echo -e Built $@ successfully."\n"
#=============================================================

#=============================================================
tests: ${TEST_DIR}/$(OUT_DIR)/tests.o

${TEST_DIR}/$(OUT_DIR)/tests.o: $(TEST_DIR)/*tests.cpp $(INCLUDE_DIR)/*/*.hpp
	@echo -e Building $@..."\n"
	@mkdir -p $(TEST_DIR)/$(OUT_DIR)
	$(CXX) $(CXXFLAGS) $(TESTS_INCL) $(TESTS_LIB) $(TEST_DIR)/*tests.cpp -o $@
	@echo -e Built $@ successfully."\n"
#=============================================================
