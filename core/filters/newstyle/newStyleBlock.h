#ifndef NEWSTYLEBLOCK_H
#define NEWSTYLEBLOCK_H

#include "core/xml/generated/adderSubstractorParametersBase.h"

namespace corecvs {




class NewStyleBlock
{
public:
    NewStyleBlock();

    virtual int operator()() { return 0; }
    virtual ~NewStyleBlock();
};


class AdderSubstractor : public AdderSubstractorParametersBase, public NewStyleBlock
{

    virtual int operator()() override
    {
        if (parameter()) {
            setOutput1(input1() + input2());
            setOutput2(input1() - input2());
        } else {
            setOutput2(input1() + input2());
            setOutput1(input1() - input2());
        }
        return 0;
    }
    virtual ~AdderSubstractor();


};

}  // namespace corecvs

#endif // NEWSTYLEBLOCK_H
