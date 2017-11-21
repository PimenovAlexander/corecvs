#ifndef OBSERVATION_LIST_MODEL_H
#define OBSERVATION_LIST_MODEL_H

#include <QAbstractItemModel>

#include "core/alignment/selectableGeometryFeatures.h"
#include "core/camerafixture/fixtureScene.h"

using corecvs::Vector2dd;
using corecvs::Vector3dd;
using corecvs::SelectableGeometryFeatures;
using corecvs::ObservationList;
using corecvs::PointObservation;


Q_DECLARE_METATYPE(ObservationList *)




class ObservationListModel : public QAbstractItemModel
{
    Q_OBJECT

signals:

public:
    enum {
        COLUMN_X,
        COLUMN_DATA = COLUMN_X,
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
    void    dataTemporaryInvalid();
    void    dataIsValid();

    void    setObservationList(ObservationList *observationList);

    /* Some additional methods */
    int     elementCount();

//private:
    ObservationList *mObservationList;
};





#endif // OBSERVATION_LIST_MODEL_H
