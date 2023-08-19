#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <syslog.h>
#include <sys/stat.h>
#include <time.h>
#include "daemon_task.h"
#include <fcntl.h>  

int main() {

    // Create a named pipe (FIFO)
    if (mkfifo("/tmp/output_fifo", 0666) == -1) {
        perror("Failed to create named pipe");
        exit(EXIT_FAILURE);
    }

    time_t now;
    struct tm backup_time;
    time(&now);  /* get current time; same as: now = time(NULL)  */
    backup_time = *localtime(&now);
    backup_time.tm_hour = 12; 
    backup_time.tm_min = 30; 
    backup_time.tm_sec = 0;

    int pid = fork();
    if (pid > 0) {
        // if PID > 0 :: this is the parent
        // this process performs printf and finishes
        sleep(1); // Wait for 5 seconds (adjust as needed)
        exit(EXIT_SUCCESS);
    } else if (pid == 0) {
        // Step 1: Create the orphan process
        
        // Step 2: Elevate the orphan process to session leader, to loose controlling TTY
        // This command runs the process in a new session
        if (setsid() < 0) { exit(EXIT_FAILURE); }
        
        // We could fork here again , just to guarantee that the process is not a session leader
        int pid = fork();
        if (pid > 0) {
            exit(EXIT_SUCCESS);
        } else {
            
            // Step 3: call umask() to set the file mode creation mask to 0
            umask(0);

            // Step 4: Change the current working dir to root.
            // This will eliminate any issues of running on a mounted drive,
            // that potentially could be removed etc..
            if (chdir("/") < 0 ) { exit(EXIT_FAILURE); }
            
            // Step 5: Close all open file descriptors
            /* Close all open file descriptors */
            int x;
             for (x = sysconf(_SC_OPEN_MAX); x>=0; x--)
             {
               close (x);
             } 
            
                // Open the named pipe in read mode
            int pipe_fd = open("/tmp/output_fifo", O_RDONLY);
            if (pipe_fd == -1) {
                perror("Failed to open the named pipe for reading");
                exit(EXIT_FAILURE);
            }

            // Open a text file for writing
            int file_fd = open("/tmp/output.txt", O_WRONLY | O_CREAT | O_TRUNC, 0666);
            if (file_fd == -1) {
                perror("Failed to open the file for writing");
                exit(EXIT_FAILURE);
            }

            // Read data from the named pipe and write to the file
            char buffer[1024]; // Adjust the buffer size as needed
            ssize_t bytes_read;

            while ((bytes_read = read(pipe_fd, buffer, sizeof(buffer))) > 0) {
                write(file_fd, buffer, bytes_read);
            }

            // Close the named pipe and the file
            close(pipe_fd);
            close(file_fd);


      struct tm check_uploads_time;
	  time(&now);  /* get current time; same as: now = time(NULL)  */
	  check_uploads_time = *localtime(&now);
	  check_uploads_time.tm_hour = 9; 
	  check_uploads_time.tm_min = 30; 
	  check_uploads_time.tm_sec = 0;

      
  	  while(1) {
	  	sleep(1);

		if(signal(SIGINT, sig_handler) == SIG_ERR) {
			syslog(LOG_ERR, "ERROR: daemon.c : SIG_ERR RECEIVED");
		} 

		
		//countdown to 12:30
	  	time(&now);
		double seconds_to_files_check = difftime(now,mktime(&check_uploads_time));
		syslog(LOG_INFO, "%.f seconds until check for jml uploads", seconds_to_files_check);
		if(seconds_to_files_check == 0) {
			check_file_uploads();
			//change to tommorow's day
			update_timer(&check_uploads_time);
		}
        
        //countdown to 1:00
		time(&now);
		double seconds_to_transfer = difftime(now, mktime(&backup_time));
		//syslog(LOG_INFO, "%.f seconds until backup", seconds_to_files_check);
		if(seconds_to_transfer == 0) {
			lock_directories();
			collect_reports();	  
			backup_dashboard();
			sleep(30);
			unlock_directories();
			generate_reports();
			//after actions are finished, start counting to next day
			update_timer(&backup_time);
		}	
            
        }
    }
    
    return 0;
}
