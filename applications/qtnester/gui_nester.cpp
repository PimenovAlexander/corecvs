#include "gui_nester.h"
#include "ui_gui_nester.h"
#include <QDialog>
#include "gui_nester.h"
#include "ui_gui_nester.h"
#include "core/buffers/rgb24/rgb24Buffer.h"
#include "core/fileformats/svgLoader.h"
#include <list>
#include <QFileDialog>
#include "pointListEditImageWidget.h"
#include "core/buffers/bufferFactory.h"
#include "core/buffers/rgb24/rgb24Buffer.h"
#include "core/buffers/converters/debayer.h"
#include "core/fileformats/ppmLoader.h"
#include "utils/corestructs/g12Image.h"
#include "nester.h"
#include "core/buffers/rgb24/rgb24Buffer.h"
#include "core/fileformats/svgLoader.h"
#include <QPixmap>
#include <QMessageBox>

GUI_nester::GUI_nester(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GUI_nester)
{
    ui->setupUi(this);
}

GUI_nester::~GUI_nester() {
    delete ui;
}

void GUI_nester::on_Load_clicked() {
    BufferFactory::getInstance()->printCaps();
    QString name = QFileDialog::getOpenFileName(this, "Open file");
               /* QFileDialog::getOpenFileName(
                this,
                "Choose filename with Bayer or demosaic image",
                ".",
                "PPM Images (*.pgm *.ppm);;Generic Images (*.png *.jpg *.bmp)"
            );*/
    if (name.isEmpty())
        return;
    if (name.endsWith(".svg")) {
        inputPolygons = {};
        std::ifstream proxi(name.toStdString(), std::ifstream::binary);
        svgFileP = new SvgFile;
        svgLoader.loadSvg(proxi, *svgFileP);
        for (auto sh : (*svgFileP).shapes) {
            if (sh->type == 2) {
                inputPolygons.push_back(((SvgPolygon*)sh)->polygon);
            }
        }
        delete svgFileP;
    }
    QPixmap picture1(name);
    ui->pic1->setPixmap(picture1.scaled(ui->pic1->height(), ui->pic1->width(), Qt::KeepAspectRatio));
}

void GUI_nester::on_Save_as_released() {
    QString name = QFileDialog::getSaveFileName(this, "save as");
    std::list<Polygon> inputPolygonsWithArea = inputPolygons;
    inputPolygonsWithArea.push_back(polFromRec(area));
    drawSvgPolygons(inputPolygonsWithArea, area.height(), area.width(), name.toStdString());
}

void GUI_nester::on_Run_nest_released() {
    if (!inputPolygons.empty()) {
        vinilPlacementNester(inputPolygons, area, indentSize, 1, 1, 0.2, rotationsAmount);
        std::list<Polygon> inputPolygonsWithArea = inputPolygons;
        inputPolygonsWithArea.push_back(polFromRec(area));
        drawSvgPolygons(inputPolygonsWithArea, area.height(), area.width(), "clicked.svg");
        QPixmap picture2("clicked.svg");
        ui->pic2->setPixmap(picture2.scaled(ui->pic1->height(), ui->pic1->width(), Qt::KeepAspectRatio));
        double verticalFullness = getMaxValueY(inputPolygons) / area.height();
        cout << verticalFullness;
        ui->Verctical_fullness->setPlainText(QString::number(verticalFullness));
    }
}

void GUI_nester::on_Rotations_windowIconTextChanged(const QString &iconText) {
    rotationsAmount = iconText.toInt();
    cout << rotationsAmount << "brah";
}

void GUI_nester::on_Indent_windowIconTextChanged(const QString &iconText) {
    indentSize = iconText.toDouble();
}
