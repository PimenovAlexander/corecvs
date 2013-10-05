#include "basefilter.h"
#include "gainoffset.h"
#include "bitselector.h"
#include "sobelfilter.h"
#include "scalefilter.h"

BaseFilter::BaseFilter(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);
}

G12Buffer *BaseFilter::filter (G12Buffer *input)
{
    G12Buffer *toReturn = new G12Buffer(input);
    return toReturn;
}

BaseFilter::~BaseFilter()
{

}


BaseFilter *FilterFabric::createFilterInstance(QString name)
{
//    if (name == QString(BaseFilter::getName()))
//        return new BaseFilter();
    if (name == QString(GainOffset::getName()))
        return new GainOffset();
    if (name == QString(BitSelector::getName()))
        return new BitSelector();
    if (name == QString(SobelFilter::getName()))
        return new SobelFilter();
    if (name == QString(ScaleFilter::getName()))
        return new ScaleFilter();
    return NULL;
}

QVector<QString> FilterFabric::getFilterNames()
{
    QVector<QString> list;
//    list.append(BaseFilter::getName());
    list.append(GainOffset::getName());
    list.append(BitSelector::getName());
    list.append(SobelFilter::getName());
    list.append(ScaleFilter::getName());
    return list;
}
