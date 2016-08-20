include ../makefile.ubuntu

LIBS := -lQuantLib -lgtest
INCLUDES := -I../include
	
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
    ./feed/timeseries.cpp \
    ./feed/bar.cpp \
    ./feed/feed.cpp
FEED_OBJS = $(FEED_SRCS:.cpp=.o)

EXCHANGE=test_exchange	
EXCHANGE_TARGET=$(TARGETDIR)/$(EXCHANGE)
EXCHANGE_SRCS=./main.cpp \
    ./exchange/fillstrategy.cpp \
    ./exchange/order.cpp \
    ./exchange/exchange.cpp
EXCHANGE_OBJS = $(EXCHANGE_SRCS:.cpp=.o)

STRATEGY=test_strategy	
STRATEGY_TARGET=$(TARGETDIR)/$(STRATEGY)
STRATEGY_SRCS=./main.cpp \
    ./strategy/strategy.cpp
STRATEGY_OBJS = $(STRATEGY_SRCS:.cpp=.o)

#@{ math	
GBM=test_gbm	
GBM_TARGET=$(TARGETDIR)/$(GBM)
GBM_SRCS=\
    ./feed/gbm.cpp
GBM_OBJS = $(GBM_SRCS:.cpp=.o)
#@} math
#@} targets

OBJS = $(SANITY_OBJS) $(DETAIL_OBJS) $(FEED_OBJS) $(EXCHANGE_OBJS) $(STRATEGY_OBJS) $(GBM_OBJS) 
SRCS = $(SANITY_SRCS) $(DETAIL_SRCS) $(FEED_SRCS) $(EXCHANGE_SRCS) $(STRATEGY_SRCS) $(GBM_SRCS)
TARGETS = $(SANITY_TARGET) $(DETAIL_TARGET) $(FEED_TARGET) $(EXCHANGE_TARGET) $(STRATEGY_TARGET) $(GBM_TARGET) 

.cpp.o:
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

.PHONY: depend clean

all:    $(TARGETS)
	@echo  '$(TARGETS)' done!
	@$(TARGETS)

clean:
	$(RM) *~ $(TARGETS) $(OBJS) $(TARGETDIR)/core* $(TARGETDIR)/*.log

depend: $(SRCS)
	makedepend $(INCLUDES) $^
	
#@{ builds	
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
	
#@{math
$(GBM_TARGET): $(GBM_OBJS) 
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $(GBM_TARGET) $(GBM_OBJS) $(LFLAGS) $(LIBS) -lQuantLib
$(GBM): $(GBM_TARGET)
#@} math
#@} builds
	
# DO NOT DELETE
