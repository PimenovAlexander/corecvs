#include <iostream>
#include <fstream>

#include <QPushButton>
#include <QGridLayout>
#include <QApplication>
#include <QtCore>

#ifdef WITH_LIBJPEG
#include "libjpegFileReader.h"
#endif
#ifdef WITH_LIBPNG
#include "libpngFileReader.h"
#endif

#include "core/filters/blocks/newStyleBlock.h"
#include "widgetBlockHarness.h"

#include "qtFileLoader.h"
#include "reflectionWidget.h"

#include "core/math/vector/vector2d.h"
#include "core/math/matrix/homographyReconstructor.h"
#include "core/rectification/sceneStereoAlignerBlock.h"

void initLoaders()
{
#ifdef WITH_LIBJPEG
    LibjpegFileReader::registerMyself();
    SYNC_PRINT(("Libjpeg support on\n"));
#endif
#ifdef WITH_LIBPNG
    LibpngFileReader::registerMyself();
    SYNC_PRINT(("Libpng support on\n"));
#endif
    QTRGB24Loader::registerMyself();
}


int main(int argc, char **argv)
{    
    QApplication a(argc, argv);
    Q_INIT_RESOURCE(main);

    initLoaders();

    /* Assert modules are linked */
    cout << "Fields:" << HomorgaphyReconstructorBlockBase::reflection.fields.size() << std::endl;
    cout << "Fields:" << SceneStereoAlignerBlockBase     ::reflection.fields.size() << std::endl;
    cout << "Fields:" << AdderSubstractor                ::reflection.fields.size() << std::endl;

    SceneStereoAlignerBlock::relinkCompositeFields();

    ReflectionDirectory &directory = *ReflectionDirectoryHolder::getReflectionDirectory();

    if (argc == 1) {
        cout << "Usage:" << endl;
        cout << endl;
        cout << argv[0] << " blocks - List all blocks linked to the executable" << endl;
        cout << argv[0] << " block <name> - List all blocks linked to the executable" << endl;
        cout << argv[0] << " test - Make a gtk freeze test" << endl;
        return 0;
    }

    std::string mode(argv[1]);

    if (mode == "blocks") {
        cout << "The list of reflected blocks" << endl;

        for (auto it : directory)
        {
            if (it.second->isActionBlock())
            {
                cout << "  -  " << it.first << endl;
            }
        }

        cout << "  -  add_subtract"     << endl;
        cout << "  -  homography"     << endl;
        cout << "  -  stereo_aligner" << endl;
        return 0;
    }

    DynamicObjectWrapper blockReflection;
    NewStyleBlock *block = NULL;

    if ( mode == "block")
    {
        std::string blockName;
        if (argc >= 2)
            blockName = argv[2];        

        if (blockName == "homography") {
            HomographyReconstructorBlock *theBlock = new HomographyReconstructorBlock();
            blockReflection = DynamicObjectWrapper(static_cast<HomorgaphyReconstructorBlockBase *>(theBlock));
            block = theBlock;
        }
        if (blockName == "stereo_aligner") {
            SceneStereoAlignerBlock *theBlock = new SceneStereoAlignerBlock();
            blockReflection = DynamicObjectWrapper(static_cast<SceneStereoAlignerBlockBase *>(theBlock));
            block = theBlock;
        }
        if (blockName == "adder_test") {
            AdderSubstractor *theBlock = new AdderSubstractor();
            blockReflection = DynamicObjectWrapper(static_cast<AdderSubstractorParametersBase *>(theBlock));
            block = theBlock;
        }

        if (block != NULL)
        {
           WidgetBlockHarness *harness = new WidgetBlockHarness(&blockReflection, block, NULL);
           harness->show();
        }

    }

    a.exec();

    // delete_safe(aabWidget);
    // delete_safe(reciever);
    return 0;
}

