CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -g -O2
LDFLAGS = -lpthread

# Directories
SRCDIR = src
SOURCES = $(wildcard $(SRCDIR)/**/*.c $(SRCDIR)/*.c)
# Exclude Mithril source files from automatic compilation
SOURCES := $(filter-out $(SRCDIR)/security/mithril/%.c, $(SOURCES))
OBJECTS = $(SOURCES:.c=.o)

# Include Mithril headers
MITHRIL_INCLUDE = -Isrc/security/mithril/include -Isrc/security/mithril

# Mithril library (compile separately or link if pre-built)
MITHRIL_LIB_DIR = src/security/mithril/lib
MITHRIL_LIB = -L$(MITHRIL_LIB_DIR) -lmithril

# Include directories
INCLUDES = -Iinclude $(MITHRIL_INCLUDE)

TARGET = drone_networking

.PHONY: all clean test mithril-build

all: mithril-build $(TARGET)

# Build Mithril library first
mithril-build:
	@echo "Building Mithril security library..."
	@if [ -f src/security/mithril/Makefile ]; then \
		$(MAKE) -C src/security/mithril; \
	elif [ -f src/security/mithril/build.sh ]; then \
		cd src/security/mithril && ./build.sh; \
	else \
		echo "Mithril build system not found, assuming pre-built library"; \
	fi

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS) $(MITHRIL_LIB)

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

test:
	@echo "Running tests..."
	# Add test compilation here when tests are added

clean:
	rm -f $(OBJECTS) $(TARGET)
	@if [ -f src/security/mithril/Makefile ]; then \
		$(MAKE) -C src/security/mithril clean; \
	fi

install: $(TARGET)
	@echo "Installing $(TARGET)..."
	# Add installation commands here

# Dependencies
$(SRCDIR)/main.o: $(SRCDIR)/core/network_core.h $(SRCDIR)/protocols/message_protocol.h $(SRCDIR)/communication/udp_comm.h $(SRCDIR)/security/encryption_wrapper.h
$(SRCDIR)/core/network_core.o: include/common.h
$(SRCDIR)/protocols/message_protocol.o: include/common.h
$(SRCDIR)/communication/udp_comm.o: include/common.h $(SRCDIR)/protocols/message_protocol.h
$(SRCDIR)/security/encryption_wrapper.o: include/common.h $(SRCDIR)/protocols/message_protocol.h src/security/mithril/mithril.h

debug:
	@echo "Sources: $(SOURCES)"
	@echo "Objects: $(OBJECTS)"
	@echo "Mithril included from: src/security/mithril/"