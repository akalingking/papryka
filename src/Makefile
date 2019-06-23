include ../makefile.ubuntu
LIBS += -lQuantLib
INCLUDES += -I../include

TARGET_DIR=../bin

#@{ targets
SYNTHETIC=synthetic_feed
SYNTHETIC_TARGET=$(TARGET_DIR)/$(SYNTHETIC)
SYNTHETIC_SRCS=./examples/feed/synthetic.cpp
SYNTHETIC_OBJS=$(SYNTHETIC_SRCS:.cpp=.o)
#@} targets

OBJS = $(SANITY_OBJS) $(SYNTHETIC_OBJS)
SRCS = $(SANITY_SRCS) $(SYNTHETIC_SRCS)
TARGETS = $(SANITY_TARGET) $(SYNTHETIC_TARGET)

.cpp.o:
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

.PHONY: depend clean

all:    $(TARGETS)
	@echo  '$(TARGETS)' done!

clean:
	$(RM) *~ $(TARGETS) $(OBJS) ../bin/examples/core* ../bin/examples/*.log examples/feed/*.gcno

depend: $(SRCS)
	makedepend $(INCLUDES) $^

#@{ builds
$(SYNTHETIC_TARGET): $(SYNTHETIC_OBJS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $(SYNTHETIC_TARGET) $(SYNTHETIC_OBJS) $(LFLAGS) $(LIBS)
$(SYNTHETIC): $(SYNTHETIC_TARGET)
#@} builds

# DO NOT DELETE

