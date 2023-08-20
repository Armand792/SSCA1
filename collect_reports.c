#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <syslog.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>

void collect_reports(void) {
    //collect the reports from the shared directory
    openlog("collect_reports", LOG_PID, LOG_USER);

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
    
    // Command to collect the reports from the shared directory
    if (pid == 0) {
        if (execl("/bin/mv", "-c", "mv", "/workspaces/SSCA1/shared/*.json", "/workspaces/SSCA1/reporting", NULL) == -1) {
            syslog(LOG_ERR, "execl failed: %m");
            exit(EXIT_FAILURE);
        }
    } else {

    // Parent process
    waitpid(pid, &status, 0);
    if (WIFEXITED(status)) {
        if (WEXITSTATUS(status) == 0) {
            message_to_pipe = "Reporting has been succesfull";
            write(fileno(stdout), message_to_pipe, strlen(message_to_pipe));
            syslog(LOG_INFO, "Reports collected");
        } else {
            message_to_pipe = "Reporting has failed";
            write(fileno(stdout), message_to_pipe, strlen(message_to_pipe));
            syslog(LOG_ERR, "Reports not collected");
            exit(EXIT_FAILURE);
        }
    }
    }
    closelog();
}