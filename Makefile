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

# Compile and run the program
run: $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(OUTPUT) -lm
	./$(OUTPUT)

# Clean up object files and the output executable
clean:
	rm -f $(OBJ) $(OUTPUT)
