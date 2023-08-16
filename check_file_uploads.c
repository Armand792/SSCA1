#include <unistd.h>
#include <syslog.h>


//function to check if the files have been uploaded to the shared directory
// Need to figuer out identify changes and uploads, and by who ? loggin details.
//Also generate a txt. report. 

void check_file_uploads(void) {
      if (access("./shared/Warehouse.json", F_OK) != -1) {
         syslog(LOG_INFO, "Warehouse file uploaded");
      } else {
         syslog(LOG_ERR, "Warehouse file not uploaded");
      }
   
      if (access("./shared/Manufacturing.json", F_OK) != -1) {
         syslog(LOG_INFO, "Manufacturing file uploaded");
      } else {
         syslog(LOG_ERR, "Manufacturing file not uploaded");
      }
   
      if (access("./shared/Marketing.json", F_OK) != -1) {
         syslog(LOG_INFO, "Marketing file uploaded");
      } else {
         syslog(LOG_ERR, "Marketing file not uploaded");
      }

      if (access("./shared/HR.json", F_OK) != -1) {
         syslog(LOG_INFO, "HR file uploaded");
      } else {
         syslog(LOG_ERR, "HR file not uploaded");
      }

      if (access("./shared/Sales.json", F_OK) != -1) {
         syslog(LOG_INFO, "Sales file uploaded");
      } else {
         syslog(LOG_ERR, "Sales file not uploaded");
      }  
   
      return;
}
