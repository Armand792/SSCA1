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
    pid_t pid;
    int status;

    pid = fork();
    if (pid < 0) {
        syslog(LOG_ERR, "fork failed: %m");
        exit(1);
    }

    if (pid == 0) {
        execl("bin/cp", "cp", "-r", "./dashboard", "./backup", NULL);
        syslog(LOG_ERR, "execl failed: %m");
        exit(1);
    }

    waitpid(pid, &status, 0);
    if (WIFEXITED(status)) {
        if (WEXITSTATUS(status) == 0) {
            syslog(LOG_INFO, "Dashboard backed up");
        } else {
            syslog(LOG_ERR, "Dashboard not backed up");
        }
    }
    
}
