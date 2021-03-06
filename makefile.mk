CXX         = g++
#CXXFLAGS   = -O2 -Wall -Wno-switch -Wreorder -Wunused-function -ggdb -std=c++17 -DNDEBUG -DREENTRANT
CXXFLAGS    = -O2 -Wall -Wno-switch -Wno-reorder -Wno-unused-function -ggdb -std=c++17 -D_DEBUG -DREENTRANT -DPAPRYKA_DEBUG_ON #-DPAPRYKA_TRACE_ON
INCLUDES    = -I/usr/include -I/usr/local/include
LFLAGS      = -L/usr/lib -L/usr/local/lib -L/usr/lib/x86_64-linux-gnu
LIBS        = -lstdc++ -lpthread -lboost_system

UNITTEST_ON=true
COVERAGE_ON=false

## Flag to enable unit test execution
ifeq ($(UNITTEST_ON),true)
    ## Declare gcov flags to enable coverage report generation only when tests are enabled
    LIBS += -lgtest
    ifeq ($(COVERAGE_ON),true)
        CXXFLAGS += -fPIC -ftest-coverage -fno-inline -fno-inline-small-functions -fno-default-inline
        LIBS += -lgcov
    endif
endif
