#ifndef LOGGERWIDGET_H
#define LOGGERWIDGET_H

#include <QtGui/QWidget>
#include <QtGui/QPushButton>

#include "log.h"
#include "ui_loggerWidget.h"


class LoggerWidget : public QWidget, public LogDrain
{
    Q_OBJECT

public:
    LoggerWidget(QWidget *parent = 0);
    ~LoggerWidget();

    virtual void drain(Log::Message &message);

    static const char* iconResources[];
public slots:
	void doDrain(Log::Message message);

private:
    Ui::LoggerWidgetClass ui;

    QIcon **mLevelIcons;
    QPushButton **mLevelFilters;

    QFont *logFont;

};

#endif // LOGGERWIDGET_H
