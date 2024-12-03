# Barnes-Hut Simulation

## Table of Contents
1. [Introduction](#introduction)
2. [Features](#features)
3. [Getting Started](#getting-started)
    - [Prerequisites](#prerequisites)
    - [Installation](#installation)
    - [Building the Project](#building-the-project)
4. [Usage](#usage)
5. [Configuration](#configuration)
6. [Testing](#testing)
7. [Performance](#performance)
8. [Contributing](#contributing)
9. [License](#license)
10. [Acknowledgments](#acknowledgments)

## Introduction
The Barnes-Hut simulation is an approximation algorithm created by Josh Barnes and Piet Hut in 1986 intended for the simulation of large clusters of particles, such as galaxies or solar systems. To avoid re-computing gravitational forces, the algorithm uses a tree structure to group nearby particles and thus also force effects. This tree structure reduces the complexity of the N-body problem from O(N^2) to O(NlogN).

Generally, the Barnes-Hut simulation holds importance in its clustering abilities and reduced complexity. Fields such as astrophysics and engineering use these ideas for the handling of a large number of particles or bodies, as they scale well with large values of N. Without this scalability, simulations and clustering algorithms would require far too large of computational resources.

## Features
- Description of the main features of the simulation.
- Any specific algorithms or optimizations used.
- Visualization options (if any).

## Getting Started
Instructions to get the project up and running on your local machine.

### Prerequisites
- List any software, libraries, or tools needed (e.g., C++ compiler, CMake).
- Mention the supported operating systems.

### Installation
- Steps to clone the repository.
- Instructions for installing dependencies.

### Building the Project

This project can be compiled using the provided Makefile.
Execute `make main [OPTIMIZATION_LEVEL=x] [OPTIONS=y...]` to build the project.
Execute `make tests [OPTIMIZATION_LEVEL=x] [OPTIONS=y...]` to build the tests
project. Requires GTest.
Similarly, `make [OPTIMIZATION_LEVEL=x] [OPTIONS=y...]` will build both.

#### Optimization Level

The project supports three levels of optimization, which is controlled through
the optional build flag `OPTIMIZATION_LEVEL`. Where x is one of {0, 1, 2}.
0: Debug mode. Enables all the supported compiler checks and sanitizers. It is
the default mode. Should not be used for heavy simulations due to performance
issues.
1: Release mode: Enables compiler optimization. Sanitizers are still
enabled.
2: Full release mode: Enable full compiler optimization and disables all sanitizers. Enables some floating point math (generally safe) optimizations.

#### Build Options
The optional build flags `OPTIONS` are used to enable parts of the project that might require dependencies or might not make sense always.
The supported arguments are
- ENABLE_UNIT_SYSTEM=1: Enables a basic unit system that provides type checking of
  compile time units and formatting. Defaults to 0.
- ENABLE_ROOT_PLOTTING=1: Enables the root plotting backend. Default is not plotting. Requires the root library properly configured (root header files and libraries must be in the include and lib search path). Defaults to 0.
- ENABLE_BOOST_LOGGING=1: Enables boost log as the backend for logging. Default backend is iostream. Requires Boost properly configured (boost header files and libraries must be in the include and lib search path). Defaults to 0.
- ENABLE_FFAST_MATH: Adds -ffast-math to compiler flags. Use carefully. Defaults
  to 0.

#### Example
Using all flags can be done with:
`make main OPTIMIZATION_LEVEL=2 ENABLE_UNIT_SYSTEM=1 ENABLE_BOOST_LOGGING=1 ENABLE_ROOT_PLOTTING=1 ENABLE_FFAST_MATH=1`
This is meant to be an example rather than a sensible way of executing the code.
Leaving out any flag or setting equal to 0 (or any other number) disables the
respective conditional compilation.

## Usage
The code can be compiled by running:
`make main [OPTIMIZATION_LEVEL=x] [OPTIONS=y...]`
This will generate the executable './bin/[bin,release,full_release]/main.o',
which can be directly executed (by running './bin/[bin,release,full_release]/main.o').

## Configuration
- Details about configuration files (if applicable).
- Explanation of the parameters that can be set.

## Testing
- How to run the tests included with the project.
- Instructions for running unit tests or integration tests.

## Performance
- Overview of expected performance metrics.
- Benchmarks or comparisons to other implementations (if available).

## Contributing
- Guidelines for contributing to the project.
- Information on how to submit issues or pull requests.

## Documentation

- [Numerical simulations of gravitational dynamics](docs/numerical_simulation_of_gravitational_dynamics.pdf)
- [Optimal doftening for gravitational computations.pdf](docs/optimal_doftening_for_gravitational_computations.pdf)

## License
This project is licensed under the [GNU General Public License](./LICENSE).

For more information, please see the [LICENSE](./LICENSE) file.

## Acknowledgments
- Credits for any resources, libraries, or individuals who contributed to the project.
