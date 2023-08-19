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


//    //loop to reiterate to each json file in directory 
//    for (size_t i = 0; i < sizeof(jsonFiles) / sizeof(jsonFiles[0]); i++){
        
//       //Open each Json file
//       FILE *file = fopen(jsonFiles[i], "r");
        
//       if (file != NULL) {
//          //use predefined strucutre stat to check timestamp in files
//          struct stat fileStat;
//          //check if the info from the Json file was retrieve into the struc, 0 is YES.
//          if (stat(jsonFiles[i], &fileStat) == 0) {

//             //get the username of the file
//             uid_t fileOwnerUID = fileStat.st_uid;
//             struct passwd *ownerInfo = getpwuid(fileOwnerUID);
//             if (ownerInfo != NULL) {
//                if (fileStat.st_mtime >= modifiedThreshold) {
//                   syslog(LOG_INFO, "JSON file %s was modified", jsonFiles[i]);
//                   log_change(jsonFiles[i], "Modified", ownerInfo->pw_name);

//                } else if (fileStat.st_ctime >= createdThreshold) {
//                   syslog(LOG_INFO, "JSON file %s is new", jsonFiles[i]);
//                   log_change(jsonFiles[i], "New", ownerInfo->pw_name);
//                } else  {
//                   syslog(LOG_INFO, "JSON file %s wasn't uploaded", jsonFiles[i]);
//                   log_change(jsonFiles[i], "Not Uploaded", ownerInfo->pw_name);
//                }
//             } else {
//                printf("getpwuid error: %s\n", strerror(errno));
//             }
//          } else {
//             printf("stat error: %s\n", strerror(errno));
//          }
//          fclose(file);
//       } else {
//          printf("fopen error: %s\n", strerror(errno));
//       }
//    }
//    return;
// }