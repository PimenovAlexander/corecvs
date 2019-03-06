/**
 * \file treeSceneController.cpp
 *
 * \date Mar 1, 2013
 **/

#include "core/reflection/reflection.h"

#include "treeSceneController.h"

using corecvs::BaseReflectionStatic;



void TreeSceneController::paramtersChanged()
{
    // SYNC_PRINT(("TreeSceneController::paramtersChanged()\n"));

    if (mObject.isNull() || mParametersWidget == NULL)
        return;

    BaseReflectionStatic *params = mParametersWidget->createParametersVirtual();
    if (params != NULL)
        mObject->setParameters(params);

    delete_safe(params);
    emit redrawRequest();
}

void TreeSceneController::transformationChanged()
{
    if (mObject.isNull())
        return;

    mObject->transform = mTransform3DWidget->getTransform();
    emit redrawRequest();
}

void TreeSceneController::visibilityChanged(bool state)
{
    if (mObject.isNull())
        return;

    //qDebug () << mFrame->name << "is now:" << (state ? "visible" : "invisible");
    mObject->visible = state;
    emit redrawRequest();
}

void TreeSceneController::clicked()
{
    mTransform3DWidget->show();
    mTransform3DWidget->raise();

    if (mParametersWidget != NULL)
    {
        mParametersWidget->show();
        mParametersWidget->raise();
    }
}

TreeSceneController::TreeSceneController(
    QString name,
    QSharedPointer<Scene3D> object,
    TreeSceneModel *treeModel,
    bool visible
) :
    mTreeModel(treeModel),
    mParentController(NULL),
    mName(name),
    mObject(object),
    mParametersWidget(NULL)
{
    if (!mObject.isNull())
    {
       // qDebug() << "Setting name object name to: " << name;
        mObject->name = name;
    }
    mTransform3DWidget = new Transform3DSelector();
    mTransform3DWidget->setWindowTitle(QString("Transform: ") + name);

    generateWidget();

    connect(mTransform3DWidget, SIGNAL(parametersChanged()), this, SLOT(transformationChanged()));

    visibilityChanged(visible);

    connect(this, SIGNAL(redrawRequest()), mTreeModel, SIGNAL(modelChanged()));
}

void TreeSceneController::generateWidget()
{
    SYNC_PRINT(("TreeSceneController::generateWidget():called\n"));
    if (mObject.isNull())
        return;

    if (mParametersWidget != NULL)
        return;

    //qDebug("TreeSceneController::generateWidget() : Calling creating Control widget");
    mParametersWidget = mObject->getContolWidget();
    if (mParametersWidget == NULL)
        return;

    mParametersWidget->setWindowTitle(QString("Parameters for: ") + mName);
    connect(mParametersWidget,  SIGNAL(paramsChanged()),     this, SLOT(paramtersChanged()));
}

void TreeSceneController::replaceScene(QSharedPointer<Scene3D> newObject)
{
    SYNC_PRINT(("TreeSceneController::replaceScene():called\n"));

    if (newObject.isNull()) {
        SYNC_PRINT(("TreeSceneController::replaceScene():newObject is null\n"));
        return;
    }

    /* Rearrange mesh hierarchy according to the controller */
    if (mParentController != NULL && !mParentController->mObject.isNull())
    {
        if (!mParentController->mObject->isComplexObject())
        {
            qDebug("Seems like internal error\n");
            return;
        }

        QSharedPointer<Scene3D>        current = mObject;
        QSharedPointer<CoordinateFrame> parent = mParentController->mObject.staticCast<CoordinateFrame>();

        unsigned i = 0;
        for (; i < parent->mChildren.size(); i++)
        {
            if (parent->mChildren[i] == current)
            {
                parent->mChildren[i] = newObject;
                break;
            }
        }
        if (i >= parent->mChildren.size())
        {
            qDebug("Seems like internal error\n");
            return;
        }
    }

    SYNC_PRINT(("TreeSceneController::replaceScene(): name=%s, visible=%s\n", mName.toLatin1().constData(), mObject->visible ? "visible" : "hidden"));

    newObject->visible = mObject->visible;
    mObject = newObject;
    mObject->name = mName;
    generateWidget();

    paramtersChanged();
    transformationChanged();
}

QString TreeSceneController::print(const QString &prefix)
  {
    QString result;
    result += prefix + mName + " ";
    result += "(";
    if (!mObject.isNull())
    {
        result += mObject->name + ":" + (mObject->isComplexObject() ? "Frame" : "Object");
    } else {
        result += "null";
    }
    result += ")";
    result += "[" + (mParentController == NULL ? "null" : mParentController->mName) + "]";
    result += "\n";

    for (unsigned i = 0; i < mChildren.size(); i++)
    {
        result += mChildren[i]->print(prefix + "> ");
    }
    return result;
}

TreeSceneController * TreeSceneController::addChildObject(
    QString name, QSharedPointer<Scene3D> object, TreeSceneModel *treeModel, bool visible)
{
#if 0
    SYNC_PRINT(("TreeSceneController::addChildObject(%s, %s, %s, %s): called\n",
                name.toLatin1().constData(),
                object.isNull() ? "NULL" : "nonnull",
                treeModel == NULL ? "NULL" : "nonnull",
                visible ? "visible" : "invisible"));
#endif

    if (!mObject->isComplexObject())
    {
        SYNC_PRINT(("Not a complex object"));
        return NULL;
    }

    TreeSceneController *newController = new  TreeSceneController(name, object, treeModel, visible);
    mChildren.push_back(newController);
    newController->mParentController = this;

    QSharedPointer<CoordinateFrame> frame = mObject.staticCast<CoordinateFrame>();

    bool shouldAdd = true;
    for (unsigned i = 0; i < frame->mChildren.size(); i++ )
    {
        if (frame->mChildren[i] == newController->mObject)
            shouldAdd = false;
    }

    if (shouldAdd)
    {
        frame->mChildren.push_back(newController->mObject);
    }

    return newController;
}

TreeSceneController * TreeSceneController::addChildObjectRecursive(
    QString name, QSharedPointer<Scene3D> object, TreeSceneModel *treeModel, bool visible )
{
#if 0
    SYNC_PRINT(("TreeSceneModel::addChildObjectRecursive(%s, %s, %s, %s): called\n",
                name.toLatin1().constData(),
                object.isNull() ? "NULL" : "nonnull",
                treeModel == NULL ? "NULL" : "nonnull",
                visible ? "visible" : "invisible"));
#endif

    TreeSceneController *newController = addChildObject (name, object, treeModel, visible);
    if (newController == NULL) {
        SYNC_PRINT(("Returning null controller\n"));
        return NULL;
    }

    if (object->isComplexObject())
    {
        //qDebug("Breaking complex object and adding parts\n");
        QSharedPointer<CoordinateFrame> frame = object.staticCast<CoordinateFrame>();
        for (unsigned i = 0; i < frame->mChildren.size(); i++)
        {
            //qDebug("  Adding subobject\n");
            newController->addChildObjectRecursive(frame->mChildren[i]->name, frame->mChildren[i], treeModel, visible);
        }
    }

    //cout << print(QString("  ")).toStdString() << std::endl;

    return newController;
}

TreeSceneController::~TreeSceneController()
{
    delete_safe(mTransform3DWidget);
    delete_safe(mParametersWidget);

    for (unsigned i = 0; i < mChildren.size(); i++)
    {
       delete_safe(mChildren[i]);
    }
}

template <>
void TreeSceneController::accept<SettingsSetter>(SettingsSetter &visitor)
{
    acceptCommon(visitor);
    if (mParametersWidget != NULL)
    {
        WidgetSaver saver(&visitor);
        mParametersWidget->saveParamWidget(saver);
    }
}

template <>
void TreeSceneController::accept<SettingsGetter>(SettingsGetter &visitor)
{
    acceptCommon(visitor);
    if (mParametersWidget != NULL)
    {
        WidgetLoader loader(&visitor);
        mParametersWidget->loadParamWidget(loader);
    }
}


/* Model code */

const QString TreeSceneModel::MIME_TYPE = "application/x-vimouse-internal";

TreeSceneModel::TreeSceneModel(QObject * parent)
    : QAbstractItemModel(parent)
    , mTopItem(NULL)
{
    mTopItem = new TreeSceneController(
        "root",
        QSharedPointer<CoordinateFrame>(new CoordinateFrame()),
        this,
        true);
}

TreeSceneModel::~TreeSceneModel()
{
    delete_safe(mTopItem);
}

TreeSceneController * TreeSceneModel::addObject(
    QString name,
    QSharedPointer<Scene3D> object,
    bool visible)
{
#if 0
    SYNC_PRINT(("TreeSceneModel::addObject(%s, %s, %s): called\n",
                name.toLatin1().constData(),
                object.isNull() ? "NULL" : "nonnull",
                visible ? "visible" : "invisible"));
#endif

    if (mTopItem == NULL)
        return NULL;

    int toInsertId = (int)mTopItem->mChildren.size();
    //SYNC_PRINT(("TreeSceneModel::addObject(): Will insert at %d\n", toInsertId));

    //SYNC_PRINT(("Rowcount before: %d\n", this->rowCount(QModelIndex())));
    beginInsertRows(QModelIndex(), toInsertId, toInsertId);
    //TreeSceneController *result = mTopItem->addChildObject(name, object, visible);
    TreeSceneController *result = mTopItem->addChildObjectRecursive(name, object, this, visible);

    //SYNC_PRINT(("Rowcount end: %d\n", this->rowCount(QModelIndex())));
    endInsertRows();

    return result;
}

Qt::ItemFlags TreeSceneModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags flags = Qt::ItemIsEnabled | Qt::ItemIsSelectable ;

    if (index.column() == FLAG_COLUMN)
    {
        flags |= Qt::ItemIsUserCheckable | Qt::ItemIsEditable;
    }

    if (index.column() == NAME_COLUMN)
    {
        flags |= Qt::ItemIsDragEnabled;
        flags |= Qt::ItemIsDropEnabled;
        flags |= Qt::ItemIsEditable;
    }

    if (index == QModelIndex())
    {
        flags |= Qt::ItemIsDropEnabled;
    }

    return flags;
}

QVariant TreeSceneModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    TreeSceneController *scene = indexToScene(index);

    if (role == Qt::DisplayRole)
    {
        switch (index.column())
        {
        case NAME_COLUMN:
            return scene->mName;
        case FLAG_COLUMN:
            return scene->mObject->visible ? "ON" : "OFF";
        default:
            break;
        }
    }
    else if (role == Qt::ToolTipRole)
    {
        switch (index.column())
        {
        case NAME_COLUMN:
            return QString("The name of the object.");
        case FLAG_COLUMN:
            return QString("ON/OFF Flag");
        case PARAMETER_COLUMN:
            return QString("Parameters");
        default:
            break;
        }
    }
    else if ( role == Qt::CheckStateRole )
    {
        switch (index.column())
        {
            case FLAG_COLUMN:
            {
                if (scene->mObject) {
//                    qDebug("data called: returning %d", scene->mObject->visible);
                    return scene->mObject->visible ? Qt::Checked : Qt::Unchecked;
                }
            }
            default:
                break;
        }
    }
    else  if (role == Qt::DecorationRole)
    {
        if (index.column() == NAME_COLUMN)
        {
            if (scene->mObject->isComplexObject()) {
                return QIcon(":/new/prefix1/workspace.png");
            } else {
                return QIcon(":/new/our/our/3D.png");
            }
        }
        if (index.column() == PARAMETER_COLUMN)
        {
            return QIcon(":/new/prefix1/advanced.png");
        }
    }

    return QVariant();
}

bool TreeSceneModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid())
        return false;

//    qDebug("TreeSceneModel::setData((%d x %d), %d, %d)", index.row(), index.column(), value.toBool(), role);

    if (role == Qt::CheckStateRole)
    {
//        qDebug("   This was a check box");

        TreeSceneController *scene = indexToScene(index);
        Qt::CheckState checkState = static_cast<Qt::CheckState>(value.toInt());
        if (scene->mObject)
        {
            scene->mObject->visible = checkState;
//            qDebug("setData called: returning true");
            emit modelChanged();
            /* Well... this should be called */
            emit dataChanged(index, index);

            //qDebug() << scene->mObject->name << "(" << scene->mObject.data() << ")" << " is now:" << scene->mObject->visible;
            return true;
        }
    }

    qDebug("setData called: returning false");
    return false;
}

QVariant TreeSceneModel::headerData(int section, Qt::Orientation orientation, int role ) const
{
    if( role != Qt::DisplayRole || orientation != Qt::Horizontal )
        return QVariant();

    switch( section )
    {
    case 0:
        return QString( "Object" );
    case 1:
        return QString( "ON/OFF" );
    default:
        return QVariant();
    }
}

int TreeSceneModel::rowCount(const QModelIndex &parent) const
{
    TreeSceneController *parentObject = NULL;

    if (!parent.isValid()) {
        parentObject = mTopItem;
    } else {
        parentObject = indexToScene(parent);
    }

    if (parentObject == NULL)
        return 0;

    return (int)parentObject->mChildren.size();
}

int TreeSceneModel::columnCount(const QModelIndex &/*parent*/ ) const
{
    return MAX_COLUMN;
}

/* Get model sub-index */
QModelIndex TreeSceneModel::index(int row, int column, const QModelIndex &parent ) const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    TreeSceneController *parentController = (!parent.isValid() ? mTopItem : indexToScene(parent));

    if (row < (int)parentController->mChildren.size())
    {
        return createIndex( row, column, parentController->mChildren[row]);
    }

    return QModelIndex();
}


QModelIndex TreeSceneModel::parent(const QModelIndex &index) const
{
    if (!index.isValid() )
        return QModelIndex();

    TreeSceneController *indexObject = indexToScene(index);
    TreeSceneController *parentObject = indexObject->mParentController;

    if (parentObject == mTopItem)
        return QModelIndex();

    TreeSceneController *grandParentObject = parentObject->mParentController;

    unsigned int row = 0;
    for (; row < grandParentObject->mChildren.size(); row++ )
    {
        if (grandParentObject->mChildren[row] == parentObject)
            break;
    }

    return createIndex( row, 0, parentObject );
}

int TreeSceneModel::getRow(const QModelIndex &index) const
{
    return index.row();
}

/*Drag and drop support functions*/

QStringList TreeSceneModel::mimeTypes() const
{
    QStringList types;
    types << MIME_TYPE;
    return types;
}

/* Serialize and deserilize index */
void TreeSceneModel::packPath(QModelIndex index, QDataStream &stream) const
{
    vector<int> path;
    QModelIndex pathIter = index;
    while (pathIter != QModelIndex())
    {
        path.push_back(pathIter.row());
        pathIter = parent(pathIter);
    }

    for (unsigned i = 0; i < path.size(); i++)
    {
        stream << path[path.size() - 1 - i ];
    }
}

QModelIndex  TreeSceneModel::unpackPath(QDataStream &stream) const
{
    QModelIndex pathIter;
    qDebug() << "Got data:";
    QString prefix = ">";
    while (!stream.atEnd())
    {
        int origRow = 0;
        stream >> origRow;
        pathIter = index(origRow, 0, pathIter);
        qDebug() << prefix << "[" << origRow << "]";
        prefix += ">";
    }
    return pathIter;
}

QMimeData *TreeSceneModel::mimeData(const QModelIndexList &indices) const
{
    //qDebug() << "Packing object to mime";
    QMimeData *mimeData = new QMimeData();
    QByteArray encodedData;

    QDataStream stream(&encodedData, QIODevice::WriteOnly);

    if (indices.size() == 0)
        return NULL;

    packPath(indices[0], stream);
    mimeData->setData(MIME_TYPE, encodedData);
    return mimeData;
}

bool TreeSceneModel::dropMimeData(
        const QMimeData *data,
        Qt::DropAction action,
        int row,
        int column,
        const QModelIndex &target)
{
    if (action == Qt::IgnoreAction)
        return true;

    if (!data->hasFormat(MIME_TYPE))
        return false;

    QByteArray encodedData = data->data(MIME_TYPE);
    QDataStream stream(&encodedData, QIODevice::ReadOnly);

    QModelIndex pathIter = unpackPath(stream);
    if (pathIter == QModelIndex())
    {
        qDebug() << "Source path is root. Returning";
        return false;
    }

    /* Detaching old data */
    TreeSceneController *controller             = indexToScene(pathIter);
    QSharedPointer<Scene3D> scene               = controller->mObject;
    TreeSceneController *parentController       = controller->mParentController;
    QSharedPointer<CoordinateFrame> parentScene = parentController->mObject.staticCast<CoordinateFrame>();

    /* A Place to add new data */
    qDebug() << "Position to paste " << row << " " << column;

    TreeSceneController *targetController = indexToScene(target);
    if (targetController->mObject.isNull())
        return false;

    QModelIndex insertParent = target;
    if (!targetController->mObject->isComplexObject())
    {
        qDebug() << targetController->mName << " is not a frame. Going to parent";
        insertParent = parent(target);
        qDebug() << "Controller name:" << targetController->mName;
        row = getRow(target);
        qDebug() << "Getting new parent controller";
        targetController = indexToScene(insertParent);
    }

    QSharedPointer<CoordinateFrame> targetScene = targetController->mObject.staticCast<CoordinateFrame>();

    qDebug() << "Paste request to " << targetController->mName << " row=" << row << " column=" << column;

    /* Deleting link in scene */
    qDebug() << "Object to be moved:" << controller->mName;

    /* Deleting link in the controller */
    unsigned indexToDetach = 0;
    for (; indexToDetach < parentController->mChildren.size(); indexToDetach++)
    {
        if (parentController->mChildren[indexToDetach] == controller)
        {
            break;
        }
    }
    if (indexToDetach >= parentController->mChildren.size())
        return false;

    beginRemoveRows(parent(pathIter), indexToDetach, indexToDetach);
    parentController->mChildren.erase (parentController->mChildren.begin() + indexToDetach);

    for (unsigned i = 0; i < parentScene->mChildren.size(); i++)
    {
        if (parentScene->mChildren[i] == scene)
        {
            parentScene->mChildren.erase (parentScene->mChildren.begin() + i);
            break;
        }
    }
    endRemoveRows();

    /* Adding back */
    if (row == -1) {
        row = (int)targetController->mChildren.size();
    }
    qDebug() << "Adding child to:"          << targetController->mName;
    qDebug() << "Child name:" << controller->mName;

    beginInsertRows(insertParent, row, row);
    targetController->mChildren.insert(targetController->mChildren.begin() + row, controller);
    controller->mParentController = targetController;
    targetScene->mChildren.insert(targetScene->mChildren.begin() + row, scene);
    endInsertRows();

    qDebug() << "Finally tree looks like this\n" << mTopItem->print();

    return true;
}

Qt::DropActions TreeSceneModel::supportedDropActions() const
{
    return Qt::MoveAction;
}

bool TreeSceneModel::clicked(const QModelIndex &index)
{
    if (index.column() == PARAMETER_COLUMN)
    {
        TreeSceneController *object = indexToScene(index);
        object->clicked();
    }
    return true;
}

TreeSceneController *TreeSceneModel::indexToScene(const QModelIndex &index) const
{
    if (index != QModelIndex())
        return static_cast<TreeSceneController*>(index.internalPointer());

    return mTopItem;
}
