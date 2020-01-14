TARGET = core_perf                                      # default "test-core-perf" is incorrect. The "test.pri" will set it properly to the "test_core_perf"
include(../test.pri)

SOURCES += \
    main.cpp \
    \
    fastkernel_double/main_test_fastkernel_double.cpp \
    fastkernel_profile/main_test_fastkernel_profile.cpp \
    \
    matrix_profile/main_test_matrix_profile.cpp \
    \
    hamilton/main_test_hamilton_profile.cpp \
    \
    ../deform/test_deform.cpp \
    deform_profile/main_test_deform_profile.cpp \


OTHER_FILES += CMakeLists.txt
