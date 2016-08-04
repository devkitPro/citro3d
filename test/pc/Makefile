TARGET   := test

CFILES   := $(wildcard *.c) $(wildcard ../../source/maths/*.c)
CXXFILES := $(wildcard *.cpp)
OFILES   := $(CXXFILES:.cpp=.o) $(CFILES:.c=.o)
DFILES   := $(wildcard *.d) $(wildcard ../../source/maths/*.d)

CFLAGS   := -Wall -g -pipe -I../../include
CXXFLAGS := $(CFLAGS) -std=gnu++11 -DGLM_FORCE_RADIANS
LDFLAGS  := $(ARCH) -pipe -lm

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OFILES)
	@echo "Linking $@"
	$(CXX) -o $@ $^ $(LDFLAGS)

%.o : %.cpp $(wildcard *.h)
	@echo "Compiling $@"
	@$(CXX) -o $@ -c $< $(CXXFLAGS) -MMD -MP -MF $*.d

%.o : %.c $(wildcard *.h)
	@echo "Compiling $@"
	@$(CC) -o $@ -c $< $(CFLAGS) -MMD -MP -MF $*.d

clean:
	$(RM) $(OFILES) $(DFILES) $(TARGET)

-include $(DFILES)
