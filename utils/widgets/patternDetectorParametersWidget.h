#ifndef PATTERNDETECTORPARAMETERSWIDGET_H
#define PATTERNDETECTORPARAMETERSWIDGET_H

#include <map>
#include <iostream>
#include <string>
#include <QWidget>
#include <corestructs/reflectionWidget.h>

#include "core/reflection/dynamicObject.h"
#include "generated/patternFromPoseParameters.h"

namespace Ui {
class PatternDetectorParametersWidget;
}


class GeneralPatternDetectorParameters {
public:
    PatternFromPoseParameters poseParameters;

    std::string provider;
    std::map<std::string, corecvs::DynamicObject> providerParameters;

    friend std::ostream& operator << (std::ostream &out, GeneralPatternDetectorParameters &params)
    {
        out << "Pose Params:" << endl << params.poseParameters << endl;

        out << "Provider Params:" << params.provider << endl;

        out << "Params:"   << endl;
        for (auto it : params.providerParameters)
        {
            out << it.first  << endl;
            out << it.second << endl;
        }
        return out;
    }


};

struct ProviderParametersWidgetMetadata  : public QObject {
    Q_OBJECT
public:
    int id;
    std::string providerName;
    QWidget *tabWidget = nullptr;
    std::map<std::string, ReflectionWidget*> reflectionWidgets;

public slots:
    void uiParamsChanged();

signals:
    void paramsChanged(int index);
public:
    virtual ~ProviderParametersWidgetMetadata();
};


class PatternDetectorParametersWidget : public QWidget, public SaveableWidget
{
    Q_OBJECT

public:


    vector<ProviderParametersWidgetMetadata *> providerMetadata;

    explicit PatternDetectorParametersWidget(QWidget *parent = nullptr);
    virtual ~PatternDetectorParametersWidget();

    GeneralPatternDetectorParameters getParameters();

    virtual void loadParamWidget(WidgetLoader &loader);
    virtual void saveParamWidget(WidgetSaver  &saver );

    virtual void loadFromQSettings(const QString &fileName, const QString &_root);
    virtual void saveToQSettings  (const QString &fileName, const QString &_root);


    ReflectionWidget *mPoseParameters = NULL;

public slots:
    void setCurrentToDefaults();
    void uiParamsChanged();
    void tabParamsChanged(int tabIndex);

signals:
    void paramsChanged();

private:
    Ui::PatternDetectorParametersWidget *ui;
};

#endif // PATTERNDETECTORPARAMETERSWIDGET_H
