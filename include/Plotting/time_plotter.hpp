#ifndef INCLUDED_TIME_PLOTTER
#define INCLUDED_TIME_PLOTTER

#include "TGraph.h"
#include <TCanvas.h>
#include <memory>

namespace root_plotting
{
class time_plotter
{
public:
    std::unique_ptr<TCanvas> c1;
    std::unique_ptr<TGraph>  gr1;
    std::unique_ptr<TGraph>  gr2;

    explicit time_plotter();

    auto plot(const int n, float const* const y1, float const* const y2) -> void;
};

} // namespace root_plotting

#endif // INCLUDED_TIME_PLOTTER
