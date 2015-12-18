#include <QtCore/QtCore>
/**
 * \file qtHelper.cpp
 *
 * \date Apr 27, 2013
 **/

#include "qtHelper.h"

using corecvs::Vector2dd;
using corecvs::Vector3dd;

QDebug & operator<< (QDebug & stream, const Vector2dd & vector)
{
    stream << "[" << vector.x() << "," << vector.y() << "]";
    return stream;
}

QDebug & operator<< (QDebug & stream, const Vector3dd & vector)
{
    stream << "[" << vector.x() << "," << vector.y() << "," << vector.z() << "]";

    return stream;
}

void setValueBlocking(QDoubleSpinBox *box, double value)
{
    bool wasBlocked = box->blockSignals(true);
    box->setValue(value);
    box->blockSignals(wasBlocked);
}


QDebug &operator<<(QDebug &stream, const corecvs::Vector2d<int> &vector)
{
    stream << "[" << vector.x() << "," << vector.y() << "]";
    return stream;
}

/* This class uses some of Qt Examples code*/
QString printWindowFlags(const Qt::WindowFlags &flags)
{
    QString text;

    Qt::WindowFlags type = (flags & Qt::WindowType_Mask);
    if (type == Qt::Window) {
        text = "Qt::Window";
    } else if (type == Qt::Dialog) {
        text = "Qt::Dialog";
    } else if (type == Qt::Sheet) {
        text = "Qt::Sheet";
    } else if (type == Qt::Drawer) {
        text = "Qt::Drawer";
    } else if (type == Qt::Popup) {
        text = "Qt::Popup";
    } else if (type == Qt::Tool) {
        text = "Qt::Tool";
    } else if (type == Qt::ToolTip) {
        text = "Qt::ToolTip";
    } else if (type == Qt::SplashScreen) {
        text = "Qt::SplashScreen";
    }

    if (flags & Qt::MSWindowsFixedSizeDialogHint)
        text += "\n| Qt::MSWindowsFixedSizeDialogHint";
    if (flags & Qt::X11BypassWindowManagerHint)
        text += "\n| Qt::X11BypassWindowManagerHint";
    if (flags & Qt::FramelessWindowHint)
        text += "\n| Qt::FramelessWindowHint";
    if (flags & Qt::NoDropShadowWindowHint)
        text += "\n| Qt::NoDropShadowWindowHint";
    if (flags & Qt::WindowTitleHint)
        text += "\n| Qt::WindowTitleHint";
    if (flags & Qt::WindowSystemMenuHint)
        text += "\n| Qt::WindowSystemMenuHint";
    if (flags & Qt::WindowMinimizeButtonHint)
        text += "\n| Qt::WindowMinimizeButtonHint";
    if (flags & Qt::WindowMaximizeButtonHint)
        text += "\n| Qt::WindowMaximizeButtonHint";
    if (flags & Qt::WindowCloseButtonHint)
        text += "\n| Qt::WindowCloseButtonHint";
    if (flags & Qt::WindowContextHelpButtonHint)
        text += "\n| Qt::WindowContextHelpButtonHint";
    if (flags & Qt::WindowShadeButtonHint)
        text += "\n| Qt::WindowShadeButtonHint";
    if (flags & Qt::WindowStaysOnTopHint)
        text += "\n| Qt::WindowStaysOnTopHint";
    if (flags & Qt::CustomizeWindowHint)
        text += "\n| Qt::CustomizeWindowHint";

    return text;
}
