echo "Generating H stub for $1"


lowerCamel=$1
upperCamel=`echo $1 | sed 's/^\(.\)/\U\1/'`


className="${upperCamel}ControlWidget"
paramsName="${upperCamel}"

paramsFileName="${lowerCamel}.h"
fileName="${lowerCamel}ControlWidget.h"
uiName="ui_${lowerCamel}ControlWidget.h"

echo "Class name: ${className}"
echo "File  name: ${paramsFileName}"
echo "UI    name: ${uiName}"

cat >${fileName} <<EOF
#pragma once
#include <QWidget>
#include "generated/${paramsFileName}"
#include "${uiName}"
#include "parametersControlWidgetBase.h"


namespace Ui {
    class ${className};
}

class ${className} : public ParametersControlWidgetBase
{
    Q_OBJECT

public:
    explicit ${className}(QWidget *parent = 0, bool autoInit = false, QString rootPath = QString());
    ~${className}();

    ${paramsName}* createParameters() const;
    void getParameters(${paramsName} &param) const;
    void setParameters(const ${paramsName} &input);
    virtual void setParametersVirtual(void *input);
    
    virtual void loadParamWidget(WidgetLoader &loader);
    virtual void saveParamWidget(WidgetSaver  &saver);
    
public slots:
    void changeParameters()
    {
        // emit paramsChanged();
    }

signals:
    void valueChanged();
    void paramsChanged();

private:
    Ui_${className} *mUi;
    bool autoInit;
    QString rootPath;
};

EOF