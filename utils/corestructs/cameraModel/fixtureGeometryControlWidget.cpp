
#include "fixtureGeometryControlWidget.h"
#include "ui_fixtureGeometryControlWidget.h"

using namespace corecvs;

FixtureGeometryControlWidget::FixtureGeometryControlWidget(QWidget *parent) :
    ParametersControlWidgetBase(parent),
    ui(new Ui::FixtureGeometryControlWidget)
{
    ui->setupUi(this);

    connect(ui->originXSpinBox, SIGNAL(valueChanged(double)), this, SLOT(paramsChangedInUI()));
    connect(ui->originYSpinBox, SIGNAL(valueChanged(double)), this, SLOT(paramsChangedInUI()));
    connect(ui->originZSpinBox, SIGNAL(valueChanged(double)), this, SLOT(paramsChangedInUI()));

    connect(ui->ort1XSpinBox, SIGNAL(valueChanged(double)), this, SLOT(paramsChangedInUI()));
    connect(ui->ort1YSpinBox, SIGNAL(valueChanged(double)), this, SLOT(paramsChangedInUI()));
    connect(ui->ort1ZSpinBox, SIGNAL(valueChanged(double)), this, SLOT(paramsChangedInUI()));

    connect(ui->ort2XSpinBox, SIGNAL(valueChanged(double)), this, SLOT(paramsChangedInUI()));
    connect(ui->ort2YSpinBox, SIGNAL(valueChanged(double)), this, SLOT(paramsChangedInUI()));
    connect(ui->ort2ZSpinBox, SIGNAL(valueChanged(double)), this, SLOT(paramsChangedInUI()));

    connect(ui->polygonWidget, SIGNAL(cellChanged(int,int)), this, SLOT(paramsChangedInUI()));

    /* Things that need to be decoupled later */
    ui->advancedWidget->setVisible(false);

    connect(ui->advancedButton, SIGNAL(toggled(bool)), ui->advancedWidget, SLOT(setVisible(bool)));
    connect(ui->relatedGraphButton  , SIGNAL(released()), this, SLOT(showRelatedGraph()));
    connect(ui->hilightRelatedButton, SIGNAL(released()), this, SLOT(hilightRelatedGraph()));

    connect(ui->binSizeSpinBox, SIGNAL(valueChanged(double)), this, SLOT(regenGraph()));
    /* No direct call for signalBlock to work on this connection */
    connect(this, SIGNAL(paramsChanged()), this, SLOT(updateHelperUI()));

}

FixtureGeometryControlWidget::~FixtureGeometryControlWidget()
{
     delete_safe(ui);
}


void FixtureGeometryControlWidget::paramsChangedInUI()
{
    emit paramsChanged();
}

void FixtureGeometryControlWidget::updateHelperUI()
{
    FlatPolygon params;
    getParameters(params);
    Vector3dd normal = params.frame.getNormal().normalised();
    ui->normalX->setText(QString::number(normal.x()));
    ui->normalY->setText(QString::number(normal.y()));
    ui->normalZ->setText(QString::number(normal.z()));
}

void FixtureGeometryControlWidget::showRelatedGraph()
{
    mGraphDialog.show();
}


/*
void FixtureGeometryControlWidget::addEntry()
{
    QListWidgetItem *item = new QListWidgetItem();
    ui->listWidget->insertItem(ui->polygonWidget->count(), item);
    QDoubleSpinBox *widget = new QDoubleSpinBox;
    widget->show();
    widget->setMaximum(mMaximum);
    widget->setMinimum(mMinimum);

    ui->listWidget->setItemWidget(item, widget);
    connect(widget, SIGNAL(valueChanged(double)), this, SIGNAL(valueChanged()));

    emit valueChanged();
}

void FixtureGeometryControlWidget::removeEntry()
{
    QListWidgetItem *item = ui->polygonWidget->takeItem(ui->listWidget->count() - 1);
    delete item;
    emit valueChanged();
}

void FixtureGeometryControlWidget::resize(int size)
{
    if (size < 0) size = 0;

    while ( ui->polygonWidget->count() > size)
        removeEntry();

    while ( ui->polygonWidget->count() < size)
        addEntry();

}
*/

/**/

void FixtureGeometryControlWidget::getParameters(FlatPolygon& params) const
{
    params.frame.p1 = Vector3dd(
                ui->originXSpinBox->value(),
                ui->originYSpinBox->value(),
                ui->originZSpinBox->value()
            );
    params.frame.e1 = Vector3dd(
                ui->ort1XSpinBox->value(),
                ui->ort1YSpinBox->value(),
                ui->ort1ZSpinBox->value()
            );

    params.frame.e2 = Vector3dd(
                ui->ort2XSpinBox->value(),
                ui->ort2YSpinBox->value(),
                ui->ort2ZSpinBox->value()
            );

    params.polygon.resize(ui->polygonWidget->rowCount());
    for (int i = 0; i < ui->polygonWidget->rowCount(); i++)
    {
        QDoubleSpinBox *xWidget = static_cast<QDoubleSpinBox *>(ui->polygonWidget->cellWidget(i,1));
        QDoubleSpinBox *yWidget = static_cast<QDoubleSpinBox *>(ui->polygonWidget->cellWidget(i,2));
        params.polygon[i] = Vector2dd(xWidget->value(), yWidget->value());
    }
}

FlatPolygon *FixtureGeometryControlWidget::createParameters() const
{

    /**
     * We should think of returning parameters by value or saving them in a preallocated place
     **/
    FlatPolygon *result = new FlatPolygon();
    getParameters(*result);
    return result;
}

void FixtureGeometryControlWidget::setParameters(const FlatPolygon &input)
{
    // Block signals to send them all at once
    bool wasBlocked = blockSignals(true);

    ui->originXSpinBox->setValue(input.frame.p1.x());
    ui->originYSpinBox->setValue(input.frame.p1.y());
    ui->originZSpinBox->setValue(input.frame.p1.z());

    ui->ort1XSpinBox->setValue(input.frame.e1.x());
    ui->ort1YSpinBox->setValue(input.frame.e1.y());
    ui->ort1ZSpinBox->setValue(input.frame.e1.z());

    ui->ort2XSpinBox->setValue(input.frame.e2.x());
    ui->ort2YSpinBox->setValue(input.frame.e2.y());
    ui->ort2ZSpinBox->setValue(input.frame.e2.z());

    ui->polygonWidget->clear();
    for (size_t i = 0; i < input.polygon.size(); i++)
    {
        ui->polygonWidget->insertRow((int)i);
        ui->polygonWidget->setItem((int)i, 0, new QTableWidgetItem(QString::number(i)));
        {
            QDoubleSpinBox *widget = new QDoubleSpinBox;
            widget->show();
            widget->setMaximum( 9999999);
            widget->setMinimum(-9999999);
            widget->setDecimals(5);
            widget->setValue(input.polygon[i].x());
            connect(widget, SIGNAL(valueChanged(double)), this, SLOT(paramsChangedInUI()));
            ui->polygonWidget->setCellWidget((int)i, 1, widget);
        }
        {
            QDoubleSpinBox *widget = new QDoubleSpinBox;
            widget->show();
            widget->setMaximum( 9999999);
            widget->setMinimum(-9999999);
            widget->setDecimals(5);
            widget->setValue(input.polygon[i].y());
            connect(widget, SIGNAL(valueChanged(double)), this, SLOT(paramsChangedInUI()));
            ui->polygonWidget->setCellWidget((int)i, 2, widget);
        }
    }

    blockSignals(wasBlocked);
    paramsChangedInUI();
}

void FixtureGeometryControlWidget::setParametersVirtual(void *input)
{
    // Modify widget parameters from outside
    FlatPolygon *inputCasted = static_cast<FlatPolygon *>(input);
    setParameters(*inputCasted);
}

void FixtureGeometryControlWidget::loadParamWidget(WidgetLoader &)
{
    SYNC_PRINT(("FixtureGeometryControlWidget::loadParamWidget : Not yet implemented\n"));
}

void FixtureGeometryControlWidget::saveParamWidget(WidgetSaver &)
{
    SYNC_PRINT(("FixtureGeometryControlWidget::saveParamWidget : Not yet implemented\n"));
}

void FixtureGeometryControlWidget::hilightRelatedGraph()
{
    for (size_t i = 0; i < mGeometry->relatedPoints.size(); i++)
    {
        SceneFeaturePoint *point = mGeometry->relatedPoints[i];
        point->color = RGBColor::Red();
    }
}

void FixtureGeometryControlWidget::setFixtureGeometry(FixtureSceneGeometry *geometry)
{
    mGeometry = geometry;

    QString result;

    ui->colorWidget->setRGBColor(geometry->color);

    result += QString("Related points :%1\n").arg(geometry->relatedPoints.size());
    double sum = 0;
    double sumRepr = 0;

    int num = 0;
    int numRepr = 0;

    Plane3d plane = geometry->frame.toPlane();

    for (size_t i = 0; i < geometry->relatedPoints.size(); i++)
    {
        SceneFeaturePoint *point = geometry->relatedPoints[i];
        Vector3dd position = point->position;
        Vector3dd repr     = point->reprojectedPosition;

        if (point->hasKnownPosition) {
            sum     += plane.distanceTo(position);
            num++;
        }

        if (point->hasKnownReprojectedPosition)
        {
            sumRepr += plane.distanceTo(repr);
            numRepr++;
        }
    }

    result += QString("Positions: %1 mean deviation %2\n").arg(num).arg(sum / num);
    result += QString("Reprojected: %1 mean deviation %2\n").arg(numRepr).arg(sumRepr / numRepr);

    regenGraph();

    ui->infoLabel->setText(result);

}

RGBColor FixtureGeometryControlWidget::getColor()
{
    return ui->colorWidget->getColor();
}

void FixtureGeometryControlWidget::regenGraph( void)
{
    SYNC_PRINT(("FixtureGeometryControlWidget::regenGraph(void): called\n"));
    if (mGeometry == NULL)
        return;

    double binSize = ui->binSizeSpinBox->value();
    double binsP[1000];
    double binsR[1000];
    int H_ZERO = 500;

    for (int i = 0; i < 1000; i++)
    {
        binsP[i] = 0.0;
        binsR[i] = 0.0;
    }


    Plane3d plane = mGeometry->frame.toPlane();

    for (size_t i = 0; i < mGeometry->relatedPoints.size(); i++)
    {
        SceneFeaturePoint *point = mGeometry->relatedPoints[i];
        Vector3dd position = point->position;
        Vector3dd repr     = point->reprojectedPosition;

        if (point->hasKnownPosition) {
            double dev = plane.deviationTo(position);
            dev /= binSize;
            dev += H_ZERO;
            int index= fround(dev);
            if (index >= 0 && index < 1000) {
                binsP[index]++;
            }
        }

        if (point->hasKnownReprojectedPosition) {
            double dev = plane.deviationTo(repr);
            dev /= binSize;
            dev += H_ZERO;
            int index= fround(dev);
            if (index >= 0 && index < 1000) {
                binsR[index]++;
            }
        }
    }

    for (size_t i = 0; i < 1000; i++)
    {
        mGraphDialog.addGraphPoint("Postions"   , binsP[i], true);
        mGraphDialog.addGraphPoint("Reprojected", binsR[i], true);
    }


    mGraphDialog.update();
}

