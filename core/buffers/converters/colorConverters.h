#ifndef COLORCONVERTERS_H_
#define COLORCONVERTERS_H_

/*
 * colorConverters.h
 *
 *  Created on: Jul 29, 2012
 *      Author: alexander
 */


namespace corecvs
{

template<typename InputType, typename OutputType>
class YUYVtoG12
{

    void operator(const InputType &in, OutputType &out)
    {

    }
};

template<typename InputType, typename OutputType>
class YUYVtoG12G12
{

    void operator(const InputType &in, OutputType &out)
    {

    }
};

class ColorConverters
{
public:
    ColorConverters();
    virtual ~ColorConverters();
};

} /* namespace corecvs */
#endif /* COLORCONVERTERS_H_ */
