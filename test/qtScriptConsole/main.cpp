#include "scriptWindow.h"
#include <QApplication>
#include "coreToScript.h"

#include "qtFileLoader.h"

int main(int argc, char *argv[])
{

    QApplication a(argc, argv);
    Q_INIT_RESOURCE(main);
    QTRGB24Loader::registerMyself();


    ScriptWindow w;

#if 1
    //ReflectionToScript axisScriptObject(&AxisAlignedBoxParameters::reflection);
    qScriptRegisterMetaType(w.mEngine,
        ReflectionToScript::toScriptValue<AxisAlignedBoxParameters>,
        ReflectionToScript::fromScriptValue<AxisAlignedBoxParameters>);

    QScriptEngine::FunctionSignature constructor = ReflectionToScript::createMyStruct<AxisAlignedBoxParameters>;
    QScriptValue ctor = w.mEngine->newFunction(constructor);
    w.mEngine->globalObject().setProperty("AxisAlignedBoxParameters", ctor);

#endif

    w.show();

    return a.exec();
}
