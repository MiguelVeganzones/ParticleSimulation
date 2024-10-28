#include "plotting.hpp"
#include "TGraph.h"
#include <TCanvas.h>

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
    auto gr1 = new TGraph(n, x, y1);
    auto gr2 = new TGraph(n, x, y2);
    auto gr3 = new TGraph(n, x, y3);
    auto gr4 = new TGraph(n, x, y4);

    TCanvas* canvas = new TCanvas("canavs", "Graph Draw Options", 200, 10, 600, 400);

    gr1->SetLineColor(4);
    gr1->Draw("AC*");
    gr1->SetLineColor(2);
    gr2->Draw("CP");
    gr1->SetLineColor(1);
    gr3->Draw("C+");
    gr1->SetLineColor(0);
    gr4->Draw("C.");
}

auto plot(
    int                n,
    const float* const x,
    const float* const y1,
    const float* const y2,
    const float* const y3
) -> void
{
    auto gr1 = new TGraph(n, x, y1);
    auto gr2 = new TGraph(n, x, y2);
    auto gr3 = new TGraph(n, x, y3);

    TCanvas* canvas = new TCanvas("canavs", "Graph Draw Options", 200, 10, 600, 400);

    gr1->SetLineColor(4);
    gr1->Draw("AC*");

    gr2->SetMarkerStyle(21);
    gr2->SetLineColor(2);
    gr2->Draw("CP");

    gr3->SetMarkerStyle(20);
    gr3->SetLineColor(1);
    gr3->Draw("CP");
}

auto plot(int n, const float* const x, const float* const y1, const float* const y2)
    -> void
{
    auto gr1 = new TGraph(n, x, y1);
    auto gr2 = new TGraph(n, x, y2);

    TCanvas* canvas = new TCanvas("canavs", "Graph Draw Options", 200, 10, 600, 400);

    gr1->SetLineColor(4);
    gr1->Draw("AC*");

    gr2->SetMarkerStyle(21);
    gr2->SetLineColor(2);
    gr2->Draw("CP");
}

auto plot(int n, const float* const x, const float* const y) -> void
{
    auto gr1 = new TGraph(n, x, y);
    gr1->Draw("AC*");
}

} // namespace root_plotting
