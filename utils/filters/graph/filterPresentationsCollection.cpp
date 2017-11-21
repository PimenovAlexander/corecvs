#include "filterPresentationsCollection.h"
#include "core/filters/filtersCollection.h"

#include "inputBlockPresentation.h"
#include "outputBlockPresentation.h"

#include "sobelParametersControlWidget.h"
#include "gainOffsetParametersControlWidget.h"
#include "bitSelectorParametersControlWidget.h"
#include "cannyParametersControlWidget.h"
#include "backgroundFilterParametersControlWidget.h"
#include "inputFilterParametersControlWidget.h"
#include "outputFilterParametersControlWidget.h"
#include "operationParametersControlWidget.h"
#include "binarizeParametersControlWidget.h"
#include "thickeningParametersControlWidget.h"
#include "maskingParametersControlWidget.h"

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
