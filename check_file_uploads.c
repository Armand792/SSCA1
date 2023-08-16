#include <unistd.h>
#include <syslog.h>
#include <time.h> 
#include <stdio.h>
#include <sys/stat.h>
#include <pwd.h>  
#include <errno.h>
#include <string.h>


//Identify new or modified json reports and log details of who made the changes, 
//this should be generated as a text file report and stored on the server.

const char *jsonFiles[] = {
    "./shared/warehouse.json",
    "./shared/Manufacturing.json",
    "./shared/Marketing.json",
    "./shared/Hr.json",
    "./shared/Sales.json",
};

void log_change(const char *filename, const char *changeType, const char *username) {
    FILE *logFile = fopen("change_log.txt", "a"); // Open in append mode
    if (logFile != NULL) {
        time_t currentTime = time(NULL);
        struct tm *timeInfo = localtime(&currentTime);
        fprintf(logFile, "File: %s, Change Type: %s, Username: %s, Timestamp: %04d-%02d-%02d %02d:%02d:%02d\n",
                filename, changeType, username,
                timeInfo->tm_year + 1900, timeInfo->tm_mon + 1, timeInfo->tm_mday,
                timeInfo->tm_hour, timeInfo->tm_min, timeInfo->tm_sec);
        fclose(logFile);
    }
}

void check_file_uploads(void) {

   // Calculate time thresholds (e.g., within last 24 hours)
   time_t currentTime = time(NULL);
   time_t modifiedThreshold = currentTime - 24 * 60 * 60; // 24 hours ago
   time_t createdThreshold = currentTime - 2 * 24 * 60 * 60; // 48 hours ago new file

   //loop to reiterate to each json file in directory 
   for (size_t i = 0; i < sizeof(jsonFiles) / sizeof(jsonFiles[0]); i++){
        
      //Open each Json file
      FILE *file = fopen(jsonFiles[i], "r");
        
      if (file != NULL) {
         //use predefined strucutre stat to check timestamp in files
         struct stat fileStat;
         //check if the info from the Json file was retrieve into the struc, 0 is YES.
         if (stat(jsonFiles[i], &fileStat) == 0) {

            //get the username of the file
            uid_t fileOwnerUID = fileStat.st_uid;
            struct passwd *ownerInfo = getpwuid(fileOwnerUID);
            if (ownerInfo != NULL) {
               if (fileStat.st_mtime >= modifiedThreshold) {
                  syslog(LOG_INFO, "JSON file %s was modified", jsonFiles[i]);
                  log_change(jsonFiles[i], "Modified", ownerInfo->pw_name);

               } else if (fileStat.st_ctime >= createdThreshold) {
                  syslog(LOG_INFO, "JSON file %s is new", jsonFiles[i]);
                  log_change(jsonFiles[i], "New", ownerInfo->pw_name);
               }

            } else {printf("getpwuid error: %s\n", strerror(errno));
            }
         } else {printf("stat error: %s\n", strerror(errno));
         }
         fclose(file);
      } else {printf("fopen error: %s\n", strerror(errno));
      }
   }
   return;
}