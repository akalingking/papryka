include ../makefile.ubuntu

LIBS := -lQuantLib -lgtest
INCLUDES := -I../include
	
TARGETDIR=../bin/test

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
    ./feed/bartimeseries.cpp \
    ./feed/feed.cpp
FEED_OBJS = $(FEED_SRCS:.cpp=.o)
#@} targets

TARGETS = $(FEED_TARGET) $(DETAIL_TARGET) 
OBJS = $(FEED_OBJS) $(DETAIL_OBJS) 
SRCS = $(FEED_SRCS) $(DETAIL_SRCS) 

.cpp.o:
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

.PHONY: depend clean

all:    $(TARGETS)
	@echo  '$(TARGETS)' done!	

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
#@} builds
	
# DO NOT DELETE
