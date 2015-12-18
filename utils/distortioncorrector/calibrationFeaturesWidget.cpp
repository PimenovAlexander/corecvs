#include <QDoubleSpinBox>
#include <QSettings>


#include "calibrationFeaturesWidget.h"
#include "ui_calibrationFeaturesWidget.h"

#include "qSettingsSetter.h"
#include "qSettingsGetter.h"

#include "painterHelpers.h"
#include "qtHelper.h"

using namespace corecvs;


const int CalibrationFeaturesWidget::REASONABLE_INF = 999999;

CalibrationFeaturesWidget::CalibrationFeaturesWidget(QWidget *parent) :
    QWidget(parent),
    mObservationListModel(NULL),
    mGeometryFeatures(NULL),
    ui(new Ui::CalibrationFeaturesWidget)
{
    ui->setupUi(this);

   /* ui->imageXSpinBox->setMaximum( REASONABLE_INF);
    ui->imageYSpinBox->setMaximum( REASONABLE_INF);
    ui->imageXSpinBox->setMinimum(-REASONABLE_INF);
    ui->imageYSpinBox->setMinimum(-REASONABLE_INF);


    ui->xCoordSpinBox->setMaximum( REASONABLE_INF);
    ui->yCoordSpinBox->setMaximum( REASONABLE_INF);
    ui->zCoordSpinBox->setMaximum( REASONABLE_INF);
    ui->xCoordSpinBox->setMinimum(-REASONABLE_INF);
    ui->yCoordSpinBox->setMinimum(-REASONABLE_INF);
    ui->zCoordSpinBox->setMinimum(-REASONABLE_INF);*/

    /*connect(ui->pointsTableWidget, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(choosePoint(int,int)));*/

    connect(ui->addButton   , SIGNAL(released()), this, SLOT(addPoint()));
    connect(ui->deleteButton, SIGNAL(released()), this, SLOT(removePoint()));

  /*  PointObservation P(Vector3dd(1.0, 2.0, 3.0),Vector2dd(4.0, 5.0));
    observationList.push_back(P);
    ObsevationListModel *model = new ObsevationListModel;
    model->mObservationList = &observationList;*/
    ui->tableView->setModel(mObservationListModel);       
}



CalibrationFeaturesWidget::~CalibrationFeaturesWidget()
{
    delete ui;
}

void CalibrationFeaturesWidget::manualAddPoint(const corecvs::Vector2dd &/*point*/)
{

}

void CalibrationFeaturesWidget::setObservationModel(ObservationListModel *observationListModel)
{
    mObservationListModel = observationListModel;
    ui->tableView->setModel(mObservationListModel);
}

void CalibrationFeaturesWidget::addPoint()
{
    mObservationListModel->insertRow(mObservationListModel->rowCount());
}

void CalibrationFeaturesWidget::removePoint()
{
    QItemSelectionModel *selection = ui->tableView->selectionModel();
    QModelIndex current = selection->currentIndex();

    if (current == QModelIndex()) {
        mObservationListModel->removeRow(mObservationListModel->rowCount() - 1);
        return;
    }

    mObservationListModel->removeRow(current.row());
}

void CalibrationFeaturesWidget::deleteObservation()
{
   /* QTableWidget *table = mUi->pointsTableWidget;
    Vector2dd value(mUi->imageXSpinBox->value(), mUi->imageYSpinBox->value());
    unsigned i = 0;
    while (i < mCorrectionMap.size())
    {
        if ((mCorrectionMap.at(i).projection - value).l2Metric() < EPSILON)
        {
            mCorrectionMap.erase(mCorrectionMap.begin() + i);
        } else {
            i ++;
        }
    }

    for (int i = 0; i < table->rowCount(); i ++)
    {
        if (table->item(i, 3)->text().toDouble() == value.x() &&
            table->item(i, 4)->text().toDouble() == value.y())
        {
            table->removeRow(i);
            break;
        }
    }
    // TODO: PaintFeature
    // ui->widget->deletePoint(QPointF(value.x(), value.y()));*/
}

void CalibrationFeaturesWidget::updateWidget()
{
    /* And tree */

    QTreeWidget *tree = ui->treeWidget;
    tree->clear();
    tree->setColumnCount(4);
    tree->header()->resizeSection(0, 200);
    tree->header()->resizeSection(1, 100);
    tree->header()->resizeSection(2, 100);

    if (mGeometryFeatures)
    {
        for (unsigned i = 0; i < mGeometryFeatures->mPaths.size(); i++)
        {
            SelectableGeometryFeatures::VertexPath *path = mGeometryFeatures->mPaths[i];
            QTreeWidgetItem *item = new QTreeWidgetItem(QStringList("Line"));
            tree->insertTopLevelItem(tree->topLevelItemCount(),item);
            for(unsigned j = 0; j < path->vertexes.size(); j++ )
            {
                SelectableGeometryFeatures::Vertex* vertex = path->vertexes[j];

                QTreeWidgetItem *subitem = new QTreeWidgetItem(QStringList("Vertex"));
                item->addChild(subitem);
                QDoubleSpinBox *xSpinBox = new QDoubleSpinBox();
                xSpinBox->setMaximum( REASONABLE_INF);
                xSpinBox->setMinimum(-REASONABLE_INF);
                xSpinBox->setValue(vertex->position.x());

                QDoubleSpinBox *ySpinBox = new QDoubleSpinBox();
                ySpinBox->setMaximum( REASONABLE_INF);
                ySpinBox->setMinimum(-REASONABLE_INF);
                ySpinBox->setValue(vertex->position.y());

                tree->setItemWidget(subitem, 1, xSpinBox);
                tree->setItemWidget(subitem, 2, ySpinBox);
            }
        }
    }
}
/*
void CalibrationFeaturesWidget::editPoint(const QPointF &prevPoint, const QPointF &newPoint)
{
    QTableWidget *table = mUi->pointsTableWidget;
    for (int i = 0; i < table->rowCount(); i ++)
    {
        QPointF tablePoint(table->item(i, 3)->text().toDouble(), table->item(i, 4)->text().toDouble());
        if ((prevPoint - tablePoint).manhattanLength() < EPSILON)
        {
            table->item(i, 3)->setText(QString::number(newPoint.x()));
            table->item(i, 4)->setText(QString::number(newPoint.y()));
            choosePoint(i, 0);
        }
    }
    Vector2dd prevVector(prevPoint.x(), prevPoint.y());
    Vector3dd spacePos(-1, -1, -1);
    for (unsigned i = 0; i < mCorrectionMap.size(); i ++)
    {
        if ((mCorrectionMap.at(i).projection - prevVector).l2Metric() < EPSILON)
        {
            spacePos = mCorrectionMap.at(i).point;
            mCorrectionMap.erase(mCorrectionMap.begin() + i);
            break;
        }
    }
    Vector2dd newVector(newPoint.x(), newPoint.y());
    mCorrectionMap.push_back(PointObservation(spacePos, newVector));
}
*/


/**
 *   Loading and saving of current points
 **/
void CalibrationFeaturesWidget::savePoints()
{
    ObservationList *list = mObservationListModel->mObservationList;
    SettingsSetter setter("distortionCorrection.conf");
    setter.visit(*list, "points");

    /*
    for (unsigned i = 0; i < observationList.size(); i ++)
    {
        PointObservation & observation = observationList.at(i);
        settings.setArrayIndex(i);
        settings.setValue("spacePoint_X", observation.x());
        settings.setValue("spacePoint_Y", observation.y());
        settings.setValue("spacePoint_Z", observation.z());
        settings.setValue("imagePoint_X", observation.u());
        settings.setValue("imagePoint_Y", observation.v());
    }*/

    /*vector<vector<Vector2dd> > straights = mUi->widget->getPaths();
    settings.beginWriteArray("lines");
    for (unsigned i = 0; i < straights.size(); i++) {
        settings.setArrayIndex(i);
        settings.beginWriteArray("subpoints");
        for (unsigned j = 0; j < straights[i].size(); j++) {
            settings.setArrayIndex(j);

            settings.setValue("point.x", straights[i][j].x());
            settings.setValue("point.y", straights[i][j].y());
        }
        settings.endArray();
    }
    settings.endArray();*/
}

void CalibrationFeaturesWidget::loadPoints()
{
#if 0
    QSettings settings("distortionCorrection.conf", QSettings::IniFormat);
    int size = settings.beginReadArray("points");
    for (int i = 0; i < size; i ++)
    {
        PointObservation point;
        settings.setArrayIndex(i);
        point.x() = settings.value("spacePoint_X").toDouble();
        point.y() = settings.value("spacePoint_Y").toDouble();
        point.z() = settings.value("spacePoint_Z").toDouble();
        point.u() = settings.value("imagePoint_X").toDouble();
        point.v() = settings.value("imagePoint_Y").toDouble();
        addObservation(point);
    }
    settings.endArray();

    updateWidget();
#endif

    /* TODO: Make an interface for this */
    /*PaintImageWidget *canvas = mUi->widget;

    int lines = settings.beginReadArray("lines");
    for (int i = 0; i < lines; i++) {
        canvas->mPaths.append(PaintImageWidget::VertexPath());
        PaintImageWidget::VertexPath &path = canvas->mPaths.last();

        settings.setArrayIndex(i);
        int subpoints = settings.beginReadArray("subpoints");
        for (int j = 0; j < subpoints; j++) {
            settings.setArrayIndex(j);
            Vector2dd pos;
            pos.x() = settings.value("point.x").toDouble();
            pos.y() = settings.value("point.y").toDouble();
            canvas->addVertex(pos);
            canvas->addVertexToPath(&canvas->mPoints.last(), &path);
        }
        settings.endArray();
    }
    settings.endArray();*/
}


/* Model */

ObservationListModel::ObservationListModel(QObject *parent) :
    QAbstractItemModel(parent),
    mObservationList(NULL)
{}

Qt::ItemFlags ObservationListModel::flags(const QModelIndex & /*index*/) const
{
    Qt::ItemFlags flags = Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
    return flags;
}

QVariant ObservationListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || mObservationList == NULL)
    {
        return QVariant();
    }

    if ((int)mObservationList->size() <= index.row())
    {
        return QVariant();
    }

    PointObservation &P = mObservationList->at(index.row());

    if (role == Qt::DisplayRole)
    {
        switch (index.column())
        {
        case COLUMN_X: return QString::number(P.x());
        case COLUMN_Y: return QString::number(P.y());
        case COLUMN_Z: return QString::number(P.z());
        case COLUMN_U: return QString::number(P.u());
        case COLUMN_V: return QString::number(P.v());
        default:
            break;
        }
    }

    if (role == Qt::EditRole)
    {
        switch (index.column())
        {
        case COLUMN_X: return P.x();
        case COLUMN_Y: return P.y();
        case COLUMN_Z: return P.z();
        case COLUMN_U: return P.u();
        case COLUMN_V: return P.v();
        default:
            break;
        }
    }

    if (role == Qt::ToolTipRole)
    {
        switch (index.column())
        {
        case COLUMN_X: return QString( "Space X" );
        case COLUMN_Y: return QString( "Space Y" );
        case COLUMN_Z: return QString( "Space Z" );
        case COLUMN_U: return QString( "Image X" );
        case COLUMN_V: return QString( "Image Y" );
        default:
            break;
        }
    }

    return QVariant();
}

bool ObservationListModel::setData(const QModelIndex &index, const QVariant &value, int /*role*/)
{
    if (!index.isValid())
    {
        return false;
    }
    PointObservation &P = mObservationList->at(index.row());

    bool ok = false;
    double newValue = value.toDouble(&ok);
    if (!ok) {
        return false;
    }

    switch (index.column())
    {
         case COLUMN_X: P.x() = newValue; break;
         case COLUMN_Y: P.y() = newValue; break;
         case COLUMN_Z: P.z() = newValue; break;
         case COLUMN_U: P.u() = newValue; break;
         case COLUMN_V: P.v() = newValue; break;
         default:
             return false;
    }

    emit dataChanged(index, index);
    return true;
}

int ObservationListModel::rowCount(const QModelIndex &/*parent*/) const
{
    return (mObservationList == NULL) ? 0 : (int)mObservationList->size();
}

int ObservationListModel::columnCount(const QModelIndex &/*parent*/) const
{
    return COLUMN_NUM;
}

bool ObservationListModel::insertRows(int row, int count, const QModelIndex &parent)
{
    if (mObservationList == NULL) {
        return false;
    }

    emit beginInsertRows(parent, row, row + count - 1);
    mObservationList->insert(mObservationList->begin() + row, count, PointObservation());
    emit endInsertRows();
    // FIXME: I just do not know what happens here, but without return statement
    //        it does not builds on windows
    return false;
}

bool ObservationListModel::removeRows(int row, int count, const QModelIndex &parent)
{
    if (mObservationList == NULL) {
        return false;
    }

    emit beginRemoveRows(parent, row, row + count - 1);
    mObservationList->erase(mObservationList->begin() + row, mObservationList->begin() + row + count);
    emit endRemoveRows();
    // FIXME: I just do not know what happens here, but without return statement
    //        it does not builds on windows
    return false;
}

QVariant ObservationListModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if( role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal )
    {
        switch( section )
        {
        case COLUMN_X:
            return QString( "Space X" );
        case COLUMN_Y:
            return QString( "Space Y" );
        case COLUMN_Z:
            return QString( "Space Z" );

        case COLUMN_U:
            return QString( "Image X" );
        case COLUMN_V:
            return QString( "Image Y" );

        default:
            return QVariant();
        }
    }

    if (orientation == Qt::Vertical )
    {
        return QString::number(section + 1);
    }

    return QVariant();
}

QModelIndex ObservationListModel::index(int row, int column, const QModelIndex &/*parent*/) const
{
    if (mObservationList == NULL) {
        return QModelIndex();
    }

    if (row < (int)mObservationList->size() && column < COLUMN_NUM) {
        return createIndex(row, column);
    }
    return QModelIndex();
}

QModelIndex ObservationListModel::parent(const QModelIndex &/*index*/) const
{
    return QModelIndex();
}

void ObservationListModel::clearObservationPoints()
{
    emit beginResetModel();
    mObservationList->clear();
    emit endResetModel();
}

void ObservationListModel::setObservationList(ObservationList *observationList)
{
    emit beginResetModel();
    mObservationList = observationList;
    emit endResetModel();
}

int ObservationListModel::elementCount()
{
    if (mObservationList == NULL)
        return 0;

    return (int)mObservationList->size();
}

/*void ObsevationListModel::clearObservationPoints()
{
    mObservationList->clear();
    emit mode
}*/

PointListEditImageWidget::PointListEditImageWidget(QWidget *parent, bool showHeader) :
    AdvancedImageWidget(parent, showHeader),
    mObservationListModel(NULL),
    mSelectedPoint(-1)
{
    mMoveButton    = addToolButton("Select",            QIcon(":/new/prefix1/select_by_color.png"));
    mAddButton     = addToolButton("Add Point To Path", QIcon(":/new/prefix1/vector_add.png"     ), false);
    mDeleteButton  = addToolButton("Delete Point",      QIcon(":/new/prefix1/vector_delete.png"  ), false);
    mAddInfoButton = addToolButton("Toggle info",       QIcon(":/new/prefix1/info_rhombus.png"   ), false);
    mAddInfoButton ->setCheckable(true);

}

void PointListEditImageWidget::setObservationModel(ObservationListModel *observationListModel)
{
    disconnect(mObservationListModel, 0, this, 0);

    mObservationListModel = observationListModel;

    connect(mObservationListModel, SIGNAL(dataChanged(QModelIndex,QModelIndex,QVector<int>)),
            this, SLOT(update()));

    connect(mObservationListModel, SIGNAL(columnsInserted(QModelIndex,int,int))             , this, SLOT(invalidateModel()));
    connect(mObservationListModel, SIGNAL(columnsMoved(QModelIndex,int,int,QModelIndex,int)), this, SLOT(invalidateModel()));
    connect(mObservationListModel, SIGNAL(columnsRemoved(QModelIndex,int,int))              , this, SLOT(invalidateModel()));

    connect(mObservationListModel, SIGNAL(rowsInserted(QModelIndex,int,int))             , this, SLOT(invalidateModel()));
    connect(mObservationListModel, SIGNAL(rowsMoved(QModelIndex,int,int,QModelIndex,int)), this, SLOT(invalidateModel()));
    connect(mObservationListModel, SIGNAL(rowsRemoved(QModelIndex,int,int))              , this, SLOT(invalidateModel()));

    connect(mObservationListModel, SIGNAL(modelReset())                                     , this, SLOT(invalidateModel()));
}

/* This is called when model indexes are changed, and our cache is no longer valid */
void PointListEditImageWidget::invalidateModel()
{
    mSelectedPoint = -1;
    update();
}

void PointListEditImageWidget::childRepaint(QPaintEvent *event, QWidget *who)
{
    AdvancedImageWidget::childRepaint(event, who);
    if (mImage.isNull())
    {
        return;
    }

    if (mObservationListModel == NULL)
    {
        return;
    }

    /* Now the points */
    QPainter painter(who);

    QModelIndex topLevel = mObservationListModel->parent(QModelIndex());



    int rows = mObservationListModel->rowCount(topLevel);

    for (int i = 0; i < rows; i ++)
    {
        Vector2dd point = getPointById(i);
        Vector2dd imageCoords = imageToWidgetF(point);
        painter.setPen(Qt::yellow);
        drawCircle(painter, imageCoords, 5);
        painter.setPen(Qt::blue);
        drawCircle(painter, imageCoords, 10);

        if (mAddInfoButton->isChecked())
        {
            QString meta = getMetaById(i);
            QPointF pos = Core2Qt::QPointFromVector2dd(imageCoords + Vector2dd(5, -10));
            painter.setPen(Qt::black);
            painter.drawText(pos, meta);
            pos += QPointF(1,1);
            painter.setPen(Qt::white);
            painter.drawText(pos, meta);
        }

        if (i == mSelectedPoint) {
            painter.setPen(Qt::red);
            drawCircle(painter, imageCoords, 7);

            /* Test it a bit and use QSelectionModel */
            imageCoords = imageToWidgetF(widgetToImageF(imageCoords));
            painter.setPen(Qt::cyan);
            drawCircle(painter, imageCoords, 3);
        }
    }
}


void PointListEditImageWidget::toolButtonReleased(QWidget *button)
{

    mUi->widget->unsetCursor();
    AdvancedImageWidget::toolButtonReleased(button);

    if (button == mMoveButton)
    {
        qDebug() << "Move Button";
        mCurrentToolClass = (ToolClass)MOVE_POINT_TOOL;
        mUi->widget->setCursor(Qt::ArrowCursor);
    }
    else if (button == mAddButton)
    {
        qDebug() << "Add Button";
        //mCurrentToolClass = (ToolClass)ADD_POINT_TOOL;
        mObservationListModel->insertRow(mObservationListModel->rowCount());
        mSelectedPoint = mObservationListModel->rowCount() - 1;

        QModelIndex indexX = mObservationListModel->index(mSelectedPoint, ObservationListModel::COLUMN_U);
        QModelIndex indexY = mObservationListModel->index(mSelectedPoint, ObservationListModel::COLUMN_V);

        mObservationListModel->setData(indexY, QVariant(mZoomCenter.y()), Qt::EditRole);
        mObservationListModel->setData(indexX, QVariant(mZoomCenter.x()), Qt::EditRole);
        mUi->widget->update();
    }
    else if (button == mDeleteButton)
    {
        qDebug() << "Delete Button";
        if (mSelectedPoint >= 0)
        {
            mObservationListModel->removeRow(mSelectedPoint);
            mSelectedPoint = -1;
        }
        mUi->widget->update();
    } else if (button == mAddInfoButton)
    {
        mUi->widget->update();
    }
}

Vector2dd PointListEditImageWidget::getPointById(int row)
{
    QModelIndex topLevel = mObservationListModel->parent(QModelIndex());
    QModelIndex indexX = mObservationListModel->index(row, ObservationListModel::COLUMN_U, topLevel);
    QModelIndex indexY = mObservationListModel->index(row, ObservationListModel::COLUMN_V, topLevel);

    double x = mObservationListModel->data(indexX).toDouble();
    double y = mObservationListModel->data(indexY).toDouble();

    return Vector2dd(x,y);
}

QString PointListEditImageWidget::getMetaById(int row)
{
    QModelIndex topLevel = mObservationListModel->parent(QModelIndex());
    QModelIndex indexX = mObservationListModel->index(row, ObservationListModel::COLUMN_X, topLevel);
    QModelIndex indexY = mObservationListModel->index(row, ObservationListModel::COLUMN_Y, topLevel);
    QModelIndex indexZ = mObservationListModel->index(row, ObservationListModel::COLUMN_Z, topLevel);


    double x = mObservationListModel->data(indexX).toDouble();
    double y = mObservationListModel->data(indexY).toDouble();
    double z = mObservationListModel->data(indexZ).toDouble();

    return QString("%1 [%2 %3 %4]").arg(row + 1).arg(x).arg(y).arg(z);
}

int PointListEditImageWidget::findClosest(Vector2dd imagePoint, double limitDistance )
{
    QModelIndex topLevel = mObservationListModel->parent(QModelIndex());
    int rows = mObservationListModel->rowCount(topLevel);

    double bestDistance = limitDistance;
    int bestIndex = -1;

    for (int i = 0; i < rows; i ++)
    {
        Vector2dd currentPoint = getPointById(i);
        double dist = (imagePoint - currentPoint).l2Metric();
        if (dist < bestDistance) {
            bestDistance = dist;
            bestIndex = i;
        }
    }

    return bestIndex;
}

void PointListEditImageWidget::childMousePressed(QMouseEvent *event)
{
    AdvancedImageWidget::childMousePressed(event);

    PaintToolClass tool = (PaintToolClass)mCurrentToolClass;

    if (tool == ADD_POINT_TOOL)
    {
    }

    if (tool == DEL_POINT_TOOL)
    {
    }

    if (tool == MOVE_POINT_TOOL)
    {

        Vector2dd releasePoint = Vector2dd(event->x(), event->y());
//        bool shiftPressed = event->modifiers().testFlag(Qt::ShiftModifier);

        Vector2dd imagePoint = widgetToImageF(releasePoint);
        mSelectedPoint = findClosest(imagePoint, 5);
        mUi->widget->update();

    }
}

void PointListEditImageWidget::childMouseMoved(QMouseEvent * event)
{
    QModelIndex topLevel = mObservationListModel->parent(QModelIndex());

    switch (mCurrentToolClass)
    {
    case MOVE_POINT_TOOL:
    {
        //   qDebug() << "Drag in selected tool";
        if (!mIsMousePressed)
            break;

        Vector2dd dragStart    = widgetToImageF(Qt2Core::Vector2ddFromQPoint(mSelectionEnd));
        Vector2dd currentPoint = widgetToImageF(Qt2Core::Vector2ddFromQPoint(event->pos()));
        Vector2dd shift = (dragStart - currentPoint);


       /* for (unsigned i = 0; i < mFeatures.mSelectedPoints.size(); i++)
        {
            mFeatures.mSelectedPoints[i]->position -= shift;
        }*/
        if (mSelectedPoint >= 0) {
            Vector2dd currentPoint = getPointById(mSelectedPoint);

            QModelIndex indexX = mObservationListModel->index(mSelectedPoint, ObservationListModel::COLUMN_U, topLevel);
            QModelIndex indexY = mObservationListModel->index(mSelectedPoint, ObservationListModel::COLUMN_V, topLevel);

            currentPoint -= shift;
            mObservationListModel->setData(indexY, QVariant(currentPoint.y()), Qt::EditRole);
            mObservationListModel->setData(indexX, QVariant(currentPoint.x()), Qt::EditRole);
        }


        mUi->widget->update();
    }
    default:
        break;
    }
    AdvancedImageWidget::childMouseMoved(event);
}
