#ifndef CALIBRATIONFEATURESWIDGET_H
#define CALIBRATIONFEATURESWIDGET_H

#include <QWidget>
#include <QAbstractItemModel>

#include "vector2d.h"
#include "vector3d.h"
#include "calibrationFeaturesWidget.h"
#include "selectableGeometryFeatures.h"

#include "advancedImageWidget.h"

using corecvs::Vector2dd;
using corecvs::Vector3dd;
using corecvs::SelectableGeometryFeatures;
using corecvs::ObservationList;
using corecvs::PointObservation;



namespace Ui {
class CalibrationFeaturesWidget;
}

Q_DECLARE_METATYPE(ObservationList *);

class ObservationListModel : public QAbstractItemModel
{
    Q_OBJECT

signals:

public:
    enum {
        COLUMN_X,
        COLUMN_Y,
        COLUMN_Z,
        COLUMN_U,
        COLUMN_V,
        COLUMN_NUM
    };

    ObservationListModel(QObject *parent = NULL);

    virtual Qt::ItemFlags flags  (const QModelIndex &index) const;
    virtual QVariant       data  (const QModelIndex &index, int role = Qt::DisplayRole) const;
    virtual bool        setData  (const QModelIndex &index, const QVariant &value, int role);

    virtual int         rowCount    (const QModelIndex &parent = QModelIndex()) const;
    virtual int         columnCount (const QModelIndex &parent = QModelIndex()) const;

    virtual bool        insertRows(int row, int count, const QModelIndex &parent = QModelIndex());
    virtual bool        removeRows(int row, int count, const QModelIndex &parent = QModelIndex());

    virtual QVariant    headerData(int section, Qt::Orientation orientation, int role ) const;

    virtual QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
    virtual QModelIndex parent(const QModelIndex &index) const;

public:
    void    clearObservationPoints();
    void    setObservationList(ObservationList *observationList);

    /* Some additional methods */
    int     elementCount();

//private:
    ObservationList *mObservationList;
};


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

class PointListEditImageWidget : public AdvancedImageWidget {
   Q_OBJECT

public:
    enum PaintToolClass
    {
        MOVE_POINT_TOOL = TOOL_CLASS_LAST,
        ADD_POINT_TOOL,
        DEL_POINT_TOOL,
        PAINT_TOOL_CLASS_LAST
    };


   ObservationListModel *mObservationListModel;

   QToolButton *mAddButton;
   QToolButton *mMoveButton;
   QToolButton *mDeleteButton;
   QToolButton *mAddInfoButton;


   PointListEditImageWidget(QWidget *parent = NULL, bool showHeader = true);
   void setObservationModel(ObservationListModel *observationListModel);

   int mSelectedPoint;

   // AdvancedImageWidget interface
public slots:
   virtual void childRepaint(QPaintEvent *event, QWidget *who) override;
   virtual void toolButtonReleased(QWidget *button) override;
   virtual void childMousePressed(QMouseEvent *event) override;
   virtual void childMouseMoved(QMouseEvent *event) override;
   void invalidateModel();

protected:
   int findClosest(Vector2dd imagePoint, double limitDistance = numeric_limits<double>::max());
   Vector2dd getPointById(int row);
   QString   getMetaById (int row);
};

#endif // CALIBRATIONFEATURESWIDGET_H
