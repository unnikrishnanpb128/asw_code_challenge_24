# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Idevice_interface -Idevice_driver -Iport

# Source files
SRCS = main.c device_interface/device_interface.c device_driver/magnetometer_driver.c port/i2c.c

# Object files
OBJS = $(SRCS:.c=.o)

# Executable name
EXEC = TestApplication

# Default target
all: $(EXEC)

# Compile each source file into object files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Link all object files into the executable
$(EXEC): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@

# Clean up
clean:
	rm -f $(OBJS) $(EXEC)

