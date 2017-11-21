#include <QFileDialog>
#include "core/buffers/bufferFactory.h"
#include "pointListEditImageWidget.h"

#include "imageViewMainWindow.h"
#include "ui_imageViewMainWindow.h"
#include "core/buffers/rgb24/rgb24Buffer.h"
#include "g12Image.h"

#include "g12Image.h"
#include "core/fileformats/ppmLoader.h"
#include "core/buffers/converters/debayer.h"

ImageViewMainWindow::ImageViewMainWindow(QWidget *parent) :
    QWidget(parent),
    bayer(NULL),
    input(NULL),
    ui(new Ui::ImageViewMainWindow)
{
    ui->setupUi(this);
#if 0
    delete(ui->widget);
    PointListEditImageWidgetUnited *pointList = new PointListEditImageWidgetUnited(this);
    pointList->setRightDrag(true);
    ui->widget = pointList;
    ui->mainLayout->addWidget(ui->widget, 0, 0, 3, 1);
#endif

    BitSelectorParameters defaultSelector;
    defaultSelector.setShift(-2);
    ui->bitSelector->setParameters(defaultSelector);

    ui->widget->setFitWindow(true);
    ui->widget->setKeepAspect(true);
    ui->widget->setRightDrag(true);

    connect(ui->bitSelector, SIGNAL(paramsChanged()), this, SLOT(paramsChanged()));
    connect(ui->loadButton, SIGNAL(released())      , this, SLOT(loadImageAction()));
    connect(ui->parameters, SIGNAL(paramsChanged()) , this, SLOT(debayer()));
}

ImageViewMainWindow::~ImageViewMainWindow()
{
    delete_safe(ui);
    delete_safe(input);
}

void ImageViewMainWindow::setImage(RGB48Buffer *image)
{
    delete_safe(input);
    input = image;
    paramsChanged();
}

void ImageViewMainWindow::paramsChanged()
{
    if (input == NULL)
        return;

    BitSelectorParameters mBitSelectorParameters;
    ui->bitSelector->getParameters(mBitSelectorParameters);

    uint16_t mask = 0x0;
    if (mBitSelectorParameters.bit0 ()) mask |= 0x0001;
    if (mBitSelectorParameters.bit1 ()) mask |= 0x0002;
    if (mBitSelectorParameters.bit2 ()) mask |= 0x0004;
    if (mBitSelectorParameters.bit3 ()) mask |= 0x0008;

    if (mBitSelectorParameters.bit4 ()) mask |= 0x0010;
    if (mBitSelectorParameters.bit5 ()) mask |= 0x0020;
    if (mBitSelectorParameters.bit6 ()) mask |= 0x0040;
    if (mBitSelectorParameters.bit7 ()) mask |= 0x0080;

    if (mBitSelectorParameters.bit8 ()) mask |= 0x0100;
    if (mBitSelectorParameters.bit9 ()) mask |= 0x0200;
    if (mBitSelectorParameters.bit10()) mask |= 0x0400;
    if (mBitSelectorParameters.bit11()) mask |= 0x0800;

    if (mBitSelectorParameters.bit12()) mask |= 0x1000;
    if (mBitSelectorParameters.bit13()) mask |= 0x2000;
    if (mBitSelectorParameters.bit14()) mask |= 0x4000;
    if (mBitSelectorParameters.bit15()) mask |= 0x8000;

    int shift = mBitSelectorParameters.shift();

    RGB24Buffer *result = new RGB24Buffer(input->getSize());
    for (int i = 0; i < result->h; i ++)
    {
        for (int j = 0; j < result->w; j ++)
        {
            RGB48Buffer::InternalElementType colorIn = input->element(i, j);
            RGBColor colorOut;
            if (shift >= 0) {
                colorOut.r() = (colorIn.r() & mask) << shift;
                colorOut.g() = (colorIn.g() & mask) << shift;
                colorOut.b() = (colorIn.b() & mask) << shift;
            }
            if (shift < 0) {
                colorOut.r() = (colorIn.r() & mask) >> (-shift);
                colorOut.g() = (colorIn.g() & mask) >> (-shift);
                colorOut.b() = (colorIn.b() & mask) >> (-shift);
            }
            result->element(i,j) = colorOut;
        }
    }
    // SYNC_PRINT(("Updating widget with [%d x %d]\n", result->w, result->h));
    ui->widget->setImage(QSharedPointer<QImage>(new RGB24Image(result)));
    ui->widget->update();
    delete_safe(result);
}

void ImageViewMainWindow::loadImageAction()
{
    BufferFactory::getInstance()->printCaps();

    QString name = QFileDialog::getOpenFileName(
                this,
                "Choose filename with Bayer or demosaic image",
                ".",
                "PPM Images (*.pgm *.ppm);;Generic Images (*.png *.jpg *.bmp)"
            );
    if (name.isEmpty())
        return;

    loadImage(name);
}

void ImageViewMainWindow::loadImage(QString name)
{
    delete_safe(bayer);
    ui->widget->setInfoString("Loading...");
    int shift = 0;
    if (name.endsWith(".ppm"))
    {
        SYNC_PRINT(("Loading PPM <%s>\n", name.toLatin1().constData()));
        RGB48Buffer* result = PPMLoader().rgb48BufferCreateFromPPM(name.toStdString(), &meta);
        setImage(result);
        shift = 8 - meta["bits"][0];                // left shift:  8 => 0,  10 => -2,  12 => -4

    }
    else if (name.endsWith(".raw"))
    {
        SYNC_PRINT(("Loading RAW <%s>\n", name.toLatin1().constData()));
        bayer = PPMLoader().g12BufferCreateFromPGM(name.toStdString(), &meta);
        if (bayer == NULL) {
            qDebug("Can't open Bayer file: %s", name.toLatin1().constData());
        }
        debayer();
        shift = 8 - meta["bits"][0];                // left shift:  8 => 0,  10 => -2,  12 => -4
    } else {
        SYNC_PRINT(("Loading Generic <%s>\n", name.toLatin1().constData()));
        RGB24Buffer *input = BufferFactory::getInstance()->loadRGB24Bitmap(name.toStdString());
        if (input == NULL)
        {
            qDebug() << "Unable to load" << name;
            return;
        }

        SYNC_PRINT(("Loaded size %d x %d\n", input->w, input->h ));
        RGB48Buffer *image = new RGB48Buffer(input->getSize());

        for (int i = 0; i < image->h; i ++)
        {
            for (int j = 0; j < image->w; j ++)
            {
                RGBColor   c = input->element(i,j);
                RGBColor48 c48(c.r() << 8, c.g() << 8, c.b() << 8);
                image->element(i,j) = c48;
            }
        }
        setImage(image);
    }


    ui->widget->setInfoString("---");

    BitSelectorParameters bitSelector;
    ui->bitSelector->getParameters(bitSelector);
    bitSelector.setShift(shift);
    ui->bitSelector->setParameters(bitSelector);
}

void ImageViewMainWindow::debayer()
{
    if (bayer == NULL) {
        return;
    }

    DebayerParameters params;
    ui->parameters->getParameters(params);

    Debayer d(bayer, meta["bits"][0], &meta, params.bayerPos());
    RGB48Buffer* result = new RGB48Buffer(bayer->h, bayer->w, false);
    d.toRGB48(params.method(), result);
    setImage(result);
}
