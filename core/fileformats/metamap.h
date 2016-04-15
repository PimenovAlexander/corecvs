/**
 * \file    fileformats\metamap.h
 *
 * Declares the metamap for metadata handling.
 */
#ifndef CMETAMAP_H
#define CMETAMAP_H

#include <map>
#include <string>
#include <vector>

namespace corecvs {

typedef std::vector<double>                 MetaValue;
typedef std::map<std::string, MetaValue>    MetaData;

} // namespace corecvs

#endif // CMETAMAP_H
