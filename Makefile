# leave empty for release
DEBUG :=

CXX := g++
CXXFLAGS := -std=c++20 -Iinclude $(if $(DEBUG),-Wall -Wextra -pedantic -g,-O3)
LDFLAGS := -lfmt -lgiga -lz
TARGET := unpac
MODULES := arcv brres brsar main

SOURCEDIR := src
SOURCES := $(foreach MODULE,$(MODULES),$(SOURCEDIR)/$(MODULE).cpp)

OBJECTDIR := build
OBJECTS := $(foreach MODULE,$(MODULES),$(OBJECTDIR)/$(MODULE).o)

all: $(TARGET)

$(OBJECTS): $(OBJECTDIR)/%.o: $(SOURCEDIR)/%.cpp
	$(CXX) $< $(CXXFLAGS) -c -o $@

$(TARGET): $(OBJECTS)
	$(CXX) $^ $(LDFLAGS) -o $(TARGET)

clean:
	rm -f $(OBJECTS) $(TARGET)

.PHONY: all clean
