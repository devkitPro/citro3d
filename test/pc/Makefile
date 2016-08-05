TARGET   := test

CFILES   := $(wildcard *.c) $(wildcard ../../source/maths/*.c)
CXXFILES := $(wildcard *.cpp)
OFILES   := $(addprefix build/,$(CXXFILES:.cpp=.o)) \
            $(patsubst ../../source/maths/%,build/%,$(CFILES:.c=.o))
DFILES   := $(wildcard build/*.d)

CFLAGS   := -Wall -g -pipe -I../../include
CXXFLAGS := $(CFLAGS) -std=gnu++11 -DGLM_FORCE_RADIANS
LDFLAGS  := $(ARCH) -pipe -lm

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OFILES)
	@echo "Linking $@"
	$(CXX) -o $@ $^ $(LDFLAGS)

$(OFILES): | build

build:
	@[ -d build ] || mkdir build

build/%.o : %.cpp $(wildcard *.h)
	@echo "Compiling $@"
	@$(CXX) -o $@ -c $< $(CXXFLAGS) -MMD -MP -MF build/$*.d

build/%.o : ../../source/maths/%.c $(wildcard *.h)
	@echo "Compiling $@"
	@$(CC) -o $@ -c $< $(CFLAGS) -MMD -MP -MF build/$*.d

clean:
	$(RM) -r $(TARGET) build/

-include $(DFILES)
