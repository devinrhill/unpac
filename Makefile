# leave empty for release
DEBUG :=

CXX := g++
CXXFLAGS := -std=c++2c -Iinclude $(if $(DEBUG),-Wall -Wextra -pedantic -g,-O3)
LDFLAGS := -lfmt -lz
TARGET := unpac
MODULES := main unpac/arcv unpac/brres unpac/brsar
BMODULES := giga/archive giga/bytestream giga/endianness giga/lzss giga/platform

SOURCEDIR := src
SOURCES := $(foreach MODULE,$(MODULES),$(SOURCEDIR)/$(MODULE).cpp)

OBJECTDIR := build
OBJECTS := $(foreach MODULE,$(MODULES),$(OBJECTDIR)/$(MODULE).o)
BOBJECTS := $(foreach BMODULE,$(BMODULES),$(OBJECTDIR)/$(BMODULE).o)

all: $(TARGET)

$(OBJECTS): $(OBJECTDIR)/%.o: $(SOURCEDIR)/%.cpp
	$(CXX) $< $(CXXFLAGS) -c -o $@

$(TARGET): $(OBJECTS)
	$(CXX) $(BOBJECTS) $^ $(LDFLAGS) -o $(TARGET)
	$(if $(DEBUG),@echo,)

clean:
	rm -f $(OBJECTS) $(TARGET)

.PHONY: all clean
