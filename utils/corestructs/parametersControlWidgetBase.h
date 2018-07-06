#ifndef PARAMETERS_CONTROL_WIDGET_BASE_H_
#define PARAMETERS_CONTROL_WIDGET_BASE_H_
/**
 * \file parametersControlWidgetBase.h
 *
 *
 *
 * \date Jun 2, 2012
 * \author alexander
 */

#include <QWidget>

#include "core/utils/global.h"

#include "core/reflection/reflection.h"
#include "core/utils/visitors/propertyListVisitor.h"
#include "visitors/qSettingsSetter.h"
#include "visitors/qSettingsGetter.h"
#include "visitors/xmlSetter.h"
#include "visitors/xmlGetter.h"
#include "visitors/jsonSetter.h"
#include "visitors/jsonGetter.h"

using corecvs::BaseReflectionStatic;

/*Rework this. Inability to use virtual template calls complicate the design*/
class WidgetSaver
{
public:
    WidgetSaver()
        : mQtSettings(NULL)
        , mXmlSetter(NULL)
        , mJsonSetter(NULL)
		, mPropertyListSetter(NULL)
	{}

    WidgetSaver(SettingsSetter *qtSettigns)
        : mQtSettings(qtSettigns)
        , mXmlSetter(NULL)
        , mJsonSetter(NULL)
		, mPropertyListSetter(NULL)
	{}

    WidgetSaver(XmlSetter *xmlSetter)
        : mQtSettings(NULL)
        , mXmlSetter(xmlSetter)
        , mJsonSetter(NULL)
		, mPropertyListSetter(NULL)
	{}

    WidgetSaver(JSONSetter *jsonSetter)
        : mQtSettings(NULL)
        , mXmlSetter(NULL)
        , mJsonSetter(jsonSetter)
		, mPropertyListSetter(NULL)
    {}

    /* Auto init */
    WidgetSaver(QString filename)
    {
        qDebug("WidgetSaver::WidgetSaver(%s): called", filename.toLatin1().constData());
        autoInit = true;
        if (filename.endsWith(".json")) {
            qDebug("WidgetSaver::WidgetSaver(): created JSON Setter");
            mJsonSetter = new JSONSetter(filename);
        }

        if (filename.endsWith(".list")) {
            mPropertyListSetter = new PropertyListWriterVisitor(filename.toStdString());
            qDebug("WidgetSaver::WidgetSaver(): created PropertyList Setter");
        }

        if (filename.endsWith(".conf")) {
            mQtSettings = new SettingsSetter(filename);
            qDebug("WidgetSaver::WidgetSaver(): created QSettings Setter");
        }
    }

template <class ParametersClass>
	void saveParameters (ParametersClass &paramsClass, QString name)
	{
		if (mQtSettings) {
		    qDebug() << "Saving widget: " << name;
		    mQtSettings->settings()->beginGroup(name);
			    paramsClass.accept(*mQtSettings);
            mQtSettings->settings()->endGroup();
			return;
		}

		if (mXmlSetter) {
			paramsClass.accept(*mXmlSetter);
			return;
		}

         if (mJsonSetter) {
            mJsonSetter->pushChild(name.toLatin1().constData());
            paramsClass.accept(*mJsonSetter);
            mJsonSetter->popChild(name.toLatin1().constData());
        }

        if (mPropertyListSetter){
            mPropertyListSetter->pushChild(name.toStdString());
            paramsClass.accept(*mPropertyListSetter);
            mPropertyListSetter->popChild();
        }
	}

 ~WidgetSaver() {
        if (autoInit) {
            delete_safe(mQtSettings);
            delete_safe(mXmlSetter);
            delete_safe(mJsonSetter);
            delete_safe(mPropertyListSetter);
        }
    }

private:
    bool autoInit = false;

    SettingsSetter *mQtSettings = NULL;
    XmlSetter      *mXmlSetter = NULL;
    JSONSetter     *mJsonSetter = NULL;
    PropertyListWriterVisitor *mPropertyListSetter = NULL;

};

class WidgetLoader
{
public:
    WidgetLoader()
        : mQtSettings(NULL)
        , mXmlGetter(NULL)
        , mJsonGetter(NULL)
		, mPropertyListGetter(NULL)
	{}

    WidgetLoader(SettingsGetter *qtSettigns)
        : mQtSettings(qtSettigns)
        , mXmlGetter(NULL)
        , mJsonGetter(NULL)
		, mPropertyListGetter(NULL)
	{}

    WidgetLoader(XmlGetter *xmlSetter)
        : mQtSettings(NULL)
        , mXmlGetter(xmlSetter)
        , mJsonGetter(NULL)
		, mPropertyListGetter(NULL)
	{}

    WidgetLoader(JSONGetter *jsonGetter)
        : mQtSettings(NULL)
        , mXmlGetter(NULL)
        , mJsonGetter(jsonGetter)
		, mPropertyListGetter(NULL)
    {}

    /* Auto init */
    WidgetLoader(QString filename)
    {
        qDebug("WidgetLoader::WidgetLoader(%s): called", filename.toLatin1().constData());
        autoInit = true;
        if (filename.endsWith(".json")) {
            mJsonGetter = new JSONGetter(filename);
            qDebug("WidgetLoader::WidgetLoader(): created JSON Getter");
        }

        if (filename.endsWith(".list")) {
            mPropertyListGetter = new PropertyListReaderVisitor(filename.toStdString());
            qDebug("WidgetLoader::WidgetLoader(): created PropertyList Getter");
        }

        if (filename.endsWith(".conf")) {
            mQtSettings = new SettingsGetter(filename);
            qDebug("WidgetSaver::WidgetSaver(): created QSettings Getter");
        }
    }


template <class ParametersClass>
	void loadParameters (ParametersClass &paramsClass, QString name)
	{
		if (mQtSettings) {
		    mQtSettings->settings()->beginGroup(name);
			    paramsClass.accept(*mQtSettings);
			mQtSettings->settings()->endGroup();
			return;
		}

		if (mXmlGetter) {
			paramsClass.accept(*mXmlGetter);
			return;
		}

        if (mJsonGetter){
            mJsonGetter->pushChild(name.toLatin1().constData());
            paramsClass.accept(*mJsonGetter);
            mJsonGetter->popChild();
        }

        if (mPropertyListGetter){
            mPropertyListGetter->pushChild(name.toStdString());
            paramsClass.accept(*mPropertyListGetter);
            mPropertyListGetter->popChild();
        }
	}

    ~WidgetLoader() {
//         qDebug("WidgetLoader::~WidgetLoader(): called");
        if (autoInit) {
            delete_safe(mQtSettings);
            delete_safe(mXmlGetter);
            delete_safe(mJsonGetter);
            delete_safe(mPropertyListGetter);
        }
    }

private:
    bool autoInit = false;

    SettingsGetter            *mQtSettings = NULL;
    XmlGetter                 *mXmlGetter = NULL;
    JSONGetter                *mJsonGetter = NULL;
    PropertyListReaderVisitor *mPropertyListGetter = NULL;
};

class SaveableWidget
{
public:
    virtual void loadFromQSettings(const QString &fileName, const QString &_root)
    {
//        qDebug("SaveableWidget::loadFromQSettings(\"%s\", \"%s\"): called",fileName.toLatin1().constData(), _root.toLatin1().constData());

        SettingsGetter visitor(fileName, _root);
        WidgetLoader loader(&visitor);
        loadParamWidget(loader);
    }

    virtual void saveToQSettings (const QString &fileName, const QString &_root)
    {
//        qDebug("SaveableWidget::saveToQSettings(\"%s\", \"%s\"): called",fileName.toLatin1().constData(), _root.toLatin1().constData());

        SettingsSetter visitor(fileName, _root);
        WidgetSaver saver(&visitor);
        saveParamWidget(saver);
    }

    virtual void loadParamWidget(WidgetLoader &/*loader*/) {}
    virtual void saveParamWidget(WidgetSaver  &/*saver*/ ) {}
};


class ParametersControlWidgetBase : public QWidget, public SaveableWidget
{
    Q_OBJECT

public:
    ParametersControlWidgetBase(QWidget *parent = NULL) : QWidget(parent) {}

    virtual BaseReflectionStatic *createParametersVirtual() const;
    virtual ~ParametersControlWidgetBase();

signals:
    void paramsChanged();
};


class ParametersControlWidgetBaseFabric
{
public:
    virtual ParametersControlWidgetBase *produce() = 0;
};


template <class WidgetToProduce>
class ParametersControlWidgetBaseFabricImpl : public ParametersControlWidgetBaseFabric
{
public:
    virtual ParametersControlWidgetBase *produce()
    {
        return new WidgetToProduce();
    }
};

#endif  /* #ifndef PARAMETERS_CONTROL_WIDGET_BASE_H_ */
