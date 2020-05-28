#include "camerafixture/wildcardablePointerPair.h"
#include "camerafixture/sceneFeaturePoint.h"

namespace corecvs {

#if !defined(WIN32) || (_MSC_VER >= 1900) // Sometime in future (when we switch to VS2015 due to https://msdn.microsoft.com/library/hh567368.apx ) we will get constexpr on windows
#else
WPP::UTYPE const WPP::UWILDCARD = nullptr;
WPP::VTYPE const WPP::VWILDCARD = nullptr;
#endif

} // namespace corecvs
