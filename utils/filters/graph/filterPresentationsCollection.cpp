#include "filterPresentationsCollection.h"
#include "core/filters/legacy/filtersCollection.h"

#include "inputBlockPresentation.h"
#include "outputBlockPresentation.h"

#include "filters/ui/sobelParametersControlWidget.h"
#include "filters/ui/gainOffsetParametersControlWidget.h"
#include "filters/ui/bitSelectorParametersControlWidget.h"
#include "filters/ui/cannyParametersControlWidget.h"
#include "filters/ui/backgroundFilterParametersControlWidget.h"
#include "filters/ui/inputFilterParametersControlWidget.h"
#include "filters/ui/outputFilterParametersControlWidget.h"
#include "filters/ui/operationParametersControlWidget.h"
#include "filters/ui/binarizeParametersControlWidget.h"
#include "filters/ui/thickeningParametersControlWidget.h"
#include "filters/ui/maskingParametersControlWidget.h"

FilterBlockPresentation *FilterPresentationsCollection::presentationByName(const int row,
                                                                           FilterBlock* filter,
                                                                           FilterGraphPresentation* parent,
                                                                           DiagramScene* scene)
{
    FilterBlockPresentation* presentation = NULL;
    FilterParametersControlWidgetBase* controlWidget = NULL;

    switch (row)
    {
    case FiltersCollection::GAIN_OFFSET_FILTER:
    {   controlWidget = new GainOffsetParametersControlWidget();
        presentation  = new FilterBlockPresentation(filter, controlWidget, scene);
        break;
    }
    case FiltersCollection::BIT_SELECTOR_FILTER:
    {   controlWidget = new BitSelectorParametersControlWidget();
        presentation  = new FilterBlockPresentation(filter, controlWidget, scene);
        break;
    }
    case FiltersCollection::SOBEL_FILTER:
    {   controlWidget = new SobelParametersControlWidget();
        presentation  = new FilterBlockPresentation(filter, controlWidget, scene);
        break;
    }
    case FiltersCollection::CANNY_FILTER:
    {   controlWidget = new CannyParametersControlWidget();
        presentation  = new FilterBlockPresentation(filter, controlWidget, scene);
        break;
    }
    case FiltersCollection::BACKGROUND_FILTER:
    {   controlWidget = new BackgroundFilterParametersControlWidget();
        presentation  = new FilterBlockPresentation(filter, controlWidget, scene);
        break;
    }
    case FiltersCollection::OPERATION_FILTER:
    {   controlWidget = new OperationParametersControlWidget();
        presentation  = new FilterBlockPresentation(filter, controlWidget, scene);
        break;
    }
    case FiltersCollection::TXT_FILTER:
    {//    controlWidget = new TxtFilterParametersControlWidget();
        presentation  = new FilterBlockPresentation(filter, controlWidget, scene);
        break;
    }
    case FiltersCollection::INPUT_FILTER:
    {   controlWidget = new InputFilterParametersControlWidget();
        presentation  = new InputBlockPresentation(filter, controlWidget, scene);
        break;
    }
    case FiltersCollection::OUTPUT_FILTER:
    {   controlWidget = new OutputFilterParametersControlWidget();
        presentation  = new OutputBlockPresentation(filter, controlWidget, scene);
        break;
    }
    case FiltersCollection::BINARIZE_BLOCK:
    {   controlWidget = new BinarizeParametersControlWidget();
        presentation  = new FilterBlockPresentation(filter, controlWidget, scene);
        break;
    }
    case FiltersCollection::MASK_FILTER_BLOCK:
    {   controlWidget = new MaskingParametersControlWidget();
        presentation  = new FilterBlockPresentation(filter, controlWidget, scene);
        break;
    }
    case FiltersCollection::THICKENING_BLOCK:
    {   controlWidget = new ThickeningParametersControlWidget();
        presentation  = new FilterBlockPresentation(filter, controlWidget, scene);
        break;
    }
    default:
        if (row - FiltersCollection::numberOfFilterTypes() < fCollection->compoundFilters.size())
             presentation = new CompoundBlockPresentation(parent, filter, controlWidget, scene);
        else return NULL;
    }

    if (controlWidget)
        connect(controlWidget, SIGNAL(paramsChanged()), presentation, SLOT(parametersChanged()));
    return presentation;
}

bool FilterPresentationsCollection::filterIsEdited(int _typeid)
{
    return openedFilters.find(_typeid) != openedFilters.end();
}
