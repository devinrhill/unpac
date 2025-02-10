# leave empty for release
DEBUG := 1

CXX := g++
CXXFLAGS := -std=c++20 -Iinclude $(if $(DEBUG),-Wall -Wextra -pedantic -g,-O3)
LDFLAGS := -lfmt -lgiga -lm -lraylib
TARGET := unpac
MODULES := arcv brres g03_item_set main

SOURCEDIR := src
SOURCES := $(foreach MODULE,$(MODULES),$(SOURCEDIR)/$(MODULE).cpp)

OBJECTDIR := build
OBJECTS := $(foreach MODULE,$(MODULES),$(OBJECTDIR)/$(MODULE).o)

all: $(TARGET)

$(OBJECTS): $(OBJECTDIR)/%.o: $(SOURCEDIR)/%.cpp
	$(CXX) $< $(CXXFLAGS) -c -o $@

$(TARGET): $(OBJECTS)
	$(CXX) $^ $(LDFLAGS) -o $(TARGET)

install: $(TARGET)
	cp $(TARGET) /usr/bin

clean:
	rm -f $(OBJECTS) $(TARGET)

.PHONY: all clean
