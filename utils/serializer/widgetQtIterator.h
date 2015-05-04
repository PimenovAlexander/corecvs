#pragma once
/**
 * \file widgetQtIterator.h
 *
 * \date Mar 18, 2013
 **/
#include <QWidget>
#include <QtCore/qstring.h>
#include <QLabel>
#include <QCheckBox>
#include <QSpinBox>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QGroupBox>
#include <QtCore/QVariant>
#include <QtCore/QDebug>

class WidgetQtIterator
{
public:

    static bool isAdvanced(QObject *object)
    {
        QVariant prop = object->property("tags");
        if (!prop.isNull())
        {
            QString string = prop.toString();
//            qDebug() << "tags: " << string;
            if (string.contains("advanced"))
                return true;
        }
        return false;
    }

    static QString print (QWidget *widget, QString prefix)
    {
        QString output;

        const char *name = widget->metaObject()->className();
        output += prefix + name + " " + (isAdvanced(widget) ? "advanced" : "ordinary");
        output += "\n";

        bool isLeaf = false;
        if (widget->inherits("QLabel"))
        {
            QLabel *label = static_cast<QLabel *>(widget);
            output += prefix + "   Label:" + label->text() + "\n";
            isLeaf = true;
        }
        if (widget->inherits("QComboBox"))
        {
            QComboBox *box = static_cast<QComboBox *>(widget);
            output += prefix + "   QComboBox:" + box->itemText(0) + "\n";
            isLeaf = true;
        }
        if (widget->inherits("QCheckBox"))
        {
            QCheckBox *box = static_cast<QCheckBox *>(widget);
            output += prefix + "   QCheckBox:" + box->text() + " " + (box->isChecked() ? "true" : "false") + "\n";
            isLeaf = true;
        }
        if (widget->inherits("QSpinBox"))
        {
            QSpinBox *box = static_cast<QSpinBox *>(widget);
            output += prefix + "   QSpinBox:" + QString::number(box->value()) + "\n";
            isLeaf = true;
        }
        if (widget->inherits("QDoubleSpinBox"))
        {
            QDoubleSpinBox *box = static_cast<QDoubleSpinBox *>(widget);
            output += prefix + "   QDoubleSpinBox:" + QString::number(box->value()) + "\n";
            isLeaf = true;
        }
        if (widget->inherits("QGroupBox"))
        {
            //QGroupBox *group = static_cast<QGroupBox *>(widget);
            output += prefix + "   QGroupBox:" + "\n";
        }

        QObjectList children = widget->children();
        if (!children.isEmpty() && !isLeaf )
        {
            output += prefix + "Children:\n";
            int num = 0;
            foreach(QObject* object, children)
            {
                if (!object->isWidgetType())
                    continue;

                num++;

                output += prefix + " " + QString::number(num) + " [" + QString::number(children.size()) + "]\n";

                output += print(static_cast<QWidget *>(object), prefix + "--->");

            }
        }

        return output;
    }

    static void advancedVisibility (QWidget *widget, bool off)
    {
        if (isAdvanced(widget))
            widget->setVisible(!off);

        bool isLeaf = widget->inherits("QLabel")
                || widget->inherits("QComboBox")
                || widget->inherits("QCheckBox")
                || widget->inherits("QSpinBox")
                || widget->inherits("QDoubleSpinBox");

        QObjectList children = widget->children();
        if (!children.isEmpty() && !isLeaf )
        {
            foreach(QObject* object, children)
            {
                if (!object->isWidgetType())
                    continue;

                advancedVisibility(static_cast<QWidget *>(object), off);
            }
        }
    }

};

/* EOF */
