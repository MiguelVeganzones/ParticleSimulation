#include "time_plotter.hpp"
#include "TGraph.h"
#include <TCanvas.h>
#include <memory>

#include <iostream>

namespace root_plotting
{

time_plotter::time_plotter()
{
    c1  = std::make_unique<TCanvas>("canvas", "Graph Draw Options", 200, 10, 800, 600);
    gr1 = std::make_unique<TGraph>();
    gr2 = std::make_unique<TGraph>();

    gr1->SetLineColor(4);
    gr1->Draw("AC*");

    gr2->SetLineColor(2);
    gr2->Draw("CP");
};

auto time_plotter::append(const float y1, const float y2) -> void
{
    if (!std::isfinite(y1) || !std::isfinite(y2))
    {
        std::cout << "Invalid point: x=" << y1 << ", y=" << y2 << std::endl;
    }

    if (n < 0)
    {
        std::cout << "Invalid index: n=" << n << std::endl;
    }
    min_y = n == 0 ? std::min(y1, y2) : std::min(min_y, std::min(y1, y2));
    max_y = n == 0 ? std::max(y1, y2) : std::max(max_y, std::max(y1, y2));
    gr1->SetMinimum(min_y);
    gr1->SetMaximum(max_y);

    gr1->SetPoint(n, n, y1);
    gr2->SetPoint(n, n, y2);

    ++n;

    c1->Draw();
    c1->Update();
}

} // namespace root_plotting
