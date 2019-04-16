#include <iostream>
#include <fstream>

#include <QApplication>
#include <QtCore>

#ifdef WITH_LIBJPEG
#include "libjpegFileReader.h"
#endif
#ifdef WITH_LIBPNG
#include "libpngFileReader.h"
#endif

#ifdef INCLUDE_EXAMPLE
#include "testClass.h"
#endif

#include "qtFileLoader.h"
#include "reflectionWidget.h"
#include "core/xml/generated/axisAlignedBoxParameters.h"
#include "core/xml/generated/chessBoardAssemblerParamsBase.h"
#include "core/xml/generated/checkerboardDetectionParameters.h"
#include "core/xml/generated/chessBoardCornerDetectorParamsBase.h"
#include "core/xml/generated/sceneStereoAlignerBlockBase.h"

//#include "iterativeReconstructionNonlinearOptimizationParamsWrapper.h"

#include "core/math/vector/vector2d.h"
#include "changeReceiver.h"
#include "testNativeWidget.h"

#include "core/math/matrix/homographyReconstructor.h"
#include "core/rectification/sceneStereoAlignerBlock.h"

int main(int argc, char **argv)
{    
    QApplication a(argc, argv);
    Q_INIT_RESOURCE(main);


    Vector2dd dummy(0.0);
    cout << "Main out test:" <<  dummy.reflection.fields.size() << std::endl;
    cout << "Main out test:" << HomorgaphyReconstructorBlockBase::fields().size() << std::endl;


#ifdef INCLUDE_EXAMPLE
    cout << "Main out test:" << TestSubClass::reflection.fields.size() << std::endl;
    cout << "Main out test:" << TestClass   ::reflection.fields.size() << std::endl;
#endif


#ifdef WITH_LIBJPEG
    LibjpegFileReader::registerMyself();
    SYNC_PRINT(("Libjpeg support on\n"));
#endif
#ifdef WITH_LIBPNG
    LibpngFileReader::registerMyself();
    SYNC_PRINT(("Libpng support on\n"));
#endif
    QTRGB24Loader::registerMyself();

    ReflectionDirectory &directory = *ReflectionDirectoryHolder::getReflectionDirectory();

    if (argc == 1) {
        cout << "Usage:" << endl;
        cout << endl;
        cout << argv[0] << " list  - List all the the reflections linked to the executable" << endl;
        cout << argv[0] << " example  - Create a generator test widget (if one was compiled)" << endl;
        cout << argv[0] << " widget - Create a specific test widget from reflection" << endl;
        cout << argv[0] << " widget <name> - Create a specific test widget from reflection" << endl;
        cout << argv[0] << " blocks - List all blocks linked to the executable" << endl;
        cout << argv[0] << " block <name> - List all blocks linked to the executable" << endl;

        cout << argv[0] << " test - Make a gtk freeze test" << endl;


        return 0;
    }

    std::string mode(argv[1]);

    if (mode == "list") {
        cout << "The list of reflected objects" << endl;
        for (auto it : directory)
        {
            cout << "  -  " << it.first << endl;
        }
        return 0;
    }

    if (mode == "blocks") {
        cout << "The list of reflected blocks" << endl;
        cout << "  -  homography"     << endl;
        cout << "  -  stereo_aligner" << endl;
        return 0;
    }



    ReflectionWidget *aabWidget = NULL;
    ChangeReceiver *reciever = NULL;

    std::unique_ptr<TestNativeWidget> testWidget;
    if (mode == "test")
    {
        testWidget.reset(new TestNativeWidget);
        testWidget->show();
    }

    if (mode == "example") {
#ifdef INCLUDE_EXAMPLE
        Reflection *widget_ref = &TestClass::reflection;
        aabWidget = new ReflectionWidget(widget_ref);
        reciever  = new ChangeReceiver(aabWidget);
        QObject::connect(aabWidget, SIGNAL(paramsChanged()), reciever, SLOT(processChange()));
        aabWidget->show();
#endif
    } else if ( mode == "widget")
    {
        std::string refName;
        if (argc >= 3)
            refName = argv[2];

        auto it = directory.find(refName);
        if (it == directory.end())
        {
/*
            {
                Reflection *widget_ref = &AxisAlignedBoxParameters::reflection;
                ReflectionWidget *aabWidget = new ReflectionWidget(widget_ref);
                aabWidget->show();
            }*/
            {
                const Reflection *widget_ref = RgbColorParameters::getReflection();
                ReflectionWidget *aabWidget = new ReflectionWidget(widget_ref);
                aabWidget->show();
            }
/*            {
                Reflection *widget_ref = &ChessBoardAssemblerParamsBase::reflection;
                ReflectionWidget *aabWidget = new ReflectionWidget(widget_ref);
                aabWidget->show();
            }
            {
                Reflection *widget_ref = &CheckerboardDetectionParameters::reflection;
                ReflectionWidget *aabWidget = new ReflectionWidget(widget_ref);
                aabWidget->show();
            }
            {
                Reflection *widget_ref = &ChessBoardCornerDetectorParamsBase::reflection;
                ReflectionWidget *aabWidget = new ReflectionWidget(widget_ref);
                aabWidget->show();
            }*/
            {
                const Reflection *widget_ref = SceneStereoAlignerBlockBase::getReflection();
                ReflectionWidget *aabWidget = new ReflectionWidget(widget_ref);
                aabWidget->show();
            }
           /* {
                Reflection *widget_ref = &IterativeReconstructionNonlinearOptimizationParamsWrapper::reflection;
                ReflectionWidget *aabWidget = new ReflectionWidget(widget_ref);
                aabWidget->show();
            } */
        }
        else
        {
            Reflection *widget_ref = (*it).second;
            aabWidget = new ReflectionWidget(widget_ref);
            aabWidget->show();

            //bool isBlock = false;
            for (int fieldId = 0; fieldId < widget_ref->fieldNumber(); fieldId++)
            {
                const BaseField *field = widget_ref->fields[fieldId];
                if (field->isInputPin()) {
                    SYNC_PRINT(("We have an input field %s\n", field->name.name));
              //      isBlock = true;
                }
                if (field->isOuputPin()) {
                    SYNC_PRINT(("We have an output field %s\n", field->name.name));
              //      isBlock = true;
                }
            }
            //SYNC_PRINT(("isBlock %d\n", isBlock));
        }
    }
    else if (mode == "block")
    {
        std::string blockName;
        if (argc >= 2)
            blockName = argv[2];
        if (blockName == "homography") {
            HomographyReconstructorBlock block;

            const Reflection *widget_ref = HomographyReconstructorBlock::getReflection();
            aabWidget = new ReflectionWidget(widget_ref);
            aabWidget->show();
        }

        if (blockName == "stereo_aligner") {
            SceneStereoAlignerBlock block;

            const Reflection *widget_ref = SceneStereoAlignerBlock::getReflection();
            aabWidget = new ReflectionWidget(widget_ref);
            aabWidget->show();
        }


    }

    a.exec();

    delete_safe(aabWidget);
    delete_safe(reciever);
    return 0;
}
