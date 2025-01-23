# Barnes-Hut Simulation

## Table of Contents
1. [Introduction](#introduction)
2. [Features](#features)
    - [NDTree](#ndtree)
    - [Numerical Solvers](#numerical-solvers)
    - [Particle System](#particle-system)
    - [Plotting](#plotting)
    - [Random Distributions](#random-distributions)
3. [Getting Started](#getting-started)
    - [Prerequisites](#prerequisites)
    - [Installation](#installation)
    - [Building the Project](#building-the-project)
4. [Usage](#usage)
5. [Configuration](#configuration)
    - [Compile Time Configuration](#compile-time-configuration)
        - [Optimization Level](#optimization-level)
        - [Build Options](#build-options)
    - [Runtime Configuration](#runtime-configuration)
6. [Testing](#testing)
7. [Performance](#performance)
8. [Documentation](#documentation)
9. [Contributing](#contributing)
10. [License](#license)
11. [Acknowledgments](#acknowledgments)

## Introduction
The Barnes-Hut simulation is an approximation algorithm for the N-body problem, designed for the simulation of large clusters of particles, such as galaxies.
This algorithm uses an NDTree to cluster particles spatially, allowing the treatment of local clusters as a single particle when computing their gravitational interaction with a distant particle.
This approximation reduces the complexity of the N-body problem from \(O(N^2)\) to \(O(N\log N)\), where \(N\) is the number of particles.
The precision of force calculations can be controlled via the configuration parameter `theta`, enabling a balance between performance and accuracy.
**Note**: This software is not yet thoroughly tested. Sensible `theta` values can achieve 99.9% accuracy for short simulations but may be unsuitable for long-running simulations due to the chaotic nature of the N-body problem.

## Features
### NDTree
The NDTree data structure is a templated, tree-like, non-owning spatial view into a set of particles. It enables the Barnes-Hut approximation and offers:
- Support for n-dimensional sample types.
- Concept-based interface.
- Flexible number of subdivisions per dimension.
- Static or dynamic limit computation at construction.

**Limitations**:
- Parallel construction or recaching is not supported (yet).
- Empty boxes are not recollected.

### Numerical Solvers
Experimental numerical integrators include:
- **RK4**: Standard Runge-Kutta 4th order.
- **Yoshida**: 4th order symplectic integrator.
- **Leapfrog**: Baseline symplectic integrator.
- **ODEX2**: Under development.

### Particle System
A flexible particle type system supports a conditional unit system, allowing high-level simulations with minimal performance overhead.

### Plotting
Basic APIs for ROOT-based plotting are included, though still under development.

### Random Distributions
A simple random distribution and random number library is packaged in this
project to support the creation of large scale particle systems through density
fields.

### Configuration files

Some simulation parameters can be specified through a configuration file
located in `data/input/{debug/release}/config.ini`. This allows configuration
without recompilation. The project build output must be run from the project directory so that these files can be found.

## Getting Started
Clone the project with `git clone https://gitlab.lrz.de/advprog2024/83-barnes-hut-galaxy-simulation.git` \
Build with cmake. Refer to [Building the Project](#building-the-project)
Execute as: `./build/bin/{debug,release,full_release}/main` if the project was built in the directory `build`.

### Prerequisites
This project uses C++23 features and thus, a modern compiler is needed.
Currently only gcc-13 and gcc-14 are supported.\
CMake 3.30 or better is needed as part of the build system.
ROOT (CERN) ("Root" from now on) is an optional dependency for real-time plotting. Root plotting functionality is conditionally compiled.\
Boost log is an optional dependency for logging. Boost logging is conditionally compiled.\
Boost program options is a required dependency since configuration files where added.

### Installation
Clone the project with `git clone https://gitlab.lrz.de/advprog2024/83-barnes-hut-galaxy-simulation.git` \

### Building the Project

This project can be compiled with cmake using the provided CMakeLists.txt.\
One possible way to compile is:
1. Create a build directory and move into it: `mkdir build && cd build`
2. Run cmake: `cmake -DCMAKE_BUILD_TYPE={Debug,Release,FullRelease} [-DOPTIONS=ON...] ..`
3. Run the generated Makefile: `make`
If compilation is successful, the build output will be located in
`bin/{debug,release,full_release}/{main,tests}`, but it should be run from the
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
- `ROOT_PLOTTING={OFF,ON}`: Disables/Enables the Root plotting backend. Default is not plotting. Enabling this option requires the Root library properly configured (Root header files and libraries must be in the include and lib search path). Defaults to `OFF`.
- `BOOST_LOGGING={OFF,ON}`: Disables/Enables boost log as the backend for logging. Default backend is iostream. Enabling this option requires Boost properly configured (boost header files and libraries must be in the include and lib search path). Defaults to `OFF`.
- `FFAST_MATH={OFF,ON}`: Disables/Enables -ffast-math compiler flags. Use carefully. Defaults to `OFF`.

### Runtime configuration

Simulation parameters can be configured through a config file in
`data/input/{debug,release}/config.ini` or through compile time values in the
code.

## Testing
After compiling the project, execute as: `./build/bin/{debug,release,full_release}/tests`

## Performance

### Optimization Steps

Performance optimization was conducted based on a single benchmark test with the following parameters:
- 3D space
- 250 seconds duration
- 0.5 seconds time increment
- 1000 particles
- Theta = 0.5 (Barnes-Hut approximation factor)
- Gravitational interaction
- Release mode (`-O3` optimization level)
- No `-ffast-math`
- No sanitizers
- No plotting
 - Initial conditions will be the same in all cases. This will be done by hand-seeding the random number engines.

Valgrind Callgrind was used to identify hotspots in the simulation, guiding optimizations for this specific performance benchmark. A broader test suit should be used, but due to time limitation this will be enough. Each experiment was executed three times, and the minimum clock time was considered.

Performance was evaluated in particle-to-particle (P2P) interaction computations per microsecond. This is clearly a non standard unit, but it will allow measuring the throughput of our physics engine for simulations of arbitrary size and duration. We will measure the total clock time of the simulation against the theoretical number of P2P interactions required, which is not the real amount of P2P interactions computed due to the Barnes-Hut approximation. This is because some computation is used to maintain the `ndtree`, which should add positively to the throughput, as well as the approximation factor theta.

For this specific benchmark, the number of P2P interactions depends on:
- The number of time steps: `500`
- The number of particles: `1,000`
- The force calculations the solver requires per update: `3`
- Gravitational interactions per step: `1.5E9` theoretical P2P interactions.

### Key Optimizations

1. Eliminating `std::pow` in the critical path:
   - Valgrind revealed that `std::pow` accounted for approximately 40% of the execution time. This makes sense because `std::pow` is a fairly expensive operation to compute and it is present in our hot path.
I
   - `std::pow` was used in the force calculation:
     \[
     F(d, e) = (d^2 + e^2)^{3/2} \approx d^3
     \]
     Or in C++: `std::pow(d * d + e * e, 1.5)`.
     This equation is an approximation to \( d^3 \) that introduces an epsilon term \( e \) that prevents division by zero and ensures numerical stability.
   - This call to `std::pow` can be ditched by changing the approximation to:
     \[
     F(d, e) = d^3 + \frac{\epsilon}{2} \approx d^3
     \]
   - This resulted in a reduction in clock time of about 40%.

2. Optimizing `l2_norm` calculations:
   - After removing `std::pow`, the `l2_norm` vector calculation represents a significant part of the CPU time 13.8% approximately. This function was implemented using `std::ranges::fold_left`, which is high-level but less efficient than a raw loop.
   - We tried optimizing this implementation by replacing `fold_left` with:
     - A raw `for` loop, which would be easy for the compiler to unroll, as the
     vector size is a small compile time constant.
     - `std::reduce`, which is a more specialized version of `std::accumualte` for numeric computation and also allows execution policies such as `std::execution::unseq` for SIMD optimization.
    - After trying out both alternatives, the most effective optimization was the raw `for` loop.
   - This optimization resulted in a 24 CPU time reduction.

3. Specialization of `l2_norm` with `l2_norm_sq`:
   - A significant portion of the `l2_norm` computation time was spent in the `std::sqrt` call. However, some computations can work with the squared norm too.
   - Providing a `l2_norm_sq` version of `l2_norm` that does not compute the square root would improve performance. Wherever possible, we replaced calls to `l2_norm` with `l2_norm_sq`.
   - Execution time was reduced by around 5%. This is not a very significant
   reduction, but out tests showed that this specific `std::sqrt` call amounts
   for around 50% of the total execution time, as this `l2_norm` is needed in
   more places in the simulation. Anyways, `std::sqrt` is an expensive operation
   but not that much.

### Summary of Results

| Total Clock Time | P2P Interaction Throughput | Speedup (%) | Optimization                               |
|------------------|----------------------------|-------------|--------------------------------------------|
| 35.443 [s]       | 42.32 [us<sup>-1</sup>]    | Baseline    | Base performance                           |
| 20.489 [s]       | 73.21 [us<sup>-1</sup>]    | +42%        | Eliminated `std::pow` in the critical path |
| 15.597 [s]       | 96.17 [us<sup>-1</sup>]    | +25%        | Replaced `fold_left` with raw `for` loop   |
| 14.833 [s]       | 101.13 [us<sup>-1</sup>]   | +5%         | Introduced `l2_norm_sq` specialization     |

Improving performance further is not trivial and would require modifying
algorithms or memory pools to eliminate the very few allocations we are
currently doing in the `ndtree`.

### Failed Attempts

1. Expression templates for vector operations:
   - We implemented physical magnitude and vector operations with expression templates to reduce temporaries. However, this did not improve performance, probably because the containers we use do not require dynamic allocation and some overhead is needed for this technique. Please refer to the [expression templates branch](https://gitlab.lrz.de/advprog2024/83-barnes-hut-galaxy-simulation/-/tree/expression_templates) for the implementation.

2. Multithreading and SIMD:
   - We attempted to parallelize solver computations using `std::execution::par_unseq` and `std::execution::unseq`. Each particle calculation is independent in the integrator. Previous value buffers are read only and only one element of the current buffer is written at each iteration, so it can be parallelized and vectorized with `std::execution::par_unseq`. This did not improve performance, presumably because the overhead of launching threads was greater than the work they did. A thread pool would be required to improve performance 

### ToDo

- Implement a real benchmarking suite with diverse parameters.
- Implement a thread pool to add efficient multithreading.
- Implement a memory pool for `ndbox` allocation.

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
