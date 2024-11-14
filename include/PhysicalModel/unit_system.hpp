#pragma once

#ifndef USE_UNIT_SYSTEM
#define USE_UNIT_SYSTEM (true)
#endif

namespace pm::units
{

enum struct Units
{
    s,
    m,
    m_s,
    m_s2,
    rad,
    rad_s,
    rad_s2,
    kg,
    newton,
    _runtime_unit_,
};

} // namespace pm::units
