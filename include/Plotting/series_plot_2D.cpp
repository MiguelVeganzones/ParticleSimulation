#include "series_plot_2D.hpp"
#include <vector>
#ifdef USE_ROOT_PLOTTING
#include "TGraph.h"
#include <TCanvas.h>
#else
#include <iostream>
#endif

namespace plotting::plots_2D
{

series_plot_2D::series_plot_2D(
    std::vector<float>&              x,
    std::vector<std::vector<float>>& y,
    VisualizationMode                visualization_mode
) noexcept :
    x_{ x },
    y_{ y },
    vm_{ visualization_mode }
{
#ifdef USE_ROOT_PLOTTING
    canvas_ =
        std::make_unique<TCanvas>("series_plot", "Graph Draw Options", 200, 10, 800, 600);
    multigraph_ = std::make_unique<TMultiGraph>();
    for (auto i = 0uz; i != y_.size(); ++i)
    {
        graphs_.push_back(std::move(std::make_unique<TGraph>()));
        graphs_[i]->SetLineColor(utils::colors[i + 2]);
        graphs_[i]->SetLineWidth(3);
        multigraph_->Add(graphs_[i].get());
    }
#else
#endif
}

auto series_plot_2D::render() -> void
{
#ifdef USE_ROOT_PLOTTING
    for (auto i = 0; i != int(x_.size()); i++)
    {
        for (auto j = 0uz; j != y_.size(); ++j)
        {
            graphs_[j]->SetPoint(i, x_[i], y_[j][i]);
        }
    }
    canvas_->cd();
    multigraph_->Draw(vm_sv(vm_));
    canvas_->Update();
#else
    for (auto i = 0uz; i != y_.size(); ++i)
    {
        std::cout << "y_" << i << " | ";
        for (auto y : y_[i])
        {
            std::cout << y << ' ';
        }
        std::cout << '\n';
    }
    std::cout << "x   | ";
    for (auto x : x_)
    {
        std::cout << x << ' ';
    }
    std::cout << '\n';
#endif
}

} // namespace plotting::plots_2D
