#pragma once
/**
 * \file treeSceneController.h
 *
 * \date Mar 1, 2013
 **/

#include <QtCore/QString>
#include <QtCore/QSharedPointer>

#include "3d/scene3D.h"
#include "transform3DSelector.h"
#include "coordinateFrame.h"


class TreeSceneModel;

class TreeSceneController :  public QObject
{
    Q_OBJECT

public:

    /**
     *  Scenes(in general not in this application) could form DAG, one scene could be a
     *  child of two or more Coordinate Frames. However controllers form the tree hierarchy
     **/
    TreeSceneModel                  *mTreeModel;

    TreeSceneController             *mParentController;
    vector<TreeSceneController *>    mChildren;

    QString                          mName;
    QSharedPointer<Scene3D>          mObject;

    Transform3DSelector             *mTransform3DWidget;
    ParametersControlWidgetBase     *mParametersWidget;

public slots:
    void paramtersChanged();
    void transformationChanged();
    void visibilityChanged(bool state);

    void clicked();


signals:
    void redrawRequest(void);

public:
    TreeSceneController(
        QString name,
        QSharedPointer<Scene3D> object,
        TreeSceneModel *treeModel,
        bool visible = true
    );

    void generateWidget();
    void replaceScene(QSharedPointer<Scene3D> frame);

    QString print(const QString &prefix = QString(""));
    TreeSceneController * addChildObject          ( QString name, QSharedPointer<Scene3D> object, TreeSceneModel *treeModel, bool visible = true );
    TreeSceneController * addChildObjectRecursive( QString name, QSharedPointer<Scene3D> object, TreeSceneModel *treeModel, bool visible = true );

template <class Visitor>
    void acceptCommon(Visitor &visitor)
    {
        if (mTransform3DWidget)
        {
            visitor.visit(*mTransform3DWidget, "transformation");
        }

        if (this->mObject.isNull())
        {
            return;
        }
        visitor.visit(this->mObject->visible, true, "isVisible");

        for (unsigned i = 0; i < mChildren.size(); i++)
        {
            visitor.visit(*(mChildren[i]), mChildren[i]->mName.toLatin1().data());
        }
    }

template <class Visitor>
    void accept(Visitor &visitor)
    {
        acceptCommon(visitor);
    }

    virtual ~TreeSceneController();
};

/* Well so far only QSetting is supported for saving objects */
template <>
void TreeSceneController::accept<SettingsSetter>(SettingsSetter &visitor);

template <>
void TreeSceneController::accept<SettingsGetter>(SettingsGetter &visitor);


/* The model of our controllers */
class TreeSceneModel: public QAbstractItemModel
{
    Q_OBJECT
signals:
    void modelChanged();

public:
    enum ColumnNameing {
        NAME_COLUMN,
        FLAG_COLUMN,
        PARAMETER_COLUMN,
        MAX_COLUMN
    };

    TreeSceneController *mTopItem;

    TreeSceneModel(QObject * parent = 0);
    ~TreeSceneModel();

    TreeSceneController * addObject(
            QString name,
            QSharedPointer<Scene3D> object,
            bool visible = true);

    Qt::ItemFlags flags  (const QModelIndex &index) const;
    QVariant data        (const QModelIndex &index, int role) const;
    bool     setData     (const QModelIndex &index, const QVariant &value, int role);
    int      rowCount    (const QModelIndex &parent) const;
    int      columnCount (const QModelIndex &parent) const;

    QVariant headerData(int section, Qt::Orientation orientation, int role ) const;

    /* Get model sub-index */
    QModelIndex index(int row, int column, const QModelIndex &parent ) const;
    QModelIndex parent(const QModelIndex &index) const;
    int         getRow(const QModelIndex &index) const;

    void packPath(QModelIndex index, QDataStream &stream) const;
    QModelIndex unpackPath(QDataStream &stream) const;

    QStringList mimeTypes() const;
    QMimeData *mimeData(const QModelIndexList &indices) const;

    bool dropMimeData(
        const QMimeData *data,
        Qt::DropAction action,
        int row,
        int column,
        const QModelIndex &parent);

    Qt::DropActions supportedDropActions() const ;

public slots:
    bool clicked(const QModelIndex &index);

private:

    TreeSceneController *indexToScene(const QModelIndex &index) const;
    const static QString MIME_TYPE;
};

/* EOF */
