#pragma once

#include <QString>

/// Singleton class to represent config settings
class ConfigManager
{
public:
    static QString configName();
    static void setConfigName(QString const &name);

    static QString camConfigName();
    static void setCamConfigName(QString const &name);

    static ConfigManager* instance();

private:
    ConfigManager();

    QString name();
    void setName(QString const &name);

    QString camName();
    void setCamName(QString const &name);

    static ConfigManager* mInstance;

    QString mName;
    QString mCamName;
};

