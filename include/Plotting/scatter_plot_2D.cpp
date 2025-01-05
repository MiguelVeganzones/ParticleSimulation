#include "scatter_plot_2D.hpp"
#include <iostream>
#include <vector>
#ifdef USE_ROOT_PLOTTING
#include "plotting_common.hpp"
#include <TCanvas.h>
#include <TNtuple.h>
#else
#endif

namespace plotting::plots_2D
{

scatter_plot_2D::scatter_plot_2D(std::vector<std::vector<data_point>>& data) noexcept :
    data_{ data }
{
#ifdef USE_ROOT_PLOTTING
    canvas_ = std::make_unique<TCanvas>(
        "scatter_canvas", "Graph Draw Options", 200, 10, 800, 600
    );
    multigraph_ = std::make_unique<TMultiGraph>();
    for (auto i = 0uz; i != data_.size(); ++i)
    {
        graphs_.push_back(std::move(std::make_unique<TGraph>()));
        graphs_[i]->SetMarkerStyle(utils::markers[i + 8]);
        graphs_[i]->SetMarkerColor(utils::colors[i + 2]);
        multigraph_->Add(graphs_[i].get());
    }
#else
#endif
}

auto scatter_plot_2D::render() -> void
{
#ifdef USE_ROOT_PLOTTING
    canvas_->cd();
    for (auto i = 0uz; i != data_.size(); ++i)
    {
        for (auto j = 0; j != int(data_[i].size()); ++j)
        {
            graphs_[i]->SetPoint(j, data_[i][j].x, data_[i][j].y);
        }
    }
    canvas_->cd();
    multigraph_->Draw("AP");
    canvas_->Update();
#else
#endif
}

} // namespace plotting::plots_2D
