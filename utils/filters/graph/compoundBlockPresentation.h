#ifndef COMPOUNDBLOCKPRESENTATION_H
#define COMPOUNDBLOCKPRESENTATION_H

#include "core/filters/legacy/blocks/compoundFilter.h"
#include "filterBlockPresentation.h"

class FilterGraphPresentation;

class CompoundBlockPresentation : public FilterBlockPresentation
{
public:
    CompoundBlockPresentation(FilterGraphPresentation* _parent,
                              FilterBlock* _block,
                              FilterParametersControlWidgetBase* _widget,
                              DiagramScene* _scene):
        FilterBlockPresentation(_block, _widget, _scene), parent(_parent), child(NULL)
    {
        compoundFilter = dynamic_cast<CompoundFilter*>(mBlock);
    }

    FilterGraphPresentation* parent;
    CompoundFilter* compoundFilter;
    FilterGraphPresentation* child;

    void addInputPin(Pin* pin)
    {
        addPin(mInputPins, pin);
        drawInputPins();
    }

    void addOutputPin(Pin* pin)
    {
        addPin(mOutputPins, pin);
        drawOutputPins();
    }

    void addPin(vector<FilterPinPresentation*> &vec, Pin* pin)
    {
        FilterPinPresentation* ppin = FilterPinPresentation::getPinByType(pin, scene, this);
        vec.push_back(ppin);
    }

    void removeInputPin(Pin* pin)
    {
        removePin(mInputPins, pin);
        drawInputPins();
    }

    void removeOutputPin(Pin* pin)
    {
        removePin(mOutputPins, pin);
        drawOutputPins();
    }

    void removePin(vector<FilterPinPresentation*> &vec, Pin* pin)
    {
        FilterPinPresentation* ppin = dynamic_cast<FilterPinPresentation*>(scene->diagramItemByPin(pin));
        vector<FilterPinPresentation*>::iterator pinsIterator
                = std::find(vec.begin(), vec.end(), ppin);
        if (pinsIterator != vec.end())
        {
            delete *pinsIterator;
            vec.erase(pinsIterator);
        }
    }
};

#endif // COMPOUNDBLOCKPRESENTATION_H
