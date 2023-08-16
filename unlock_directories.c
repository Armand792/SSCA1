#include <sys/types.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <syslog.h>

void unlock_directories() {
    //printf("unlock directory functionality should go here. fork/chmod will be used here to change permissions");
    //make sure the directories are unlocked
    chmod("./shared", 0777);
    chmod("./reporting", 0777);

    //check if the directories are unlocked
    struct stat shared;
    struct stat reporting;
    stat("./shared", &shared);
    stat("./reporting", &reporting);
    if (shared.st_mode == 16877 && reporting.st_mode == 16877) {
        syslog(LOG_INFO, "Directories unlocked");
    } else {
        syslog(LOG_ERR, "Directories not unlocked");
    }

    return;
}
