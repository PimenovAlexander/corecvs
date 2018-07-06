with_libcgal {
    CGAL_ROOT = $$(CGAL_ROOT)
    win32 {
        !isEmpty(CGAL_ROOT) {
            exists($$CGAL_ROOT/include/CGAL/convex_hull_3.h) {
                INCLUDEPATH += $$CGAL_ROOT/include
                LIBS        += -L$$CGAL_ROOT/lib/
                DEFINES     += WITH_LIBCGAL
            } else {
                !build_pass:message(CGAL not found.)
            }
        }
    } else {
        isEmpty(CGAL_ROOT) {
            !build_pass: message(Linking with system CGAL)
            exists(/usr/include/CGAL/convex_hull_3.h) {
#               INCLUDEPATH += /usr/include
                DEFINES     += WITH_LIBCGAL
            } else {
                !build_pass:message(CGAL not found.)
            }
        } else {
            !build_pass: message(Linking with CGAL from $$CGAL_ROOT)
            DEPENDPATH  += $$CGAL_ROOT/include
            INCLUDEPATH += $$CGAL_ROOT/include
            LIBS        += -L$$CGAL_ROOT/lib/
            DEFINES     += WITH_LIBCGAL
        }
    }

    contains(DEFINES, WITH_LIBCGAL) {                    # if it's installed properly with found path for inc
        LIBS += -lCGAL

	!win32: LIBS += -lgmp
    }
}
