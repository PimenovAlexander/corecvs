TEMPLATE = subdirs
CONFIG  += ordered

#SUBDIRS += coptercontrol
SUBDIRS += drone-core
SUBDIRS += drone-ui
SUBDIRS += drone-app
SUBDIRS += physics-test



coptercontrol.file         = coptercontrol/coptercontrol.pro
drone-core.file            = drone-core/drone-core.pro

drone-ui.file             = drone-ui/drone-ui.pro
drone-ui.depends         += drone-core

drone-app.file             = drone-app/drone-app.pro
drone-app.depends         += drone-ui


physics-test.file          = physics-test/physics-test.pro
physics-test.depends      += drone-core

