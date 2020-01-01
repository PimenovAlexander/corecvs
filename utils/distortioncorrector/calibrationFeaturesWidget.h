#ifndef CALIBRATIONFEATURESWIDGET_H
#define CALIBRATIONFEATURESWIDGET_H

#include <QWidget>

#include "math/vector/vector2d.h"
#include "math/vector/vector3d.h"
#include "calibrationFeaturesWidget.h" //???
#include "alignment/selectableGeometryFeatures.h"

#include "uis/advancedImageWidget.h"
#include "widgets/observationListModel.h"

using corecvs::Vector2dd;
using corecvs::Vector3dd;
using corecvs::SelectableGeometryFeatures;
using corecvs::ObservationList;
using corecvs::PointObservation;



namespace Ui {
class CalibrationFeaturesWidget;
}

class CalibrationFeaturesWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CalibrationFeaturesWidget(QWidget *parent = 0);
    ~CalibrationFeaturesWidget();

    static const int REASONABLE_INF;

    enum {
        COLUMN_X,
        COLUMN_Y,
        COLUMN_Z,
        COLUMN_U,
        COLUMN_V
    };

    ObservationListModel        *mObservationListModel;
    SelectableGeometryFeatures  *mGeometryFeatures;

    void manualAddPoint(const Vector2dd &point);
    void setObservationModel(ObservationListModel *observationListModel);

    void savePoints();
    void loadPoints();

public slots:
    void addPoint();
    void removePoint();
    void deleteObservation();


    void updateWidget();

signals:
    void dataUpdated();

private:
    Ui::CalibrationFeaturesWidget *ui;
};



#endif // CALIBRATIONFEATURESWIDGET_H
