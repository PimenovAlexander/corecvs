/**
 * \file neonWrapper.cpp
 * \brief Add Comment Here
 *
 * \ingroup cppcorefiles
 * \date Oct 7, 2010
 */

#include "math/neon/neonWrapper.h"
namespace corecvs {

#ifdef PROFILE_ACCESS_ALIGNMENT
uint64_t Int16x8::alignedUWrites   = 0;
uint64_t Int16x8::alignedAWrites   = 0;
uint64_t Int16x8::unalignedUWrites = 0;
uint64_t Int16x8::streamedWrites   = 0;
#endif // PROFILE_ACCESS_ALIGNMENT

using namespace std;

#ifdef WITH_NEON
ostream & operator <<(ostream &out, const Int16x8 &vector)
{
   ALIGN_DATA(16) int16_t data[8];
   vector.saveAligned(data);
   out << "[";
   for (unsigned i = 0; i < 8; i++) {
       out << (i == 0 ? "" : ", ") << data[i];
   }
   out << "]";
   return out;
}

ostream & operator <<(ostream &out, const UInt16x8 &vector)
{
   ALIGN_DATA(16) uint16_t data[8];
   vector.saveAligned(data);
   out << "[";
   for (unsigned i = 0; i < 8; i++) {
       out << (i == 0 ? "" : ", ") << data[i];
   }
   out << "]";
   return out;
}

ostream & operator <<(ostream &out, const Int32x4 &vector)
{
   ALIGN_DATA(16) int32_t data[4];
   vector.saveAligned(data);
   out << "[";
   for (unsigned i = 0; i < 4; i++) {
       out << (i == 0 ? "" : ", ") << data[i];
   }
   out << "]";
   return out;
}

ostream & operator <<(ostream &out, const Int32x8v &vector)
{
   ALIGN_DATA(16) int32_t data[8];
   vector[0].saveAligned(data);
   vector[1].saveAligned(data + 4);

   out << "[";
   for (unsigned i = 0; i < 8; i++) {
       out << (i == 0 ? "" : ", ") << data[i];
   }
   out << "]";
   return out;
}

#endif

} //namespace corecvs

