#ifndef UTILS_H_
#define UTILS_H_
/**
 * \file utils.h
 * \brief This calss conatins CPP helper functions
 *
 *
 *
 * \ingroup cppcorefiles
 * \date Apr 14, 2010
 * \author alexander
 */

#include <string>
#include <iostream>

#include "global.h"


namespace corecvs {

//@{
/**
 * This is a helper meta-programming template to represent binary constants.
 *
 * For example:
 *         9 == binary<1001>::value
 *
 *
 **/
template <unsigned long int N>
class binary
{
public:
    const static unsigned value = (binary<N / 10>::value * 2) + ((N % 10 == 0) ? 0 : 1);
};

template <>
class binary<0>
{
    const static unsigned value = 0;
};

//@}

/*
#define MAX(X, Y) (((X) > (Y)) ? (X) : (Y))
#define MIN(X, Y) (((X) > (Y)) ? (Y) : (X))
*/

/* */
namespace HelperUtils {
    using std::string;
    using std::istream;

    bool startsWith(const string &str, const string &prefix);
    istream& getlineSafe(istream& is, string& str);
}

/* OS related stuff */
void setStdTerminateHandler();




} //namespace corecvs
#endif /* UTILS_H_ */
