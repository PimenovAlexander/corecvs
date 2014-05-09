#ifndef GRAPHPLOTDIALOG_H
#define GRAPHPLOTDIALOG_H

#include <deque>
#include <QtGui/QWidget>
#include <QtGui/QtGui>
#include <QtXml/QDomDocument>

#include "ui_graphPlotDialog.h"

using namespace std;

class GraphValue
{
public:
    double value;
    bool isValid;

    explicit GraphValue(double _value, bool _isValid = true) :
        value(_value), isValid(_isValid)
    {}
};

class GraphHistory : public deque<GraphValue>
{
public:
    bool isSelected;
    QString name;
};


class GraphPlotDialog : public ViAreaWidget, public SaveableWidget
{
    Q_OBJECT

public:
    GraphPlotDialog(QWidget *parent = 0);
    ~GraphPlotDialog();

    /* Returns graph Id by name. Creates new graph if nessesary. */
    unsigned getGraphId(QString name);
    /* Returns graph Name by id. Creates new graph if nessesary. */
    QString getGraphName(unsigned id);

    /* Add point to graph with ID graphId. Create new graph if nessesary.*/
    void addGraphPoint(unsigned graphId, double value, bool isValid = true);
    /* Add point to graph with Name graphName. Create new graph if nessesary.*/
    void addGraphPoint(QString graphName, double value, bool isValid = true);
    void addMotionIntensity(const vector<double>& mData);

    void update(void);

    /* Automatic saving and loading */
    virtual void loadFromQSettings(const QString &fileName, const QString &_root);
    virtual void saveToQSettings  (const QString &fileName, const QString &_root);

public slots:
    /* Repaint graphs */
    void childRepaint(QPaintEvent *event, QWidget *who);
    void exportToCSV();

    /* Move graph to visible/invisible list*/
    void setGraphVisibility(int currentVisibility);
    /* Mark selected graph*/
    void selectGraph(QTableWidgetItem*,QTableWidgetItem*);
    /* Close graph name editor*/
    void closePrevItemEditor(QTableWidgetItem*,QTableWidgetItem*);
    /* Open graph name editor*/
    void editGraphName(QTableWidgetItem*);
    /* Save changed graph name */
    void saveGraphName(QTableWidgetItem*);
    /* Save graph plot settings */
    void saveSettingsToFile();
    /* Load graph plot settings */
    void loadSettingsFromFile();
    /* Reset graph plot settings */
    void resetSettings();
    /* Update graph plot parameters */
    void graphPlotParametersChanged();
    /* Set snapshot point for values of graphs */
    void setSnapshotValue(QMouseEvent*);

private:
    Ui::GraphPlotDialogClass mUi;
    QSharedPointer<GraphPlotParameters> mGraphPlotParameters;

    /* Color pallete for graph visualisation */
    static QColor mGraphPallete[];

    /* List of graphs*/
    vector<GraphHistory> mData;
    deque<vector<double> > mIntensity;

    /* Point on grid to monitor graphs values*/
    int mMouseClickedValue;

    /* Time point to monitor graphs values*/
    int mGraphShowPoint;

    void drawGrid(QPainter &painter);
    /* Draw line to monitor values */
    void drawGridFixLine(QPainter &painter);

    QSignalMapper *mUpDownMapper;
//    ParametersMapperGraphPlot *mParamMapper;

    /* Set graph color*/
    void setItemColor(QTableWidgetItem*);
    void showGraphsValue(QTableWidget*);

    QString mRootPath;
public:
    void setSavingRoot(const QString &root)
    {
        mRootPath = root;
    }
};

#endif // GRAPHPLOTDIALOG_H
