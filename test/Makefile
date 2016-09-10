include ../makefile.ubuntu

LIBS += -lQuantLib
INCLUDES += -I../include
	
TARGETDIR=../bin

#@{ targets
DETAIL=test_detail
DETAIL_TARGET=$(TARGETDIR)/$(DETAIL)
DETAIL_SRCS=./main.cpp \
./detail/logger.cpp \
./detail/date.cpp
DETAIL_OBJS=$(DETAIL_SRCS:.cpp=.o)

FEED=test_feed	
FEED_TARGET=$(TARGETDIR)/$(FEED)
FEED_SRCS=./main.cpp \
./feed/bar.cpp \
./feed/timeseries.cpp
FEED_OBJS = $(FEED_SRCS:.cpp=.o)

EXCHANGE=test_exchange	
EXCHANGE_TARGET=$(TARGETDIR)/$(EXCHANGE)
EXCHANGE_SRCS=./main.cpp \
    ./exchange/filltype.cpp \
    ./exchange/order.cpp \
    ./exchange/exchange.cpp 
EXCHANGE_OBJS = $(EXCHANGE_SRCS:.cpp=.o)

STRATEGY=test_strategy	
STRATEGY_TARGET=$(TARGETDIR)/$(STRATEGY)
STRATEGY_SRCS=./main.cpp \
./strategy/strategybasic.cpp
STRATEGY_OBJS = $(STRATEGY_SRCS:.cpp=.o)

OBJS = $(SANITY_OBJS) $(DETAIL_OBJS) $(FEED_OBJS) $(EXCHANGE_OBJS) $(STRATEGY_OBJS) $(GBM_OBJS) 
SRCS = $(SANITY_SRCS) $(DETAIL_SRCS) $(FEED_SRCS) $(EXCHANGE_SRCS) $(STRATEGY_SRCS) $(GBM_SRCS)
TARGETS = $(SANITY_TARGET) $(DETAIL_TARGET) $(FEED_TARGET) $(EXCHANGE_TARGET) $(STRATEGY_TARGET)

.cpp.o:
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

.PHONY: depend clean

ifeq ($(UNITTEST_ON),true)
all:    $(TARGETS)
	@echo "\n'$(TARGETS)' done!"
	@echo "\nRUNNING TESTS..."
	@$(DETAIL_TARGET) --gtest_output="xml:$(TARGETDIR)/$(DETAIL)_report.xml"
	@$(FEED_TARGET) --gtest_output="xml:$(TARGETDIR)/$(FEED)_report.xml"
	@$(EXCHANGE_TARGET) --gtest_output="xml:$(TARGETDIR)/$(EXCHANGE)_report.xml"	
	@$(STRATEGY_TARGET) --gtest_output="xml:$(TARGETDIR)/$(STRATEGY)_report.xml"
ifeq ($(COVERAGE_ON),true)
	@gcovr -d -r ../src --html -o $(TARGETDIR)/coverage.html
endif
else
all:    $(TARGETS)
	@echo "\n'$(TARGETS)' done!"
	@echo "\nRUNNING TESTS..."
	@$(TARGETS)
	@echo "\nTest execution and coverage report generation disabled!"
endif

clean:
	$(RM) *~ $(TARGETS) $(OBJS) $(TARGETDIR)/core* $(TARGETDIR)/*.log $(TARGETDIR)/*.html $(TARGETDIR)/*.xml \
	*.log *.gcno feed/*.gcno detail/*.gcno exchange/*.gcno strategy/*.gcno

depend: $(SRCS)
	makedepend $(INCLUDES) $^
	
#@{ builds	
$(SANITY_TARGET): $(SANITY_OBJS) 
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $(SANITY_TARGET) $(SANITY_OBJS) $(LFLAGS) $(LIBS)
$(SANITY): $(SANITY_TARGET)

$(DETAIL_TARGET): $(DETAIL_OBJS) 
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $(DETAIL_TARGET) $(DETAIL_OBJS) $(LFLAGS) $(LIBS)
$(DETAIL) : $(DETAIL_TARGET)
	
$(FEED_TARGET): $(FEED_OBJS) 
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $(FEED_TARGET) $(FEED_OBJS) $(LFLAGS) $(LIBS)
$(FEED): $(FEED_TARGET)

$(EXCHANGE_TARGET): $(EXCHANGE_OBJS) 
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $(EXCHANGE_TARGET) $(EXCHANGE_OBJS) $(LFLAGS) $(LIBS)
$(EXCHANGE): $(EXCHANGE_TARGET)
	
$(STRATEGY_TARGET): $(STRATEGY_OBJS) 
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $(STRATEGY_TARGET) $(STRATEGY_OBJS) $(LFLAGS) $(LIBS)
$(STRATEGY): $(STRATEGY_TARGET)
#@} builds
	
# DO NOT DELETE
