#include <sys/types.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <syslog.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>  
#include <string.h>

void lock_directories() {
    //printf("lock directory functionality should go here. fork/chmod will be used here to change permissions");
    openlog("lock_directories", LOG_PID, LOG_USER);

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
        syslog(LOG_ERR, "Fork failed: %m");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        // Child process
        if (execl("/bin/chmod", "chmod", "g-rwx", "/workspaces/SSCA1/shared/", NULL) == -1) {
            syslog(LOG_ERR, "execl failed: %m");
            exit(1);
        }
    } else {
        // Parent process
        waitpid(pid, &status, 0);
        if (WEXITSTATUS(status) == 0) {
            message_to_pipe = "Directory 'shared' has beem locked";
            write(fileno(stdout), message_to_pipe, strlen(message_to_pipe));
            syslog(LOG_INFO, "Directory 'shared' locked");
        } else {
            message_to_pipe = "Directory 'shared' has failed";
            write(fileno(stdout), message_to_pipe, strlen(message_to_pipe));
            syslog(LOG_ERR, "Error locking directory 'shared'");
            exit(EXIT_FAILURE);
        }
        }
    

    pid = fork();
    if (pid < 0) {
        syslog(LOG_ERR, "Fork failed: %m");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        if (execl("/bin/chmod", "chmod", "g-rwx", "/workspaces/SSCA1/dashboard/", NULL) == -1) {
            syslog(LOG_ERR, "execl failed: %m");
            exit(1);
        }
    } else {
        // Parent process
        waitpid(pid, &status, 0);
        if (WIFEXITED(status)) {
            if (WEXITSTATUS(status) == 0) {
                message_to_pipe = "Directory 'dashboard' has beem locked";
                write(fileno(stdout), message_to_pipe, strlen(message_to_pipe));
                syslog(LOG_INFO, "Directory 'dashboard' locked");
            } else {
                message_to_pipe = "Directory 'dashboard' has failed";
                write(fileno(stdout), message_to_pipe, strlen(message_to_pipe));
                syslog(LOG_ERR, "Error locking directory 'dashboard'");
                exit(EXIT_FAILURE);
            }
        }
    }
    closelog();
}