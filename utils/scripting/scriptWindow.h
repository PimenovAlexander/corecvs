#ifndef MAINQSCRIPTWINDOW_H
#define MAINQSCRIPTWINDOW_H

#include <QMainWindow>
#include <QScriptEngine>
#include <QListWidgetItem>
#include <QMessageBox>
#include <QDebug>

#include "loggerWidget.h"

namespace Ui {
class ScriptWindow;
}

class ScriptPreset {
public:
    QString name;
    enum ScriptType{
        NONE,
        PRESET,
        IMUTABLE,
        SAVEABLE
    };
    ScriptType type = ScriptPreset::PRESET;

    QString text;

    ScriptPreset() {}
    ScriptPreset(QString name, QString text) :
        name(name),
        type(SAVEABLE),
        text(text)
    {}

};

class ScriptLog : public QObject
{
    Q_OBJECT

    /* These are not owned */
    QScriptEngine *mEngine = NULL;
    LoggerWidget  *mWidget = NULL;
public:
    ScriptLog(QScriptEngine *engine, LoggerWidget *widget) :
        mEngine(engine),
        mWidget(widget)
    {}

public slots:
    void drain(QString string)
    {
        QString functionName = mEngine->currentContext()->backtrace().first();
        qDebug() << "drain from " << functionName;

        if (mWidget == NULL)
            return;
        Log::Message message;
        message.allocate();
        message.get()->mLevel = Log::LEVEL_DEBUG;
        message.get()->mOriginFileName = NULL;
        message.get()->mOriginLineNumber = 0;
        message.get()->mOriginFunctionName = NULL;
        time(&message.get()->mTime);
        *message.get() << "Output:" << string.toLatin1().constData();
        mWidget->drain(message);
    }

    void print(QString string)
    {
        drain(string);
    }

    void about(QString string)
    {
        QMessageBox::information(NULL, "Script", string);
    }
};

class ScriptWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit ScriptWindow(QWidget *parent = 0);
    ~ScriptWindow();

    QScriptEngine *mEngine = NULL;
    ScriptLog *mLog = NULL;

    QHash<QString, ScriptPreset> scriptCache;

public slots:
    QString checkScript();
    void executeScript();

    void reloadScripts(QString path);
    void presetChanged(QListWidgetItem* item);

private:
    Ui::ScriptWindow *ui;
};

#endif // MAINQSCRIPTWINDOW_H
