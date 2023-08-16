#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <syslog.h>
#include <stdlib.h>

void collect_reports(void) {
    
    //collect the reports from the shared directory
    pid_t pid;
    int status;

    pid = fork();
    if (pid < 0) {
        syslog(LOG_ERR, "fork failed: %m");
        exit(1);
    }
    //command to collect the reports from the shared directory
    if (pid == 0) {
        execl("/bin/mv", "mv", "./shared/*", "./reporting", NULL);
        syslog(LOG_ERR, "execl failed: %m");
        exit(1);
    }
    waitpid(pid, &status, 0);
    if (WIFEXITED(status)) {
        if (WEXITSTATUS(status) == 0) {
            syslog(LOG_INFO, "Reports collected");
        } else {
            syslog(LOG_ERR, "Reports not collected");
        }
    }
}