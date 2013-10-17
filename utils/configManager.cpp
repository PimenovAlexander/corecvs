#include "configManager.h"

ConfigManager* ConfigManager::mInstance = NULL;

QString ConfigManager::configName()
{
    return instance()->name();
}

void ConfigManager::setConfigName(QString const &name)
{
    instance()->setName(name);
}

QString ConfigManager::camConfigName()
{
    return instance()->camName();
}

void ConfigManager::setCamConfigName(QString const &name)
{
    instance()->setCamName(name);
}

ConfigManager *ConfigManager::instance()
{
    if (mInstance == NULL)
        mInstance = new ConfigManager();

    return mInstance;
}

ConfigManager::ConfigManager()
{
}

QString ConfigManager::name()
{
    return mName;
}

void ConfigManager::setName(const QString &name)
{
    mName = name;
}

QString ConfigManager::camName()
{
    return mCamName;
}

void ConfigManager::setCamName(const QString &name)
{
    mCamName = name;
}
