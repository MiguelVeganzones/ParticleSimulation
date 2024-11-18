#ifndef INCLUDED_SCATTER_PLOTTER
#define INCLUDED_SCATTER_PLOTTER

#include "TGraph.h"
#include <TCanvas.h>
#include <TNtuple.h>
#include <memory>

namespace root_plotting
{
class scatter_plot_3D
{
public:
    std::unique_ptr<TCanvas> c1;
    std::unique_ptr<TNtuple> tn1;

    explicit scatter_plot_3D();

    auto plot(
        const int          n,
        float const* const x,
        float const* const y,
        float const* const z
    ) -> void;
};

} // namespace root_plotting

#endif // INCLUDED_SCATTER_PLOTTER
