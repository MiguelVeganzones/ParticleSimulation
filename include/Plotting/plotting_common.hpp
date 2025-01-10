#pragma once

#ifdef USE_ROOT_PLOTTING
#include <Rtypes.h>
#include <TApplication.h>
#include <TAttMarker.h>
#include <array>
#endif

namespace plotting
{

enum struct VisualizationMode
{
    layout_2D,
    layout_3D,
};

struct utils
{
#ifdef USE_ROOT_PLOTTING
    inline static constexpr auto colors =
        std::array{ kBlack, kGray,   kRed,    kGreen, kBlue,  kYellow, kMagenta,
                    kCyan,  kOrange, kSpring, kTeal,  kAzure, kViolet, kPink };
    inline static constexpr auto markers = std::array{ kDot,
                                                       kPlus,
                                                       kStar,
                                                       kCircle,
                                                       kMultiply,
                                                       kFullDotSmall,
                                                       kFullDotMedium,
                                                       kFullDotLarge,
                                                       kFullCircle,
                                                       kFullSquare,
                                                       kFullTriangleUp,
                                                       kFullTriangleDown,
                                                       kOpenCircle,
                                                       kOpenSquare,
                                                       kOpenTriangleUp,
                                                       kOpenDiamond,
                                                       kOpenCross,
                                                       kFullStar,
                                                       kOpenStar,
                                                       kOpenTriangleDown,
                                                       kFullDiamond,
                                                       kFullCross,
                                                       kOpenDiamondCross,
                                                       kOpenSquareDiagonal,
                                                       kOpenThreeTriangles,
                                                       kOctagonCross,
                                                       kFullThreeTriangles,
                                                       kOpenFourTrianglesX,
                                                       kFullFourTrianglesX,
                                                       kOpenDoubleDiamond,
                                                       kFullDoubleDiamond,
                                                       kOpenFourTrianglesPlus,
                                                       kFullFourTrianglesPlus,
                                                       kOpenCrossX,
                                                       kFullCrossX,
                                                       kFourSquaresX,
                                                       kFourSquaresPlus };
#endif
};

} // namespace plotting
