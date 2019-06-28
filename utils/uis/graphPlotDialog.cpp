#include <QPainter>
#include <QFileDialog>
#include <QtXml/QDomElement>

#include <fstream>

#include "core/utils/global.h"

#include "graphPlotDialog.h"
#include "core/math/mathUtils.h"


using namespace corecvs;
using std::fstream;

GraphPlotDialog::GraphPlotDialog(QWidget *parent)
        : ViAreaWidget(parent)
{
    mUi.setupUi(this);

    connect(mUi.loadButton,      SIGNAL(released()),     this, SLOT(loadSettingsFromFile()));
    connect(mUi.saveButton,      SIGNAL(released()),     this, SLOT(saveSettingsToFile()));
    connect(mUi.resetButton,     SIGNAL(released()),     this, SLOT(resetSettings()));
    connect(mUi.exportCSVButton, SIGNAL(released()),     this, SLOT(exportToCSV()));

    connect(mUi.widget,          SIGNAL(notifyParentMouseRelease(QMouseEvent*)),
            this,               SLOT(setSnapshotValue(QMouseEvent*)));

    connect(mUi.visibleGraph,    SIGNAL(currentItemChanged(QTableWidgetItem*,QTableWidgetItem*)),
            this,               SLOT(selectGraph(QTableWidgetItem*,QTableWidgetItem*)));
    connect(mUi.visibleGraph,    SIGNAL(itemDoubleClicked(QTableWidgetItem*)),
            this,               SLOT(editGraphName(QTableWidgetItem*)));
    connect(mUi.visibleGraph,    SIGNAL(itemChanged(QTableWidgetItem*)),
            this,               SLOT(saveGraphName(QTableWidgetItem*)));

    connect(mUi.invisibleGraph,  SIGNAL(currentItemChanged(QTableWidgetItem*,QTableWidgetItem*)),
            this,               SLOT(closePrevItemEditor(QTableWidgetItem*,QTableWidgetItem*)));
    connect(mUi.invisibleGraph,  SIGNAL(itemDoubleClicked(QTableWidgetItem*)),
            this,               SLOT(editGraphName(QTableWidgetItem*)));
    connect(mUi.invisibleGraph,  SIGNAL(itemChanged(QTableWidgetItem*)),
            this,               SLOT(saveGraphName(QTableWidgetItem*)));

    connect(mUi.widget, SIGNAL(askParentRepaint(QPaintEvent *, QWidget *)),
                              this, SLOT(childRepaint(QPaintEvent *, QWidget *)));

    mUpDownMapper = new QSignalMapper(this);
    connect(mUi.up, SIGNAL(released()), mUpDownMapper, SLOT(map()));
    mUpDownMapper->setMapping(mUi.up, 0);
    connect(mUi.down, SIGNAL(released()), mUpDownMapper, SLOT(map()));
    mUpDownMapper->setMapping(mUi.down, 1);
    connect(mUpDownMapper, SIGNAL(mapped(int)), this, SLOT(setGraphVisibility(int)));

    connect(mUi.parametersWidget, SIGNAL(paramsChanged()),
             this,                SLOT(graphPlotParametersChanged()));

    mGraphPlotParameters = QSharedPointer<GraphPlotParameters>(mUi.parametersWidget->createParameters());

    //Set default params to trace graph values
    mMouseClickedValue =  mUi.widget->width() / 2;
    mGraphShowPoint = -1;
}

QColor GraphPlotDialog::mGraphPallete[] = { Qt::red, Qt::darkGreen, Qt::cyan, Qt::darkRed, Qt::darkBlue, Qt::darkMagenta };


void GraphPlotDialog::drawGrid(QPainter &painter)
{
    unsigned i;
    unsigned lineNumber;
    unsigned w = mUi.widget->width();
    unsigned h = mUi.widget->height();

    QPen gridPen(Qt::DashDotLine);
    QPen subgridPen (Qt::DotLine);
    subgridPen.setColor(Qt::darkGreen);
    gridPen.setColor(Qt::green);

    if (mGraphPlotParameters->xGrid()) {
        double gainX = mGraphPlotParameters->xScale();

        while (gainX > 100) {
            gainX /= 10;
        }

        while (gainX < 10) {
            gainX *= 10;
        }
        unsigned stepX = gainX * 2;

        lineNumber = w / stepX;
        painter.setPen(gridPen);
        for (i = 0; i < lineNumber; i++) {
            if (stepX > 50) {
                painter.setPen(subgridPen);
                painter.drawLine(i * stepX  + stepX / 2, 0, i * stepX + stepX / 2, h - 1);
                painter.setPen(gridPen);
            }
            painter.drawLine((i + 1) * stepX, 0, (i + 1) * stepX, h - 1);
        }
    }
    if (mGraphPlotParameters->yGrid() && mGraphPlotParameters->yScale() > 0.005) {
        double gainY = mGraphPlotParameters->yScale();

        while (gainY > 100) {
            gainY /= 10;
        }

        while (gainY < 10) {
            gainY *= 10;
        }
        unsigned stepY = gainY * 2;

        lineNumber = w / stepY;
        painter.setPen(gridPen);
        for (i = 0; i < h / 2; i += stepY) {
            if (stepY > 50) {
                painter.setPen(subgridPen);
                painter.drawLine(0, h / 2 + i + stepY / 2, w - 1, h / 2 + i + stepY / 2);
                painter.drawLine(0, h / 2 - i - stepY / 2, w - 1, h / 2 - i - stepY / 2);
                painter.setPen(gridPen);
            }

            painter.drawLine(0, h / 2 + i + stepY, w - 1, h / 2 + i + stepY);
            painter.drawLine(0, h / 2 - i - stepY, w - 1, h / 2 - i - stepY);
        }
    }
}

void GraphPlotDialog::childRepaint(QPaintEvent * /*event*/, QWidget *who)
{
    if (!who)
        return;

    QPainter painter(who);
    unsigned h = who->height();
    unsigned w = who->width();

    painter.setPen(Qt::gray);
    painter.setBrush(Qt::darkGray);
    painter.drawRect(0, 0, w - 1, h - 1);

    int zero = h / 2;

    /* Draw oscilloscope
     *
     */
    double offset = mGraphPlotParameters->centerAt();
    double gainY = mGraphPlotParameters->yScale();
    double gainX = mGraphPlotParameters->xScale();

#if 0

    double oscContrast = ui.oscContrastSpinBox->value();
    deque<vector<double> >::iterator it;

    unsigned i = 0;

    for (it = intensity.begin(); it != intensity.end() && i < w; ++it, i++)
    {
        vector<int> line(h);

        const vector<double> &slice = *it;
        for (unsigned j = 0; j < slice.size(); j++)
        {
            double value = slice[j];
            double toShow = (value - offset) * gainY;
            unsigned bin = fround(zero - toShow);
            if (bin >= h)
                continue;
            line[bin]++;
        }

        for (unsigned j = 0; j < h; j++)
        {
            int val = line[j];
            val *= oscContrast;
            if (val > 255)
                val = 255;
            QPen pen;
            pen.setColor(QColor(0, val, val));
            painter.setPen(pen);
            painter.drawPoint(i, j);
        }
    }
#endif
    drawGrid(painter);
    drawGridFixLine(painter);

    QPen axisPen(Qt::DashLine);
    axisPen.setColor(Qt::blue);
    painter.setPen(axisPen);
    painter.drawLine(0, zero, w - 1, zero);

    int style = mGraphPlotParameters->graphStyle();


    for (int visId = 0; visId < mUi.visibleGraph->rowCount(); visId++) {
        QTableWidgetItem * item = mUi.visibleGraph->item(visId, 0);
        unsigned graphId = item->data(Qt::UserRole).toUInt();
        GraphHistory &graph = mData[graphId];
        QPen pen;
        pen.setColor(item->foreground().color());
        if (mData[graphId].isSelected && mGraphPlotParameters->selectGraph()) {
            pen.setWidth(mGraphPlotParameters->width() + 2);
        }
        else {
            pen.setWidth(mGraphPlotParameters->width());
        }
        painter.setPen(pen);

        unsigned i = 0;
        double x = 0;

        switch (style)
        {
        case 0:
            for (; i < graph.size() - 1 && x < w - 1; i++, x += gainX) {
                if (!graph[i].isValid || !graph[i + 1].isValid)
                    continue;

                double value1 = graph[i].value;
                double value2 = graph[i + 1].value;
                double toShow1 = (value1 - offset) * gainY;
                double toShow2 = (value2 - offset) * gainY;
                painter.drawLine(fround(x), zero - toShow1, fround(x + gainX), zero - toShow2);
            }
            break;
        case 1:
            for (; i < graph.size() && x < w - 1; i++, x += gainX) {
                if (!graph[i].isValid)
                    continue;

                double value = graph[i].value;
                double toShow = (value - offset) * gainY;
                painter.drawPoint(fround(x), zero - toShow);
            }
        }
    }

    // Calculate values for paused widget
    if (mUi.pauseButton->isChecked())
    {
        int dataSize0 = mData.size() ? (int)mData[0].size() : 0;
        if (mGraphPlotParameters->fixGridValue())
        {
            mGraphShowPoint = fround(mMouseClickedValue / gainX);
            if (mGraphShowPoint >= dataSize0) {
                mGraphShowPoint = dataSize0 - mGraphShowPoint - 1;
            }
        }
        else
        {
            if (mGraphShowPoint < 0) {
                mMouseClickedValue = fround((dataSize0 - mGraphShowPoint - 1) * gainX);
            }
            else {
                mMouseClickedValue = fround(mGraphShowPoint * gainX);
            }

            if (mMouseClickedValue >= mUi.widget->width()) {
                mMouseClickedValue = -1;
            }
        }
    }
    showGraphsValue(mUi.visibleGraph);
    showGraphsValue(mUi.invisibleGraph);
}

void GraphPlotDialog::exportToCSV()
{
    unsigned w = mUi.widget->width();

    printf("Export To CSV slot called...\n");
    char name[100];
    snprintf2buf(name, "exported.csv");

    printf("Dumping current graph to <%s>...", name);
    fstream file(name, fstream::out);

    double gainX = mGraphPlotParameters->xScale();

    for (unsigned graphId = 0; graphId < mData.size(); graphId++)
    {
        GraphHistory &graph = mData[graphId];
        GraphHistory::iterator it;
        double x = 0;
        for (it = graph.begin(); it != graph.end() && x < w - 1; ++it, x += gainX)
        {
            if ((*it).isValid) {
                file << (*it).value;
            }
            file << ",";
        }
        file << std::endl;
    }

    file.close();
    printf("done\n");

}

void GraphPlotDialog::setGraphVisibility(int currentVisibility)
{
    QTableWidget *currentList = NULL;
    QTableWidget *endList = NULL;
    if (currentVisibility == 1) {
        currentList = mUi.visibleGraph;
        endList = mUi.invisibleGraph;
    }
    else if (currentVisibility == 0) {
        currentList = mUi.invisibleGraph;
        endList = mUi.visibleGraph;
    }
    else {
        return;
    }
    QTableWidgetItem *item = currentList->takeItem(currentList->currentRow(), 0);
    QTableWidgetItem *valueItem = currentList->takeItem(currentList->currentRow(), 1);

    if (!item || !valueItem) {
        return;
    }
    currentList->removeRow(currentList->currentRow());
    unsigned graphId = item->data(Qt::UserRole).toUInt();
    mData[graphId].isSelected = false;
    int rowCount = endList->rowCount();
    endList->setRowCount(rowCount + 1);
    endList->setItem(rowCount, 0, item);
    endList->setItem(rowCount, 1, valueItem);
}

void GraphPlotDialog::selectGraph(QTableWidgetItem* current,QTableWidgetItem* prev)
{
    if (prev != NULL) {
        unsigned graphId = prev->data(Qt::UserRole).toUInt();
        mData[graphId].isSelected = false;
        mUi.visibleGraph->closePersistentEditor(prev);
    }
    if (current != NULL) {
        unsigned graphId = current->data(Qt::UserRole).toUInt();
        mData[graphId].isSelected = true;
    }
}

void GraphPlotDialog::editGraphName(QTableWidgetItem*item)
{
    QTableWidget *graphList = (QTableWidget*)sender();
    if (item != NULL && graphList->column(item) == 0) {
        QTableWidget *graphList = (QTableWidget*)sender();
        graphList->openPersistentEditor(item);
    }
}
void GraphPlotDialog::saveGraphName(QTableWidgetItem*item)
{
    QTableWidget *graphList = (QTableWidget*)sender();
    if (item != NULL && graphList->column(item) == 0) {
        unsigned graphId = item->data(Qt::UserRole).toUInt();
        mData[graphId].name = item->text().toStdString();
        graphList->closePersistentEditor(item);
    }
}

void GraphPlotDialog::addGraphPoint(unsigned graphId, double value, bool isValid)
{
//    qDebug("GraphPlotDialog::addGraphPoint(%d, %lf, %s): called", graphId, value, isValid ? "valid" : "invalid");

    if (mUi.pauseButton->isChecked())
        return;

    if (graphId >= mData.size()) {
        // qDebug("GraphPlotDialog::addGraphPoint(): adding new graph");

        mData.resize(graphId + 1);
        mData[graphId].isSelected = false;
        mData[graphId].name = QString("Graph %2").arg(graphId).toStdString();

        addNewGraphToUI(graphId);
    }

/*
    qDebug("GraphPlotDialog::addGraphPoint(): adding point to graph %d(%d) : %s ", graphId, mData.size(), mData[graphId].name.toLatin1().constData());
    for (int i = 0; i < mData.size(); i++)
    {
        qDebug("GraphPlotDialog::addGraphPoint(): %d %s", i, mData[i].name.toLatin1().constData());
    }
*/

    mData[graphId].push_front(GraphValue(value, isValid));
    while (mData[graphId].size() > 2000) {
        mData[graphId].pop_back();
    }

    if (graphId == 0)
    {
        double gainX = mGraphPlotParameters->xScale();
        if (mGraphPlotParameters->fixGridValue())
        {
            mGraphShowPoint = fround(double(mMouseClickedValue) / gainX);
            if (mGraphShowPoint >= (int)mData[0].size()) {
                mGraphShowPoint = (int)(mData[0].size() - mGraphShowPoint - 1);
            }
        }
        else
        {
            if (mGraphShowPoint < 0) {
                mMouseClickedValue = fround(double(mData[0].size() - mGraphShowPoint - 1) * gainX);
            }
            else {
                mGraphShowPoint++;
                mMouseClickedValue = fround(double(mGraphShowPoint) * gainX);
            }

            if (mMouseClickedValue >= mUi.widget->width()) {
                mGraphShowPoint = 0;
                mMouseClickedValue = 0;
            }
        }
    }
}
void GraphPlotDialog::addGraphPoint(QString graphName, double value, bool isValid)
{
//    qDebug("GraphPlotDialog::addGraphPoint(%s, %lf, %s): called", graphName.toLatin1().constData(), value, isValid ? "valid" : "invalid");

    if (mUi.pauseButton->isChecked())
        return;
    unsigned graphId = getGraphId(graphName);
    addGraphPoint(graphId, value, isValid);
}

void GraphPlotDialog::addMotionIntensity(const vector<double>& data)
{
    if (mUi.pauseButton->isChecked())
        return;

    mIntensity.push_front(data);
    while (mIntensity.size() > 200) {
            mIntensity.pop_back();
    }
}

void GraphPlotDialog::update(void)
{
    mUi.widget->update();
}

void GraphPlotDialog::loadFromQSettings(const QString &fileName, const QString &_root)
{
    mUi.parametersWidget->loadFromQSettings(fileName, _root + mRootPath);
}

void GraphPlotDialog::saveToQSettings  (const QString &fileName, const QString &_root)
{
    mUi.parametersWidget->saveToQSettings(fileName, _root + mRootPath);
}


void GraphPlotDialog::loadSettingsFromFile()
{
    QString filename = QFileDialog::getOpenFileName(
                this,
                tr("Load settings"),
                "./",
                tr("XML files(*.xml)"));

    mUi.parametersWidget->loadFromQSettings(filename, "GraphPlotConfiguration");
}

void GraphPlotDialog::saveSettingsToFile()
{
    QString filename = QFileDialog::getSaveFileName(
                this,
                tr("Save settings"),
                "./GraphSettings.xml",
                tr("XML files(*.xml)"));

    mUi.parametersWidget->saveToQSettings(filename, "GraphPlotConfiguration");
}

void GraphPlotDialog::resetSettings()
{
    GraphPlotParameters defaultParameters;
    defaultParameters.setGraphStyle(GraphStyle::DRAW_LINES);
    defaultParameters.setWidth(1);
    defaultParameters.setCenterAt(0.0);
    defaultParameters.setXGrid(false);
    defaultParameters.setYGrid(false);
    defaultParameters.setXScale(1.0);
    defaultParameters.setYScale(1.0);
    defaultParameters.setContrast(50.0);
    defaultParameters.setSelectGraph(false);

    mUi.parametersWidget->setParameters(defaultParameters);
}

GraphPlotDialog::~GraphPlotDialog()
{
    delete_safe(mUpDownMapper);
}

void GraphPlotDialog::addNewGraphToUI(unsigned graphId)
{
    QTableWidgetItem* graphItem = new QTableWidgetItem(QString::fromStdString(mData[graphId].name));
    graphItem->setData(Qt::UserRole, QVariant(graphId));
    setItemColor(graphItem);
    int rowCount = mUi.visibleGraph->rowCount();
    mUi.visibleGraph->setRowCount(rowCount + 1);
    mUi.visibleGraph->setItem(rowCount, 0, graphItem);

    QTableWidgetItem* valueItem = new QTableWidgetItem();
    valueItem->setForeground(graphItem->foreground());
    valueItem->setData(Qt::UserRole, QVariant(graphId));
    mUi.visibleGraph->setItem(rowCount, 1, valueItem);

}


unsigned GraphPlotDialog::getGraphId(QString name)
{
    unsigned graphId = 0;
//    qDebug("GraphPlotDialog::getGraphId(%s): called", name.toLatin1().constData());

    while (graphId < mData.size()) {
        if (mData[graphId].name == name.toStdString()) {
            return graphId;
        }
        graphId++;
    }

//    qDebug("GraphPlotDialog::getGraphId(): adding ui.. ");

    mData.resize(graphId + 1);
    mData[graphId].isSelected = false;
    mData[graphId].name = name.toStdString();

    addNewGraphToUI(graphId);
    return graphId;
}

void GraphPlotDialog::graphPlotParametersChanged()
{
    mGraphPlotParameters = QSharedPointer<GraphPlotParameters>(mUi.parametersWidget->createParameters());
    mUi.widget->update();
}

QString GraphPlotDialog::getGraphName(unsigned id)
{
    if (id >= mData.size())
        return "";

    return QString::fromStdString(mData[id].name);
}

void GraphPlotDialog::setItemColor(QTableWidgetItem *item)
{
    unsigned graphId = item->data(Qt::UserRole).toUInt();
    int colornum = CORE_COUNT_OF(mGraphPallete);
    QBrush brush;
    brush.setColor(mGraphPallete[graphId % colornum]);
    item->setForeground(brush);
}

void GraphPlotDialog::closePrevItemEditor(QTableWidgetItem*,QTableWidgetItem* prev)
{
    if (prev == NULL) {
        return;
    }

    mUi.invisibleGraph->closePersistentEditor(prev);
}

void GraphPlotDialog::drawGridFixLine(QPainter &painter)
{
    if (mMouseClickedValue < 0) {
        return;
    }

    QPen gridPen(Qt::SolidLine);
    mGraphPlotParameters->fixTimeValue() ? gridPen.setColor(Qt::yellow) : gridPen.setColor(Qt::blue);
    painter.setPen(gridPen);
    painter.drawLine(mMouseClickedValue, 0,
                     mMouseClickedValue, mUi.widget->height() - 1);

    painter.drawText(mMouseClickedValue, 10, QString::number(mGraphShowPoint));
}


void GraphPlotDialog::showGraphsValue(QTableWidget *graphTable)
{
    for (int i = 0; i < graphTable->rowCount(); i++) {
        unsigned graphId = graphTable->item(i, 0)->data(Qt::UserRole).toUInt();
        GraphHistory &graph = mData[graphId];

        QTableWidgetItem *item = graphTable->item(i, 1);
        if (mGraphShowPoint >= 0 && graph[mGraphShowPoint].isValid) {
            item->setText(QString("%1").arg(graph[mGraphShowPoint].value, 0, 'g', 3));
        }
        else {
            item->setText(QString(""));
        }
    }
}

void GraphPlotDialog::setSnapshotValue(QMouseEvent *mouseEvent)
{
    if (mData.size() == 0) {
        return;
    }

    // Update snapshot point
    mMouseClickedValue = mouseEvent->x();
    mGraphShowPoint = fround(double(mMouseClickedValue) / mGraphPlotParameters->xScale());

    if (mGraphShowPoint >= (int)mData[0].size()) {
       mGraphShowPoint = (int)(mData[0].size() - mGraphShowPoint - 1);
    }
    mUi.widget->update();
}

