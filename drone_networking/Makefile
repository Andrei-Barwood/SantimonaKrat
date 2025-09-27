CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -g
LDFLAGS = -lpthread

# Directories
SRCDIR = src
SOURCES = $(wildcard $(SRCDIR)/**/*.c $(SRCDIR)/*.c)
OBJECTS = $(SOURCES:.c=.o)
TARGET = drone_networking

# Include directories
INCLUDES = -Iinclude

.PHONY: all clean test

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

test:
	@echo "Running tests..."
	# Add test compilation here when tests are added

clean:
	rm -f $(OBJECTS) $(TARGET)

install: $(TARGET)
	@echo "Installing $(TARGET)..."
	# Add installation commands here

# Dependencies
$(SRCDIR)/main.o: $(SRCDIR)/core/network_core.h $(SRCDIR)/protocols/message_protocol.h $(SRCDIR)/communication/udp_comm.h
$(SRCDIR)/core/network_core.o: include/common.h
$(SRCDIR)/protocols/message_protocol.o: include/common.h
$(SRCDIR)/communication/udp_comm.o: include/common.h $(SRCDIR)/protocols/message_protocol.h

debug:
	@echo "Sources: $(SOURCES)"
	@echo "Objects: $(OBJECTS)"