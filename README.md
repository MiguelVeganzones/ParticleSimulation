# Barnes-Hut Simulation

## Table of Contents
1. [Introduction](#introduction)
2. [Features](#features)
    - [NDTree](#ndtree)
    - [Numerical solvers](#numerical-solvers)
    - [Particle System](#particle-system)
    - [Plotting](#plotting)
    - [Random Distributions](#random-distributions)
3. [Getting Started](#getting-started)
    - [Prerequisites](#prerequisites)
    - [Installation](#installation)
    - [Building the Project](#building-the-project)
4. [Usage](#usage)
5. [Configuration](#configuration)
    - [Optimization Level](#optimization-level)
    - [Build Options](#build-options)
6. [Testing](#testing)
7. [Performance](#performance)
8. [Documentation](#documentation)
9. [Contributing](#contributing)
10. [License](#license)
11. [Acknowledgments](#acknowledgments)

## Introduction
The Barnes-Hut simulation is an approximation algorithm for the n-body problem intended for the simulation of large clusters of particles, such as galaxies.
This algorithm uses a nd-tree to cluster particles spatially.
This allows treating local clusters as a single particle when computing their gravitational interaction with another particle that is far away enough.
This approximation reduces the complexity of the N-body problem from O(N^2) to O(NlogN) where N is the amount of particles in the system.
This approximation is necessary to scale n-body simulations to large particle
systems.

The precision of force calculations can be arbitrarily high (compared to the
brute force computation) through a configuration parameter (theta).
This software is not well tested enough yet, but sensible values of theta
approximate the force up to a 99.9%.
This may be good enough for short-running simulations, but due to the chaotic
nature of the n-body problem it is not suit for long-running simulations.

## Features
The main building blocks of this project are described below.

### NDTree
The ndtree data structure is a templated tree-like non-owning spatial view into a set of particles. This data structure enables the Barnes Hut approximation. Some of its
features are:
 - Supports n-dimensional sample types.
 - Concept based interface
 - Supports an arbitrary (enough) number of subdivisions per dimension.
 - Limits can be either dynamically computed or established on construction, but
   they do not update.
 - It does NOT support (yet) parallel construction or recaching.
 - It does not support (yet) recollection of empty boxes.

### Numerical solvers
Numerical integrators serve as the engine for the simulations. Experimental, not
too tested versions of the following solvers are provided:
 - RK4: Standard Runge Kutta 4th order integrator.
 - Yoshida 4th order symplectic integrator.
 - Leapfrog: Baseline of the leapfrog family symplectic integrators.
 - ODEX2: Under development....

### Particle System
This project implements an expressive particle type system that supports
conditional compilation of a basic unit system.
This allows for a high level implementation of the simulations without a big
penalty in performance or space.

### Plotting
Simple APIs into root plotting utilities have been implemented, but they are
still in development.

### Random Distributions
A simple random distribution and random number library is packaged in this
project to support the creation of large scale particle systems through density
fields.

## Getting Started
Clone the project with `git clone https://gitlab.lrz.de/advprog2024/83-barnes-hut-galaxy-simulation.git` \
Build with cmake. Refer to [Building the Project](#building-the-project)
Execute as: `./build/bin/[debug,release,full_release]/main.o` if the project was built in the directory `build`.

### Prerequisites
This project uses C++23 features and thus, a modern compiler is needed.
Currently only gcc-13 and gcc-14 are supported.\
Make is needed as part of the build chain too, not any specific version.\
ROOT (CERN) ("Root" from now on) is an optional dependency for real-time plotting. Root plotting functionality are conditionally compiled.\
Boost log is an optional dependency for logging. Boost logging is conditionally compiled.\
Boost program options is a required dependency since configuration files where
added.

### Installation
Clone the project with `git clone https://gitlab.lrz.de/advprog2024/83-barnes-hut-galaxy-simulation.git` \

### Building the Project

This project can be compiled with cmake using the provided CMakeLists.txt.\
One possible way to compile is:
1. Create a build directory: `mkdir build`
2. Move into that directory: `cd build`
3. Run cmake: `cmake -DCMAKE_BUILD_TYPE=[Debug,Release,FullRelease] [-DOPTIONS=ON...] ..`
4. Run the generated Makefile: `make`
If compilation is successful, the build output will be located in
`bin/[debug,release,full_release]/[main,tests]`, but it should be run from the
project directory rather than from `build`, as `cd ..; ./build/bin/release/main`
Refer to [Compile time configuration](#Compile_time_configuration) for further details.

#### Example
Using all flags can be done with:\
`cmake -DCMAKE_BUILD_TYPE=Release -DUNIT_SYSTEM=ON -DBOOST_LOGGING=ON -DROOT_PLOTTING=ON -DFFAST_MATH=ON ..`\
This is meant to be an example rather than a sensible way of executing the code.
\
Leaving out any flag or setting equal to `OFF` disables the respective conditional compilation.

#### Video of live simulation
![Video with root](/assets/simulation_video.mp4)

## Usage
The code must be run from the project directory, or the configuration files will
not be found. These files can be used to configure some parameters of the simulation without recompiling.

## Configuration

### Compile time configuration

#### Optimization Level

The project supports three levels of optimization, which is controlled through
the optional cmake flag `-DCMAKE_BUILD_TYPE=x`. Where x is one of {Debug, Release,
FullRelease}.
`Debug`: Debug mode. Enables all the supported compiler checks and sanitizers. It is
the default mode. Should not be used for heavy simulations due to performance
issues.
`Release`: Release mode: Enables compiler optimization. Sanitizers are still
enabled. Should be the preferred execution mode in general.
`FullRelease`: Full release mode: Enable full compiler optimization and disables all sanitizers. Enables some floating point math (generally safe) optimizations. Execution mode for larger simulations.

#### Build Options
The optional build flags `OPTIONS` are used to enable parts of the project that might require dependencies or might not make sense always.
The supported arguments are
- `ENABLE_SANITIZERS={OFF,ON}`:Disables/Enables the use of sanitizers. Defaults
  to `ON`.
- `UNIT_SYSTEM={OFF,ON}`: Disables/Enables a basic unit system that provides type checking of compile time units and formatting. Defaults to `OFF`.
- `ROOT_PLOTTING={OFF,ON}`: Disables/Enables the root plotting backend. Default is not plotting. Enabling this option requires the root library properly configured (root header files and libraries must be in the include and lib search path). Defaults to `OFF`.
- `BOOST_LOGGING={OFF,ON}`: Disables/Enables boost log as the backend for logging. Default backend is iostream. Enabling this option requires Boost properly configured (boost header files and libraries must be in the include and lib search path). Defaults to `OFF`.
- `FFAST_MATH={OFF,ON}`: Disables/Enables -ffast-math compiler flags. Use carefully. Defaults to `OFF`.

### Runtime configuration

Simulation parameters can be configured through a config file in
`data/input/[debug/release]/config.ini` or through compile time values in the
code.

## Testing
After compiling the project, execute as: `./build/bin/{debug,release,full_release}/tests`

## Performance
ToDo

## Contributing
- Guidelines for contributing to the project.
- Information on how to submit issues or pull requests.

## Documentation

- [Numerical simulations of gravitational dynamics](http://eugvas.net/teaching/nbody2016/lectures.pdf)
- [Optimal Softening for Gravitational Force Calculations in N-body Dynamics](https://doi.org/10.3847/1538-4357/abe94d)
- [Construction of higher order symplectic integrators](https://www.sciencedirect.com/science/article/abs/pii/0375960190900923)
- [Solving Ordinary Differential Equations I Nonstiff Problems](https://link.springer.com/book/10.1007/978-3-540-78862-1)
- [Optimal Smoothing for N -Body Codes](https://arxiv.org/abs/astro-ph/9511146v2)
- [Numerical Integration Methods for Orbital Motion](https://link.springer.com/article/10.1007/BF00049361)

## License
This project is licensed under the [GNU General Public License](./LICENSE).

For more information, please see the [LICENSE](./LICENSE) file.

## Acknowledgments
- Credits for any resources, libraries, or individuals who contributed to the project.
