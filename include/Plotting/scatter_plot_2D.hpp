#pragma once

#include "plotting_base.hpp"
#include "plotting_common.hpp"
#include "series_plot_2D.hpp"
#include <vector>
#ifdef USE_ROOT_PLOTTING
#include <TCanvas.h>
#include <TGraph.h>
#include <memory>
#endif

namespace plotting::plots_2D
{

class scatter_plot_2D final : public scatter_plot
{
public:
    struct data_point
    {
        float x;
        float y;
        float z;
    };

public:
    scatter_plot_2D(std::vector<std::vector<data_point>>& data) noexcept;

    auto render() -> void final;

private:
#ifdef USE_ROOT_PLOTTING
    std::unique_ptr<TCanvas>             canvas_;
    std::unique_ptr<TMultiGraph>         multigraph_;
    std::vector<std::unique_ptr<TGraph>> graphs_;
#endif
    std::vector<std::vector<data_point>>& data_;
};

} // namespace plotting::plots_2D
