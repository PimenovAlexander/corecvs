TEMPLATE = subdirs
CONFIG  += ordered

SUBDIRS +=                   \
    core                     \
    test-core                \
    test-core-perf           \
    utils                    \
    \
    tests \
#    \
    base_application         \
#    base_application_example \
    recorder                 \
#    scanner                  \
#    testbed                  \
    cloudview                \
    physics                  \
    physics-test             \
    imageview                \
    nester                   \
#    \
     generator                \
#    egomotion                \
#    applications/laserscan
     coptercontrol           \


win32 {
    SUBDIRS += directshow
}

win32 {
#    SUBDIRS += preprocSrc
#    SUBDIRS += opencl
} else {
    #SUBDIRS += preprocSrc
    #SUBDIRS += opencl                      # it waits a time for an accurate integration... :(
}

generator.depends        += core
test-core.depends        += core
test-core-perf.depends   += core
utils.depends            += core
#utils.depends            += preprocSrc       # preprocSrc tool is needed to convert OpenCL sources into one binary archive
baseApplication.depends  += core utils        # utils:    as libs building could be done in parallel except apps!
tests.depends            += core utils        # utils:    as libs building could be done in parallel except apps!


directshow.depends       += core

hostSoftStub.depends     += baseApplication utils
recorder.depends         += baseApplication utils
scanner.depends          += baseApplication utils

cloudview.depends        += utils
imageview.depends        += utils
testbed.depends          += utils



core.file                     = core/core.pro
test-core.file                = test-core/test-core.pro
test-core-perf.file           = test-core/-perf/test-core-perf.pro
utils.file                    = utils/utils.pro
tests.file                    = test/tests.pro

directshow.file               = wrappers/directShow/directShow.pro
base_application.file         = applications/base/baseApplication.pro
base_application_example.file = applications/base/baseApplicationExample.pro
recorder.file                 = applications/recorder/recorder.pro
scanner.file                  = applications/scanner/scanner.pro
egomotion.file                = applications/egomotion/egomotion.pro
cloudview.file                = applications/cloudview/cloudview.pro
physics.file                  = applications/physics/physics.pro
physics-test.file             = applications/physics-test/physics-test.pro
imageview.file                = applications/imageview/imageview.pro
testbed.file                  = applications/testbed/testbed.pro
nester.file                  = applications/nester/nester.pro
generator.file                = tools/generator/generator.pro

coptercontrol.file            = applications/coptercontrol/coptercontrol.pro

OTHER_FILES += cvs-config.pri
OTHER_FILES += CMakeLists.txt
OTHER_FILES += cmake/Modules/*.cmake
