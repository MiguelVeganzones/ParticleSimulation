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
A brief description of the Barnes-Hut simulation, its purpose, and its significance in computational astrophysics or other relevant fields.

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
Execute `make main [OPTIONS]` to build the project.

#### Build Options

The `[OPTIONS]` argument to the `make` command are used to enable parts of the project that might require dependencies.
The supported arguments are
- ENABLE_UNIT_SYSTEM: Enables a basic unit system that provides type checking of
  compile time units and formatting.
- ENABLE_ROOT_PLOTTING: Enables the root plotting backend. Default is not plotting. Requires the root library properly configured (root header files and libraries must be in the include and lib search path).
- ENABLE_BOOST_LOGGING: Enables boost log as the backend for logging. Default backend is iostream. Requires Boost properly configured (boost header files and libraries must be in the include and lib search path)


## Usage
- How to run the simulation.
- Explanation of command-line arguments (if any).
- Example usage scenarios.

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
