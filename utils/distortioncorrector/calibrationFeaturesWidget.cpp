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




/*void ObsevationListModel::clearObservationPoints()
{
    mObservationList->clear();
    emit mode
}*/

