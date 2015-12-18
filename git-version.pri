#
# Extract GIT version to be included into any application that wants to know it
#
unix {
    GIT_INFO1=$$system(git rev-parse HEAD)
    GIT_INFO2=$$system(git rev-parse --abbrev-ref HEAD)
} else:win32 {
    GitProg = "C:\\Program Files (x86)\\Git\\bin\\git"
    GIT_INFO1=$$system(\"$$GitProg\" rev-parse HEAD)
    GIT_INFO2=$$system(\"$$GitProg\" rev-parse --abbrev-ref HEAD)
}
DEFINES += "GIT_VERSION=\"$$GIT_INFO1 $$GIT_INFO2\""
#message(GIT_VERSION=\"$$GIT_INFO1 $$GIT_INFO2\")
