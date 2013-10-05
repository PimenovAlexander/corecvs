#ifndef INPUTBLOCKPRESENTATION_H
#define INPUTBLOCKPRESENTATION_H

#include "filterBlockPresentation.h"

class InputBlockPresentation : public FilterBlockPresentation
{
public:
    InputBlockPresentation(FilterBlock* _block,
                           FilterParametersControlWidgetBase* _widget,
                           DiagramScene* _scene):
        FilterBlockPresentation(_block, _widget, _scene)
    {}
    //virtual void draw(DiagramScene *scene, int x, int y);
};

#endif // INPUTBLOCKPRESENTATION_H
