#include "scatter_plot.hpp"

#ifdef USE_ROOT_PLOTTING
#include "TGraph.h"
#include <TCanvas.h>
#include <TNtuple.h>
#include <memory>
#endif

namespace root_plotting
{

scatter_plot_3D::scatter_plot_3D()
{
#ifdef USE_ROOT_PLOTTING
    c1  = std::make_unique<TCanvas>("canvas", "Graph Draw Options", 200, 10, 800, 600);
    tn1 = std::make_unique<TNtuple>("n", "n", "x:y:z:color");
    tn1->SetMarkerStyle(7);
#endif
}

auto scatter_plot_3D::plot(
   [[maybe_unused]] const int          n,
   [[maybe_unused]] float const* const x,
   [[maybe_unused]] float const* const y,
   [[maybe_unused]] float const* const z
) -> void
{
#ifdef USE_ROOT_PLOTTING
    tn1->Reset();
    for (int i = 0; i != n; ++i)
    {
        tn1->Fill(x[i], y[i], z[i], 40);
    }
    c1->cd();
    tn1->Draw("x:y:z:color");
    c1->Update();
#endif
}

} // namespace root_plotting
