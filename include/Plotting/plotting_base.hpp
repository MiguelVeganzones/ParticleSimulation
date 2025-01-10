#pragma once

#include <vector>

namespace plotting
{

namespace interface
{

class appendable
{
public:
    virtual ~appendable()                                         = default;
    virtual auto append(float, std::vector<float> const&) -> void = 0;
};

class clearable
{
public:
    virtual ~clearable()         = default;
    virtual auto clear() -> void = 0;
};

} // namespace interface

class plot
{
public:
    virtual ~plot()               = default;
    virtual auto render() -> void = 0;
};

class scatter_plot : public plot
{
public:
    virtual ~scatter_plot() = default;
};

class time_plot : public plot, public interface::clearable, public interface::appendable
{
public:
    virtual ~time_plot() = default;
};

class series_plot : public plot
{
public:
    virtual ~series_plot() = default;
};

} // namespace plotting
