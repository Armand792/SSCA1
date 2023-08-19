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
    char *argv[] = { "/bin/sh", "-c", "ls -al /workspaces/SSCA1/shared/*.json | awk '{print $9, $3, $4, $6, $7}' | tail -n +2", NULL };

    // Open the named pipe in write mode
    int pipe_fd = open("/tmp/output_fifo", O_WRONLY);
    if (pipe_fd == -1) {
        perror("Failed to open the named pipe for writing");
        exit(EXIT_FAILURE);
    }

    // Redirect stdout to the named pipe
    dup2(pipe_fd, fileno(stdout));
    close(pipe_fd);

    execv("/bin/sh", argv);
}