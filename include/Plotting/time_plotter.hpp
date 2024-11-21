#ifndef INCLUDED_TIME_PLOTTER
#define INCLUDED_TIME_PLOTTER

#include "TGraph.h"
#include <TCanvas.h>
#include <memory>

namespace root_plotting
{

class time_plotter2
{
public:
    std::unique_ptr<TCanvas> c1;
    std::unique_ptr<TGraph>  gr1;
    std::unique_ptr<TGraph>  gr2;
    float                    min_y{};
    float                    max_y{};
    int                      n{};

    explicit time_plotter2();

    auto append(const float y1, float y2) -> void;
};

class time_plotter4
{
public:
    std::unique_ptr<TCanvas> c1;
    std::unique_ptr<TGraph>  gr1;
    std::unique_ptr<TGraph>  gr2;
    std::unique_ptr<TGraph>  gr3;
    std::unique_ptr<TGraph>  gr4;
    float                    min_y{};
    float                    max_y{};
    int                      n{};

    explicit time_plotter4();

    auto append(const float y1, float y2, const float y3, const float y4) -> void;
};

} // namespace root_plotting

#endif // INCLUDED_TIME_PLOTTER
