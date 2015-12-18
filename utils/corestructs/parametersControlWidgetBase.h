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

#include "global.h"

#include "reflection.h"
#include "visitors/qSettingsSetter.h"
#include "visitors/xmlSetter.h"
#include "visitors/qSettingsGetter.h"
#include "visitors/xmlGetter.h"
#include "visitors/jsonSetter.h"
#include "visitors/jsonGetter.h"

using corecvs::BaseReflectionStatic;

/*Rework this. Inability to use virtual template calls complicate the design*/
class WidgetSaver {
	SettingsSetter *mQtSettings;
	XmlSetter      *mXmlSetter;
    JSONSetter     *mJsonSetter;

public:
	WidgetSaver() :
		mQtSettings(NULL),
        mXmlSetter(NULL),
        mJsonSetter(NULL)
	{}

	WidgetSaver(SettingsSetter *qtSettigns) :
		mQtSettings(qtSettigns),
        mXmlSetter(NULL),
        mJsonSetter(NULL)
	{}

	WidgetSaver(XmlSetter *xmlSetter) :
		mQtSettings(NULL),
        mXmlSetter(xmlSetter),
        mJsonSetter(NULL)
	{}

    WidgetSaver(JSONSetter *jsonSetter) :
        mQtSettings(NULL),
        mXmlSetter(NULL),
        mJsonSetter(jsonSetter)
    {}

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

        if (mJsonSetter){
            paramsClass.accept(*mJsonSetter);
        }
	}
};

class WidgetLoader {
	SettingsGetter *mQtSettings;
	XmlGetter      *mXmlGetter;
    JSONGetter     *mJsonGetter;
public:
	WidgetLoader() :
		mQtSettings(NULL),
        mXmlGetter(NULL),
        mJsonGetter(NULL)
	{}

	WidgetLoader(SettingsGetter *qtSettigns) :
		mQtSettings(qtSettigns),
        mXmlGetter(NULL),
        mJsonGetter(NULL)
	{}

	WidgetLoader(XmlGetter *xmlSetter) :
		mQtSettings(NULL),
        mXmlGetter(xmlSetter),
        mJsonGetter(NULL)
	{}

    WidgetLoader(JSONGetter *jsonGetter) :
        mQtSettings(NULL),
        mXmlGetter(NULL),
        mJsonGetter(jsonGetter)
    {}

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
            paramsClass.accept(*mJsonGetter);
        }
	}
};

class SaveableWidget
{
public:

    virtual void loadFromQSettings  (const QString &fileName, const QString &_root)
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
};

class ParametersControlWidgetBaseFabric {
public:
    virtual ParametersControlWidgetBase *produce() = 0;
};

template <class WidgetToProduce>
class ParametersControlWidgetBaseFabricImpl : public ParametersControlWidgetBaseFabric {
public:
    virtual ParametersControlWidgetBase *produce()
    {
        return new WidgetToProduce();
    }
};




#endif  /* #ifndef PARAMETERS_CONTROL_WIDGET_BASE_H_ */


