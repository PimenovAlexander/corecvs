/**
 * \file sseWrapper.cpp
 * \brief Add Comment Here
 *
 * \ingroup cppcorefiles
 * \date Oct 7, 2010
 * \author alexander
 */

#include "sseWrapper.h"
namespace corecvs {

#ifdef PROFILE_ACCESS_ALIGNMENT
uint64_t Int16x8::alignedUWrites   = 0;
uint64_t Int16x8::alignedAWrites   = 0;
uint64_t Int16x8::unalignedUWrites = 0;
uint64_t Int16x8::streamedWrites   = 0;
#endif // PROFILE_ACCESS_ALIGNMENT

#ifdef WITH_SSE
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

ostream & operator <<(ostream &out, const Int8x16 &vector)
{
   ALIGN_DATA(16) int8_t data[16];
   vector.saveAligned(data);
   out << "[";
   for (unsigned i = 0; i < 16; i++) {
       out << (i == 0 ? "" : ", ") << (int)data[i];
   }
   out << "]";
   return out;
}

ostream & operator <<(ostream &out, const UInt8x16 &vector)
{
   ALIGN_DATA(16) uint8_t data[16];
   vector.saveAligned(data);
   out << "[";
   for (unsigned i = 0; i < 16; i++) {
       out << (i == 0 ? "" : ", ") << (unsigned)data[i];
   }
   out << "]";
   return out;
}


ostream & operator <<(ostream &out, const Int64x2 &vector)
{
   ALIGN_DATA(16) int64_t data[2];
   vector.saveAligned(data);
   out << "[";
   for (unsigned i = 0; i < 2; i++) {
       out << (i == 0 ? "" : ", ") << (int)data[i];
   }
   out << "]";
   return out;
}

#endif // WITH_SSE

} //namespace corecvs

