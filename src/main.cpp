#include "particle.hpp"
#include "random.hpp"
#include "random_distributions.hpp"
#include <chrono>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <string>
#include <vector>


using namespace std;

// g++ -std=c++11 worksheet_1.cpp -o output `pkg-config --cflags'

// NOTE: a random vector with 3D entries is FLAT
void generateRandomVector(
    int            n,
    vector<float>& return_vec,
    float          lower_bound,
    float          upper_bound,
    bool           is3D
)
{
    float ran_float;
    float ran_number_in_bounds;

    int   num_dim = ((int)is3D) * 2 + 1;
    float dif     = upper_bound - lower_bound;
    for (int i = 0; i < n; ++i)
    {
        for (int cur_dim = 0; cur_dim < num_dim; ++cur_dim)
        {
            // generates ran float between 0.0 and 1.0 with resolution of e-3
            ran_float = ((float)(rand() % 1000)) / 1000.f;

            ran_number_in_bounds = ran_float * dif + lower_bound;
            return_vec.push_back(ran_number_in_bounds);
        }
    }
}

void generateAllFields(
    int                             n,
    vector<float>&                  mass,
    [[maybe_unused]] vector<float>& velocity,
    [[maybe_unused]] vector<float>& position
)
{
    float mass_lower = 1.0;
    float mass_upper = 10.0;

    float velocity_lower = -3.0;
    float velocity_upper = 3.0;

    float position_lower = -100;
    float position_upper = 100;

    generateRandomVector(n, mass, mass_lower, mass_upper, false);
    generateRandomVector(n, mass, velocity_lower, velocity_upper, true);
    generateRandomVector(n, mass, position_lower, position_upper, true);
}

int main0()
{
    vector<float> mass;
    vector<float> velocity;
    vector<float> position;
    int           n = 100;
    generateAllFields(n, mass, velocity, position);

    return 0;
}

int particle_test()
{
    using namespace ndt;
    using F                 = float;
    static constexpr auto N = 3;
    using sample_t          = particle::ndparticle<N, float>;

    const auto            size = 30;
    std::vector<sample_t> samples;

    for ([[maybe_unused]]
         auto _ : std::views::iota(0, size))
    {
        samples.emplace_back(
            pm::position<N, F>{ utility::random::srandom::randfloat<F>(),
                                utility::random::srandom::randfloat<F>(),
                                utility::random::srandom::randfloat<F>() } *
                F{ 3 },
            // operator*(mass, F)
            pm::mass<F>{ utility::random::srandom::randfloat<F>() * F{ 20 } },
            pm::linear_velocity<N, F>{ F{ 100.f } *
                                           -utility::random::srandom::randfloat<F>(),
                                       -utility::random::srandom::randfloat<F>(),
                                       -utility::random::srandom::randfloat<F>() },

            pm::linear_acceleration<N, F>{ utility::random::srandom::randfloat<F>(),
                                           utility::random::srandom::randfloat<F>() /
                                               F{ 10 },
                                           utility::random::srandom::randfloat<F>() }
        );
    }

    for (auto i = 0uz; auto const& s : samples)
    {
        std::cout << "Sample: " << i++ << '\n' << s << '\n';
    }
    ndtree<sample_t> tree(std::span{ samples }, 5, 4);
    std::cout << tree;

    return EXIT_SUCCESS;
}

int main()
{
    particle_test();
}
