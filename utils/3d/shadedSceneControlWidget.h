#ifndef SHADEDSCENECONTROLWIDGET_H
#define SHADEDSCENECONTROLWIDGET_H

#include "corestructs/parametersControlWidgetBase.h"
#include "draw3dParametersControlWidget.h"

#include <QWidget>
#include <QHash>

namespace Ui {
class ShadedSceneControlWidget;
}

class ShaderPreset {
public:
    QString name;
    enum ShaderType{
        NONE,
        PRESET,
        PRESET1,
        IMUTABLE,
        SAVEABLE
    };
    ShaderType type = ShaderPreset::PRESET1;

    QString vertex;
    QString fragment;


    ShaderPreset() {}
    ShaderPreset(QString name, QString vertex, QString fragment) :
        name(name),
        type(SAVEABLE),
        vertex(vertex),
        fragment(fragment)
    {}

};

class ShadedSceneControlParameters : public Draw3dParameters
{
public:

    ShadedSceneControlParameters()
    {
        point.type = ShaderPreset::PRESET;
        edge.type  = ShaderPreset::PRESET;
        face.type  = ShaderPreset::PRESET;
    }

    ShaderPreset point;
    ShaderPreset edge;
    ShaderPreset face;
};

class ShadedSceneControlWidget : public ParametersControlWidgetBase
{
    Q_OBJECT

public:
    explicit ShadedSceneControlWidget(QWidget *parent = 0);
    ~ShadedSceneControlWidget();

    ShadedSceneControlParameters* createParameters() const;
    void getParameters(ShadedSceneControlParameters &params) const;
    void setParameters(const ShadedSceneControlParameters &input);
    virtual void setParametersVirtual(void *input);


    virtual BaseReflectionStatic *createParametersVirtual() const
    {
        return createParameters();
    }

    QHash<QString, ShaderPreset> shaderCache;


    void getShaderPreset(ShaderPreset &preset, const QComboBox *combo) const;
public slots:
    void applyPressed();
    void reloadShaders(QString path);
    void presetChanged();

private:
    Ui::ShadedSceneControlWidget *ui;

signals:
    void paramsChanged();

};

#endif // SHADEDSCENECONTROLWIDGET_H
