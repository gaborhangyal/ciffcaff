CC := g++
CFLAGS := -std=c++11 -Wall -Wextra

# Name of the executable file
TARGET := parser

# Source files
SRCS := ciffcaff.c

# Object files
OBJS := $(SRCS:.cpp=.o)

# Compile the source files into object files
%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

# Link the object files to create the executable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@ -ljpeg -lstdc++

# Clean the object files and the executable
clean:
	rm -f $(OBJS) $(TARGET)
