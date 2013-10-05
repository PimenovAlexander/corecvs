#ifndef OUTPUTBLOCKPRESENTATION_H
#define OUTPUTBLOCKPRESENTATION_H

#include "filterBlockPresentation.h"

class OutputBlockPresentation : public FilterBlockPresentation
{
public:
    OutputBlockPresentation(FilterBlock* _block,
                            FilterParametersControlWidgetBase* _widget,
                            DiagramScene* _scene):
        FilterBlockPresentation(_block, _widget, _scene)
    {}
    //virtual void draw(DiagramScene *scene, int x, int y);
};

#endif // OUTPUTBLOCKPRESENTATION_H
