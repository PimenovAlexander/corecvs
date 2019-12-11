#include "patternDetectorParametersWidget.h"
#include "ui_patternDetectorParametersWidget.h"
#include "core/patterndetection/patternDetector.h"

#include <reflectionWidget.h>

using namespace corecvs;

PatternDetectorParametersWidget::PatternDetectorParametersWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PatternDetectorParametersWidget)
{
    ui->setupUi(this);

    std::vector<std::string> hints;
    hints = PatternDetectorFabric::getInstance()->getCaps();

    for (size_t id = 0; id < hints.size(); id++)
    {
        std::string hint = hints[id];
        QString str = QString::fromStdString(hint);
        ui->providerComboBox->addItem(str);

        PatternDetector *detector = PatternDetectorFabric::getInstance()->fabricate(hint);

        ProviderParametersWidgetMetadata *metadata = new ProviderParametersWidgetMetadata;
        providerMetadata.push_back(metadata);

        metadata->id = id;
        metadata->providerName = hint;
        metadata->tabWidget = new QWidget;
        QGridLayout *layout = new QGridLayout();
        metadata->tabWidget->setLayout(layout);

        std::map<std::string, corecvs::DynamicObject> defaultParameters = detector->getParameters();
        delete_safe(detector);

        if (defaultParameters.empty()) {
            SYNC_PRINT(("There are no parameters\n"));
            return;
        } else {
            SYNC_PRINT(("There are %" PRISIZE_T " parameters\n", defaultParameters.size()));
        }

        for (auto &it : defaultParameters)
        {            
            ReflectionWidget *refWidget = new ReflectionWidget(it.second.reflection);
            int row = layout->rowCount();
            layout->addWidget(new QLabel(QString::fromStdString(it.first)), row, 0 );
            layout->addWidget(refWidget,                                    row, 1);

            metadata->reflectionWidgets[it.first] = refWidget;
            connect(refWidget, SIGNAL(paramsChanged()), metadata, SLOT(uiParamsChanged()));
            layout->addItem(new QSpacerItem(10,10, QSizePolicy::Expanding, QSizePolicy::Expanding), layout->rowCount(), 0);
        }


        ui->tabWidget->addTab(metadata->tabWidget, QString::fromStdString(hint));
        connect(metadata, SIGNAL(paramsChanged(int)), this, SLOT(tabParamsChanged(int)));        


        //knownParameters.emplace_back(str);
        SYNC_PRINT(("PatternDetectorParametersWidget(): %s provider added\n", str.toStdString().c_str()));
    }
    connect(ui->providerComboBox, SIGNAL(activated(int)), ui->tabWidget, SLOT(setCurrentIndex(int)));
    connect(ui->providerComboBox, SIGNAL(activated(int)), this, SLOT(uiParamsChanged()));

    mPoseParameters = new ReflectionWidget(PatternFromPoseParameters::getReflection());
    mPoseParameters->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    ui->gridLayout->addWidget(mPoseParameters);

    connect(mPoseParameters, SIGNAL(paramsChanged()), this, SLOT(uiParamsChanged()));
    /**/
}

PatternDetectorParametersWidget::~PatternDetectorParametersWidget()
{
    delete ui;
    for (ProviderParametersWidgetMetadata *data : providerMetadata)
    {
        delete_safe(data);
    }

}

GeneralPatternDetectorParameters PatternDetectorParametersWidget::getParameters()
{
    GeneralPatternDetectorParameters toReturn;
    size_t id = (size_t)ui->providerComboBox->currentIndex();
    toReturn.provider       = providerMetadata[id]->providerName;

    mPoseParameters->getParameters(&toReturn.poseParameters);

    for (auto &it : providerMetadata[id]->reflectionWidgets)
    {
        std::string name = it.first;
        ReflectionWidget *widget = it.second;

        toReturn.providerParameters.emplace(name, DynamicObject(widget->reflection));
        widget->getParameters(toReturn.providerParameters[name].rawObject);
    }

    return toReturn;
}

void PatternDetectorParametersWidget::loadParamWidget(WidgetLoader &loader)
{
    SYNC_PRINT(("PatternDetectorParametersWidget::loadParamWidget(): called"));
}

void PatternDetectorParametersWidget::saveParamWidget(WidgetSaver &saver)
{
    SYNC_PRINT(("PatternDetectorParametersWidget::saveParamWidget(): called"));
}

void PatternDetectorParametersWidget::loadFromQSettings(const QString &fileName, const QString &_root)
{
    SettingsGetter visitor(fileName, _root);

    PatternFromPoseParameters pose;
    mPoseParameters->getParameters(&pose);
    visitor.visit(pose, pose, "pose");

    std::string name;
    visitor.visit(name, name, "provider");
    for (size_t id = 0; id < providerMetadata.size(); id++ )
    {
        if (providerMetadata[id]->providerName == name)
        {
            ui->providerComboBox->setCurrentIndex(id);
            ui->tabWidget->setCurrentIndex(id);
            break;
        }
    }
    for (size_t id = 0; id < providerMetadata.size(); id++ )
    {
        std::string provider = providerMetadata[id]->providerName;
        std::map<std::string, ReflectionWidget*> &wmap = providerMetadata[id]->reflectionWidgets;
        for (auto &it : wmap)
        {
            std::string name = it.first;
            ReflectionWidget *widget = it.second;

            DynamicObject block(widget->reflection);
            visitor.visit(block, (provider + "." + name).c_str());
            cout << block << endl;
            widget->setParameters(block.rawObject);
        }
    }



}

void PatternDetectorParametersWidget::saveToQSettings (const QString &fileName, const QString &_root)
{
    SettingsSetter visitor(fileName, _root);

    PatternFromPoseParameters pose;
    visitor.visit(pose, pose, "pose");
    mPoseParameters->setParameters(&pose);

    size_t id = (size_t)ui->providerComboBox->currentIndex();
    std::string name = providerMetadata[id]->providerName;
    visitor.visit(name, name, "provider");

    for (size_t id = 0; id < providerMetadata.size(); id++ )
    {
        std::string provider = providerMetadata[id]->providerName;
        std::map<std::string, ReflectionWidget*> &wmap = providerMetadata[id]->reflectionWidgets;
        for (auto &it : wmap)
        {
            std::string name = it.first;
            ReflectionWidget *widget = it.second;

            DynamicObject block(widget->reflection);
            widget->getParameters(block.rawObject);
            visitor.visit(block, (provider + "." + name).c_str());
        }
    }
}


void PatternDetectorParametersWidget::setCurrentToDefaults()
{

}

void PatternDetectorParametersWidget::tabParamsChanged(int tabIndex)
{
     SYNC_PRINT(("PatternDetectorParametersWidget::tabParamsChanged(%d): called\n", tabIndex));
     int id = ui->providerComboBox->currentIndex();

     if (id == tabIndex) {
        emit paramsChanged();
     }
}

void PatternDetectorParametersWidget::uiParamsChanged()
{
     SYNC_PRINT(("PatternDetectorParametersWidget::uiParamsChanged(): called\n"));
     emit paramsChanged();
}

void ProviderParametersWidgetMetadata::uiParamsChanged()
{
    SYNC_PRINT(("ProviderParametersWidgetMetadata::uiParamsChanged(): called for %d\n", id));
    emit paramsChanged(id);
}

ProviderParametersWidgetMetadata::~ProviderParametersWidgetMetadata()
{

}

