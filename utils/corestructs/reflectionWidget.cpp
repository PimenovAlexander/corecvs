#include <QGridLayout>
#include <QLabel>
#include <QSpinBox>
#include <QComboBox>
#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QTextEdit>
#include <QPushButton>

#include "core/reflection/jsonPrinter.h"

#include "pointerFieldWidget.h"
#include "core/reflection/reflection.h"
#include "reflectionWidget.h"
#include "vectorWidget.h"
#include "exponentialSlider.h"
#include "core/reflection/dynamicObject.h"

using namespace corecvs;

ReflectionWidget::ReflectionWidget(const Reflection *reflection, FieldsType type, bool addButtons, QWidget *parent) :
    ParametersControlWidgetBase(parent),
    reflection(reflection)
{
    //resize(396, 356);

    setWindowTitle(reflection->name.name);

    gridLayout = new QGridLayout(this);
    gridLayout->setSpacing(3);
    gridLayout->setContentsMargins(3, 3, 3, 3);

    fieldToPosition.resize(reflection->fields.size(), -1);

    int position = 0;
    for (int i = 0; i < (int)reflection->fields.size(); i++)
    {
        const BaseField *field = reflection->fields[i];
        if (trace) qDebug() << "Processing field:" <<  field->getSimpleName();

        if (type == TYPE_INPUT_PINS && !field->isInputPin())
            continue;
        if (type == TYPE_OUTPUT_PINS && !field->isOuputPin())
            continue;
        if (type == TYPE_PARAMS && (field->isInputPin() || field->isOuputPin()))
            continue;

        /* Column 1 */
        QString mark;
        if (field->isInputPin())
            mark += "(in)";
        if (field->isOuputPin())
            mark += "(out)";

        QLabel *markLabel = new QLabel(this);
        markLabel->setText(mark);
        markLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        gridLayout->addWidget(markLabel, i, MARK_COLUMN, 1, 1);

        QLabel *label = new QLabel(this);
        label->setText(QString(field->getSimpleName()));
        label->setToolTip(mark + QString(field->name.decription));
        gridLayout->addWidget(label, i, NAME_COLUMN, 1, 1);
        QWidget *widget = NULL;

        switch (field->type) {
            case BaseField::TYPE_INT:
            {
                const IntField *iField = static_cast<const IntField *>(field);
                QSpinBox *spinBox = new QSpinBox(this);

                if (iField->hasAdditionalValues)
                {
                    spinBox->setMinimum(iField->min);
                    spinBox->setMaximum(iField->max);

                    if (iField->step != 0) {
                        spinBox->setSingleStep(iField->step);
                    }
                    // qDebug("ReflectionWidget::Setting limits(%d %d %d)", iField->min, iField->max, iField->step);
                }
                spinBox->setValue(iField->defaultValue);
                if (iField->suffixHint != NULL)
                    spinBox->setSuffix(iField->suffixHint);
                if (iField->prefixHint != NULL)
                    spinBox->setPrefix(iField->prefixHint);

                gridLayout->addWidget(spinBox, i, WIDGET_COLUMN, 1, 1);
                connect(spinBox, SIGNAL(valueChanged(int)), this, SIGNAL(paramsChanged()));
                widget = spinBox;
                break;
            }
            case BaseField::TYPE_DOUBLE:
            {
                const DoubleField *dField = static_cast<const DoubleField *>(field);

                if (dField->widgetHint != BaseField::SLIDER)
                {
                    QDoubleSpinBox *spinBox = new QDoubleSpinBox(this);

                    if (dField->hasAdditionalValues)
                    {
                        spinBox->setMinimum(dField->min);
                        spinBox->setMaximum(dField->max);
                        spinBox->setSingleStep(dField->step);
                        // spinBox->setDecimals(); /*Not supported so far*/
                    }
                    spinBox->setDecimals(dField->precision);
                    if (dField->suffixHint != NULL)
                        spinBox->setSuffix(dField->suffixHint);
                    if (dField->prefixHint != NULL)
                        spinBox->setPrefix(dField->prefixHint);

                    spinBox->setValue(dField->defaultValue);
                    gridLayout->addWidget(spinBox, i, WIDGET_COLUMN, 1, 1);
                    connect(spinBox, SIGNAL(valueChanged(double)), this, SIGNAL(paramsChanged()));
                    widget = spinBox;
                } else {
                     ExponentialSlider *expBox = new ExponentialSlider(this);
                     if (dField->hasAdditionalValues)
                     {
                         expBox->setMaxZoom(dField->max);
                     }
                     expBox->setValue(dField->defaultValue);
                     gridLayout->addWidget(expBox, i, WIDGET_COLUMN, 1, 1);
                     connect(expBox, SIGNAL(valueChanged(double)), this, SIGNAL(paramsChanged()));
                     widget = expBox;
                }
                break;
            }
            case BaseField::TYPE_STRING:
            {
                const StringField *sField = static_cast<const StringField *>(field);
                //QTextEdit *textBox = new QTextEdit(this);
                QLineEdit *textBox = new QLineEdit(this);

                textBox->setText(QString::fromStdString(sField->defaultValue));
                gridLayout->addWidget(textBox, i, WIDGET_COLUMN, 1, 1);
                textBox->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
                //connect(textBox, SIGNAL(textChanged()), this, SIGNAL(paramsChanged()));
                connect(textBox, SIGNAL(textChanged(QString)), this, SIGNAL(paramsChanged()));
                widget = textBox;
                break;
            }
            case BaseField::TYPE_WSTRING:
            {
                const WStringField *sField = static_cast<const WStringField *>(field);
                //QTextEdit *textBox = new QTextEdit(this);
                QLineEdit *textBox = new QLineEdit(this);

                textBox->setText(QString::fromStdWString(sField->defaultValue));
                gridLayout->addWidget(textBox, i, WIDGET_COLUMN, 1, 1);
                //connect(textBox, SIGNAL(textChanged()), this, SIGNAL(paramsChanged()));
                connect(textBox, SIGNAL(textChanged(QString)), this, SIGNAL(paramsChanged()));

                widget = textBox;
                break;
            }
            case BaseField::TYPE_BOOL:
            {
                const BoolField *bField = static_cast<const BoolField *>(field);
                QCheckBox *checkBox = new QCheckBox(this);
                checkBox->setChecked(bField->defaultValue);
                gridLayout->addWidget(checkBox, i, WIDGET_COLUMN, 1, 1);
                connect(checkBox, SIGNAL(toggled(bool)), this, SIGNAL(paramsChanged()));
                widget = checkBox;
                break;
            }
            case BaseField::TYPE_ENUM:
            {
                const EnumField *eField = static_cast<const EnumField *>(field);
                const EnumReflection *enumOptions = eField->enumReflection;

                QComboBox *comboBox = new QComboBox(this);

                for(size_t enumCount = 0; enumCount < enumOptions->options.size(); enumCount++)
                {
                    const EnumOption *option = enumOptions->options[enumCount];
                    if (option->presentationHint != NULL && strlen(option->presentationHint) != 0)
                    {
                        const QString iconName = QString(option->presentationHint).split("@")[0];
                        comboBox->addItem(QIcon(iconName), option->name.name);

                    } else {
                        comboBox->addItem(option->name.name);
                    }
                }
                comboBox->setCurrentIndex(eField->defaultValue);
                gridLayout->addWidget(comboBox, i, WIDGET_COLUMN, 1, 1);
                connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SIGNAL(paramsChanged()));
                widget = comboBox;
                break;
            }
            /* Two Vector fields*/
            /*case BaseField::TYPE_INT | BaseField::TYPE_VECTOR_BIT:
            {
                const IntField *iField = static_cast<const IntField *>(field);
                QSpinBox *spinBox = new QSpinBox(this);

                if (iField->hasAdditionalValues)
                {
                    spinBox->setMinimum(iField->min);
                    spinBox->setMaximum(iField->max);
                    spinBox->setSingleStep(iField->step);
                }
                spinBox->setValue(iField->defaultValue);
                layout->addWidget(spinBox, i, 1, 1, 1);
                connect(spinBox, SIGNAL(valueChanged(int)), this, SIGNAL(paramsChanged()));
                break;
            }*/
            case BaseField::TYPE_DOUBLE | BaseField::TYPE_VECTOR_BIT:
            {
                const DoubleVectorField *dField = static_cast<const DoubleVectorField *>(field);
                DoubleVectorWidget *vectorWidget = new DoubleVectorWidget(this);

                if (dField->hasAdditionalValues)
                {
                    vectorWidget->setMinimum(dField->min);
                    vectorWidget->setMaximum(dField->max);
                    // vectorWidget->setSingleStep(dField->step);
                    // spinBox->setDecimals(); /*Not supported so far*/
                }
                vectorWidget->setValue(dField->defaultValue);
                gridLayout->addWidget(vectorWidget, i, WIDGET_COLUMN, 1, 1);
                connect(vectorWidget, SIGNAL(valueChanged()), this, SIGNAL(paramsChanged()));
                widget = vectorWidget;
                break;
            }

            /* Composite field */
            case BaseField::TYPE_COMPOSITE:
            {
                const CompositeField *cField = static_cast<const CompositeField *>(field);
                const Reflection *subReflection = cField->reflection;
                if (subReflection != NULL) {
                    ReflectionWidget *refWidget = new ReflectionWidget(subReflection);
                    gridLayout->addWidget(refWidget, i, WIDGET_COLUMN, 1, 1);
                    connect(refWidget, SIGNAL(paramsChanged()), this, SIGNAL(paramsChanged()));
                    widget = refWidget;
                } else {
                    qDebug() << "There is no known reflection for: " <<  cField->name.name;
                }
                break;
            }

            /* Composite field */
            case BaseField::TYPE_POINTER:
            {
    #if 0
                const PointerField *pField = static_cast<const PointerField *>(field);
                QString targetName = pField->name.name;

                QPushButton *buttonWidget = new QPushButton(this);
                layout->addWidget(buttonWidget, i, WIDGET_COLUMN, 1, 1);
                buttonWidget->setEnabled(false);
                buttonWidget->setText(targetName);

                //connect(buttonWidget, SIGNAL(paramsChanged()), this, SIGNAL(paramsChanged()));
                widget = buttonWidget;
    #endif
                const PointerField *pField = static_cast<const PointerField *>(field);
                QString targetName = pField->name.name;

                PointerFieldWidget *pointerWidget = new PointerFieldWidget(pField, this);
                gridLayout->addWidget(pointerWidget, i, WIDGET_COLUMN, 1, 1);
                pointerWidget->setValue(NULL);
                widget = pointerWidget;
                break;
            }

            /* Well something is not supported */
            default:
                QLabel *label = new QLabel(this);
                label->setText(QString("NOT SUPPORTED"));
                gridLayout->addWidget(label, i, WIDGET_COLUMN, 1, 1);
                break;
        }

        positionToWidget.push_back(widget);
        positionToField.push_back(i);
        fieldToPosition[i] = position;
        position++;

    }

    QSpacerItem *spacer = new QSpacerItem(0, 20, QSizePolicy::Expanding, QSizePolicy::Expanding);
    gridLayout->addItem(spacer, (int)reflection->fields.size(), 0, 1, 2);

/*    if (reflection->isActionBlock()) {
        QPushButton *executeButton = new QPushButton(this);
        executeButton->setIcon(QIcon(":/new/prefix1/lightning.png"));
        executeButton->setText("Execute");
        layout->addWidget(executeButton, layout->rowCount(), 1, 1, 1);
        connect(executeButton, SIGNAL(released()), this, SIGNAL(executeCalled()));
    }*/

    if (addButtons)
    {
        QGridLayout *buttonLayout = new QGridLayout(this);

        QPushButton *loadButton = new QPushButton(this);
        loadButton->setIcon(QIcon(":/new/prefix1/remove.png"));
        loadButton->setText("Load");
        buttonLayout->addWidget(loadButton, 0, 0, 1, 1);
        connect(loadButton, SIGNAL(released()), this, SLOT(loadCalled()));

        QPushButton *saveButton = new QPushButton(this);
        saveButton->setIcon(QIcon(":/new/prefix1/install.png"));
        saveButton->setText("Save");
        buttonLayout->addWidget(saveButton, 0, 1, 1, 1);
        connect(saveButton, SIGNAL(released()), this, SLOT(saveCalled()));

        QPushButton *resetButton = new QPushButton(this);
        resetButton->setIcon(QIcon(":/new/prefix1/bullet_blue.png"));
        resetButton->setText("Reset");
        buttonLayout->addWidget(resetButton, 0, 2, 1, 1);
        connect(resetButton, SIGNAL(released()), this, SLOT(resetCalled()));

        gridLayout->addLayout(buttonLayout, gridLayout->rowCount(), 1, 1, 3);
    }

    setLayout(gridLayout);
    // qDebug() << "Reflection had: " << reflection->fields.size();
    // qDebug() << "Widget has positions: " << position;


}

BaseReflectionStatic *ReflectionWidget::createParametersVirtual() const
{
    DynamicObjectWrapper obj;
    obj.reflection = reflection;
    obj.rawObject = malloc(reflection->objectSize);
    obj.simulateConstructor();
    getParameters(obj.rawObject);
    return static_cast<BaseReflectionStatic *>(obj.rawObject);
}

bool ReflectionWidget::getParameters(void *param) const
{
    DynamicObjectWrapper obj(reflection, param);

    for (size_t i = 0; i < positionToField.size(); i++)
    {
        int fieldId = positionToField[i];
        const BaseField *field = reflection->fields[fieldId];
        if (trace) qDebug() << "Processing field:" <<  field->getSimpleName() << " type " << field->type;

        switch (field->type) {
            case BaseField::TYPE_INT:
            {
                // const IntField *iField = static_cast<const IntField *>(field);
                QSpinBox *spinBox = static_cast<QSpinBox *>(positionToWidget[i]);
                *obj.getField<int>(fieldId) = spinBox->value();
                break;
            }
            case BaseField::TYPE_DOUBLE:
            {
                // qDebug() << "Processing double field" << endl;
                const DoubleField *dField = static_cast<const DoubleField *>(field);
                if (dField->widgetHint != BaseField::SLIDER)
                {
                    QDoubleSpinBox *spinBox = static_cast<QDoubleSpinBox *>(positionToWidget[i]);
                    *obj.getField<double>(fieldId) = spinBox->value();
                } else {
                    ExponentialSlider *expBox = static_cast<ExponentialSlider *>(positionToWidget[i]);
                    *obj.getField<double>(fieldId) = expBox->value();
                }
                break;
            }
            case BaseField::TYPE_STRING:
            {
                // const StringField *sField = static_cast<const StringField *>(field);
                //QTextEdit *textBox = static_cast<QTextEdit *>(positionToWidget[i]);
                //*obj.getField<std::string>(fieldId) = textBox->toPlainText().toStdString();

                 QLineEdit *textBox = static_cast<QLineEdit *>(positionToWidget[i]);
                 std::string *targetStr = obj.getField<std::string>(fieldId);
                 std::string value = textBox->text().toStdString();
                 // cout << "Obj address:" << obj.rawObject << std::endl;
                 // cout << "Target address:" << targetStr << std::endl;
                 // cout << "Old target value:" << (*targetStr) << std::endl;
                 // cout << "ReflectionWidget::getParameters(): We have parameter <" << value << ">" << std::endl;
                *targetStr = value;
                break;
            }
            case BaseField::TYPE_WSTRING:
            {
                // const StringField *sField = static_cast<const StringField *>(field);
                // QTextEdit *textBox = static_cast<QTextEdit *>(positionToWidget[i]);
                // *obj.getField<std::wstring>(fieldId) = textBox->toPlainText().toStdWString();

                QLineEdit *textBox = static_cast<QLineEdit *>(positionToWidget[i]);
                *obj.getField<std::wstring>(fieldId) = textBox->text().toStdWString();

                break;
            }
            case BaseField::TYPE_BOOL:
            {
                QCheckBox *checkBox = static_cast<QCheckBox *>(positionToWidget[i]);
                *obj.getField<bool>(fieldId) = checkBox->isChecked();
                break;
            }
            case BaseField::TYPE_ENUM:
            {
                // const EnumField *eField = static_cast<const EnumField *>(field);
                QComboBox *comboBox = static_cast<QComboBox *>(positionToWidget[i]);
                *obj.getField<int>(fieldId) = comboBox->currentIndex();
                break;
            }
            /* Two Vector fields*/
            /*case BaseField::TYPE_INT | BaseField::TYPE_VECTOR_BIT:
            {
                const IntField *iField = static_cast<const IntField *>(field);
                break;
            }*/
            case BaseField::TYPE_DOUBLE | BaseField::TYPE_VECTOR_BIT:
            {
                // const DoubleVectorField *dField = static_cast<const DoubleVectorField *>(field);
                DoubleVectorWidget *vectorWidget = static_cast<DoubleVectorWidget *>(positionToWidget[i]);
                *obj.getField<vector<double>>(fieldId) = vectorWidget->value();
                break;
            }

            /* Composite field */
            case BaseField::TYPE_COMPOSITE:
            {
                const CompositeField *cField = static_cast<const CompositeField *>(field);
                // const Reflection *subReflection = cField->reflection;
                ReflectionWidget *refWidget = static_cast<ReflectionWidget *>(positionToWidget[i]);

                if (refWidget != NULL) {
                    void *targetObj = obj.getField<void *>(fieldId);
                    cout << "In composite field:" << targetObj << std::endl;
                    refWidget->getParameters(targetObj);
                } else {
                    qDebug() << "There is no widget for" << cField->getSimpleName();
                }
                break;
            }

            case BaseField::TYPE_POINTER:
            {
                // const PointerField *pField = static_cast<const PointerField *>(field);
                PointerFieldWidget *refWidget = static_cast<PointerFieldWidget *>(positionToWidget[i]);
                *obj.getField<void *>(fieldId) = refWidget->rawPointer;
                break;
            }

            /* Well something is not supported */
            default:
                break;
        }
    }

    return true;
}

bool ReflectionWidget::setParameters(void *param) const
{
    DynamicObjectWrapper obj(reflection, param);
    //SYNC_PRINT(("ReflectionWidget::setParameters(): called for %s\n", reflection->name.name));
    // obj.printRawObject();

    for (size_t i = 0; i < positionToField.size(); i++)
    {
        int fieldId = positionToField[i];

        const BaseField *field = reflection->fields[fieldId];
        if (trace) qDebug() << "Processing field:" <<  field->getSimpleName();

        switch (field->type) {
            case BaseField::TYPE_INT:
            {
                QSpinBox *spinBox = static_cast<QSpinBox *>(positionToWidget[i]);
                spinBox->setValue(*obj.getField<int>(fieldId));
                break;
            }
            case BaseField::TYPE_DOUBLE:
            {
                const DoubleField *dField = static_cast<const DoubleField *>(field);
                if (dField->widgetHint != BaseField::SLIDER)
                {
                    QDoubleSpinBox *spinBox = static_cast<QDoubleSpinBox *>(positionToWidget[i]);
                    spinBox->setValue(*obj.getField<double>(fieldId));
                } else {
                    ExponentialSlider *expBox = static_cast<ExponentialSlider *>(positionToWidget[i]);
                    expBox->setValue(*obj.getField<double>(fieldId));
                }
                break;
            }
            case BaseField::TYPE_STRING:
            {
                //QTextEdit *textBox = static_cast<QTextEdit *>(positionToWidget[i]);
                QLineEdit *textBox = static_cast<QLineEdit *>(positionToWidget[i]);
                textBox->setText(QString::fromStdString(*obj.getField<std::string>(fieldId)));
                break;
            }
            case BaseField::TYPE_WSTRING:
            {
                //QTextEdit *textBox = static_cast<QTextEdit *>(positionToWidget[i]);
                QLineEdit *textBox = static_cast<QLineEdit *>(positionToWidget[i]);
                textBox->setText(QString::fromStdWString(*obj.getField<std::wstring>(fieldId)));
                break;
            }
            case BaseField::TYPE_BOOL:
            {
                QCheckBox *checkBox = static_cast<QCheckBox *>(positionToWidget[i]);
                checkBox->setChecked(*obj.getField<bool>(fieldId));
                break;
            }
            case BaseField::TYPE_ENUM:
            {
                QComboBox *comboBox = static_cast<QComboBox *>(positionToWidget[i]);
                comboBox->setCurrentIndex(*obj.getField<int>(fieldId));
                break;
            }
            /* Two Vector fields*/
            /*case BaseField::TYPE_INT | BaseField::TYPE_VECTOR_BIT:
            {
                const IntField *iField = static_cast<const IntField *>(field);
                break;
            }*/
            case BaseField::TYPE_DOUBLE | BaseField::TYPE_VECTOR_BIT:
            {
                //DoubleVectorWidget *vectorWidget = static_cast<DoubleVectorWidget *>(positionToWidget[i]);

                break;
            }

            /* Composite field */
            case BaseField::TYPE_COMPOSITE:
            {
                const CompositeField *cField = static_cast<const CompositeField *>(field);
                ReflectionWidget *refWidget = static_cast<ReflectionWidget *>(positionToWidget[i]);
                if (refWidget != NULL) {
                    refWidget->setParameters(obj.getField<void *>(fieldId));
                } else {
                     qDebug() << "There is no widget for" << cField->getSimpleName();
                }
                break;
            }

            case BaseField::TYPE_POINTER:
            {
               // const PointerField *pField = static_cast<const PointerField *>(field);
                PointerFieldWidget *pointerWidget = static_cast<PointerFieldWidget *>(positionToWidget[i]);
                void *rawPointer = (*obj.getField<void *>(fieldId));
                pointerWidget->setValue(rawPointer);
                break;
            }


            /* Well something is not supported */
            default:
                break;
        }
    }

    return true;
}


void ReflectionWidget::loadCalled()
{
    qDebug() << "ReflectionWidget::loadParams(): called";
    QString filename = QFileDialog::getOpenFileName(
                this,
                "Choose an file name",
                ".",
                "json params (*.json)"
                );

    if (!filename.isEmpty())
    {
        DynamicObject obj(reflection);
        JSONGetter getter(filename);
        getter.visit(obj, "params");
        setParameters(obj.rawObject);
        /*WidgetLoader loader(&getter);
        loadParamWidget(loader);*/
    }
}

void ReflectionWidget::saveCalled()
{
    qDebug() << "ReflectionWidget::saveParams(): called";
    QString filename = QFileDialog::getSaveFileName(
                this,
                "Choose an file name",
                ".",
                "json params (*.json)"
                );
    if (!filename.isEmpty())
    {
        DynamicObject obj(reflection);
        getParameters(obj.rawObject);
        std::ofstream file;
        file.open(filename.toStdString(), std::ofstream::out);
        JSONPrinter setter(file);
        setter.visit(obj, "params");
    }

}

void ReflectionWidget::resetCalled()
{
    qDebug() << "ReflectionWidget::resetCalled(): called";
    DynamicObject obj(reflection);
    setParameters(obj.rawObject);
}

