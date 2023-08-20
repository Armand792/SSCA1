#include <time.h>
#include <unistd.h>
#include <stdio.h>
#include <syslog.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>

void backup_dashboard(void) {
    //create a backup directory if it doesn't exist
    //copy the contents of the dashboard directory to the backup directory
    //log the backup
    //return
    openlog("backup_dashboard", LOG_PID, LOG_USER);
    
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
    

    // Fork the first child process to create the backup directory
    pid = fork();
    if (pid < 0) {
        syslog(LOG_ERR, "First fork failed: %m");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        // Child process 1: Create the backup directory if it doesn't exist
        if (execl("/bin/mkdir", "mkdir", "-p", "/workspaces/SSCA1/backup", NULL) == -1) {
            syslog(LOG_ERR, "Error creating backup directory: %m");
            exit(EXIT_FAILURE);
        }
    } else {
        // Parent process: Wait for the first child process to complete
        waitpid(pid, &status, 0);

        if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
            message_to_pipe = "Backup directory was succesfully created";
            write(fileno(stdout), message_to_pipe, strlen(message_to_pipe));
            syslog(LOG_INFO, "Backup directory created successfully");
        } else {
            message_to_pipe = "Backup directory was not created";
            write(fileno(stdout), message_to_pipe, strlen(message_to_pipe));
            syslog(LOG_ERR, "Error creating backup directory");
            exit(EXIT_FAILURE);
        }
    }

    // Fork the second child process to copy files
    pid = fork();
    if (pid < 0) {
        syslog(LOG_ERR, "Second fork failed: %m");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        // Child process 2: Copy files from dashboard to backup
        if (execl("bin/cp", "cp", "-r", "/workspaces/SSCA1/dashboard/", "/workspaces/SSCA1/backup/", NULL) == -1) {
            syslog(LOG_ERR, "Error copying files to backup: %m");
            exit(EXIT_FAILURE);
        }
    } else {
        // Parent process: Wait for the second child process to complete
        waitpid(pid, &status, 0);

        if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
            syslog(LOG_INFO, "Files copied to backup successfully");
        } else {
            syslog(LOG_ERR, "Error copied files to backup");
            exit(EXIT_FAILURE);
        }
    }
    closelog();
}
