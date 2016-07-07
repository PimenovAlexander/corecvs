TEMPLATE = subdirs
CONFIG  += ordered

SUBDIRS +=                   \
    core                     \
    unitTests                \
    utils                    \
    \
    tests \
    \
    base_application         \
    base_application_example \
    recorder                 \
    testbed                  \
    cloudview                \
    imageview                \
#    \
    generator                \
    egomotion                \
    cap

    

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
unitTests.depends        += core
utils.depends            += core
#utils.depends            += preprocSrc       # preprocSrc tool is needed to convert OpenCL sources into one binary archive
baseApplication.depends  += core utils        # utils:    as libs building could be done in parallel except apps!
tests.depends            += core utils        # utils:    as libs building could be done in parallel except apps!

directshow.depends       += core

hostSoftStub.depends     += baseApplication utils
recorder.depends         += baseApplication utils

cloudview.depends        += utils
imageview.depends        += utils
testbed.depends          += utils



core.file                     = core/core.pro
unitTests.file                = test-core/test-core.pro
utils.file                    = utils/utils.pro
tests.file                    = test/tests.pro

directshow.file               = wrappers/directShow/directShow.pro
base_application.file         = applications/base/baseApplication.pro
base_application_example.file = applications/base/baseApplicationExample.pro
recorder.file                 = applications/recorder/recorder.pro
egomotion.file                = applications/egomotion/egomotion.pro
cloudview.file                = applications/cloudview/cloudview.pro
imageview.file                = applications/imageview/imageview.pro
testbed.file                  = applications/testbed/testbed.pro
generator.file                = tools/generator/generator.pro

