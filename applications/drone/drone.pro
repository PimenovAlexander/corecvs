TEMPLATE = subdirs
CONFIG  += ordered

#SUBDIRS += coptercontrol
SUBDIRS += drone-core
SUBDIRS += drone-gui
SUBDIRS += physics-test



coptercontrol.file         = coptercontrol/coptercontrol.pro
drone-core.file            = drone-core/drone-core.pro

drone-gui.file             = drone-gui/drone-gui.pro
drone-gui.depends         += drone-core

physics-test.file          = physics-test/physics-test.pro
physics-test.depends      += drone-core

