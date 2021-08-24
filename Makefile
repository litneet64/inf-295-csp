CC := g++ # This is the main compiler
# CC := clang --analyze # and comment out the linker last line for sanity
SRCDIR := src
BUILDDIR := build
TARGET := bin/solver

SRCEXT := cpp
SOURCES := $(shell find $(SRCDIR) -type f -name *.$(SRCEXT))
OBJECTS := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.o))
CFLAGS := -g -Wall -O3
INC := -I include
CLEAN := find $(BUILDDIR) -name "*.o" -exec rm -v '{}' \;


$(TARGET): $(OBJECTS)
	  @echo " Linking..."
		@echo " $(CC) $^ -o $(TARGET)"; $(CC) $^ -o $(TARGET)


$(BUILDDIR)/%.o: $(SRCDIR)/%.$(SRCEXT)
	  @mkdir -p $(BUILDDIR)
		@echo " $(CC) $(CFLAGS) $(INC) -c -o $@ $<"; $(CC) $(CFLAGS) $(INC) -c -o $@ $<

clean:
	  @echo " Cleaning...";
		@echo " $(CLEAN) && $(RM) -v $(TARGET)"; $(CLEAN) && $(RM) -v $(TARGET)

.PHONY: clean
