#include "plotting.hpp"
#ifdef USE_ROOT_PLOTTING
#include "TGraph.h"
#include <TCanvas.h>
#endif

namespace root_plotting
{

auto plot(
    int                n,
    const float* const x,
    const float* const y1,
    const float* const y2,
    const float* const y3,
    const float* const y4
) -> void
{
#ifdef USE_ROOT_PLOTTING
    auto gr1 = new TGraph(n, x, y1);
    auto gr2 = new TGraph(n, x, y2);
    auto gr3 = new TGraph(n, x, y3);
    auto gr4 = new TGraph(n, x, y4);

    TCanvas* canvas = new TCanvas("canvas", "Graph Draw Options", 200, 10, 600, 400);

    gr1->SetLineColor(4);
    gr1->Draw("AC*");
    gr2->SetLineColor(2);
    gr2->Draw("CP");
    gr3->SetLineColor(1);
    gr3->Draw("C+");
    gr4->SetLineColor(0);
    gr4->Draw("C.");
#endif
}

auto plot(
    int                n,
    const float* const x,
    const float* const y1,
    const float* const y2,
    const float* const y3
) -> void
{
#ifdef USE_ROOT_PLOTTING
    auto gr1 = new TGraph(n, x, y1);
    auto gr2 = new TGraph(n, x, y2);
    auto gr3 = new TGraph(n, x, y3);

    TCanvas* canvas = new TCanvas("canvas", "Graph Draw Options", 200, 10, 600, 400);

    gr1->SetLineColor(4);
    gr1->Draw("AC*");

    gr2->SetMarkerStyle(21);
    gr2->SetLineColor(2);
    gr2->Draw("CP");

    gr3->SetMarkerStyle(20);
    gr3->SetLineColor(1);
    gr3->Draw("CP");
#endif
}

auto plot(int n, const float* const x, const float* const y1, const float* const y2)
    -> void
{
#ifdef USE_ROOT_PLOTTING
    auto gr1 = new TGraph(n, x, y1);
    auto gr2 = new TGraph(n, x, y2);

    gr1->SetMinimum(std::min(*std::min_element(y1, y1 + n), *std::min_element(y2, y2 + n))
    );
    gr1->SetMaximum(std::max(*std::max_element(y1, y1 + n), *std::max_element(y2, y2 + n))
    );

    TCanvas* canvas = new TCanvas("canvas", "Graph Draw Options", 200, 10, 600, 400);

    gr1->SetLineColor(4);
    gr1->Draw("AC*");
    gr2->SetLineColor(2);
    gr2->Draw("CP");
#endif
}

auto plot(int n, const float* const x, const float* const y) -> void
{
#ifdef USE_ROOT_PLOTTING
    auto gr1 = new TGraph(n, x, y);
    gr1->Draw("AC*");
#endif
}

} // namespace root_plotting
