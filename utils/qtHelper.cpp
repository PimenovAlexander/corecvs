/**
 * \file qtHelper.cpp
 *
 * \date Apr 27, 2013
 **/
#include "qtHelper.h"
#include <QtCore/QtCore>

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

QString printWindowState(const Qt::WindowStates &state)
{
    QString text;
    if (state == Qt::WindowNoState)
        text +=" Qt::WindowNoState";	//The window has no state set (in normal state).
    if (state & Qt::WindowMinimized)
        text +=" Qt::WindowMinimized";	//The window is minimized (i.e. iconified).
    if (state & Qt::WindowMaximized)
        text +=" Qt::WindowMaximized";	//The window is maximized with a frame around it.
    if (state & Qt::WindowFullScreen)
        text +=" Qt::WindowFullScreen";	//The window fills the entire screen without any frame around it.
    if (state & Qt::WindowActive)
        text +=" Qt::WindowActive";	//The window is the active window, i.e. it has keyboard focus.

    return text;
}

QString printSelectionModel(const QItemSelectionModel::SelectionFlags &flag)
{
    QString text;

    if ( flag == QItemSelectionModel::NoUpdate)	text = "No selection";

    if (flag & QItemSelectionModel::Clear	) text += "Cleared.";
    if (flag & QItemSelectionModel::Select	) text += "Select.";
    if (flag & QItemSelectionModel::Deselect) text += "deselected.";
    if (flag & QItemSelectionModel::Toggle  ) text += "selected or deselected depending on their current state.";
    if (flag & QItemSelectionModel::Current ) text += "The current selection will be updated.";
    if (flag & QItemSelectionModel::Rows    ) text += "All indices will be expanded to span rows.";
    if (flag & QItemSelectionModel::Columns ) text += "All indices will be expanded to span columns.";

    return text;
}

QString printWidgetAttributes(QWidget *widget)
{
    QString text;

    struct AttrName {
        Qt::WidgetAttribute attr;
        const char *name;
    };

#define PAIR(X) { X, #X }

    AttrName attrs[] = {

    PAIR(Qt::WA_AcceptDrops),
    PAIR(Qt::WA_AlwaysShowToolTips),
    PAIR(Qt::WA_ContentsPropagated),
    PAIR(Qt::WA_CustomWhatsThis),
    PAIR(Qt::WA_DeleteOnClose),
    PAIR(Qt::WA_Disabled),
    PAIR(Qt::WA_DontShowOnScreen),
    PAIR(Qt::WA_ForceDisabled),
    PAIR(Qt::WA_ForceUpdatesDisabled),
    PAIR(Qt::WA_GroupLeader),
    PAIR(Qt::WA_Hover),
    PAIR(Qt::WA_InputMethodEnabled),
    PAIR(Qt::WA_KeyboardFocusChange),
    PAIR(Qt::WA_KeyCompression),
    PAIR(Qt::WA_LayoutOnEntireRect),
    PAIR(Qt::WA_LayoutUsesWidgetRect),
    PAIR(Qt::WA_MacNoClickThrough),
    PAIR(Qt::WA_MacOpaqueSizeGrip),
    PAIR(Qt::WA_MacShowFocusRect),
    PAIR(Qt::WA_MacNormalSize),
    PAIR(Qt::WA_MacSmallSize),
    PAIR(Qt::WA_MacMiniSize),
    PAIR(Qt::WA_MacVariableSize),
    PAIR(Qt::WA_MacBrushedMetal),
    PAIR(Qt::WA_Mapped),
    PAIR(Qt::WA_MouseNoMask),
    PAIR(Qt::WA_MouseTracking),
    PAIR(Qt::WA_Moved),
    PAIR(Qt::WA_MSWindowsUseDirect3D),
    PAIR(Qt::WA_NoBackground),
    PAIR(Qt::WA_NoChildEventsForParent),
    PAIR(Qt::WA_NoChildEventsFromChildren),
    PAIR(Qt::WA_NoMouseReplay),
    PAIR(Qt::WA_NoMousePropagation),
    PAIR(Qt::WA_TransparentForMouseEvents),
    PAIR(Qt::WA_NoSystemBackground),
    PAIR(Qt::WA_OpaquePaintEvent),
    PAIR(Qt::WA_OutsideWSRange),
    PAIR(Qt::WA_PaintOnScreen),
    PAIR(Qt::WA_PaintUnclipped),
    PAIR(Qt::WA_PendingMoveEvent),
    PAIR(Qt::WA_PendingResizeEvent),
    PAIR(Qt::WA_QuitOnClose),
    PAIR(Qt::WA_Resized),
    PAIR(Qt::WA_RightToLeft),
    PAIR(Qt::WA_SetCursor),
    PAIR(Qt::WA_SetFont),
    PAIR(Qt::WA_SetPalette),
    PAIR(Qt::WA_SetStyle),
    PAIR(Qt::WA_ShowModal),
    PAIR(Qt::WA_StaticContents),
    PAIR(Qt::WA_StyleSheet),
    PAIR(Qt::WA_TranslucentBackground),
    PAIR(Qt::WA_UnderMouse),
    PAIR(Qt::WA_UpdatesDisabled),
    PAIR(Qt::WA_WindowModified),
    PAIR(Qt::WA_WindowPropagation),
    PAIR(Qt::WA_MacAlwaysShowToolWindow),
    PAIR(Qt::WA_SetLocale),
    PAIR(Qt::WA_StyledBackground),
    PAIR(Qt::WA_ShowWithoutActivating),
    PAIR(Qt::WA_NativeWindow),
    PAIR(Qt::WA_DontCreateNativeAncestors),
    PAIR(Qt::WA_X11NetWmWindowTypeDesktop),
    PAIR(Qt::WA_X11NetWmWindowTypeDock),
    PAIR(Qt::WA_X11NetWmWindowTypeToolBar),
    PAIR(Qt::WA_X11NetWmWindowTypeMenu),
    PAIR(Qt::WA_X11NetWmWindowTypeUtility),
    PAIR(Qt::WA_X11NetWmWindowTypeSplash),
    PAIR(Qt::WA_X11NetWmWindowTypeDialog),
    PAIR(Qt::WA_X11NetWmWindowTypeDropDownMenu),
    PAIR(Qt::WA_X11NetWmWindowTypePopupMenu),
    PAIR(Qt::WA_X11NetWmWindowTypeToolTip),
    PAIR(Qt::WA_X11NetWmWindowTypeNotification),
    PAIR(Qt::WA_X11NetWmWindowTypeCombo),
    PAIR(Qt::WA_X11NetWmWindowTypeDND),
    PAIR(Qt::WA_MacFrameworkScaled),
    PAIR(Qt::WA_AcceptTouchEvents),
    PAIR(Qt::WA_TouchPadAcceptSingleTouchEvents),
    PAIR(Qt::WA_X11DoNotAcceptFocus),
//    PAIR(Qt::WA_AlwaysStackOnTop)
    };

#undef PAIR

    for (size_t i = 0; i < CORE_COUNT_OF(attrs); i++)
    {
        if (widget->testAttribute(attrs[i].attr)) {
            text += attrs[i].name;
            text += " ";
        }
    }

    return text;
}

QTransform Core2Qt::QTransformFromMatrix(const corecvs::Matrix33 &m)
{
#if 0
    return QTransform(
        m.a(0, 0), m.a(0, 1), m.a(0, 2),
        m.a(1, 0), m.a(1, 1), m.a(1, 2),
        m.a(2, 0), m.a(2, 1), m.a(2, 2)
    );
#else
    return QTransform(
        m.a(0, 0), m.a(1, 0), m.a(2, 0),
        m.a(0, 1), m.a(1, 1), m.a(2, 1),
        m.a(0, 2), m.a(1, 2), m.a(2, 2)
    );
#endif
}

QString printQImageFormat(const QImage::Format &format)
{
    QString text;

    struct FormatName {
        QImage::Format format;
        const char *name;
    };

#define PAIR(X) { QImage::X, #X }

    FormatName formats[] = {
        PAIR(Format_Invalid),
        PAIR(Format_Mono),
        PAIR(Format_MonoLSB),
        PAIR(Format_Indexed8),
        PAIR(Format_RGB32),
        PAIR(Format_ARGB32),
        PAIR(Format_ARGB32_Premultiplied),
        PAIR(Format_RGB16),
        PAIR(Format_ARGB8565_Premultiplied),
        PAIR(Format_RGB666),
        PAIR(Format_ARGB6666_Premultiplied),
        PAIR(Format_RGB555),
        PAIR(Format_ARGB8555_Premultiplied),
        PAIR(Format_RGB888),
        PAIR(Format_RGB444),
        PAIR(Format_ARGB4444_Premultiplied),
        PAIR(Format_RGBX8888),
        PAIR(Format_RGBA8888),
        PAIR(Format_RGBA8888_Premultiplied)
    };

#undef PAIR

    for (size_t i = 0; i < CORE_COUNT_OF(formats); i++)
    {
        if (formats[i].format == format) {
            text += formats[i].name;
            text += " ";
        }
    }

    return text;
}

QString printModelItemRole(int role)
{
    QString text;

#define PAIR(X) { Qt::X, #X }

    struct RoleName {
        int role;
        const char *name;
    };

    RoleName roles[] =
    {
        PAIR(DisplayRole),
        PAIR(DecorationRole),
        PAIR(EditRole),
        PAIR(ToolTipRole),
        PAIR(StatusTipRole),
        PAIR(WhatsThisRole),
        PAIR(SizeHintRole),

        PAIR(FontRole),
        PAIR(TextAlignmentRole),
        PAIR(BackgroundRole),
        PAIR(BackgroundColorRole),
        PAIR(ForegroundRole),
        PAIR(TextColorRole),
        PAIR(CheckStateRole),
        PAIR(InitialSortOrderRole),

        PAIR(UserRole)

    };
#undef PAIR

    for (size_t i = 0; i < CORE_COUNT_OF(roles); i++)
    {
        if (role == roles[i].role) {
            text += roles[i].name;
            text += " ";
        }
    }

    return text;

}


const QString RecentPathKeeper::LAST_INPUT_DIRECTORY_KEY  = "lastInputDirectory";
const QString RecentPathKeeper::LAST_OUTPUT_DIRECTORY_KEY = "lastOutputDirectory";
const QString RecentPathKeeper::RECENT_SCENES_KEY         = "recent";

void RecentPathKeeper::loadFromQSettings(const QString &fileName, const QString &_root)
{
    QSettings settings(fileName, QSettings::IniFormat);
    settings.beginGroup(_root);
    updateIn (settings.value(LAST_INPUT_DIRECTORY_KEY, ".").toString());
    updateOut(settings.value(LAST_OUTPUT_DIRECTORY_KEY, ".").toString());

    unsigned size = settings.beginReadArray(RECENT_SCENES_KEY);
    for (unsigned j = 0; j < size; j++) {
        settings.setArrayIndex(j);
        lastScenes.push_back(settings.value("path", ".").toString().toStdString());
    }
    settings.endArray();
    settings.endGroup();

}

void RecentPathKeeper::saveToQSettings(const QString &fileName, const QString &_root)
{
    QSettings settings(fileName, QSettings::IniFormat);
    settings.beginGroup(_root);
    settings.setValue(LAST_INPUT_DIRECTORY_KEY,  in);
    settings.setValue(LAST_OUTPUT_DIRECTORY_KEY, out);

    settings.beginWriteArray(RECENT_SCENES_KEY);
    for (unsigned j = 0; j < lastScenes.size(); j++) {
        settings.setArrayIndex(j);
        settings.setValue("path", QString::fromStdString(lastScenes[j]));
    }
    settings.endArray();
    settings.endGroup();
}

RecentPathKeeper::~RecentPathKeeper()
{

}
