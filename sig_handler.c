#include <unistd.h>
#include <syslog.h>
#include "daemon_task.h"
#include <signal.h>
#include <stdlib.h>

//something needs to be done here. 

void sig_handler(int sigNum)
{
	if (sigNum == SIGINT) {
		openlog("SIGNAL", LOG_PID, LOG_USER);
		syslog(LOG_INFO, "RECEIVED SIGNAL INTERRUPT, INITIATING BACKUP AND TRANSFER");
		closelog();
		lock_directories();
		collect_reports();
		backup_dashboard();
		sleep(30);
		unlock_directories();
	}
}
