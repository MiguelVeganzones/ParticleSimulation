#include "scatter_plot_3D.hpp"
#include <iostream>
#include <vector>
#ifdef USE_ROOT_PLOTTING
#include "plotting_common.hpp"
#include <TCanvas.h>
#include <TNtuple.h>
#else
#endif

namespace plotting::plots_3D
{

scatter_plot_3D::scatter_plot_3D(std::vector<std::vector<data_point>>& data) noexcept :
    data_{ data }
{
#ifdef USE_ROOT_PLOTTING
    canvas_ = std::make_unique<TCanvas>(
        "scatter_canvas", "Graph Draw Options", 200, 10, 800, 600
    );
    for (auto i = 0uz; i != data_.size(); ++i)
    {
        auto ntuple_name  = "ntuple_" + std::to_string(i);
        auto ntuple_title = "Scatter Plot " + std::to_string(i);
        tntuples_.emplace_back(std::make_unique<TNtuple>(
            ntuple_name.c_str(), ntuple_title.c_str(), "x:y:z:color"
        ));
        tntuples_[i]->SetMarkerStyle(utils::markers[i + 8]);
        // TODO: Colors dont work
        // tntuples_[i]->SetMarkerColor(utils::colors[i + 2]);
    }
#else
#endif
}

auto scatter_plot_3D::render() -> void
{
#ifdef USE_ROOT_PLOTTING
    canvas_->cd();
    for (auto i = 0uz; i != data_.size(); ++i)
    {
        tntuples_[i]->Reset();
        for (auto j = 0uz; j != data_[i].size(); ++j)
        {
            tntuples_[i]->Fill(data_[i][j].x, data_[i][j].y, data_[i][j].z, 40);
        }
        if (i == 0)
        {
            tntuples_[i]->Draw("x:y:z:color");
        }
        else
        {
            tntuples_[i]->Draw("x:y:z:color", "", "SAME");
        }
    }
    canvas_->Update();
#else
#endif
}

} // namespace plotting::plots_3D
