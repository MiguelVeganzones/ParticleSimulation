#include "scatter_plot.hpp"
#include "TGraph.h"
#include <TCanvas.h>
#include <TNtuple.h>
#include <memory>

namespace root_plotting
{

scatter_plot_3D::scatter_plot_3D()
{
    c1  = std::make_unique<TCanvas>("canvas", "Graph Draw Options", 200, 10, 800, 600);
    tn1 = std::make_unique<TNtuple>("n", "n", "x:y:z:color");
    tn1->SetMarkerStyle(7);
}

auto scatter_plot_3D::plot(
    const int          n,
    float const* const x,
    float const* const y,
    float const* const z
) -> void
{
    tn1->Reset();
    for (int i = 0; i != n; ++i)
    {
        tn1->Fill(x[i], y[i], z[i], 40);
    }
    c1->cd();
    tn1->Draw("x:y:z:color");
    c1->Update();
}

} // namespace root_plotting
