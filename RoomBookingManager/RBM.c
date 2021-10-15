        #include <stdio.h>
        #include <stdlib.h>
        #include <string.h>
        #include <unistd.h>
        #include <sys/wait.h>
        
        struct booking {
                int recordId;           // record id (assigned by the main scheduler)
                int day;                // day since 10-05-2021
                int time;               // time in hour (e.g. 12:00 in "hhmm"" = 12)
                int length;             // length in hour (e.g. 4.0 in p.p = 4)
                int pplCount;           // number of participants in the booking
                char bookingType[20];   // type of booking (meeting, presentation or conference)
                char tenantLetter[2];   // letter representing the tenant (e.g. "tenant_A" = "A")
                char device[2][20];     // device booked
                char roomLetter;        // room letter 
        };
        
        struct algo_report {
                int received_booking;
                int assigned_booking;
                int rejected_booking;
        };

        struct room_report {
                int roomA;
                int roomB;
                int roomC;
        };

        struct device_report {
                int webcamFHD;
                int webcamUHD;
                int monitor50;
                int monitor75;
                int projector2K;
                int projector4K;
                int screen100;
                int screen150;
        };

        int comparator(const void *p, const void *q) 
        {
        int l = ((struct booking *)p)->length;
        int r = ((struct booking *)q)->length; 
        return (r - l);
        }

        int main () {
                /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                // PARENT SECTION //
                // -- variable initalization -- //
                int i,j,k;                                          // some varaibles for loop
                int parentToSch[2];                                 // fd for pipes from parent to main scheduler
                int schToParent[2];                                 // fd for pipes from main scheduler to parent
                int parentPid = -1;                                 // pid for distinguishing parent and main scheduler
                
                // -- main scheduler and pipe creation -- //
                // create two pipes between parent and main scheduler //
                if (pipe(parentToSch) < 0 || pipe(schToParent) < 0) {
                        printf("Error: Pipe Creation\n");
                }
        
                // fork() to create main scheduler //
                parentPid = fork();
        
                // if it is main scheduler //
                if (parentPid == 0) {
                        close(parentToSch[1]);                      // close the writing side of parentToSch
                        close(schToParent[0]);                      // close the reading side of schToParent
                }
        
                // otherwise, it is parent //
                else {
                        close(parentToSch[0]);                      // close the reading side of parentToSch
                        close(schToParent[1]);                      // close the writing side of schToParent
                }
        
                /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                // MAIN SCHEDULER SECTION //
                if (parentPid == 0) {
                        // -- variable initialization -- // 
                        int schToAlgo[3][2];                            // fd for pipes from main scheduler and sub-scheduler (algorithm)
                        int algoToSch[3][2];                            // fd for pipes from sub-scheduler to main scheduler
                        int mainSchPid = -1;                            // pid retrieved from fork() (the fork() to create sub-scheduler)
                        int algoType = -1;                              // the algorithm type (fcfs = 0, prio = 1, opti = 2)
        
                        // -- pipe and sub-scheduler creation -- //
                        for (i=0; i<3; i++) {
                                if (pipe(schToAlgo[i]) < 0 || pipe(algoToSch[i]) < 0) {
                                        printf("Error: Pipe creation in main scheduler\n");
                                }
                                algoType++;
                                mainSchPid = fork();
        
                                // if it is one of the sub-schedulers //
                                if (mainSchPid == 0) {
                                        close(schToAlgo[i][1]);         // close the writing side of schToAlgo
                                        close(algoToSch[i][0]);         // close the reading side of algoToSch
                                        break;
                                }
        
                                // otherwise, it is the main scheduler //
                                else {
                                        close(schToAlgo[i][0]);         // close the reading side of schToAlgo
                                        close(algoToSch[i][1]);         // close the writing side of algoToSch
                                }
                        }
        
        
                /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                // MAIN SCHEDULER SECTION //
                        if (mainSchPid != 0) {
                                // -- variable initialization -- //
                                int argCount = 0;                               // number of arguments in the input (to be determined)
                                char tokInputString[20][100];                   // tokenized input string (receive from pipe)
                                int charRead = 0;                               // number of char read from pipe (from parent)
                                int recordCount = 0;                            // number of record (or recordId+1) 
                                int commandToAlgo = 0;                          // flag to tell sub-schedulers what to do (-1 = exit, 0 = add booking, 1 = print booking)
                                char tempString[20];                            // temporary string for conversion of numbers
                                int deviceGood = 0;                             // whether device name is correct
                                struct booking tempBooking;                     // create a booking struct to be passed to the subschedulers
                                int invaildRequestCount = 0;
        
        
                                do {
                                        // -- receiving input from parent -- //
                                        memset(tokInputString, 0, sizeof(tokInputString));                              // initialize tokInputString
                                        charRead = read(parentToSch[0], &tokInputString, sizeof(tokInputString));                         // read from parent through pipe (in terms of 2-D array)
                                        argCount = 0;                                                                   // initialize the count of argument
                                        while ((tokInputString[argCount][0] != 0) && (argCount <= 20)) argCount++;      // retrieve the number of argument
                                        deviceGood = 0;                                                                 // initialize the valid flag for device name
        
                                        // -- if parent sends endProgram, break the loop -- //
                                        if (strcmp(tokInputString[0], "endProgram;") == 0) {
                                                puts("-> Bye!");
                                                commandToAlgo = -1;                                                     // set flag to sub-schedulers to tell them to stop
                                                write(schToAlgo[0][1], &commandToAlgo, sizeof(int));                    // send the flag to fcfs
                                                write(schToAlgo[1][1], &commandToAlgo, sizeof(int));                    // send the flag to prio
                                                write(schToAlgo[2][1], &commandToAlgo, sizeof(int));                    // send the flag to opti
                                        }
        
                                        // -- otherwise, construct a "booking" variable from the tokenized string -- //
                                        else {
                                               
                                                if (strcmp(tokInputString[0], "printBookings") != 0) {
                                                        puts("-> [Pending]");
                                                        memset(&tempBooking, 0, sizeof(struct booking));      
                                                        // record id //
                                                        tempBooking.recordId = recordCount++;                                   // assign the record id, and increment it afterwards
                                                      
                                                        // tenant letter //
                                                        if (tokInputString[1][0] == '-') {
                                                                strncpy(tempBooking.tenantLetter, tokInputString[1]+8, 1);             // take "A" in "tenant_A"
                                                        }else {
                                                                strncpy(tempBooking.tenantLetter, tokInputString[1]+10, 1);             // take "A" in "tenant_A"
                                                        }
                                                        tempBooking.tenantLetter[1] = 0;                                        // null terminate the string
        
                                                        // day //
                                                        strncpy(tempString, tokInputString[2]+8, 2);                            // take "DD" in "YYYY-MM-DD"
                                                        tempString[2] = 0;                                                      // null terminate the string
                                                        tempBooking.day = atoi(tempString) - 10;                                // assign the value of how many days since 10-05-2021
        
                                                        // time //
                                                        strncpy(tempString, tokInputString[3], 2);                              // take "hh" in "hh:mm"
                                                        tempString[2] = 0;                                                      // null terminate the string
                                                        tempBooking.time = atoi(tempString);                                     // assign the time to the booking (0-23)
        
                                                        // length //
                                                        if (strlen(tokInputString[4]) == 3) {                                   // if "n" in "n.n" has one digit
                                                                strncpy(tempString, tokInputString[4], 1);                      // take "n" in "n.n"
                                                                tempString[1] = 0;                                              // null terminate the string
                                                        }
                                                        else {                                                                  // otherwise, "n" has two digits
                                                                strncpy(tempString, tokInputString[4], 2);                      // take "nn" in "nn.n"
                                                                tempString[2] = 0;                                              // null terminate the string
                                                        }
                                                        tempBooking.length = atoi(tempString);                                  // assign the length
        
        
                                                        // number of participants (bookDevice does not have this option) //
                                                        if (strcmp(tokInputString[0], "bookDevice") != 0) {
                                                                // pplCount
                                                                strcpy(tempString, tokInputString[5]);                          // take "p"
                                                                tempBooking.pplCount = atoi(tempString);                        // assign "p" as integer
                                                                                                                                // note that ";" might be behind "p", but atoi ignores it
                                                               
                                                                // (no) devices //
                                                                if (argCount == 6) {
                                                                        strcpy(tempBooking.device[0], "*");                      // assign device as empty string
                                                                        strcpy(tempBooking.device[1], "");                      // ditto
                                                                        
                                                                }
        
                                                                // (a pair of) devices //
                                                                if (argCount == 8) {
                                                                        // check the device name //
                                                                        strcpy(tempBooking.device[0], tokInputString[6]);
                                                                        strcpy(tempBooking.device[1], tokInputString[7]);
                                                                        tempBooking.device[1][strcspn(tempBooking.device[1], ";")] = 0;
                                                                }
                                                              
                                                                
                                                                // booking type //
                                                                strcpy(tempBooking.bookingType, tokInputString[0]+3);           // get "Meeting" in "addMeeting"
                                                                //tempBooking.bookingType[0] += 32;                               // make "M" in "Meeting" lowercase
        
                                                                // check whether device name is valid //
                                                                // first device = one of the webcams && second device
                                                                if (strcmp(tempBooking.device[0], "") != 0 && strcmp(tempBooking.device[1], "")) {
                                                                        if (((((strcmp(tempBooking.device[0], "webcam_FHD") == 0)   || (strcmp(tempBooking.device[0], "webcam_UHD") == 0)) &&
                                                                            ((strcmp(tempBooking.device[1], "monitor_50") == 0)   || (strcmp(tempBooking.device[1], "monitor_75") == 0))) ||
                                                                           (((strcmp(tempBooking.device[0], "projector_2K") == 0) || (strcmp(tempBooking.device[0], "projector_4K") == 0)) &&
                                                                            ((strcmp(tempBooking.device[1], "screen_100") == 0)   || (strcmp(tempBooking.device[1], "screen_150") == 0)))) ||
                                                                           ((((strcmp(tempBooking.device[1], "webcam_FHD") == 0)   || (strcmp(tempBooking.device[1], "webcam_UHD") == 0)) &&
                                                                            ((strcmp(tempBooking.device[0], "monitor_50") == 0)   || (strcmp(tempBooking.device[0], "monitor_75") == 0))) ||
                                                                           (((strcmp(tempBooking.device[1], "projector_2K") == 0) || (strcmp(tempBooking.device[1], "projector_4K") == 0)) &&
                                                                            ((strcmp(tempBooking.device[0], "screen_100") == 0)   || (strcmp(tempBooking.device[0], "screen_150") == 0))))) {
                                                                                deviceGood = 1;
                                                                        } else {
                                                                                invaildRequestCount++;
                                                                        }
                                                                }
                                                                else {
                                                                        deviceGood = 1;
                                                                }
                                                        }
        
                                                        // if it is bookDevice, set pplCount as 0 //
                                                        else {
                                                                // pplCount
                                                                tempBooking.pplCount = 0;
        
                                                                // devices //
                                                                strcpy(tempBooking.device[0], tokInputString[5]);                   // assign device with ";"
                                                                tempBooking.device[0][strcspn(tempBooking.device[0], ";")] = 0;     // remove that ";"
                                                                strcpy(tempBooking.device[1], "");                                  // set second device as ""
        
                                                                // bookingType //
                                                                strcpy(tempBooking.bookingType, "Device");
        
                                                                // check whether device name is valid //
                                                                if (strcmp(tempBooking.device[0], "webcam_FHD") == 0 || strcmp(tempBooking.device[0], "webcam_UHD") == 0 ||
                                                                    strcmp(tempBooking.device[0], "monitor_50") == 0 || strcmp(tempBooking.device[0], "monitor_75") == 0 ||
                                                                    strcmp(tempBooking.device[0], "projector_2K") == 0 || strcmp(tempBooking.device[0], "projector_4K") == 0 || 
                                                                    strcmp(tempBooking.device[0], "screen_100") == 0 || strcmp(tempBooking.device[0], "screen_150") == 0) {
                                                                        deviceGood = 1;
                                                                } else {
                                                                        invaildRequestCount++;
                                                                } 
                                                        }
        
                                                        // printf("recordId: %d\n", tempBooking.recordId);
                                                        // printf("day: %d\n", tempBooking.day);
                                                        // printf("time: %d\n", tempBooking.time);
                                                        // printf("length: %d\n", tempBooking.length);
                                                        // printf("pplCount: %d\n", tempBooking.pplCount);
                                                        // printf("bookingType: %s\n", tempBooking.bookingType);
                                                        // printf("tenantLetter: %s\n", tempBooking.tenantLetter);
                                                        // printf("device1: %s\n", tempBooking.device[0]);
                                                        // printf("device2: %s\n", tempBooking.device[1]);
                                                        
                                                        // -- pipe the flag and booking to one of the algorithms -- //
                                                        // if the device name is valid //
                                                        if (deviceGood) {
                                                                commandToAlgo = 0;
                                                                write(schToAlgo[0][1], &commandToAlgo, sizeof(int));
                                                                write(schToAlgo[0][1], &tempBooking, sizeof (struct booking));
                                                                write(schToAlgo[1][1], &commandToAlgo, sizeof(int));
                                                                write(schToAlgo[1][1], &tempBooking, sizeof (struct booking));
                                                                write(schToAlgo[2][1], &commandToAlgo, sizeof(int));
                                                                write(schToAlgo[2][1], &tempBooking, sizeof (struct booking));
                                                        }
                                                        else {
                                                                printf("Error: Invalid device name. Booking aborted\n");
                                                        }
                                                       
                                                        printf("Please enter booking:\n");
    
                                                }
                                               
                                                // -- tell sub-schedulers to print bookings -- //
                                                else {
                                                        puts("-> [Done!]");
                                                        commandToAlgo = 1;
                                                        if (strcmp(tokInputString[1], "-fcfs;") == 0) {
                                                                write(schToAlgo[0][1], &commandToAlgo, sizeof(int));
                                                        } else if (strcmp(tokInputString[1], "-prio;") == 0) {
                                                                write(schToAlgo[1][1], &commandToAlgo, sizeof(int));
                                                        } else if (strcmp(tokInputString[1], "-opti;") == 0) {
                                                                write(schToAlgo[2][1], &commandToAlgo, sizeof(int));
                                                        } else if (strcmp(tokInputString[1], "-ALL;") == 0) {
                                                                commandToAlgo = 2;

                                                                struct algo_report fcfs_booking_report;
                                                                struct room_report fcfs_room_report;
                                                                struct device_report fcfs_device_report;

                                                                struct algo_report prio_booking_report;
                                                                struct room_report prio_room_report;
                                                                struct device_report prio_device_report;

                                                                struct algo_report opti_booking_report;
                                                                struct room_report opti_room_report;
                                                                struct device_report opti_device_report;
                                                                
                                                                write(schToAlgo[0][1], &commandToAlgo, sizeof(int));
                                                                read(algoToSch[0][0], &fcfs_booking_report, sizeof(struct algo_report));
                                                                read(algoToSch[0][0], &fcfs_room_report, sizeof(struct room_report));
                                                                read(algoToSch[0][0], &fcfs_device_report, sizeof(struct device_report));
                                                       
                                                                write(schToAlgo[1][1], &commandToAlgo, sizeof(int));
                                                                read(algoToSch[1][0], &prio_booking_report, sizeof(struct algo_report));
                                                                read(algoToSch[1][0], &prio_room_report, sizeof(struct room_report));
                                                                read(algoToSch[1][0], &prio_device_report, sizeof(struct device_report));

                                                                write(schToAlgo[2][1], &commandToAlgo, sizeof(int));
                                                                read(algoToSch[2][0], &opti_booking_report, sizeof(struct algo_report));
                                                                read(algoToSch[2][0], &opti_room_report, sizeof(struct room_report));
                                                                read(algoToSch[2][0], &opti_device_report, sizeof(struct device_report));

                                                                puts("*** Room Booking Manager - Summary Report ***\n");
                                                                puts("Performance:\n");
                                                             
                                                                puts("  For FCFS:");
                                                                printf("          Total Number of Booking Received: %d (%.2f%%)\n", fcfs_booking_report.received_booking, (float) fcfs_booking_report.received_booking/fcfs_booking_report.received_booking *100.0);
                                                                printf("                Number of Booking Assigned: %d (%.2f%%)\n", fcfs_booking_report.assigned_booking, (float) fcfs_booking_report.assigned_booking/fcfs_booking_report.received_booking *100.0);
                                                                printf("                Number of Booking Rejected: %d (%.2f%%)\n", fcfs_booking_report.rejected_booking, (float) fcfs_booking_report.rejected_booking/fcfs_booking_report.received_booking *100.0);
                                                                puts("");
                                                                printf("          Utilization of Time Slot:\n");
                                                                printf("                room_A          - %0.2f%%\n", (float) fcfs_room_report.roomA / (7*24) * 100.0);
                                                                printf("                room_B          - %0.2f%%\n", (float) fcfs_room_report.roomB / (7*24) * 100.0);
                                                                printf("                room_C          - %0.2f%%\n", (float) fcfs_room_report.roomC / (7*24) * 100.0);
                                                                printf("                webcam_FHD      - %0.2f%%\n", (float) fcfs_device_report.webcamFHD / (7*24) * 100.0);
                                                                printf("                webcam_UHD      - %0.2f%%\n", (float) fcfs_device_report.webcamUHD / (7*24) * 100.0);
                                                                printf("                monitor_50      - %0.2f%%\n", (float) fcfs_device_report.monitor75 / (7*24) * 100.0);
                                                                printf("                monitor_75      - %0.2f%%\n", (float) fcfs_device_report.monitor75 / (7*24) * 100.0);
                                                                printf("                projector_2K    - %0.2f%%\n", (float) fcfs_device_report.projector2K / (7*24) * 100.0);
                                                                printf("                projector_4K    - %0.2f%%\n", (float) fcfs_device_report.projector4K / (7*24) * 100.0);
                                                                printf("                screen_100      - %0.2f%%\n", (float) fcfs_device_report.screen100 / (7*24) * 100.0);
                                                                printf("                screen_150      - %0.2f%%\n", (float) fcfs_device_report.screen150 / (7*24) * 100.0);
                                                                printf("          Invaild request(s) made: %d\n\n", invaildRequestCount);

                                                                puts("  For PRIO:");
                                                                printf("          Total Number of Booking Received: %d (%.2f%%)\n", prio_booking_report.received_booking, (float) prio_booking_report.received_booking/prio_booking_report.received_booking *100.0);
                                                                printf("                Number of Booking Assigned: %d (%.2f%%)\n", prio_booking_report.assigned_booking, (float) prio_booking_report.assigned_booking/prio_booking_report.received_booking *100.0);
                                                                printf("                Number of Booking Rejected: %d (%.2f%%)\n", prio_booking_report.rejected_booking, (float) prio_booking_report.rejected_booking/prio_booking_report.received_booking *100.0);
                                                                puts("");
                                                                printf("          Utilization of Time Slot:\n");
                                                                printf("                room_A          - %0.2f%%\n", (float) prio_room_report.roomA / (7*24) * 100.0);
                                                                printf("                room_B          - %0.2f%%\n", (float) prio_room_report.roomB / (7*24) * 100.0);
                                                                printf("                room_C          - %0.2f%%\n", (float) prio_room_report.roomC / (7*24) * 100.0);
                                                                printf("                webcam_FHD      - %0.2f%%\n", (float) prio_device_report.webcamFHD / (7*24) * 100.0);
                                                                printf("                webcam_UHD      - %0.2f%%\n", (float) prio_device_report.webcamUHD / (7*24) * 100.0);
                                                                printf("                monitor_50      - %0.2f%%\n", (float) prio_device_report.monitor75 / (7*24) * 100.0);
                                                                printf("                monitor_75      - %0.2f%%\n", (float) prio_device_report.monitor75 / (7*24) * 100.0);
                                                                printf("                projector_2K    - %0.2f%%\n", (float) prio_device_report.projector2K / (7*24) * 100.0);
                                                                printf("                projector_4K    - %0.2f%%\n", (float) prio_device_report.projector4K / (7*24) * 100.0);
                                                                printf("                screen_100      - %0.2f%%\n", (float) prio_device_report.screen100 / (7*24) * 100.0);
                                                                printf("                screen_150      - %0.2f%%\n", (float) prio_device_report.screen150 / (7*24) * 100.0);
                                                                printf("          Invaild request(s) made: %d\n\n", invaildRequestCount);
                                                               
                                                                puts("  For OPTI:");
                                                                printf("          Total Number of Booking Received: %d (%.2f%%)\n", opti_booking_report.received_booking, (float) opti_booking_report.received_booking/opti_booking_report.received_booking *100.0);
                                                                printf("                Number of Booking Assigned: %d (%.2f%%)\n", opti_booking_report.assigned_booking, (float) opti_booking_report.assigned_booking/opti_booking_report.received_booking *100.0);
                                                                printf("                Number of Booking Rejected: %d (%.2f%%)\n", opti_booking_report.rejected_booking, (float) opti_booking_report.rejected_booking/opti_booking_report.received_booking *100.0);
                                                                puts("");
                                                                printf("          Utilization of Time Slot:\n");
                                                                printf("                room_A          - %0.2f%%\n", (float) opti_room_report.roomA / (7*24) * 100.0);
                                                                printf("                room_B          - %0.2f%%\n", (float) opti_room_report.roomB / (7*24) * 100.0);
                                                                printf("                room_C          - %0.2f%%\n", (float) opti_room_report.roomC / (7*24) * 100.0);
                                                                printf("                webcam_FHD      - %0.2f%%\n", (float) opti_device_report.webcamFHD / (7*24) * 100.0);
                                                                printf("                webcam_UHD      - %0.2f%%\n", (float) opti_device_report.webcamUHD / (7*24) * 100.0);
                                                                printf("                monitor_50      - %0.2f%%\n", (float) opti_device_report.monitor75 / (7*24) * 100.0);
                                                                printf("                monitor_75      - %0.2f%%\n", (float) opti_device_report.monitor75 / (7*24) * 100.0);
                                                                printf("                projector_2K    - %0.2f%%\n", (float) opti_device_report.projector2K / (7*24) * 100.0);
                                                                printf("                projector_4K    - %0.2f%%\n", (float) opti_device_report.projector4K / (7*24) * 100.0);
                                                                printf("                screen_100      - %0.2f%%\n", (float) opti_device_report.screen100 / (7*24) * 100.0);
                                                                printf("                screen_150      - %0.2f%%\n", (float) opti_device_report.screen150 / (7*24) * 100.0);
                                                                printf("          Invaild request(s) made: %d\n\n", invaildRequestCount);
                                                                printf("Please enter booking:\n");
                                                        }
                                                                                              
                                                }
                                                
                                        }
                                } while (strcmp(tokInputString[0], "endProgram;") != 0);
                                
                                wait(NULL);                     // main scheduler wait all sub-scheduler
                                exit(EXIT_SUCCESS);             // main scheduler exit
                        }
         
                /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                // SUB-SCHEDULER (FCFS) SECTION //
                        else {
                                if (algoType == 0) {
                                        int fcfsToComp[2][2];                   // pipe fd from fcfs to components
                                        int compToFcfs[2][2];                   // pipe fd from components to fcfs
                                        int fcfsPid = -1;                       // pid inside fcfs scheduler 
                                        int compId = -1;                        // id to identify the component (0 = room, 1 = devices)
        
                                        // -- pipe and child creation in fcfs -- //
                                        for (i=0; i<2; i++) {
                                                if (pipe(fcfsToComp[i]) < 0 || pipe(compToFcfs[i]) < 0) {
                                                        printf("Error: Pipe creation in FCFS Scheduler\n");
                                                } 
        
                                                compId++;
                                                fcfsPid = fork();
        
                                                // if it is components //
                                                if (fcfsPid == 0) {
                                                        close(fcfsToComp[i][1]);        // close the writing side of fcfsToComp
                                                        close(compToFcfs[i][0]);        // close the reading side of compToFcfs
                                                        break;                          // break to prevent fcfs component creating more child
                                                }
        
                                                // otherwise, it is fcfs scheduler //
                                                else {
                                                        close(fcfsToComp[i][0]);        // close the reading side of fcfsToComp
                                                        close(compToFcfs[i][1]);        // close the writing side of compToFcfs
                                                }
                                        } 
                
                /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                // FCFS BODY ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////  
                                        if (fcfsPid != 0) {
                                             // -- variable initialization -- //
                                                int acceptedRoomAList[7*24];          // list containing accepted room A records
                                                int acceptedRoomBList[7*24];          // list containing accepted room B records
                                                int acceptedRoomCList[7*24];          // list containing accepted room C records
                                                int acceptedDeviceList[7*24][100];         // list containing accepted device records
                                                int rejectedBookingList[3000];        // list containing rejected room records
                                                int rejectedBookingCount = 0;               // number of rejected booking
                                                int acceptedBookingCount = 0;
                                                int allBookingCount = 0;
                                                int commandFromMainSch = 0;                 // flag received from main scheduler, determining whether to make a booking or print bookings
                                                int rejected = 0;                           // whether the booking is to be rejected
                                                char roomLetter;                            // letter of the room (for switch case below)
                                                char tenantTarget;                          // the tenant to be printed (as char)
                                                int tenantTargetInt;                        // the tenant to be printed (as int)
                                                int received = 0;                           // check comp has received the data
                                                int room_rejected;
                                                int device_rejected;
                                                int total_rejected;
                                                
                                                struct booking allBooking[3000];            // list containing all bookings
                                                struct booking tempBooking;                 // temporary booking
                                                struct booking newBooking;
                                                
                                                                                        
                                                
                                                do {    
                                                     
                                                        // -- reading from main scheduler -- //
                                                        read(schToAlgo[0][0], &commandFromMainSch, sizeof(int));
                                                       
                                                        // -- if main scheduler wants a booking -- //
                                                        if (commandFromMainSch == 0) {
                                                                // first read //
                                                                read(schToAlgo[0][0], &newBooking, sizeof(struct booking));            // read the booking from main schedule                                                         
                                                                allBooking[newBooking.recordId] = newBooking;
                                                                allBookingCount++;
                                                                continue;                                              
                                                                
                                                        }
                                                         // -- if main scheduler wants printing -- //
                                                        if (commandFromMainSch == 1 || commandFromMainSch == 2) {    
                                                              
                                                                 // Reset all records //
                                                                memset(acceptedRoomAList, -1, 7*24*sizeof(int));
                                                                memset(acceptedRoomBList, -1, 7*24*sizeof(int));
                                                                memset(acceptedRoomCList, -1, 7*24*sizeof(int));
                                                                memset(acceptedDeviceList, -1, 7*24*100*sizeof(int));
                                                                rejectedBookingCount = 0;
                                                                acceptedBookingCount = 0;
                                                            
                                                            
                                                                int resetCommand = 9;
                                                                int addCommand = 1;
                                                                int analyzeCommand = 2;

                                                                write(fcfsToComp[0][1], &resetCommand, sizeof(int));              // tell the room that reset the record
                                                                write(fcfsToComp[1][1], &resetCommand, sizeof(int));              // tell the device that reset the record

                                                                
                                                                
                                                                for (j=0; j<allBookingCount; j++){
                                                                      
                                                                        room_rejected = 0;
                                                                        device_rejected = 0;
                                                                        total_rejected = 0;
                                                                        tempBooking = allBooking[j];
                                                                                                                                       
                                                                        // second write //
                                                                        write(fcfsToComp[0][1], &addCommand, sizeof(int));                                                                                    
                                                                        write(fcfsToComp[1][1], &addCommand, sizeof(int));              

                                                                        read(compToFcfs[0][0], &received, sizeof(int));
                                                                        read(compToFcfs[1][0], &received, sizeof(int));
                                                                        
                                                                        // third write //
                                                                        write(fcfsToComp[0][1], &tempBooking, sizeof(struct booking));          // send the booking to room, see if room has space left
                                                                        
                                                                        // fourth read //
                                                                        read(compToFcfs[0][0], &room_rejected, sizeof(int));                         // read from room. see if room rejects the booking
                                                                       
                                                                        // if the room says ok, we see see if device is ok //
                                                                        if (room_rejected) total_rejected = 1;

                                                                        // fifth write //
                                                                        write(fcfsToComp[1][1], &tempBooking, sizeof(struct booking));  // send the booking to device, see if device has space left
                                                                        
                                                                        // sixth read //
                                                                        read(compToFcfs[1][0], &device_rejected, sizeof(int));                 // read whether device say ok
                                                                        if (device_rejected) total_rejected = 1;        
                                         
                                                                        // seventh write //
                                                                        write(fcfsToComp[0][1], &total_rejected, sizeof(int));                        // send the final result to room                 
                                                                        // eighth write //
                                                                        write(fcfsToComp[1][1], &total_rejected, sizeof(int));                        // send the final result to device
                                                                        
                
                                                                        // if the booking is rejected, add it to the rejected list //
                                                                        if (total_rejected) {
                                                                                rejectedBookingList[rejectedBookingCount++] = tempBooking.recordId;
                                                                        }
                
                                                                        // if the booking is accepted, set the slots in the fcfs body //
                                                                        else {                                                                   
                                                                                // nineth read //
                                                                                read(compToFcfs[0][0], &roomLetter, sizeof(char));              // get the roomLetter from room 
                                                                                
                                                                                acceptedBookingCount++;
                                                                                if (roomLetter == 'A') {
                                                                                        acceptedRoomAList[tempBooking.day*24 + tempBooking.time] = tempBooking.recordId;
                                                                                        
                                                                                }
                                                                                if (roomLetter == 'B') {
                                                                                        acceptedRoomBList[tempBooking.day*24 + tempBooking.time] = tempBooking.recordId;
                                                                                        
                                                                                }
                                                                                if (roomLetter == 'C') {
                                                                                        acceptedRoomCList[tempBooking.day*24 + tempBooking.time] = tempBooking.recordId;
                                                                                        
                                                                                }
                                                                                if (roomLetter == 'D') {
                                                                                        int searchIndex = 0;
                                                                                        // search for empty slot in acceptedDeviceList
                                                                                        while (acceptedDeviceList[tempBooking.day*24 + tempBooking.time][searchIndex] != -1) {
                                                                                                searchIndex++;
                                                                                        }
                                                                                        acceptedDeviceList[tempBooking.day*24 + tempBooking.time][searchIndex] = tempBooking.recordId;
                                                                                        
                                                                                }
                                                                        }
                                                                        
                                                                }
                                                                

                                                                printf("*** Room Booking - ACCEPTED / FCFS ***\n");
                                                                for (tenantTargetInt = 65; tenantTargetInt <= 69; tenantTargetInt++) {
                                                                        tenantTarget = (char) tenantTargetInt;
                                                                        printf("\nTenant_%c has the following bookings:\n\n", tenantTarget);
                                                                        printf("Date        Start   End   Type           Room    Device\n");
                                                                        printf("===========================================================================\n");
                                                                        for (i=0; i<7*24; i++) {
                                                                                if (acceptedRoomAList[i] != -1 && allBooking[acceptedRoomAList[i]].tenantLetter[0] == tenantTarget){
                                                                                        int id = acceptedRoomAList[i];
                                                                                        printf("2021-05-%02d  %02d:00  %02d:00  %-13s  room_A  %s\n", allBooking[id].day+10, allBooking[id].time, allBooking[id].time + allBooking[id].length, allBooking[id].bookingType, allBooking[id].device[0]);
                                                                                        if (strcmp(allBooking[id].device[1], "") != 0) printf("%59s\n", allBooking[id].device[1]);
                                                                                }
                                                                                if (acceptedRoomBList[i] != -1 && allBooking[acceptedRoomBList[i]].tenantLetter[0] == tenantTarget){
                                                                                        int id = acceptedRoomBList[i];
                                                                                        printf("2021-05-%02d  %02d:00  %02d:00  %-13s  room_B  %s\n", allBooking[id].day+10, allBooking[id].time, allBooking[id].time + allBooking[id].length, allBooking[id].bookingType, allBooking[id].device[0]);
                                                                                        if (strcmp(allBooking[id].device[1], "") != 0) printf("%59s\n", allBooking[id].device[1]);
                                                                                }
                                                                                if (acceptedRoomCList[i] != -1 && allBooking[acceptedRoomCList[i]].tenantLetter[0] == tenantTarget){
                                                                                        int id = acceptedRoomCList[i];
                                                                                        printf("2021-05-%02d  %02d:00  %02d:00  %-13s  room_C  %s\n", allBooking[id].day+10, allBooking[id].time, allBooking[id].time + allBooking[id].length, allBooking[id].bookingType, allBooking[id].device[0]);
                                                                                        if (strcmp(allBooking[id].device[1], "") != 0) printf("%59s\n", allBooking[id].device[1]);
                                                                                }
                                                                                for (j=0; j<100; j++) {
                                                                                        if (acceptedDeviceList[i][j] != -1 && allBooking[acceptedDeviceList[i][j]].tenantLetter[0] == tenantTarget){
                                                                                                int id = acceptedDeviceList[i][j];
                                                                                                printf("2021-05-%02d  %02d:00  %02d:00  *              *       %s\n", allBooking[id].day+10, allBooking[id].time, allBooking[id].time + allBooking[id].length, allBooking[id].device[0]);
                                                                                                if (strcmp(allBooking[id].device[1], "") != 0) printf("%59s\n", allBooking[id].device[1]);
                                                                                        }
                                                                                }
                                                                        }
        
                                                                }               
        
                                                                printf("\n*** Room Booking - REJECTED / FCFS ***\n\n");
                                                                int tenantRejectedBookingCount;
                                                                for (tenantTargetInt = 65; tenantTargetInt <= 69; tenantTargetInt++) {
                                                                        tenantTarget = (char) tenantTargetInt;
                                                                        tenantRejectedBookingCount = 0;
                                                                        for (i=0; i<rejectedBookingCount; i++) {
                                                                                if (allBooking[rejectedBookingList[i]].tenantLetter[0] == tenantTarget) tenantRejectedBookingCount++;
                                                                        }
                                                                        printf("\nTenants_%c (there are %d bookings rejected):\n", tenantTarget, tenantRejectedBookingCount);
                                                                        printf("Date        Start   End   Type           Device\n");
                                                                        printf("===========================================================================\n");
                                                                        for (i=0; i<rejectedBookingCount; i++) {
                                                                                if (allBooking[rejectedBookingList[i]].tenantLetter[0] == tenantTarget) {
                                                                                        int id = rejectedBookingList[i];
                                                                                        printf("2021-05-%02d  %02d:00  %02d:00  %-13s  %s\n", allBooking[id].day+10, allBooking[id].time, allBooking[id].time + allBooking[id].length, allBooking[id].bookingType, allBooking[id].device[0]);
                                                                                        if (strcmp(allBooking[id].device[1], "") != 0) printf("%51s\n", allBooking[id].device[1]);
                                                                                }
                                                                        }
        
                                                                }

                                                                puts("  - End -\n");
                                                                if (commandFromMainSch == 2) {
                                                                        struct algo_report fcfs_booking_report;
                                                                        struct room_report fcfs_room_report;
                                                                        struct device_report fcfs_device_report;

                                                                        fcfs_booking_report.received_booking = allBookingCount;
                                                                        fcfs_booking_report.assigned_booking = acceptedBookingCount;
                                                                        fcfs_booking_report.rejected_booking = rejectedBookingCount;

                                                                        write(fcfsToComp[0][1], &analyzeCommand, sizeof(int));
                                                                        read(compToFcfs[0][0], &fcfs_room_report, sizeof(struct room_report));
                                                                        write(fcfsToComp[1][1], &analyzeCommand, sizeof(int));
                                                                        read(compToFcfs[1][0], &fcfs_device_report, sizeof(struct device_report));

                                                                        write(algoToSch[0][1], &fcfs_booking_report, sizeof(struct algo_report));
                                                                        write(algoToSch[0][1], &fcfs_room_report, sizeof(struct room_report));
                                                                        write(algoToSch[0][1], &fcfs_device_report, sizeof(struct device_report));
           
                                                                      
                                                                } else {
                                                                        printf("Please enter booking:\n");
                                                                }
                                                                continue;
                                                        }
        
                                                        // -- if main scheduler wants to die -- //
                                                        if (commandFromMainSch == -1) {
                                                                int endCommand = -1;
                                                                //printf("FCFS ejecting\n");
                                                                write(fcfsToComp[0][1], &endCommand, sizeof(int));                                                                                          
                                                                write(fcfsToComp[1][1], &endCommand, sizeof(int));               
                                                                break;
                                                        }
                                                       
                                                        
                                                } while (commandFromMainSch != -1);
                                                wait(NULL); 
                                                exit(EXIT_SUCCESS);
                                        }
                /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                // FCFS COMPONENT - ROOM //
                                        else { 
                                                if (compId == 0) {
                                                        struct booking tempBooking;
                                                        int roomA[7*24] = {0};          // array to store which timeslot is occupied in room A
                                                        int roomB[7*24] = {0};          // ditto, for room B
                                                        int roomC[7*24] = {0};          // ditto, for room C
                                                        int rejected = 0;               // whether the booking request is rejected due to conflicts
                                                        int day = 0;                    // copy of day in tempBooking
                                                        int time = 0;                   // copy of time in tempBooking
                                                        int pplCount = 0;               // copy of pplCount in tempBooking
                                                        int commandFromBody = 0;        // flag sent from fcfs body
                                                        int length = 0;                 // copy of length in tempBooking
                                                        char roomLetter;                // the room to be assigned

                                                        
                                                        int recevied = 0;
                                                        do {
                                                            
                                                                // second read //
                                                                read(fcfsToComp[0][0], &commandFromBody, sizeof(int));                  // check whether we need to break the loop
                                                              
                                                                if (commandFromBody == -1) break;                                       // break if it is -1
                                                                
                                                                if (commandFromBody == 2) {
                                                                        struct room_report fcfs_room_report;
                                                                        int roomA_util = 0;
                                                                        int roomB_util = 0;
                                                                        int roomC_util = 0;
                                                                        for (i=0; i<7*24; i++) {
                                                                                if (roomA[i] > 0) roomA_util++;
                                                                                if (roomB[i] > 0) roomB_util++;
                                                                                if (roomC[i] > 0) roomC_util++;
                                                                        }
                                                                        fcfs_room_report.roomA = roomA_util;
                                                                        fcfs_room_report.roomB = roomB_util;
                                                                        fcfs_room_report.roomC = roomC_util;
                                                                        
                                                                        write(compToFcfs[0][1], &fcfs_room_report, sizeof(struct room_report));
                                                                        continue;
                                                                }

                                                                // if command = 0, reset the room A B C record
                                                                if (commandFromBody == 9) {
                                                                        
                                                                        memset(roomA, 0, 7*24*sizeof(int));
                                                                        memset(roomB, 0, 7*24*sizeof(int));
                                                                        memset(roomC, 0, 7*24*sizeof(int));
                                                                        continue;
                                                                }

                                                                if (commandFromBody == 1){
                                                                       
                                                                        // tell fcfs this child get the command
                                                                        write(compToFcfs[0][1], &recevied, sizeof(int)); 

                                                                        // third read, read the booking from fcfs body //
                                                                        read(fcfsToComp[0][0], &tempBooking, sizeof(struct booking));
                                                                        
                                                                        pplCount = tempBooking.pplCount;
                                                                        day = tempBooking.day;
                                                                        time = tempBooking.time;
                                                                        length = tempBooking.length;
                                                                        rejected = 0;
                
                                                                        // -- for number of people <= 10, we search room A and room B first -- //
                                                                        if (pplCount > 0 && pplCount <= 10) {
                                                                                // room A //
                                                                                roomLetter = 'A'; 
                                                                                for (i=0; i<length; i++) {
                                                                                        if (roomA[day*24 + time + i] != 0) {
                                                                                                rejected = 1;
                                                                                                break;
                                                                                        }
                                                                                }
                
                                                                                // if room A is full, search in room B //
                                                                                if (rejected) {
                                                                                        rejected = 0;
                                                                                        roomLetter = 'B';
                                                                                        for (i=0; i<length; i++) {
                                                                                                if (roomB[day*24 + time + i] != 0) {
                                                                                                        rejected = 1;
                                                                                                        break;
                                                                                                }
                                                                                        }
                                                                                }
                
                                                                                // if room B is full, search in room C //
                                                                                if (rejected) {
                                                                                        rejected = 0;
                                                                                        roomLetter = 'C';
                                                                                        for (i=0; i<length; i++) {
                                                                                                if (roomC[day*24 + time + i] != 0) {
                                                                                                        rejected = 1;
                                                                                                        break;
                                                                                                }
                                                                                        }
                                                                                }
                                                                        }
                
                                                                        // -- otherwise for number of people <= 20, we search room C -- //
                                                                        else if (pplCount > 10 && pplCount <= 20) {
                                                                                roomLetter = 'C';
                                                                                for (i=0; i<length; i++) {
                                                                                        if (roomC[day*24 + time + i] != 0) {
                                                                                                rejected = 1;
                                                                                                break;
                                                                                        }
                                                                                }
                                                                        }
                
                
                                                                        // -- otherotherwise, number of people == 0, it is device -- //
                                                                        else if (pplCount == 0) {
                                                                                roomLetter = 'D';
                                                                        }
                
                                                                        // -- write the search result to fcfs body -- //
                                                                        // fourth write //
                                                                        write(compToFcfs[0][1], &rejected, sizeof(int));                // send the booking result to fcfs body
                                                                        
                                                                        // seventh read //
                                                                        read(fcfsToComp[0][0], &rejected, sizeof(int));                 // get whether device has rejected the the booking 
                
                                                                        // -- set the slot if it is not rejected -- //
                                                                        if (!rejected) {
                                                                                for (i=0; i<tempBooking.length; i++) {
                                                                                        if (roomLetter == 'A') {
                                                                                                roomA[day*24 + time + i] = 1;
                                                                                        }
                                                                                        if (roomLetter == 'B') {
                                                                                                roomB[day*24 + time + i] = 1;
                                                                                        }
                                                                                        if (roomLetter == 'C') {
                                                                                                roomC[day*24 + time + i] = 1;
                                                                                        }
                                                                                }
                                                                                // nineth write //
                                                                                write(compToFcfs[0][1], &roomLetter, sizeof(char));     // tell the body what room is booked
                                                                        }
                                                                }
                                                        } while (commandFromBody != -1);
                                                        exit(EXIT_SUCCESS);
                                                }
        
                /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                // FCFS COMPONENT - DEVICES //
                                                if (compId == 1) {
                                                        int commandFromBody = 0; 
                                                        struct booking tempBooking;
                                                        int webcamFHD[7*24];
                                                        int webcamUHD[7*24];
                                                        int monitor50[7*24];
                                                        int monitor75[7*24];
                                                        int projector2K[7*24];
                                                        int projector4K[7*24];
                                                        int screen100[7*24];
                                                        int screen150[7*24];

                                                        int recevied = 0;
                                                        int rejected = 0;
                                                        
                                                        
                                                        do {
                                                                // second read //
                                                                read(fcfsToComp[1][0], &commandFromBody, sizeof(int));                  // read flag from fcfs body 
                                                               
                                                                if (commandFromBody == - 1) break;                                      // if the flag is -1, break the loop

                                                                if (commandFromBody == 2) {
                                                                        struct device_report fcfs_device_report;
                                                                        int webcamFHD_util = 0;
                                                                        int webcamUHD_util = 0;
                                                                        int monitor50_util = 0;
                                                                        int monitor75_util = 0;
                                                                        int projector2K_util = 0;
                                                                        int projector4K_util = 0;
                                                                        int screen100_util = 0;
                                                                        int screen150_util = 0;
                                                                        for (i=0; i<7*24; i++) {
                                                                                if (webcamFHD[i] > 0) webcamFHD_util++;
                                                                                if (webcamUHD[i] > 0) webcamUHD_util++;
                                                                                if (monitor50[i] > 0) monitor50_util++;
                                                                                if (monitor75[i] > 0) monitor75_util++;
                                                                                if (projector2K[i] > 0) projector2K_util++;
                                                                                if (projector4K[i] > 0) projector4K_util++;
                                                                                if (screen100[i] > 0) screen100_util++;
                                                                                if (screen150[i] > 0) screen150_util++;
                                                                        }
                                                                        fcfs_device_report.webcamFHD = webcamFHD_util;
                                                                        fcfs_device_report.webcamUHD = webcamUHD_util;
                                                                        fcfs_device_report.monitor50 = monitor50_util;
                                                                        fcfs_device_report.monitor75 = monitor75_util;
                                                                        fcfs_device_report.projector2K = projector2K_util;
                                                                        fcfs_device_report.projector4K = projector4K_util;
                                                                        fcfs_device_report.screen100 = screen100_util;
                                                                        fcfs_device_report.screen150 = screen150_util;
                                                                        
                                                                        write(compToFcfs[1][1], &fcfs_device_report, sizeof(struct device_report));
                                                                        continue;
                                                                }
                                                                
                                                                if (commandFromBody == 9) {
                                                                       
                                                                        memset(webcamFHD, 0, 7*24*sizeof(int));
                                                                        memset(webcamUHD, 0, 7*24*sizeof(int));
                                                                        memset(monitor50, 0, 7*24*sizeof(int));
                                                                        memset(monitor75, 0, 7*24*sizeof(int));
                                                                        memset(projector2K, 0, 7*24*sizeof(int));
                                                                        memset(projector4K, 0, 7*24*sizeof(int));
                                                                        memset(screen100, 0, 7*24*sizeof(int));
                                                                        memset(screen150, 0, 7*24*sizeof(int));
                                                                        continue;
                                                                }
                                                                
                                                                if (commandFromBody == 1) {
                                                                        // tell fcfs this child get the command
                                                                        write(compToFcfs[1][1], &recevied, sizeof(int)); 

                                                                        // fifth read //
                                                                        read(fcfsToComp[1][0], &tempBooking, sizeof(struct booking));           // get booking from fcfs body
                                                                
                                                                        int day = tempBooking.day;
                                                                        int time = tempBooking.time;
                                                                        rejected = 0;
                
                                                                        
                                                                        // -- check whether the devices are available in the range of time -- //
                                                                        for (i=0; i<2; i++) {
                                                                                if (rejected) break;
                                                                                for (j=0; j<tempBooking.length; j++) {
                                                                                        if (strcmp(tempBooking.device[i], "webcam_FHD") == 0) {
                                                                                                if (webcamFHD[day*24 + time + j] >= 2) rejected = 1;
                                                                                        }
                                                                                        if (strcmp(tempBooking.device[i], "webcam_UHD") == 0) {
                                                                                                if (webcamUHD[day*24 + time + j] >= 1) rejected = 1;
                                                                                        }
                                                                                        if (strcmp(tempBooking.device[i], "monitor_50") == 0) {
                                                                                                if (monitor50[day*24 + time + j] >= 2) rejected = 1;
                                                                                        }
                                                                                        if (strcmp(tempBooking.device[i], "monitor_75") == 0) {
                                                                                                if (monitor75[day*24 + time + j] >= 1) rejected = 1;
                                                                                        }
                                                                                        if (strcmp(tempBooking.device[i], "projector_2K") == 0) {
                                                                                                if (projector2K[day*24 + time + j] >= 2) rejected = 1;
                                                                                        }
                                                                                        if (strcmp(tempBooking.device[i], "projector_4K") == 0) {
                                                                                                if (projector4K[day*24 + time + j] >= 1) rejected = 1;
                                                                                        }
                                                                                        if (strcmp(tempBooking.device[i], "screen_100") == 0 ){
                                                                                                if (screen100[day*24 + time + j] >= 2) rejected = 1;
                                                                                        }
                                                                                        if (strcmp(tempBooking.device[i], "screen_150") == 0) {
                                                                                                if (screen150[day*24 + time + j] >= 1) rejected = 1;
                                                                                        }
                                                                                }
                                                                        }
                                                                
                                                                        // sixth write //
                                                                        write(compToFcfs[1][1], &rejected, sizeof(int));            // tell fcfs body whether it is rejected
                
                                                                        // eighth read //
                                                                        read(fcfsToComp[1][0], &rejected, sizeof(int));             // get from fcfs whether to accept and set the booking                                                           
                                                                        
                                                                        // -- set the slot if it is not rejected -- //
                                                                        if (!rejected) {
                                                                                for (i=0; i<2; i++) {
                                                                                        for (j=0; j<tempBooking.length; j++) {
                                                                                                if (strcmp(tempBooking.device[i], "webcam_FHD") == 0) {
                                                                                                        webcamFHD[day*24 + time + j]++;
                                                                                                }
                                                                                                if (strcmp(tempBooking.device[i], "webcam_UHD") == 0) {
                                                                                                        webcamUHD[day*24 + time + j]++;
                                                                                                }
                                                                                                if (strcmp(tempBooking.device[i], "monitor_50") == 0) {
                                                                                                        monitor50[day*24 + time + j]++;
                                                                                                }
                                                                                                if (strcmp(tempBooking.device[i], "monitor_75") == 0) {
                                                                                                        monitor75[day*24 + time + j]++;
                                                                                                }
                                                                                                if (strcmp(tempBooking.device[i], "projector_2K") == 0) {
                                                                                                        projector2K[day*24 + time + j]++;
                                                                                                }
                                                                                                if (strcmp(tempBooking.device[i], "projector_4K") == 0) {
                                                                                                        projector4K[day*24 + time + j]++;
                                                                                                }
                                                                                                if (strcmp(tempBooking.device[i], "screen_100") == 0 ){
                                                                                                        screen100[day*24 + time + j]++;
                                                                                                }
                                                                                                if (strcmp(tempBooking.device[i], "screen_150") == 0) {
                                                                                                        screen150[day*24 + time + j]++;
                                                                                                }
                                                                                        }
                                                                                }
                                                                        }
                                                                        continue;
                                                                }
                                                        } while (commandFromBody != -1);
                                                        exit(EXIT_SUCCESS);
                                                }
                                        }
                                }
                
                
                
                
                /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                // SUB-SCHEDULER (PRIO) SECTION //
                                if (algoType == 1) {
                                        int prioToComp[2][2];                           // pipe fd from prio to components
                                        int compToPrio[2][2];                           // pipe fd from components to prio
                                        int prioPid = -1;                               // pid inside prio scheduler
                                        int compId = -1;                                // component Id in prio (0 = room, 1 = device)
        
                                        // -- pipe and component creation in prio -- //
                                        for (i=0; i<2; i++) {
                                                if (pipe(prioToComp[i]) < 0 || pipe(compToPrio[i]) < 0) {
                                                        printf("Error: Pipe creation in PRIO scheduler\n");
                                                }
        
                                                compId++;
                                                prioPid = fork();
                                                
                                                // if it is component //
                                                if (prioPid == 0) {
                                                        close(prioToComp[i][1]);        // close the writing side of prioToComp
                                                        close(compToPrio[i][0]);        // close the reading side of compToPrio
                                                        break;                          // break to prevent component creating more child
                                                }
        
                                                // if it is prio scheduler //
                                                else {
                                                        close(prioToComp[i][0]);        // close the reading side of prioToComp
                                                        close(compToPrio[i][1]);        // close the writing side of compToPrio
                                                }
                                        }
                                        
                /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                // PRIO BODY //
                                        if (prioPid != 0) {
                                             // -- variable initialization -- //
                                                int acceptedRoomAList[7*24];          // list containing accepted room A records
                                                int acceptedRoomBList[7*24];          // list containing accepted room B records
                                                int acceptedRoomCList[7*24];          // list containing accepted room C records
                                                int acceptedDeviceList[7*24][100];         // list containing accepted device records
                                                int rejectedBookingList[3000];        // list containing rejected room records
                                                int rejectedBookingCount = 0;               // number of rejected booking
                                                int acceptedBookingCount = 0;
                                                int allBookingCount = 0;
                                                int commandFromMainSch = 0;                 // flag received from main scheduler, determining whether to make a booking or print bookings
                                                int rejected = 0;                           // whether the booking is to be rejected
                                                char roomLetter;                            // letter of the room (for switch case below)
                                                char tenantTarget;                          // the tenant to be printed (as char)
                                                int tenantTargetInt;                        // the tenant to be printed (as int)
                                                int received = 0;                           // check comp has received the data
                                                int room_rejected;
                                                int device_rejected;
                                                int total_rejected;
                                                
                                                struct booking allBooking[3000];            // list containing all bookings
                                                struct booking tempBooking;                 // temporary booking
                                                struct booking newBooking;
                                                
                                                
                                                char prioTypeList[4][20] = {"Conference", "Presentation", "Meeting", "Device"};

                                                do {    
                                                    
                                                        // -- reading from main scheduler -- //
                                                        read(schToAlgo[1][0], &commandFromMainSch, sizeof(int));
                                                       
                                                        // -- if main scheduler wants a booking -- //
                                                        if (commandFromMainSch == 0) {
                                                                // first read //
                                                                read(schToAlgo[1][0], &newBooking, sizeof(struct booking));            // read the booking from main schedule                                                         
                                                                allBooking[newBooking.recordId] = newBooking;
                                                                allBookingCount++;                                                       
                                                                
                                                        }
                                                         // -- if main scheduler wants printing -- //
                                                        if (commandFromMainSch == 1 || commandFromMainSch == 2) {    
                                                            
                                                                 // Reset all records //
                                                                memset(acceptedRoomAList, -1, 7*24*sizeof(int));
                                                                memset(acceptedRoomBList, -1, 7*24*sizeof(int));
                                                                memset(acceptedRoomCList, -1, 7*24*sizeof(int));
                                                                memset(acceptedDeviceList, -1, 7*24*100*sizeof(int));
                                                                rejectedBookingCount = 0;
                                                                acceptedBookingCount = 0;

                                                                int resetCommand = 9;
                                                                int addCommand = 1;
                                                                int analyzeCommand = 2;
                                                                write(prioToComp[0][1], &resetCommand, sizeof(int));              // tell the room that reset the record
                                                                write(prioToComp[1][1], &resetCommand, sizeof(int));              // tell the device that reset the record

                                                                
                                                                for (i=0; i<4; i++){
                                                                        for (j=0; j<allBookingCount; j++){
                                                                                //memset(&tempBooking, 0, sizeof(struct booking));
                                                                                tempBooking = allBooking[j];
                                                                               
                                                                                if (strcmp(allBooking[j].bookingType, prioTypeList[i]) != 0)    continue;
                                                                                room_rejected = 0;
                                                                                device_rejected = 0;
                                                                                total_rejected = 0;

                                                                                
                                                                                // second write //
                                                                                write(prioToComp[0][1], &addCommand, sizeof(int));                                                                                    
                                                                                write(prioToComp[1][1], &addCommand, sizeof(int));             

                                                                                read(compToPrio[0][0], &received, sizeof(int));
                                                                                read(compToPrio[1][0], &received, sizeof(int));
                                                                                // third write //
                                                                                write(prioToComp[0][1], &tempBooking, sizeof(struct booking));          // send the booking to room, see if room has space left
                                                                                
                                                                                // fourth read //
                                                                                read(compToPrio[0][0], &room_rejected, sizeof(int));                         // read from room. see if room rejects the booking
                                                                                                                                             
                                                                                if (room_rejected) {
                                                                                        total_rejected = 1;
                                                                                }
                                                                                // if the room says ok, we see see if device is ok //
                                                                               
                                                                                // fifth write //
                                                                                write(prioToComp[1][1], &tempBooking, sizeof(struct booking));  // send the booking to device, see if device has space left
                                                                                
                                                                                // sixth read //
                                                                                read(compToPrio[1][0], &device_rejected, sizeof(int));                 // read whether device say ok
                                                                                
                                                                                if (device_rejected) {
                                                                                        total_rejected = 1;
                                                                                }      
                                                                                
                                                                                
                                                                                // seventh write //
                                                                                write(prioToComp[0][1], &total_rejected, sizeof(int));                        // send the final result to room                 
                                                                                // eighth write //
                                                                                write(prioToComp[1][1], &total_rejected, sizeof(int));                        // send the final result to device
                                                                              
                                                                                
                        
                                                                                // if the all is rejected, add it to the rejected list //
                                                                                if (total_rejected) {
                                                                                        rejectedBookingList[rejectedBookingCount++] = tempBooking.recordId;

                                                                                }
                        
                                                                                // if the booking is accepted, set the slots in the fcfs body //
                                                                                else {
                                                                                        // nineth read //
                                                                                        read(compToPrio[0][0], &roomLetter, sizeof(char));              // get the roomLetter from room 
                                                                                        acceptedBookingCount++;
                                                                                        if (roomLetter == 'A') {
                                                                                                acceptedRoomAList[tempBooking.day*24 + tempBooking.time] = tempBooking.recordId;
                                                                                                
                                                                                        }
                                                                                        if (roomLetter == 'B') {
                                                                                                acceptedRoomBList[tempBooking.day*24 + tempBooking.time] = tempBooking.recordId;
                                                                                              
                                                                                        }
                                                                                        if (roomLetter == 'C') {
                                                                                                acceptedRoomCList[tempBooking.day*24 + tempBooking.time] = tempBooking.recordId;
                                                                                                
                                                                                        }
                                                                                        if (roomLetter == 'D') {
                                                                                                int searchIndex = 0;
                                                                                                // search for empty slot in acceptedDeviceList
                                                                                                while (acceptedDeviceList[tempBooking.day*24 + tempBooking.time][searchIndex] != -1) {
                                                                                                        searchIndex++;
                                                                                                }
                                                                                                acceptedDeviceList[tempBooking.day*24 + tempBooking.time][searchIndex] = tempBooking.recordId;
                                                                                              
                                                                                        }
                                                                                }
                                                                                
                                                                        }
                                                                }
                                                                
                                                         
                                                              

                                                                printf("*** Room Booking - ACCEPTED / PRIO ***\n");
                                                                for (tenantTargetInt = 65; tenantTargetInt <= 69; tenantTargetInt++) {
                                                                        tenantTarget = (char) tenantTargetInt;
                                                                        printf("\nTenant_%c has the following bookings:\n", tenantTarget);
                                                                        printf("Date        Start   End   Type           Room    Device\n");
                                                                        printf("===========================================================================\n");
                                                                        for (i=0; i<7*24; i++) {
                                                                                if (acceptedRoomAList[i] != -1 && allBooking[acceptedRoomAList[i]].tenantLetter[0] == tenantTarget){
                                                                                        int id = acceptedRoomAList[i];
                                                                                        printf("2021-05-%02d  %02d:00  %02d:00  %-13s  room_A  %s\n", allBooking[id].day+10, allBooking[id].time, allBooking[id].time + allBooking[id].length, allBooking[id].bookingType, allBooking[id].device[0]);
                                                                                        if (strcmp(allBooking[id].device[1], "") != 0) printf("%59s\n", allBooking[id].device[1]);
                                                                                }
                                                                                if (acceptedRoomBList[i] != -1 && allBooking[acceptedRoomBList[i]].tenantLetter[0] == tenantTarget){
                                                                                        int id = acceptedRoomBList[i];
                                                                                        printf("2021-05-%02d  %02d:00  %02d:00  %-13s  room_B  %s\n", allBooking[id].day+10, allBooking[id].time, allBooking[id].time + allBooking[id].length, allBooking[id].bookingType, allBooking[id].device[0]);
                                                                                        if (strcmp(allBooking[id].device[1], "") != 0) printf("%59s\n", allBooking[id].device[1]);
                                                                                }
                                                                                if (acceptedRoomCList[i] != -1 && allBooking[acceptedRoomCList[i]].tenantLetter[0] == tenantTarget){
                                                                                        int id = acceptedRoomCList[i];
                                                                                        printf("2021-05-%02d  %02d:00  %02d:00  %-13s  room_C  %s\n", allBooking[id].day+10, allBooking[id].time, allBooking[id].time + allBooking[id].length, allBooking[id].bookingType, allBooking[id].device[0]);
                                                                                        if (strcmp(allBooking[id].device[1], "") != 0) printf("%59s\n", allBooking[id].device[1]);
                                                                                }
                                                                                for (j=0; j<100; j++) {
                                                                                        if (acceptedDeviceList[i][j] != -1 && allBooking[acceptedDeviceList[i][j]].tenantLetter[0] == tenantTarget){
                                                                                                int id = acceptedDeviceList[i][j];
                                                                                                printf("2021-05-%02d  %02d:00  %02d:00  *              *       %s\n", allBooking[id].day+10, allBooking[id].time, allBooking[id].time + allBooking[id].length, allBooking[id].device[0]);
                                                                                                if (strcmp(allBooking[id].device[1], "") != 0) printf("%59s\n", allBooking[id].device[1]);
                                                                                        }
                                                                                }
                                                                        }
        
                                                                }               
        
                                                                printf("\n*** Room Booking - REJECTED / PRIO ***\n");
                                                                int tenantRejectedBookingCount;
                                                                for (tenantTargetInt = 65; tenantTargetInt <= 69; tenantTargetInt++) {
                                                                        tenantTarget = (char) tenantTargetInt;
                                                                        tenantRejectedBookingCount = 0;
                                                                        for (i=0; i<rejectedBookingCount; i++) {
                                                                                if (allBooking[rejectedBookingList[i]].tenantLetter[0] == tenantTarget) tenantRejectedBookingCount++;
                                                                        }
                                                                        printf("\nTenants_%c (there are %d bookings rejected):\n", tenantTarget, tenantRejectedBookingCount);
                                                                        printf("Date        Start   End   Type           Device\n");
                                                                        printf("===========================================================================\n");
                                                                        for (i=0; i<rejectedBookingCount; i++) {
                                                                                if (allBooking[rejectedBookingList[i]].tenantLetter[0] == tenantTarget) {
                                                                                        int id = rejectedBookingList[i];
                                                                                        printf("2021-05-%02d  %02d:00  %02d:00  %-13s  %s\n", allBooking[id].day+10, allBooking[id].time, allBooking[id].time + allBooking[id].length, allBooking[id].bookingType, allBooking[id].device[0]);
                                                                                        if (strcmp(allBooking[id].device[1], "") != 0) printf("%51s\n", allBooking[id].device[1]);
                                                                                }
                                                                        }
        
                                                                }

                                                                puts("  - end -\n");
                                                                if (commandFromMainSch == 2) {
                                                                       
                                                                        struct algo_report prio_booking_report;
                                                                        struct room_report prio_room_report;
                                                                        struct device_report prio_device_report;

                                                                        prio_booking_report.received_booking = allBookingCount;
                                                                        prio_booking_report.assigned_booking = acceptedBookingCount;
                                                                        prio_booking_report.rejected_booking = rejectedBookingCount;

                                                                        write(prioToComp[0][1], &analyzeCommand, sizeof(int));
                                                                        read(compToPrio[0][0], &prio_room_report, sizeof(struct room_report));
                                                                        write(prioToComp[1][1], &analyzeCommand, sizeof(int));
                                                                        read(compToPrio[1][0], &prio_device_report, sizeof(struct device_report));

                                                                        write(algoToSch[1][1], &prio_booking_report, sizeof(struct algo_report));
                                                                        write(algoToSch[1][1], &prio_room_report, sizeof(struct room_report));
                                                                        write(algoToSch[1][1], &prio_device_report, sizeof(struct device_report));
                                                                } else {
                                                                        printf("Please enter booking:\n");
                                                                }
                                                                continue;
                                                        }
        
                                                        // -- if main scheduler wants to die -- //
                                                        if (commandFromMainSch == -1) {
                                                                int endCommand = -1;
                                                                //printf("PRIO ejecting\n");
                                                                write(prioToComp[0][1], &endCommand, sizeof(int));                                                                                          
                                                                write(prioToComp[1][1], &endCommand, sizeof(int));    
                                                                break;    
                                                        }
                                                       
                                                    
                                                } while (commandFromMainSch != -1);
                                                wait(NULL); 
                                                exit(EXIT_SUCCESS);
                                        }
        
                /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                // PRIO COMPONENT - ROOM //
                                        else {
                                                if (compId == 0) {
                                                        struct booking tempBooking;
                                                        int roomA[7*24] = {0};          // array to store which timeslot is occupied in room A
                                                        int roomB[7*24] = {0};          // ditto, for room B
                                                        int roomC[7*24] = {0};          // ditto, for room C
                                                        int rejected = 0;               // whether the booking request is rejected due to conflicts
                                                        int day = 0;                    // copy of day in tempBooking
                                                        int time = 0;                   // copy of time in tempBooking
                                                        int pplCount = 0;               // copy of pplCount in tempBooking
                                                        int commandFromBody = 0;        // flag sent from prio body
                                                        int length = 0;                 // copy of length in tempBooking
                                                        char roomLetter;                // the room to be assigned

                                                        int analyzeCommand = 2;
                                                        int recevied = 0;
                                                        do {
                                                            
                                                                // second read //
                                                                read(prioToComp[0][0], &commandFromBody, sizeof(int));                  // check whether we need to break the loop
                                                               
                                                                if (commandFromBody == -1) break;                                       // break if it is -1
                                                                
                                                                if (commandFromBody == 2) {
                                                                        
                                                                        struct room_report prio_room_report;
                                                                        int roomA_util = 0;
                                                                        int roomB_util = 0;
                                                                        int roomC_util = 0;
                                                                        for (i=0; i<7*24; i++) {
                                                                                if (roomA[i] > 0) roomA_util++;
                                                                                if (roomB[i] > 0) roomB_util++;
                                                                                if (roomC[i] > 0) roomC_util++;
                                                                        }
                                                                        prio_room_report.roomA = roomA_util;
                                                                        prio_room_report.roomB = roomB_util;
                                                                        prio_room_report.roomC = roomC_util;
                                                                        
                                                                        write(compToPrio[0][1], &prio_room_report, sizeof(struct room_report));
                                                                        continue;
                                                                }

                                                                // if command = 0, reset the room A B C record
                                                                if (commandFromBody == 9) {
                                                                        
                                                                        memset(roomA, 0, 7*24*sizeof(int));
                                                                        memset(roomB, 0, 7*24*sizeof(int));
                                                                        memset(roomC, 0, 7*24*sizeof(int));
                                                                        continue;
                                                                }

                                                                if (commandFromBody == 1){
                                                                       
                                                                        // tell prio this child get the command
                                                                        write(compToPrio[0][1], &recevied, sizeof(int)); 

                                                                        // third read, read the booking from fcfs body //
                                                                        read(prioToComp[0][0], &tempBooking, sizeof(struct booking));
                                                                        
                                                                        pplCount = tempBooking.pplCount;
                                                                        day = tempBooking.day;
                                                                        time = tempBooking.time;
                                                                        length = tempBooking.length;
                                                                        rejected = 0;
                
                                                                        // -- for number of people <= 10, we search room A and room B first -- //
                                                                        if (pplCount > 0 && pplCount <= 10) {
                                                                                // room A //
                                                                                roomLetter = 'A'; 
                                                                                for (i=0; i<length; i++) {
                                                                                        if (roomA[day*24 + time + i] != 0) {
                                                                                                rejected = 1;
                                                                                                break;
                                                                                        }
                                                                                }
                
                                                                                // if room A is full, search in room B //
                                                                                if (rejected) {
                                                                                        rejected = 0;
                                                                                        roomLetter = 'B';
                                                                                        for (i=0; i<length; i++) {
                                                                                                if (roomB[day*24 + time + i] != 0) {
                                                                                                        rejected = 1;
                                                                                                        break;
                                                                                                }
                                                                                        }
                                                                                }
                
                                                                                // if room B is full, search in room C //
                                                                                if (rejected) {
                                                                                        rejected = 0;
                                                                                        roomLetter = 'C';
                                                                                        for (i=0; i<length; i++) {
                                                                                                if (roomC[day*24 + time + i] != 0) {
                                                                                                        rejected = 1;
                                                                                                        break;
                                                                                                }
                                                                                        }
                                                                                }
                                                                        }
                
                                                                        // -- otherwise for number of people <= 20, we search room C -- //
                                                                        else if (pplCount > 10 && pplCount <= 20) {
                                                                                roomLetter = 'C';
                                                                                for (i=0; i<length; i++) {
                                                                                        if (roomC[day*24 + time + i] != 0) {
                                                                                                rejected = 1;
                                                                                                break;
                                                                                        }
                                                                                }
                                                                        }
                
                
                                                                        // -- otherotherwise, number of people == 0, it is device -- //
                                                                        else if (pplCount == 0) {
                                                                                roomLetter = 'D';
                                                                        }
                
                                                                        // -- write the search result to fcfs body -- //
                                                                        // fourth write //
                                                                        write(compToPrio[0][1], &rejected, sizeof(int));                // send the booking result to prio body
                                                                        
                                                                        // seventh read //
                                                                        read(prioToComp[0][0], &rejected, sizeof(int));                 // get whether device has rejected the the booking 
                
                                                                        // -- set the slot if it is not rejected -- //
                                                                        if (!rejected) {
                                                                                for (i=0; i<tempBooking.length; i++) {
                                                                                        if (roomLetter == 'A') {
                                                                                                roomA[day*24 + time + i] = 1;
                                                                                        }
                                                                                        if (roomLetter == 'B') {
                                                                                                roomB[day*24 + time + i] = 1;
                                                                                        }
                                                                                        if (roomLetter == 'C') {
                                                                                                roomC[day*24 + time + i] = 1;
                                                                                        }
                                                                                }
                                                                                // nineth write //
                                                                                write(compToPrio[0][1], &roomLetter, sizeof(char));     // tell the body what room is booked
                                                                        }
                                                                        continue;
                                                                }
                                                        } while (commandFromBody != -1);
                                                        exit(EXIT_SUCCESS);
                                                }
        
                /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                // PRIO COMPONENT - DEVICE //
                                                if (compId == 1) {
                                                        int commandFromBody = 0; 
                                                        struct booking tempBooking;
                                                        int webcamFHD[7*24];
                                                        int webcamUHD[7*24];
                                                        int monitor50[7*24];
                                                        int monitor75[7*24];
                                                        int projector2K[7*24];
                                                        int projector4K[7*24];
                                                        int screen100[7*24];
                                                        int screen150[7*24];

                                                        int recevied = 0;
                                                        int rejected = 0;
                                                        int analyzeCommand = 2;
                                                        
                                                        do {
                                                                rejected = 0;
                                                                // second read //
                                                                read(prioToComp[1][0], &commandFromBody, sizeof(int));                  // read flag from prio body 
                                                               
                                                                if (commandFromBody == - 1) break;                                      // if the flag is -1, break the loop

                                                                if (commandFromBody == 2) {
                                                                     
                                                                        struct device_report prio_device_report;
                                                                        int webcamFHD_util = 0;
                                                                        int webcamUHD_util = 0;
                                                                        int monitor50_util = 0;
                                                                        int monitor75_util = 0;
                                                                        int projector2K_util = 0;
                                                                        int projector4K_util = 0;
                                                                        int screen100_util = 0;
                                                                        int screen150_util = 0;
                                                                        for (i=0; i<7*24; i++) {
                                                                                if (webcamFHD[i] > 0) webcamFHD_util++;
                                                                                if (webcamUHD[i] > 0) webcamUHD_util++;
                                                                                if (monitor50[i] > 0) monitor50_util++;
                                                                                if (monitor75[i] > 0) monitor75_util++;
                                                                                if (projector2K[i] > 0) projector2K_util++;
                                                                                if (projector4K[i] > 0) projector4K_util++;
                                                                                if (screen100[i] > 0) screen100_util++;
                                                                                if (screen150[i] > 0) screen150_util++;
                                                                        }
                                                                        prio_device_report.webcamFHD = webcamFHD_util;
                                                                        prio_device_report.webcamUHD = webcamUHD_util;
                                                                        prio_device_report.monitor50 = monitor50_util;
                                                                        prio_device_report.monitor75 = monitor75_util;
                                                                        prio_device_report.projector2K = projector2K_util;
                                                                        prio_device_report.projector4K = projector4K_util;
                                                                        prio_device_report.screen100 = screen100_util;
                                                                        prio_device_report.screen150 = screen150_util;
                                                                        
                                                                        write(compToPrio[1][1], &prio_device_report, sizeof(struct device_report));
                                                                        continue;
                                                                }
                                                                
                                                                if (commandFromBody == 9) {
                                                                       
                                                                        memset(webcamFHD, 0, 7*24*sizeof(int));
                                                                        memset(webcamUHD, 0, 7*24*sizeof(int));
                                                                        memset(monitor50, 0, 7*24*sizeof(int));
                                                                        memset(monitor75, 0, 7*24*sizeof(int));
                                                                        memset(projector2K, 0, 7*24*sizeof(int));
                                                                        memset(projector4K, 0, 7*24*sizeof(int));
                                                                        memset(screen100, 0, 7*24*sizeof(int));
                                                                        memset(screen150, 0, 7*24*sizeof(int));
                                                                        continue;
                                                                }
                                                                
                                                                if (commandFromBody == 1) {
                                                                        // tell prio this child get the command
                                                                        write(compToPrio[1][1], &recevied, sizeof(int)); 

                                                                        // fifth read //
                                                                        read(prioToComp[1][0], &tempBooking, sizeof(struct booking));           // get booking from prio body
                                                                
                                                                        int day = tempBooking.day;
                                                                        int time = tempBooking.time;
                                                                        
                
                                                                        
                                                                        // -- check whether the devices are available in the range of time -- //
                                                                        for (i=0; i<2; i++) {
                                                                                if (rejected) break;
                                                                                for (j=0; j<tempBooking.length; j++) {
                                                                                        if (strcmp(tempBooking.device[i], "webcam_FHD") == 0) {
                                                                                                if (webcamFHD[day*24 + time + j] >= 2) rejected = 1;
                                                                                        }
                                                                                        if (strcmp(tempBooking.device[i], "webcam_UHD") == 0) {
                                                                                                if (webcamUHD[day*24 + time + j] >= 1) rejected = 1;
                                                                                        }
                                                                                        if (strcmp(tempBooking.device[i], "monitor_50") == 0) {
                                                                                                if (monitor50[day*24 + time + j] >= 2) rejected = 1;
                                                                                        }
                                                                                        if (strcmp(tempBooking.device[i], "monitor_75") == 0) {
                                                                                                if (monitor75[day*24 + time + j] >= 1) rejected = 1;
                                                                                        }
                                                                                        if (strcmp(tempBooking.device[i], "projector_2K") == 0) {
                                                                                                if (projector2K[day*24 + time + j] >= 2) rejected = 1;
                                                                                        }
                                                                                        if (strcmp(tempBooking.device[i], "projector_4K") == 0) {
                                                                                                if (projector4K[day*24 + time + j] >= 1) rejected = 1;
                                                                                        }
                                                                                        if (strcmp(tempBooking.device[i], "screen_100") == 0 ){
                                                                                                if (screen100[day*24 + time + j] >= 2) rejected = 1;
                                                                                        }
                                                                                        if (strcmp(tempBooking.device[i], "screen_150") == 0) {
                                                                                                if (screen150[day*24 + time + j] >= 1) rejected = 1;
                                                                                        }
                                                                                }
                                                                        }
                                                                
                                                                        // sixth write //
                                                                        write(compToPrio[1][1], &rejected, sizeof(int));            // tell prio body whether it is rejected
                
                                                                        // eighth read //
                                                                        read(prioToComp[1][0], &rejected, sizeof(int));             // get from prio whether to accept and set the booking                                                           
                                                                        
                                                                        // -- set the slot if it is not rejected -- //
                                                                        if (!rejected) {
                                                                                for (i=0; i<2; i++) {
                                                                                        for (j=0; j<tempBooking.length; j++) {
                                                                                                if (strcmp(tempBooking.device[i], "webcam_FHD") == 0) {
                                                                                                        webcamFHD[day*24 + time + j]++;
                                                                                                }
                                                                                                if (strcmp(tempBooking.device[i], "webcam_UHD") == 0) {
                                                                                                        webcamUHD[day*24 + time + j]++;
                                                                                                }
                                                                                                if (strcmp(tempBooking.device[i], "monitor_50") == 0) {
                                                                                                        monitor50[day*24 + time + j]++;
                                                                                                }
                                                                                                if (strcmp(tempBooking.device[i], "monitor_75") == 0) {
                                                                                                        monitor75[day*24 + time + j]++;
                                                                                                }
                                                                                                if (strcmp(tempBooking.device[i], "projector_2K") == 0) {
                                                                                                        projector2K[day*24 + time + j]++;
                                                                                                }
                                                                                                if (strcmp(tempBooking.device[i], "projector_4K") == 0) {
                                                                                                        projector4K[day*24 + time + j]++;
                                                                                                }
                                                                                                if (strcmp(tempBooking.device[i], "screen_100") == 0 ){
                                                                                                        screen100[day*24 + time + j]++;
                                                                                                }
                                                                                                if (strcmp(tempBooking.device[i], "screen_150") == 0) {
                                                                                                        screen150[day*24 + time + j]++;
                                                                                                }
                                                                                        }
                                                                                }
                                                                        }

                                                                        continue;
                                                                }
                                                        } while (commandFromBody != -1);
                                                        
                                                        exit(EXIT_SUCCESS);
                                                }
                                        }
                                }
                                                /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                // SUB-SCHEDULER (OPTI) SECTION //
                                if (algoType == 2) {
                                        int optiToComp[2][2];                           // pipe fd from opti to components
                                        int compToOpti[2][2];                           // pipe fd from components to opti
                                        int optiPid = -1;                               // pid inside opti scheduler
                                        int compId = -1;                                // component Id in opti (0 = room, 1 = device)
        
                                        // -- pipe and component creation in opti -- //
                                        for (i=0; i<2; i++) {
                                                if (pipe(optiToComp[i]) < 0 || pipe(compToOpti[i]) < 0) {
                                                        printf("Error: Pipe creation in OPTI scheduler\n");
                                                }
        
                                                compId++;
                                                optiPid = fork();
                                                
                                                // if it is component //
                                                if (optiPid == 0) {
                                                        close(optiToComp[i][1]);        // close the writing side of optiToComp
                                                        close(compToOpti[i][0]);        // close the reading side of compToOpti
                                                        break;                          // break to prevent component creating more child
                                                }
        
                                                // if it is opti scheduler //
                                                else {
                                                        close(optiToComp[i][0]);        // close the reading side of optiToComp
                                                        close(compToOpti[i][1]);        // close the writing side of compToOpti
                                                }
                                        }
                                        
                /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                // OPTI BODY //
                                        if (optiPid != 0) {
                                             // -- variable initialization -- //
                                                int acceptedRoomAList[7*24];          // list containing accepted room A records
                                                int acceptedRoomBList[7*24];          // list containing accepted room B records
                                                int acceptedRoomCList[7*24];          // list containing accepted room C records
                                                int acceptedDeviceList[7*24][100];         // list containing accepted device records
                                                int rejectedBookingList[3000];        // list containing rejected room records
                                                int rejectedBookingCount = 0;               // number of rejected booking
                                                int acceptedBookingCount = 0;
                                                int allBookingSortedCount = 0;
                                                int allBookingCount = 0;
                                                int commandFromMainSch = 0;                 // flag received from main scheduler, determining whether to make a booking or print bookings
                                                int rejected = 0;                           // whether the booking is to be rejected
                                                char roomLetter;                            // letter of the room (for switch case below)
                                                char tenantTarget;                          // the tenant to be printed (as char)
                                                int tenantTargetInt;                        // the tenant to be printed (as int)
                                                int received = 0;                           // check comp has received the data
                                                int room_rejected;
                                                int device_rejected;
                                                int total_rejected;
                                                

                                                struct booking allBooking[3000];            // list containing all bookings
                                                struct booking allBookingSorted[3000];           // sorted bookings

                                                struct booking tempBooking;                 // temporary booking
                                                struct booking newBooking;
                                                
                                                
                                              
                                                do {    
                                                    
                                                        // -- reading from main scheduler -- //
                                                        read(schToAlgo[2][0], &commandFromMainSch, sizeof(int));
                                                       
                                                        // -- if main scheduler wants a booking -- //
                                                        if (commandFromMainSch == 0) {
                                                                // first read //
                                                                read(schToAlgo[2][0], &newBooking, sizeof(struct booking));            // read the booking from main schedule                                                         
                                                                allBooking[newBooking.recordId] = newBooking;
                                                                allBookingCount++;
                                                               
                                                                allBookingSorted[allBookingSortedCount++] = newBooking;
            
                                                        }
                                                         // -- if main scheduler wants printing -- //
                                                        if (commandFromMainSch == 1 || commandFromMainSch == 2) {    
                                                             
                                                                 // Reset all records //
                                                                memset(acceptedRoomAList, -1, 7*24*sizeof(int));
                                                                memset(acceptedRoomBList, -1, 7*24*sizeof(int));
                                                                memset(acceptedRoomCList, -1, 7*24*sizeof(int));
                                                                memset(acceptedDeviceList, -1, 7*24*100*sizeof(int));
                                                                rejectedBookingCount = 0;
                                                                acceptedBookingCount = 0;

                                                                int resetCommand = 9;
                                                                int addCommand = 1;
                                                                int analyzeCommand = 2;
                                                                write(optiToComp[0][1], &resetCommand, sizeof(int));              // tell the room that reset the record
                                                                write(optiToComp[1][1], &resetCommand, sizeof(int));              // tell the device that reset the record

                                                                // sort the bookings                                                       
                                                                qsort(allBookingSorted, allBookingSortedCount, sizeof(struct booking), comparator);
                                                                                                                              
                                                                for (i=0; i<allBookingSortedCount; i++){
                                                                        //memset(&tempBooking, 0, sizeof(struct booking));
                                                                        tempBooking = allBookingSorted[i];
                                                                                           
                                                                        room_rejected = 0;
                                                                        device_rejected = 0;
                                                                        total_rejected = 0;

                                                                        
                                                                        // second write //
                                                                        write(optiToComp[0][1], &addCommand, sizeof(int));              // tell the room that we are not exiting                                                                            
                                                                        write(optiToComp[1][1], &addCommand, sizeof(int));              // tell the device that we are not exiting

                                                                        read(compToOpti[0][0], &received, sizeof(int));
                                                                        read(compToOpti[1][0], &received, sizeof(int));
                                                                        // third write //
                                                                        write(optiToComp[0][1], &tempBooking, sizeof(struct booking));          // send the booking to room, see if room has space left
                                                                        
                                                                        // fourth read //
                                                                        read(compToOpti[0][0], &room_rejected, sizeof(int));                         // read from room. see if room rejects the booking
                                                                                                                                        
                                                                        if (room_rejected) {
                                                                                total_rejected = 1;
                                                                        }
                                                                        // if the room says ok, we see see if device is ok //
                                                                        
                                                                        // fifth write //
                                                                        write(optiToComp[1][1], &tempBooking, sizeof(struct booking));  // send the booking to device, see if device has space left
                                                                        
                                                                        // sixth read //
                                                                        read(compToOpti[1][0], &device_rejected, sizeof(int));                 // read whether device say ok
                                                                        
                                                                        if (device_rejected) {
                                                                                total_rejected = 1;
                                                                        }      
                                                                        
                                                                        
                                                                        // seventh write //
                                                                        write(optiToComp[0][1], &total_rejected, sizeof(int));                        // send the final result to room                 
                                                                        // eighth write //
                                                                        write(optiToComp[1][1], &total_rejected, sizeof(int));                        // send the final result to device
                                                                        
                                                                        
                
                                                                        // if the all is rejected, add it to the rejected list //
                                                                        if (total_rejected) {
                                                                                rejectedBookingList[rejectedBookingCount++] = tempBooking.recordId;
                                                                        }
                
                                                                        // if the booking is accepted, set the slots in the fcfs body //
                                                                        else {
                                                                                // nineth read //
                                                                                read(compToOpti[0][0], &roomLetter, sizeof(char));              // get the roomLetter from room 
                                                                                acceptedBookingCount++;
                                                                                if (roomLetter == 'A') {
                                                                                        acceptedRoomAList[tempBooking.day*24 + tempBooking.time] = tempBooking.recordId;
                                                                                        
                                                                                }
                                                                                if (roomLetter == 'B') {
                                                                                        acceptedRoomBList[tempBooking.day*24 + tempBooking.time] = tempBooking.recordId;
                                                                                        
                                                                                }
                                                                                if (roomLetter == 'C') {
                                                                                        acceptedRoomCList[tempBooking.day*24 + tempBooking.time] = tempBooking.recordId;
                                                                                        
                                                                                }
                                                                                if (roomLetter == 'D') {
                                                                                        int searchIndex = 0;
                                                                                        // search for empty slot in acceptedDeviceList
                                                                                        while (acceptedDeviceList[tempBooking.day*24 + tempBooking.time][searchIndex] != -1) {
                                                                                                searchIndex++;
                                                                                        }
                                                                                        acceptedDeviceList[tempBooking.day*24 + tempBooking.time][searchIndex] = tempBooking.recordId;
                                                                                        
                                                                                }
                                                                        }
                                                                        
                                                                }
                                                                
                                                                
                                                         
                                                              

                                                                printf("*** Room Booking - ACCEPTED / OPTI ***\n");
                                                                for (tenantTargetInt = 65; tenantTargetInt <= 69; tenantTargetInt++) {
                                                                        tenantTarget = (char) tenantTargetInt;
                                                                        printf("\nTenant_%c has the following bookings:\n", tenantTarget);
                                                                        printf("Date        Start   End   Type           Room    Device\n");
                                                                        printf("===========================================================================\n");
                                                                        for (i=0; i<7*24; i++) {
                                                                                if (acceptedRoomAList[i] != -1 && allBooking[acceptedRoomAList[i]].tenantLetter[0] == tenantTarget){
                                                                                        int id = acceptedRoomAList[i];
                                                                                        printf("2021-05-%02d  %02d:00  %02d:00  %-13s  room_A  %s\n", allBooking[id].day+10, allBooking[id].time, allBooking[id].time + allBooking[id].length, allBooking[id].bookingType, allBooking[id].device[0]);
                                                                                        if (strcmp(allBooking[id].device[1], "") != 0) printf("%59s\n", allBooking[id].device[1]);
                                                                                }
                                                                                if (acceptedRoomBList[i] != -1 && allBooking[acceptedRoomBList[i]].tenantLetter[0] == tenantTarget){
                                                                                        int id = acceptedRoomBList[i];
                                                                                        printf("2021-05-%02d  %02d:00  %02d:00  %-13s  room_B  %s\n", allBooking[id].day+10, allBooking[id].time, allBooking[id].time + allBooking[id].length, allBooking[id].bookingType, allBooking[id].device[0]);
                                                                                        if (strcmp(allBooking[id].device[1], "") != 0) printf("%59s\n", allBooking[id].device[1]);
                                                                                }
                                                                                if (acceptedRoomCList[i] != -1 && allBooking[acceptedRoomCList[i]].tenantLetter[0] == tenantTarget){
                                                                                        int id = acceptedRoomCList[i];
                                                                                        printf("2021-05-%02d  %02d:00  %02d:00  %-13s  room_C  %s\n", allBooking[id].day+10, allBooking[id].time, allBooking[id].time + allBooking[id].length, allBooking[id].bookingType, allBooking[id].device[0]);
                                                                                        if (strcmp(allBooking[id].device[1], "") != 0) printf("%59s\n", allBooking[id].device[1]);
                                                                                }
                                                                                for (j=0; j<100; j++) {
                                                                                        if (acceptedDeviceList[i][j] != -1 && allBooking[acceptedDeviceList[i][j]].tenantLetter[0] == tenantTarget){
                                                                                                int id = acceptedDeviceList[i][j];
                                                                                                printf("2021-05-%02d  %02d:00  %02d:00  *              *       %s\n", allBooking[id].day+10, allBooking[id].time, allBooking[id].time + allBooking[id].length, allBooking[id].device[0]);
                                                                                                if (strcmp(allBooking[id].device[1], "") != 0) printf("%59s\n", allBooking[id].device[1]);
                                                                                        }
                                                                                }
                                                                        }
        
                                                                }               
        
                                                                printf("\n*** Room Booking - REJECTED / OPTI ***\n");
                                                                int tenantRejectedBookingCount;
                                                                for (tenantTargetInt = 65; tenantTargetInt <= 69; tenantTargetInt++) {
                                                                        tenantTarget = (char) tenantTargetInt;
                                                                        tenantRejectedBookingCount = 0;
                                                                        for (i=0; i<rejectedBookingCount; i++) {
                                                                                if (allBooking[rejectedBookingList[i]].tenantLetter[0] == tenantTarget) tenantRejectedBookingCount++;
                                                                        }
                                                                        printf("\nTenants_%c (there are %d bookings rejected):\n", tenantTarget, tenantRejectedBookingCount);
                                                                        printf("Date        Start   End   Type           Device\n");
                                                                        printf("===========================================================================\n");
                                                                        for (i=0; i<rejectedBookingCount; i++) {
                                                                                if (allBooking[rejectedBookingList[i]].tenantLetter[0] == tenantTarget) {
                                                                                        int id = rejectedBookingList[i];
                                                                                        printf("2021-05-%02d  %02d:00  %02d:00  %-13s  %s\n", allBooking[id].day+10, allBooking[id].time, allBooking[id].time + allBooking[id].length, allBooking[id].bookingType, allBooking[id].device[0]);
                                                                                        if (strcmp(allBooking[id].device[1], "") != 0) printf("%51s\n", allBooking[id].device[1]);
                                                                                }
                                                                        }
        
                                                                }

                                                                puts("  - end -\n");
                                                                if (commandFromMainSch == 2) {
                                                                       
                                                                        struct algo_report opti_booking_report;
                                                                        struct room_report opti_room_report;
                                                                        struct device_report opti_device_report;

                                                                        opti_booking_report.received_booking = allBookingCount;
                                                                        opti_booking_report.assigned_booking = acceptedBookingCount;
                                                                        opti_booking_report.rejected_booking = rejectedBookingCount;

                                                                        write(optiToComp[0][1], &analyzeCommand, sizeof(int));
                                                                        read(compToOpti[0][0], &opti_room_report, sizeof(struct room_report));
                                                                        write(optiToComp[1][1], &analyzeCommand, sizeof(int));
                                                                        read(compToOpti[1][0], &opti_device_report, sizeof(struct device_report));

                                                                        write(algoToSch[2][1], &opti_booking_report, sizeof(struct algo_report));
                                                                        write(algoToSch[2][1], &opti_room_report, sizeof(struct room_report));
                                                                        write(algoToSch[2][1], &opti_device_report, sizeof(struct device_report));
                                                                } else {
                                                                        printf("Please enter booking:\n");
                                                                }
                                                                continue;
                                                        }
        
                                                        // -- if main scheduler wants to die -- //
                                                        if (commandFromMainSch == -1) {
                                                                int endCommand = -1;
                                                                //printf("OPTI ejecting\n");
                                                                write(optiToComp[0][1], &endCommand, sizeof(int));                                                                                          
                                                                write(optiToComp[1][1], &endCommand, sizeof(int));      
                                                                break;
                                                        }
                                                       
                                                    
                                                } while (commandFromMainSch != -1);
                                                wait(NULL); 
                                                exit(EXIT_SUCCESS);
                                        }
        
                /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                // OPTI COMPONENT - ROOM //
                                        else {
                                                if (compId == 0) {
                                                        struct booking tempBooking;
                                                        int roomA[7*24] = {0};          // array to store which timeslot is occupied in room A
                                                        int roomB[7*24] = {0};          // ditto, for room B
                                                        int roomC[7*24] = {0};          // ditto, for room C
                                                        int rejected = 0;               // whether the booking request is rejected due to conflicts
                                                        int day = 0;                    // copy of day in tempBooking
                                                        int time = 0;                   // copy of time in tempBooking
                                                        int pplCount = 0;               // copy of pplCount in tempBooking
                                                        int commandFromBody = 0;        // flag sent from opti body
                                                        int length = 0;                 // copy of length in tempBooking
                                                        char roomLetter;                // the room to be assigned

                                                        int analyzeCommand = 2;
                                                        int recevied = 0;
                                                        do {
                                                            
                                                                // second read //
                                                                read(optiToComp[0][0], &commandFromBody, sizeof(int));                  // check whether we need to break the loop
                                                               
                                                                if (commandFromBody == -1) break;                                       // break if it is -1
                                                                
                                                                if (commandFromBody == 2) {
                                                                        
                                                                        struct room_report opti_room_report;
                                                                        int roomA_util = 0;
                                                                        int roomB_util = 0;
                                                                        int roomC_util = 0;
                                                                        for (i=0; i<7*24; i++) {
                                                                                if (roomA[i] > 0) roomA_util++;
                                                                                if (roomB[i] > 0) roomB_util++;
                                                                                if (roomC[i] > 0) roomC_util++;
                                                                        }
                                                                        opti_room_report.roomA = roomA_util;
                                                                        opti_room_report.roomB = roomB_util;
                                                                        opti_room_report.roomC = roomC_util;
                                                                        
                                                                        write(compToOpti[0][1], &opti_room_report, sizeof(struct room_report));
                                                                        continue;
                                                                }

                                                                // if command = 0, reset the room A B C record
                                                                if (commandFromBody == 9) {
                                                                        
                                                                        memset(roomA, 0, 7*24*sizeof(int));
                                                                        memset(roomB, 0, 7*24*sizeof(int));
                                                                        memset(roomC, 0, 7*24*sizeof(int));
                                                                        continue;
                                                                }

                                                                if (commandFromBody == 1){
                                                                       
                                                                        // tell opti this child get the command
                                                                        write(compToOpti[0][1], &recevied, sizeof(int)); 

                                                                        // third read, read the booking from fcfs body //
                                                                        read(optiToComp[0][0], &tempBooking, sizeof(struct booking));
                                                                        
                                                                        pplCount = tempBooking.pplCount;
                                                                        day = tempBooking.day;
                                                                        time = tempBooking.time;
                                                                        length = tempBooking.length;
                                                                        rejected = 0;
                
                                                                        // -- for number of people <= 10, we search room A and room B first -- //
                                                                        if (pplCount > 0 && pplCount <= 10) {
                                                                                // room A //
                                                                                roomLetter = 'A'; 
                                                                                for (i=0; i<length; i++) {
                                                                                        if (roomA[day*24 + time + i] != 0) {
                                                                                                rejected = 1;
                                                                                                break;
                                                                                        }
                                                                                }
                
                                                                                // if room A is full, search in room B //
                                                                                if (rejected) {
                                                                                        rejected = 0;
                                                                                        roomLetter = 'B';
                                                                                        for (i=0; i<length; i++) {
                                                                                                if (roomB[day*24 + time + i] != 0) {
                                                                                                        rejected = 1;
                                                                                                        break;
                                                                                                }
                                                                                        }
                                                                                }
                
                                                                                // if room B is full, search in room C //
                                                                                if (rejected) {
                                                                                        rejected = 0;
                                                                                        roomLetter = 'C';
                                                                                        for (i=0; i<length; i++) {
                                                                                                if (roomC[day*24 + time + i] != 0) {
                                                                                                        rejected = 1;
                                                                                                        break;
                                                                                                }
                                                                                        }
                                                                                }
                                                                        }
                
                                                                        // -- otherwise for number of people <= 20, we search room C -- //
                                                                        else if (pplCount > 10 && pplCount <= 20) {
                                                                                roomLetter = 'C';
                                                                                for (i=0; i<length; i++) {
                                                                                        if (roomC[day*24 + time + i] != 0) {
                                                                                                rejected = 1;
                                                                                                break;
                                                                                        }
                                                                                }
                                                                        }
                
                
                                                                        // -- otherotherwise, number of people == 0, it is device -- //
                                                                        else if (pplCount == 0) {
                                                                                roomLetter = 'D';
                                                                        }
                
                                                                        // -- write the search result to fcfs body -- //
                                                                        // fourth write //
                                                                        write(compToOpti[0][1], &rejected, sizeof(int));                // send the booking result to opti body
                                                                        
                                                                        // seventh read //
                                                                        read(optiToComp[0][0], &rejected, sizeof(int));                 // get whether device has rejected the the booking 
                
                                                                        // -- set the slot if it is not rejected -- //
                                                                        if (!rejected) {
                                                                                for (i=0; i<tempBooking.length; i++) {
                                                                                        if (roomLetter == 'A') {
                                                                                                roomA[day*24 + time + i] = 1;
                                                                                        }
                                                                                        if (roomLetter == 'B') {
                                                                                                roomB[day*24 + time + i] = 1;
                                                                                        }
                                                                                        if (roomLetter == 'C') {
                                                                                                roomC[day*24 + time + i] = 1;
                                                                                        }
                                                                                }
                                                                                // nineth write //
                                                                                write(compToOpti[0][1], &roomLetter, sizeof(char));     // tell the body what room is booked
                                                                        }
                                                                        continue;
                                                                }
                                                        } while (commandFromBody != -1);
                                                        exit(EXIT_SUCCESS);
                                                }
        
                /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                // OPTI COMPONENT - DEVICE //
                                                if (compId == 1) {
                                                        int commandFromBody = 0; 
                                                        struct booking tempBooking;
                                                        int webcamFHD[7*24];
                                                        int webcamUHD[7*24];
                                                        int monitor50[7*24];
                                                        int monitor75[7*24];
                                                        int projector2K[7*24];
                                                        int projector4K[7*24];
                                                        int screen100[7*24];
                                                        int screen150[7*24];

                                                        int recevied = 0;
                                                        int rejected = 0;
                                                        int analyzeCommand = 2;
                                                        
                                                        do {
                                                                rejected = 0;
                                                                // second read //
                                                                read(optiToComp[1][0], &commandFromBody, sizeof(int));                  // read flag from opti body 
                                                               
                                                                if (commandFromBody == - 1) break;                                      // if the flag is -1, break the loop

                                                                if (commandFromBody == 2) {
                                                                     
                                                                        struct device_report opti_device_report;
                                                                        int webcamFHD_util = 0;
                                                                        int webcamUHD_util = 0;
                                                                        int monitor50_util = 0;
                                                                        int monitor75_util = 0;
                                                                        int projector2K_util = 0;
                                                                        int projector4K_util = 0;
                                                                        int screen100_util = 0;
                                                                        int screen150_util = 0;
                                                                        for (i=0; i<7*24; i++) {
                                                                                if (webcamFHD[i] > 0) webcamFHD_util++;
                                                                                if (webcamUHD[i] > 0) webcamUHD_util++;
                                                                                if (monitor50[i] > 0) monitor50_util++;
                                                                                if (monitor75[i] > 0) monitor75_util++;
                                                                                if (projector2K[i] > 0) projector2K_util++;
                                                                                if (projector4K[i] > 0) projector4K_util++;
                                                                                if (screen100[i] > 0) screen100_util++;
                                                                                if (screen150[i] > 0) screen150_util++;
                                                                        }
                                                                        opti_device_report.webcamFHD = webcamFHD_util;
                                                                        opti_device_report.webcamUHD = webcamUHD_util;
                                                                        opti_device_report.monitor50 = monitor50_util;
                                                                        opti_device_report.monitor75 = monitor75_util;
                                                                        opti_device_report.projector2K = projector2K_util;
                                                                        opti_device_report.projector4K = projector4K_util;
                                                                        opti_device_report.screen100 = screen100_util;
                                                                        opti_device_report.screen150 = screen150_util;
                                                                        
                                                                        write(compToOpti[1][1], &opti_device_report, sizeof(struct device_report));
                                                                        continue;
                                                                }
                                                                
                                                                if (commandFromBody == 9) {
                                                                       
                                                                        memset(webcamFHD, 0, 7*24*sizeof(int));
                                                                        memset(webcamUHD, 0, 7*24*sizeof(int));
                                                                        memset(monitor50, 0, 7*24*sizeof(int));
                                                                        memset(monitor75, 0, 7*24*sizeof(int));
                                                                        memset(projector2K, 0, 7*24*sizeof(int));
                                                                        memset(projector4K, 0, 7*24*sizeof(int));
                                                                        memset(screen100, 0, 7*24*sizeof(int));
                                                                        memset(screen150, 0, 7*24*sizeof(int));
                                                                        continue;
                                                                }
                                                                
                                                                if (commandFromBody == 1) {
                                                                        // tell opti this child get the command
                                                                        write(compToOpti[1][1], &recevied, sizeof(int)); 

                                                                        // fifth read //
                                                                        read(optiToComp[1][0], &tempBooking, sizeof(struct booking));           // get booking from opti body
                                                                
                                                                        int day = tempBooking.day;
                                                                        int time = tempBooking.time;
                                                                        
                
                                                                        
                                                                        // -- check whether the devices are available in the range of time -- //
                                                                        for (i=0; i<2; i++) {
                                                                                if (rejected) break;
                                                                                for (j=0; j<tempBooking.length; j++) {
                                                                                        if (strcmp(tempBooking.device[i], "webcam_FHD") == 0) {
                                                                                                if (webcamFHD[day*24 + time + j] >= 2) rejected = 1;
                                                                                        }
                                                                                        if (strcmp(tempBooking.device[i], "webcam_UHD") == 0) {
                                                                                                if (webcamUHD[day*24 + time + j] >= 1) rejected = 1;
                                                                                        }
                                                                                        if (strcmp(tempBooking.device[i], "monitor_50") == 0) {
                                                                                                if (monitor50[day*24 + time + j] >= 2) rejected = 1;
                                                                                        }
                                                                                        if (strcmp(tempBooking.device[i], "monitor_75") == 0) {
                                                                                                if (monitor75[day*24 + time + j] >= 1) rejected = 1;
                                                                                        }
                                                                                        if (strcmp(tempBooking.device[i], "projector_2K") == 0) {
                                                                                                if (projector2K[day*24 + time + j] >= 2) rejected = 1;
                                                                                        }
                                                                                        if (strcmp(tempBooking.device[i], "projector_4K") == 0) {
                                                                                                if (projector4K[day*24 + time + j] >= 1) rejected = 1;
                                                                                        }
                                                                                        if (strcmp(tempBooking.device[i], "screen_100") == 0 ){
                                                                                                if (screen100[day*24 + time + j] >= 2) rejected = 1;
                                                                                        }
                                                                                        if (strcmp(tempBooking.device[i], "screen_150") == 0) {
                                                                                                if (screen150[day*24 + time + j] >= 1) rejected = 1;
                                                                                        }
                                                                                }
                                                                        }
                                                                
                                                                        // sixth write //
                                                                        write(compToOpti[1][1], &rejected, sizeof(int));            // tell opti body whether it is rejected
                
                                                                        // eighth read //
                                                                        read(optiToComp[1][0], &rejected, sizeof(int));             // get from opti whether to accept and set the booking                                                           
                                                                        
                                                                        // -- set the slot if it is not rejected -- //
                                                                        if (!rejected) {
                                                                                for (i=0; i<2; i++) {
                                                                                        for (j=0; j<tempBooking.length; j++) {
                                                                                                if (strcmp(tempBooking.device[i], "webcam_FHD") == 0) {
                                                                                                        webcamFHD[day*24 + time + j]++;
                                                                                                }
                                                                                                if (strcmp(tempBooking.device[i], "webcam_UHD") == 0) {
                                                                                                        webcamUHD[day*24 + time + j]++;
                                                                                                }
                                                                                                if (strcmp(tempBooking.device[i], "monitor_50") == 0) {
                                                                                                        monitor50[day*24 + time + j]++;
                                                                                                }
                                                                                                if (strcmp(tempBooking.device[i], "monitor_75") == 0) {
                                                                                                        monitor75[day*24 + time + j]++;
                                                                                                }
                                                                                                if (strcmp(tempBooking.device[i], "projector_2K") == 0) {
                                                                                                        projector2K[day*24 + time + j]++;
                                                                                                }
                                                                                                if (strcmp(tempBooking.device[i], "projector_4K") == 0) {
                                                                                                        projector4K[day*24 + time + j]++;
                                                                                                }
                                                                                                if (strcmp(tempBooking.device[i], "screen_100") == 0 ){
                                                                                                        screen100[day*24 + time + j]++;
                                                                                                }
                                                                                                if (strcmp(tempBooking.device[i], "screen_150") == 0) {
                                                                                                        screen150[day*24 + time + j]++;
                                                                                                }
                                                                                        }
                                                                                }
                                                                        }

                                                                        continue;
                                                                }
                                                        } while (commandFromBody != -1);
                                                       
                                                        exit(EXIT_SUCCESS);
                                                }
                                        }
                                }
                        }            
                }
        
                /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                // PARENT SECTION //
                else {
                        // -- variable initialization -- //
                        int argCount = 0;                               // number of arguments in the input
                        char inputString[500];                          // command prompt input string
                        char tokInputString[20][100];                   // tokenized input string
                        char *tokPtr;                                   // pointer for tokenizer (point to the char after the delimiter)
        
                        FILE *batchFile;                                // FILE pointer for the batch file
                        char batchStrings[200][500];                    // array of strings read from batch file
                        int batchLineCount = 0;                         // number of lines in the batch file (left for reading)
                        int batchLineIndex = 0;                         // the current line being read
                        printf("Parent PID: %d\n", getpid());
                        printf("Please enter booking:\n");
                        do {
                                // -- reading input -- //
                                // initialization before reading input //
                                argCount = 0;
                                memset(inputString, 0, 500);
                                memset(tokInputString, 0, 2000);
                                 
                                // if there is no batch line left to read, read from stdin (keyboard) //
                                if (batchLineCount == 0) {
                                        batchLineIndex = 0;                         
                                        fgets(inputString, 500, stdin);
                                        inputString[strcspn(inputString, "\n")] = 0;        // remove the "\n" in the input
                                }
        
                                // otherwise, read the batch lines in batchStrings //
                                else {
                                        strcpy(inputString, batchStrings[batchLineIndex]);
                                        batchLineIndex++;
                                        batchLineCount--;
                                        printf("Reading from batch file, current line: %s ,lines left: %d\n", inputString, batchLineCount);
                                }
        
                                // -- string tokenization -- //
                                tokPtr = strtok(inputString, " ");                           // tokenizing the inputString
                                while (tokPtr != NULL) {    
                                       
                                        strcpy(tokInputString[argCount], tokPtr);           // copy the token to tokInputString
                                       
                                        argCount++;
                                        tokPtr = strtok(NULL, " ");                         // point to the next token
                                }
                                
                                // -- determine the command type -- //
                                // if it is not "addBatch", we simply pipe the tokInputString to main scheduler //
                                if (strcmp(tokInputString[0], "addBatch") != 0) {
                                        write(parentToSch[1], &tokInputString, sizeof(tokInputString));
                                }
        
                                // otherwise, it is "addBatch" and we read the lines in the batch file //
                                else {
													 tokInputString[1][strlen(tokInputString[1])-1] = 0;
                                        batchFile = fopen(tokInputString[1]+1, "r");        // read the second token as the file name (hyphen is omitted)
                                        while (fgets(batchStrings[batchLineCount], 500, batchFile) != NULL) {
                                                batchStrings[batchLineCount][strcspn(batchStrings[batchLineCount], "\n")] = 0;
                                                batchLineCount++;
                                        }
                                        fclose(batchFile);
                                }
                                                        
                                // sleep to wait for the main scheduler //
                                //printf("Pending...\n");
                                usleep(1000);
                        } while (strcmp(tokInputString[0], "endProgram;") != 0);
                        wait(NULL);
                        exit(EXIT_SUCCESS);
                }
        }
        
