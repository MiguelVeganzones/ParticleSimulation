#include "time_plot_2D.hpp"
#include <cassert>
#include <vector>
#ifdef USE_ROOT_PLOTTING
#include "TGraph.h"
#include <TCanvas.h>
#else
#include <iostream>
#endif

namespace plotting::plots_2D
{

time_plot_2D::time_plot_2D(
    std::size_t       series_count,
    VisualizationMode visualization_mode
) noexcept :
    y_(series_count),
    vm_{ visualization_mode }
{
#ifdef USE_ROOT_PLOTTING
    canvas_ =
        std::make_unique<TCanvas>("time_plot", "Graph Draw Options", 200, 10, 800, 600);
    multigraph_ = std::make_unique<TMultiGraph>();
    for (auto i = 0uz; i != y_.size(); ++i)
    {
        graphs_.push_back(std::move(std::make_unique<TGraph>()));
        graphs_[i]->SetLineColor(utils::colors[i]);
        graphs_[i]->SetLineWidth(3);
        multigraph_->Add(graphs_[i].get());
    }
#else
#endif
}

// TODO: Does not clear the canvas
auto time_plot_2D::clear() -> void
{
    t_.clear();
#ifdef USE_ROOT_PLOTTING
    multigraph_->Clear();
#endif
    for (auto j = 0uz; j != y_.size(); ++j)
    {
        y_[j].clear();
#ifdef USE_ROOT_PLOTTING
        graphs_[j]->Clear();
        multigraph_->Add(graphs_[j].get());
#endif
    }
#ifdef USE_ROOT_PLOTTING
    canvas_->Clear();
    canvas_->Update();
#endif
}

auto time_plot_2D::append(float t, std::vector<float> const& y_star) -> void
{
    assert(y_star.size() == y_.size());
    t_.push_back(t);
    for (auto i = 0uz; i != y_star.size(); ++i)
    {
        y_[i].push_back(y_star[i]);
    }
}

auto time_plot_2D::render() -> void
{
#ifdef USE_ROOT_PLOTTING
    for (auto i = 0; i != int(t_.size()); i++)
    {
        for (auto j = 0uz; j != y_.size(); ++j)
        {
            graphs_[j]->SetPoint(i, t_[i], y_[j][i]);
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
    std::cout << "t   | ";
    for (auto t : t_)
    {
        std::cout << t << ' ';
    }
    std::cout << '\n';
#endif
}

} // namespace plotting::plots_2D
