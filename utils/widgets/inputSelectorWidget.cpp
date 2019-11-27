#include "inputSelectorWidget.h"

#include <regex>
#include <QFileDialog>

#include "core/buffers/bufferFactory.h"

#ifdef WITH_V4L2
#include "V4L2Capture.h"
#endif

using namespace corecvs;

InputSelectorWidget::InputSelectorWidget(QWidget *parent, bool autoInit, QString rootPath)
    : QWidget(parent),
      autoInit(autoInit),
      rootPath(rootPath)
{
    ui.setupUi(this);


    connect(ui.okPushButton, SIGNAL(pressed()), this, SIGNAL(newInputString()));

    connect(ui.aviLineEdit, SIGNAL(textChanged(QString)), this, SLOT(updateInputString()));
    connect(ui.browseAviButton, SIGNAL(released()), this, SLOT(browseAvi()));

    connect(ui.fileLineEdit, SIGNAL(textChanged(QString)), this, SLOT(updateInputString()));
    connect(ui.browseFileButton, SIGNAL(released()), this, SLOT(browseFile()));


    connect(ui.v4l2RefreshPushButton, SIGNAL(released()), this, SLOT(v4l2Refresh()));


    connect(ui.v4l2StereoCheckBox, SIGNAL(toggled(bool)), this, SLOT(updateInputString()));
    connect(ui.v4l2Device1LineEdit, SIGNAL(textChanged(QString)), this, SLOT(updateInputString()));
    connect(ui.v4l2Device2LineEdit, SIGNAL(textChanged(QString)), this, SLOT(updateInputString()));

    connect(ui.v4l2CodecComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(updateInputString()));

    connect(ui.v4l2FpsCheckBox, SIGNAL(toggled(bool)), this, SLOT(updateInputString()));
    connect(ui.v4l2DenumComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(updateInputString()));
    connect(ui.v4l2NumComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(updateInputString()));

    connect(ui.v4l2hwCheckBox, SIGNAL(toggled(bool)), this, SLOT(updateInputString()));
    connect(ui.v4l2HightComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(updateInputString()));
    connect(ui.v4l2WidthComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(updateInputString()));


}

void InputSelectorWidget::updateInputString()
{
    QString str;
    switch (ui.tabWidget->currentIndex())
    {
        case 0: str += "v4l2:";
            str += ui.v4l2Device1LineEdit->text();
            if (ui.v4l2StereoCheckBox->isChecked()) {
                str += ",";
                str += ui.v4l2Device2LineEdit->text();
            }
            if (ui.v4l2FpsCheckBox->isChecked()) {
                str += ":";
                str += ui.v4l2NumComboBox->currentText();
                str += "/";
                str += ui.v4l2DenumComboBox->currentText();
            }

            switch (ui.v4l2CodecComboBox->currentIndex()) {
                case 1: str += ":mjpeg"; break;
                case 2: str += ":fjpeg"; break;
                case 3: str += ":yuyv";  break;
                case 0:
                default:
                    break;
            }

            if (ui.v4l2hwCheckBox->isChecked()) {
                str += ":";
                str += ui.v4l2HightComboBox->currentText();
                str += "x";
                str += ui.v4l2WidthComboBox->currentText();
            }



        break;
        case 1: str += "avcodec:";
            str += ui.aviLineEdit->text();
        break;
        case 2: str += "uEye:"; break;
        case 3: str += "prec:";
            str += ui.fileLineEdit->text();
        break;

    default:
        break;
    }

    setInputString(str);
}

void InputSelectorWidget::browseAvi()
{
    QString filename = QFileDialog::getOpenFileName(
        this,
        "Choose an file name",
        ".",
        "Avi (*.avi *.mp4 *.mkv)"
        );

    ui.aviLineEdit->setText(filename);
}

void InputSelectorWidget::browseFile()
{
    std::vector<std::string> extentions = BufferFactory::getInstance()->extentionsRGB24();

    QString filename = QFileDialog::getOpenFileName(
        this,
        "Choose an file name",
        ".",
        "Images (*.png *.jpeg *.bmp)"
        );
    ui.fileLineEdit->setText(filename);

}

/** V4L2 **/
void InputSelectorWidget::v4l2Refresh()
{
#ifdef WITH_V4L2
    ui.v4l2Device1ComboBox->clear();
    ui.v4l2Device2ComboBox->clear();

    vector<std::string> cameras;
    V4L2CaptureInterface::getAllCameras(cameras);

    for (const std::string &name : cameras)
    {
        ui.v4l2Device1ComboBox->addItem(QString::fromStdString(name));
        ui.v4l2Device2ComboBox->addItem(QString::fromStdString(name));
    }
#endif
}


QString InputSelectorWidget::getInputString()
{
    return ui.inputLineEdit->text();
}

void InputSelectorWidget::setInputString(const QString &str)
{
    ui.inputLineEdit->setText(str);

    if (str.startsWith("v4l2:"))
    {
        std::string devname = str.mid(strlen("v4l2:")).toStdString();

        std::regex deviceStringPattern("^([^,:]*)(,([^:]*))?(:(\\d*)/(\\d*))?((:mjpeg)|(:yuyv)|(:fjpeg))?(:(\\d*)x(\\d*))?$");

        static const int Device1Group     = 1;
        static const int Device2Group     = 3;
        static const int FpsNumGroup      = 5;
        static const int FpsDenumGroup    = 6;
        static const int CompressionGroup = 7;
        static const int WidthGroup       = 12;
        static const int HeightGroup      = 13;

        std::smatch matches;
        printf ("Input string %s\n", devname.c_str());
        int result = std::regex_match(devname, matches, deviceStringPattern);
        if (result == -1)
        {
            printf("Error in device string format:%s\n", devname.c_str());
        }

        if (!matches[Device1Group].str().empty())
        {
            ui.v4l2Device1LineEdit->setText(QString::fromStdString(matches[Device1Group].str()));
        }

        if (!matches[Device2Group].str().empty())
        {
            ui.v4l2Device2LineEdit->setText(QString::fromStdString(matches[Device2Group].str()));
            ui.v4l2StereoCheckBox->setChecked(true);
        }

        if (!matches[FpsNumGroup].str().empty() && !matches[FpsDenumGroup].str().empty())
        {
            bool isOk = false;
            int fpsnum = HelperUtils::parseInt(matches[FpsNumGroup].str(), &isOk);
            if (!isOk || fpsnum < 0) fpsnum = 1;

            int fpsdenum = HelperUtils::parseInt(matches[FpsDenumGroup].str(), &isOk);
            if (!isOk || fpsdenum < 0) fpsdenum = 10;
            ui.v4l2FpsCheckBox->setChecked(true);
            //ui.v4l2DenumComboBox->add
        }

        if (!matches[WidthGroup].str().empty() && !matches[HeightGroup].str().empty())
        {
            bool isOk = false;
            int width = HelperUtils::parseInt(matches[WidthGroup].str(), &isOk);
            if (!isOk || width <= 0) width = 800;

            int height = HelperUtils::parseInt(matches[HeightGroup].str(), &isOk);
            if (!isOk || height <= 0) height = 600;

            ui.v4l2hwCheckBox->setChecked(true);
        };
    }


}

void InputSelectorWidget::loadParamWidget(WidgetLoader &loader)
{


}

void InputSelectorWidget::saveParamWidget(WidgetSaver &saver)
{

}


InputSelectorWidget::~InputSelectorWidget()
{

}

