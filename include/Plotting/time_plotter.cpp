#include "time_plotter.hpp"
#include "TGraph.h"
#include <TCanvas.h>
#include <memory>

namespace root_plotting
{

time_plotter::time_plotter()
{
    this->c1 =
        std::make_unique<TCanvas>("canvas", "Graph Draw Options", 200, 10, 800, 600);
    this->gr1 = std::make_unique<TGraph>();
    this->gr2 = std::make_unique<TGraph>();

    gr1->SetLineColor(4);
    gr1->Draw("AC*");

    gr2->SetLineColor(2);
    gr2->Draw("CP");
};

auto time_plotter::plot(const int n, float const* const y1, float const* const y2) -> void
{
    const auto min =
        std::min(*std::min_element(y1, y1 + n), *std::min_element(y2, y2 + n));
    const auto max =
        std::max(*std::max_element(y1, y1 + n), *std::max_element(y2, y2 + n));
    gr1->SetMinimum(min);
    gr2->SetMinimum(min);
    gr1->SetMaximum(max);
    gr2->SetMaximum(max);
    for (int i = 0; i < n; i++)
    {
        gr1->SetPoint(i, i, y1[i]);
        gr2->SetPoint(i, i, y2[i]);
    }
    c1->Draw();
    c1->Update();
}

} // namespace root_plotting
