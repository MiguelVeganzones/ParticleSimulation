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
    float                    min_y;
    float                    max_y;
    int                      n;

    explicit time_plotter();

    auto append(const float y1, float y2) -> void;
};

} // namespace root_plotting

#endif // INCLUDED_TIME_PLOTTER
