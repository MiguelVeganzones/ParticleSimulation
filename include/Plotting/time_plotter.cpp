#include "time_plotter.hpp"
#include "TGraph.h"
#include <TCanvas.h>
#include <algorithm>
#include <array>
#include <memory>

#include <iostream>

namespace root_plotting
{

time_plotter2::time_plotter2()
{
    c1  = std::make_unique<TCanvas>("canvas", "Graph Draw Options", 200, 10, 800, 600);
    gr1 = std::make_unique<TGraph>();
    gr2 = std::make_unique<TGraph>();

    gr1->SetLineColor(4);
    gr1->Draw("AC*");

    gr2->SetLineColor(2);
    gr2->Draw("CP");
};

auto time_plotter2::append(const float y1, const float y2) -> void
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

time_plotter4::time_plotter4()
{
    c1  = std::make_unique<TCanvas>("canvas", "Graph Draw Options", 200, 10, 800, 600);
    gr1 = std::make_unique<TGraph>();
    gr2 = std::make_unique<TGraph>();
    gr3 = std::make_unique<TGraph>();
    gr4 = std::make_unique<TGraph>();

    gr1->SetLineColor(4);
    gr1->Draw("AC*");

    gr2->SetLineColor(2);
    gr2->Draw("CP+");

    gr3->SetLineColor(1);
    gr3->Draw("CP");

    gr4->SetLineColor(5);
    gr4->Draw("CP.");
};

auto time_plotter4::append(const float y1, const float y2, const float y3, const float y4)
    -> void
{
    if (!std::isfinite(y1) || !std::isfinite(y2))
    {
        std::cout << "Invalid point: x=" << y1 << ", y=" << y2 << std::endl;
    }

    if (n < 0)
    {
        std::cout << "Invalid index: n=" << n << std::endl;
    }
    const std::array v{ y1, y2, y3, y4 };
    min_y = n == 0 ? *std::min_element(std::cbegin(v), std::cend(v))
                   : std::min(min_y, *std::min_element(std::cbegin(v), std::cend(v)));
    max_y = n == 0 ? *std::max_element(std::cbegin(v), std::cend(v))
                   : std::max(max_y, *std::max_element(std::cbegin(v), std::cend(v)));
    gr1->SetMinimum(min_y);
    gr1->SetMaximum(max_y);
    gr2->SetMinimum(min_y);
    gr2->SetMaximum(max_y);
    gr3->SetMinimum(min_y);
    gr3->SetMaximum(max_y);
    gr4->SetMinimum(min_y);
    gr4->SetMaximum(max_y);

    gr1->SetPoint(n, n, y1);
    gr2->SetPoint(n, n, y2);
    gr3->SetPoint(n, n, y3);
    gr4->SetPoint(n, n, y4);

    ++n;

    c1->Draw();
    c1->Update();
}

} // namespace root_plotting
