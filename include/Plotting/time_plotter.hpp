#ifndef INCLUDED_TIME_PLOTTER
#define INCLUDED_TIME_PLOTTER

#ifdef ENABLE_ROOT_PLOTTING
#include "TGraph.h"
#include <TCanvas.h>
#include <memory>
#endif

namespace root_plotting
{

class time_plotter2
{
public:
#ifdef ENABLE_ROOT_PLOTTING
    std::unique_ptr<TCanvas> c1;
    std::unique_ptr<TGraph>  gr1;
    std::unique_ptr<TGraph>  gr2;
    float                    min_y{};
    float                    max_y{};
    int                      n{};
#endif

    explicit time_plotter2();

    auto append(const float y1, float y2) -> void;
};

class time_plotter4
{
public:
#ifdef ENABLE_ROOT_PLOTTING
    std::unique_ptr<TCanvas> c1;
    std::unique_ptr<TGraph>  gr1;
    std::unique_ptr<TGraph>  gr2;
    std::unique_ptr<TGraph>  gr3;
    std::unique_ptr<TGraph>  gr4;
    float                    min_y{};
    float                    max_y{};
    int                      n{};
#endif

    explicit time_plotter4();

    auto append(const float y1, float y2, const float y3, const float y4) -> void;
};

} // namespace root_plotting

#endif // INCLUDED_TIME_PLOTTER
