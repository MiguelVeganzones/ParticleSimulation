#pragma once

#include "plotting_base.hpp"
#include "plotting_common.hpp"
#include "series_plot_2D.hpp"
#include <vector>
#ifdef USE_ROOT_PLOTTING
#include <TCanvas.h>
#include <TGraph.h>
#include <TMultiGraph.h>
#include <memory>
#endif

namespace plotting::plots_2D
{

class time_plot_2D final : public time_plot
{
#ifdef USE_ROOT_PLOTTING

    inline static constexpr auto vm_sv(VisualizationMode vm) -> const char*
    {
        switch (vm)
        {
        case VisualizationMode::layout_3D: return "a fb l3d";
        case VisualizationMode::layout_2D:
        default: return "a";
        }
    };
#endif
public:
    time_plot_2D(
        std::size_t       series_count,
        VisualizationMode visualization_mode = VisualizationMode::layout_2D
    ) noexcept;

    auto render() -> void final;

    auto clear() -> void final;

    auto append(float t, std::vector<float> const& y_star) -> void final;

private:
#ifdef USE_ROOT_PLOTTING
    std::unique_ptr<TCanvas>             canvas_;
    std::unique_ptr<TMultiGraph>         multigraph_;
    std::vector<std::unique_ptr<TGraph>> graphs_;
#endif
    std::vector<float>              t_;
    std::vector<std::vector<float>> y_;
    VisualizationMode               vm_;
};

} // namespace plotting::plots_2D
