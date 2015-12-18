#include <QtCore/QTextStream>
#include <QtCore/QFile>
#include <QPushButton>
#include <QComboBox>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QCheckBox>
#include <QtXml/QDomDocument>

#include "serializer.h"

const QString pathToFile        = "../tools/serializer/";
const QString pushButtonName    = "PushButton";
const QString comboBoxName      = "ComboBox";
const QString spinBoxName       = "SpinBox";
const QString doubleSpinBoxName = "DoubleSpinBox";
const QString checkBoxName      = "CheckBox";
const QString nameAttribute     = "name";

void Serializer::serialize(const QWidget &widget)
{
    QString const pathToXml = getPath(widget);
    QFile file(pathToXml);
    if (!file.open(QFile::ReadWrite))
    {
        return;
    }
    QTextStream textStream(&file);
    QDomDocument document;
    QDomElement root = document.createElement("WidgetsParameters");
    document.appendChild(root);
    QList<QPushButton *> pushButtons = widget.findChildren<QPushButton *>();
    foreach (QPushButton * button, pushButtons)
    {
        QDomElement element = document.createElement(pushButtonName);
        element.setAttribute("isDown", button->isDown());
        element.setAttribute(nameAttribute, button->objectName());
        root.appendChild(element);
    }
    QList<QComboBox *> comboBoxes = widget.findChildren<QComboBox *>();
    foreach (QComboBox * cBox, comboBoxes)
    {
        QDomElement element = document.createElement(comboBoxName);
        element.setAttribute(nameAttribute, cBox->objectName());
        element.setAttribute("index", cBox->currentIndex());
        root.appendChild(element);
    }
    QList<QSpinBox *> spinBoxes = widget.findChildren<QSpinBox *>();
    foreach (QSpinBox * sBox, spinBoxes)
    {
        QDomElement element = document.createElement(spinBoxName);
        element.setAttribute(nameAttribute, sBox->objectName());
        element.setAttribute("value", sBox->value());
        root.appendChild(element);
    }
    QList<QDoubleSpinBox *> doubleSpinBoxes = widget.findChildren<QDoubleSpinBox *>();
    foreach (QDoubleSpinBox * dsBox, doubleSpinBoxes)
    {
        QDomElement element = document.createElement(doubleSpinBoxName);
        element.setAttribute(nameAttribute, dsBox->objectName());
        element.setAttribute("value", dsBox->value());
        root.appendChild(element);
    }
    QList<QCheckBox *> checkBoxes = widget.findChildren<QCheckBox *>();
    foreach (QCheckBox * cBox, checkBoxes)
    {
        QDomElement element = document.createElement(checkBoxName);
        element.setAttribute(nameAttribute, cBox->objectName());
        element.setAttribute("isChecked", cBox->isChecked());
        root.appendChild(element);
    }
    document.save(textStream, 2);
    file.close();
}

void Serializer::deserialize(QWidget * widget)
{
    QFile file(getPath(*widget));

    if (!file.exists() || !file.open(QIODevice::ReadWrite))
        return;
    QDomDocument doc("document");
    doc.setContent(&file);
    file.close();
    bool isInt = false;
    QDomNodeList pushButtonElements = doc.elementsByTagName(pushButtonName);
    for (int i = 0; i < pushButtonElements.size(); i++)
    {
        QDomElement element = pushButtonElements.at(i).toElement();
        QString name = element.attribute(nameAttribute, "");
        QPushButton * button = widget->findChild<QPushButton *>(name);
        QString isDown = element.attribute("isDown", "");
        button->setDown(isDown == "true");
    }
    QDomNodeList comboBoxElements = doc.elementsByTagName(comboBoxName);
    for (int i = 0; i < comboBoxElements.size(); i++)
    {
        QDomElement element = comboBoxElements.at(i).toElement();
        QString name = element.attribute(nameAttribute, "");
        QComboBox * cBox = widget->findChild<QComboBox *>(name);
        int index = element.attribute("index", "0").toInt(&isInt, 0);
        cBox->setCurrentIndex(index);
    }
    QDomNodeList spinBoxElements = doc.elementsByTagName(spinBoxName);
    for (int i = 0; i < spinBoxElements.size(); i ++)
    {
        QDomElement element = spinBoxElements.at(i).toElement();
        QString name = element.attribute(nameAttribute, "no such attribute");
        QSpinBox * sBox = widget->findChild<QSpinBox *>(name);
        int value = element.attribute("value", "0").toInt(&isInt, 0);
        sBox->setValue(value);
    }
    QDomNodeList doubleSpinBoxElements = doc.elementsByTagName(doubleSpinBoxName);
    for (int i = 0; i < doubleSpinBoxElements.size(); i ++)
    {
        QDomElement element = doubleSpinBoxElements.at(i).toElement();
        QString name = element.attribute(nameAttribute, "no such attribute");
        QDoubleSpinBox * sBox = widget->findChild<QDoubleSpinBox *>(name);
        int value = element.attribute("value", "0").toInt(&isInt, 0);
        sBox->setValue(value);
    }
    QDomNodeList checkBoxElements = doc.elementsByTagName(checkBoxName);
    for (int i = 0; i < checkBoxElements.size(); i ++)
    {
        QDomElement element = checkBoxElements.at(i).toElement();
        QString name = element.attribute(nameAttribute, "");
        QCheckBox * cBox = widget->findChild<QCheckBox *>(name);
        QString isChecked = element.attribute("isChecked", "");
        cBox->setChecked(isChecked == "1");
    }
}

QString Serializer::getPath(const QWidget &widget)
{
    return pathToFile + widget.objectName() + "Data.xml";
}
