

# Specify the compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall -Wextra

# Source files
SRC = backup_dashboard.c check_files_uploads.c collect_reports.c daemon.c generate_reports.c \
      lock_directories.c sig_handler.c unlock_directories.c update_timer.c

# Object files (derived from source files)
OBJ = $(SRC:.c=.o)

# Name of the output executable
OUTPUT = myprogram

# Default target (the output executable)
all: $(OUTPUT)

# Compile each source file into an object file
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Link the object files to create the output executable
$(OUTPUT): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(OUTPUT) -lm

# Clean up object files and the output executable
clean:
	rm -f $(OBJ) $(OUTPUT)

