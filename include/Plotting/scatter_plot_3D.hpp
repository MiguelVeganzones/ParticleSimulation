#pragma once

#include "plotting_base.hpp"
#include "plotting_common.hpp"
#include "series_plot_2D.hpp"
#include <vector>
#ifdef USE_ROOT_PLOTTING
#include <TCanvas.h>
#include <TNtuple.h>
#include <memory>
#endif

namespace plotting::plots_3D
{

class scatter_plot_3D final : public scatter_plot
{
public:
    struct data_point
    {
        float x;
        float y;
        float z;
    };

public:
    scatter_plot_3D(std::vector<std::vector<data_point>>& data) noexcept;

    auto render() -> void final;

private:
#ifdef USE_ROOT_PLOTTING
    std::unique_ptr<TCanvas>              canvas_;
    std::vector<std::unique_ptr<TNtuple>> tntuples_;
#endif
    std::vector<std::vector<data_point>>& data_;
};

} // namespace plotting::plots_3D
