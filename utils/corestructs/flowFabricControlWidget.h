#ifndef FLOWFABRICCONTROLWIDGET_H
#define FLOWFABRICCONTROLWIDGET_H

#include "reflectionWidget.h"

#include <QPushButton>
#include <QWidget>
#include <qcombobox.h>
#include <qlabel.h>

#include <ui_flowFabricControlWidget.h>
#include <core/stereointerface/processor6D.h>

namespace Ui {
class FlowFabricControlWidget;
}







class FlowFabricControlWidget : public QWidget
{
    Q_OBJECT

public:  
    ~FlowFabricControlWidget();

private:
    Ui::FlowFabricControlWidget *mUi;
    QComboBox *hintBox = NULL;

/**/
public:
    explicit FlowFabricControlWidget(QWidget *parent = 0, bool autoInit = false, QString rootPath = QString()):
        QWidget(parent),
        mUi(new Ui::FlowFabricControlWidget)
    {
        mUi->setupUi(this);

        QGridLayout *layout = mUi->gridLayout;

        std::vector<std::string> hints;
        hints = ProcessorFlowFactoryHolder::getInstance()->getHints();

        hintBox = new QComboBox;
        for (std::string hint: hints)
        {
            QString str = QString::fromStdString(hint);
            hintBox->addItem(str);
            knownParameters.emplace_back(str);
            SYNC_PRINT(("FlowFabricControlWidget(): %s provider added\n", str.toStdString().c_str()));
        }
        connect(hintBox, SIGNAL(activated(QString)), mUi->algorithmEdit, SLOT(setText(QString)));
        connect(hintBox, SIGNAL(activated(QString)), this, SLOT(activateReflectionWidget()));

        layout->addWidget(hintBox, 0, 2);

        QPushButton *params = new QPushButton("Params");
        layout->addWidget(params, 0, 3);
        connect(params, SIGNAL(released()), this, SLOT(activateReflectionWidget()));

        QPushButton *toDefaults = new QPushButton("Set To Defaults");
        layout->addWidget(toDefaults, 0, 4);
        connect(toDefaults, SIGNAL(released()), this, SLOT(activateReflectionWidget()));
    }


    struct ParameterSubset {
        QString name;
        ReflectionWidget *parameterWidget = NULL;
        const Reflection *reflection = NULL;

        void setNewObject(const std::string &newName, const DynamicObject& obj)
        {
            name = QString::fromStdString(newName);

            delete_safe(parameterWidget);
            parameterWidget = new ReflectionWidget(obj.reflection);
            reflection = obj.reflection;

            // DynamicObjectWrapper(obj.reflection, obj.rawObject).printRawObject();
            parameterWidget->setParameters(obj.rawObject);
            parameterWidget->show();
            parameterWidget->raise();
        }

    };

    struct ParameterSet {
        QString provider;
        std::vector<ParameterSubset *> set;
        QWidget *widget;

        ParameterSet(QString provider)
        {
            widget = new QWidget;
            QGridLayout *layout = new QGridLayout();
            widget->setLayout(layout);

            this->provider = provider;
            ProcessorFlow *processor = Processor6DFactoryHolder::getInstance()->getProcessor(provider.toStdString());
            if (processor == NULL) {
                SYNC_PRINT(("Requested processor is not avaliable\n"));
                return;
            }

            std::map<std::string, DynamicObject> params = processor->getParameters();
            if (params.empty()) {
                SYNC_PRINT(("There are no parameters\n"));
                return;
            } else {
                SYNC_PRINT(("There are %" PRISIZE_T " parameters\n", params.size()));
            }

            for (auto &it : params)
            {
                ParameterSubset *newSubset = new ParameterSubset;
                set.push_back(newSubset);
                newSubset->setNewObject(it.first, it.second);
                layout->addWidget(new QLabel(QString::fromStdString(it.first)), layout->rowCount(), 0 );
                layout->addWidget(newSubset->parameterWidget,                   layout->rowCount(), 1);
            }
            //widget->show();
            //widget->raise();
        }

    };

    std::vector<ParameterSet> knownParameters;

    std::map<std::string, DynamicObject> getCurrentDynamicParameters()
    {
        std::map<std::string, DynamicObject> result;
        QString str = hintBox->currentText();
        for (ParameterSet& item : knownParameters)
        {
            if (item.provider == str)
            {
                for (ParameterSubset *sub : item.set)
                {
                    result.emplace(sub->name.toStdString(), DynamicObject(sub->reflection));
                    sub->parameterWidget->getParameters(result[sub->name.toStdString()].rawObject);
                }

                break;
            }
        }
        return result;
    }


public slots:

    void activateReflectionWidget()
    {
        SYNC_PRINT(("GenericStereoMatchingParamsControlWidgetEx::activateReflectionWidget():called\n"));
        QString str = hintBox->currentText();

        SYNC_PRINT(("GenericStereoMatchingParamsControlWidgetEx::activateReflectionWidget(): provider: <%s> \n", str.toLatin1().constData()));

        for (ParameterSet& item : knownParameters)
        {
            if (item.provider == str)
            {
                item.widget->show();
                item.widget->raise();
                return;
            }
        }
    }

    virtual void loadParamWidget(WidgetLoader &loader)
    {
        SYNC_PRINT(("GenericStereoMatchingParamsControlWidgetEx::loadParamWidget(): called\n"));
        //GenericStereoMatchingParamsControlWidget::loadParamWidget(loader);

        SYNC_PRINT(("GenericStereoMatchingParamsControlWidgetEx::loadParamWidget(): there are %" PRISIZE_T " providers\n", knownParameters.size()));
        for (ParameterSet& item : knownParameters)
        {
            SYNC_PRINT(("GenericStereoMatchingParamsControlWidgetEx::loadParamWidget(): loading %s provider %" PRISIZE_T " params\n", item.provider.toUtf8().constData(), item.set.size()));

            for (ParameterSubset *subset : item.set)
            {
                DynamicObject obj(subset->reflection);
                subset->parameterWidget->getParameters(obj.rawObject);
                loader.loadParameters(obj, item.provider + "/" + subset->name);
                subset->parameterWidget->setParameters(obj.rawObject);
            }
        }
    }

    virtual void saveParamWidget(WidgetSaver  &saver)
    {
        SYNC_PRINT(("GenericStereoMatchingParamsControlWidgetEx::saveParamWidget(): called\n"));
        //GenericStereoMatchingParamsControlWidget::saveParamWidget(saver);
        for (ParameterSet& item : knownParameters)
        {
             for (ParameterSubset *subset : item.set)
             {
                 DynamicObject obj(subset->reflection);
                 subset->parameterWidget->getParameters(obj.rawObject);
                 saver.saveParameters(obj, item.provider + "/" + subset->name);
             }
        }
    }




};

#endif // FLOWFABRICCONTROLWIDGET_H
