#include <unistd.h>
#include <syslog.h>
#include <time.h> 
#include <stdio.h>
#include <sys/stat.h>
#include <pwd.h>  
#include <errno.h>
#include <string.h>
#include <fcntl.h>  
#include <stdlib.h>

//Identify new or modified json reports and log details of who made the changes, 
//this should be generated as a text file report and stored on the server.

void check_file_uploads(void) {
    openlog("check_file_uploads", LOG_PID, LOG_USER);

    const char *filenames[] = {
        "/workspaces/SSCA1/shared/Warehouse.json",
        "/workspaces/SSCA1/shared/Manufacturing.json",
        "/workspaces/SSCA1/shared/Marketing.json",
        "/workspaces/SSCA1/shared/HR.json",
        "/workspaces/SSCA1/shared/Sales.json"
    };
    
    // Open the named pipe in write mode
    const char * message_to_pipe;
    int pipe_fd = open("/tmp/reporting_fifo", O_WRONLY);
    if (pipe_fd == -1) {
        syslog(LOG_ERR, "Failed to open the named pipe for writing %m");
        exit(EXIT_FAILURE);
    }

    // Redirect stdout to the named pipe
    dup2(pipe_fd, fileno(stdout));
    close(pipe_fd);
    
    pid_t pid;
    int status;
    
    pid = fork();
    if (pid < 0) {
        syslog(LOG_ERR, "fork failed: %m");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {

    // Child process

    // Open or create a text file for writing
    FILE *output_file = fopen("/workspaces/SSCA1/dashboard/uploads.txt", "a");
    if (output_file == NULL) {
        syslog(LOG_ERR, "Failed to open the file for writing %m");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < sizeof(filenames) / sizeof(filenames[0]); i++) {
        struct stat file_info;

        if (stat(filenames[i], &file_info) == 0 && S_ISREG(file_info.st_mode)) {
            
            fprintf(output_file, "File: %s\n", filenames[i]);
            fprintf(output_file, "Owner: %s\n", getpwuid(file_info.st_uid)->pw_name);
            fprintf(output_file, "Last Modified: %s", ctime(&file_info.st_mtime));
            fprintf(output_file, "Last Accessed: %s", ctime(&file_info.st_atime));
            fprintf(output_file, "Recorded at: %s", ctime(&(time_t){time(NULL)})); // Current date and time
            fprintf(output_file, "\n");
        } else {
            // File is missing 
            fprintf(output_file, "File missing : %s\n", filenames[i]);
        }
    }

    // Close the output file when done
    fclose(output_file);

    } else {
        // Parent process
        waitpid(pid, &status, 0);
        if (WIFEXITED(status)) {
            if (WEXITSTATUS(status) == 0) {
                message_to_pipe = "File checking has been succesfull";
                write(fileno(stdout), message_to_pipe, strlen(message_to_pipe));
                syslog(LOG_INFO, "File checking complete");
            } else {
                message_to_pipe = "File checking has failed";
                write(fileno(stdout), message_to_pipe, strlen(message_to_pipe));
                syslog(LOG_ERR, "File checking failed");
                exit(EXIT_FAILURE);
            }
        }
    }
    closelog();
}
