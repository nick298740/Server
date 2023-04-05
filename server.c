#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include<string.h>
#include <time.h>
 
#define MAX_BLACKLIST_SITES 100

int listen_fd, conn_fd, n;
struct sockaddr_in servaddr;
FILE *file, *cachedFileList, *blackList;
 
char str[2000];
char fileToOpen[256];
char currentTimeString[256];
char buffer[2000000];
char currentlyRequestedIP[100];
char currentlyUsedFileName[100];
char cacheFileName[100];
char requestedIPDateTime[100];

char blackListedSite[MAX_BLACKLIST_SITES][1000];
long long blackListedStartTime[MAX_BLACKLIST_SITES];
long long blackListedEndTime[MAX_BLACKLIST_SITES];
long long currentTimeLong = 1.0;
int blackListIndex = 0;

struct tm dateTime;

void readMessage(){
    bzero(buffer, 2000);
    n = read(conn_fd,buffer, 2000);

    if (n < 0) {
		perror("ERROR reading from socket");
 	    //exit(1);
    }
}

void writeMessage(){
    n = write( conn_fd,buffer,2000 );
    bzero(buffer,2000);

    if (n < 0) {
	   perror("ERROR reading from socket");
	  // exit(1);
    }
}

void setDateAndTime(){
	time_t timeT = time(NULL);
	dateTime = *localtime(&timeT);

	printf("CurrentTime: %d-%d-%d %d:%d:%d\n", dateTime.tm_year + 1900, dateTime.tm_mon + 1, dateTime.tm_mday, dateTime.tm_hour, dateTime.tm_min, dateTime.tm_sec);
}

int isAcceptableTime(){
	//if(){
		
	//}
}

void setUserRequestedFile(){
	fgets(currentlyUsedFileName, sizeof currentlyUsedFileName, stdin);
	// remove \n
	currentlyUsedFileName[strlen(currentlyUsedFileName) - 1] = 0;
}

// Returns 1 if trying to access black listed site. Returns 0 if site is acceptable
int compareRequestedIpWithBlackList(){
	int i;
	for(i = 0; i < blackListIndex; i++){
		printf("\nHere is requested: %s\nHere is blackListedAtIndex: %s\n\n", currentlyRequestedIP, blackListedSite[i]);
		if(strcmp(blackListedSite[i], currentlyRequestedIP) == 0){
			if(currentTimeLong > blackListedStartTime[i] && currentTimeLong < blackListedEndTime[i]){
				printf("\n\nThe current time, %lld, is greater than the blackListedStart time (%lld) and is also less than the blackListedEnd time (%lld). This means that during this time, this site is currently blocked.\n\n", currentTimeLong, blackListedStartTime[i], blackListedEndTime[i]);
				return 1;
			}
		}
	}
	
	return 0;
}

int isCurrentlyRequestedIPCached(){
	printf("\nPlease enter the cached file name with extension. For example: list.txt\n");
	setUserRequestedFile(); // Function handles prompting user for fileName
	printf("\nHere is the input: %s\n", currentlyUsedFileName);
	
	strcpy(cacheFileName, currentlyUsedFileName);
	cachedFileList = fopen(currentlyUsedFileName, "r");
	
	if(!cachedFileList){
		perror("fopen");
		strcat(buffer, "List file not found.");
	} else {
		int lineCount = 1;
	    char line[20000];
	    while(fgets(line, sizeof line, cachedFileList)){ // while we have not reached end of file (EOF)...
			//printf("Inserted line number %d\n", lineCount);

			printf("\nHere is currently requestedIP: %s,      here is line: %s\n", currentlyRequestedIP, line);			
			if(strstr(line, currentlyRequestedIP)){
				
				const char space[2] = " ";
				int i, positionOfSpace = -1;
				for(i = 0; i < strlen(line); i++){
					if(line[i] == space[0]){
						printf("\nHere is the char at %d : %c", i, line[i]);

						positionOfSpace = ++i;
						break;
					}
				}
				
				
				strncpy(fileToOpen, line + positionOfSpace, strlen(line));
				//temp[strlen(line)] = 
				//strcpy(fileToOpen, temp);
				//printf("\n\nhere is temp: %s\n\n", temp);//
				fileToOpen[strlen(fileToOpen) - 1] = '\0';
				strcat(fileToOpen, ".html");
				printf("\n\nhere is file to open: %s\n\n", fileToOpen);

				return 0;
			}
			//strcat(buffer, line);
			lineCount++;
	   }
	}
	
	return 1;
}

char getFileContents(){ 
    printf("\nHere is the current file to open: |%s|\n", fileToOpen);
    file = fopen(fileToOpen, "r");
    	
    bzero(buffer,200000);

    if(!file){ // check if file exists
	    perror("fopen"); 
	    strcat(buffer, "File not found.");
		
	} else { // if file exists, then...
	
	    int lineCount = 1;
	    char line[20000];
	    while(fgets(line, sizeof line, file)){ // while we have not reached end of file (EOF)...
			//printf("Inserted line number %d\n", lineCount);
			
			strcat(buffer, line);
			lineCount++;
	   }
	}
}

void getBlackListFileContents(){
   file = fopen(currentlyUsedFileName, "r");
      	
   bzero(buffer,200000);

   if (!file){ // check if file exists
	   perror("fopen"); 
	   strcat(buffer, "Blacklist file not found.");
	   
	} else { // if file exists, then...
	
	   int lineCount = 1;
	   char line[2000];
	  // char * token;
	   while(fgets(line, sizeof line, file)){ // while we have not reached end of file (EOF)...
			//printf("Inserted line number %d\n", lineCount);
			//printf("\nHere is the inserted line: %s\n", line);
			
			const char space[2] = " ";
			int i, positionOfSpace = -1;
			for(i = 0; i < strlen(line); i++){
				if(line[i] == space[0]){
					printf("\nHere is the char at %d : %c", i, line[i]);

					positionOfSpace = ++i;
					break;
				}
			}
			
			char temp[1000];
			strncpy(temp, line, positionOfSpace - 1);
			strcpy(blackListedSite[blackListIndex], temp);
			temp[strlen(blackListedSite[blackListIndex] - 1)] = '\0';
			
			printf("\nBlackListedIndex : %d, blackListedSite : %s, temp value: %s, line value: %s, position value: %d\n", blackListIndex, blackListedSite[blackListIndex], temp, line, positionOfSpace);						
			
			char startTime[15]; // note 6, not 5, there's one there for the null terminator
			strncpy(startTime, line + positionOfSpace, strlen(line));
			startTime[14] = '\0'; // place the null terminator
			
			char endTime[15];
			strncpy(endTime, line + positionOfSpace + 15, strlen(line));
			endTime[14] = '\0';
	
			printf("\nHere is first time: %s ...  here is second time: %s\n", startTime, endTime);
			
			// Convert first and second time to comparable
			long long firstTime = 0.0;
			sscanf(startTime, "%lld", &firstTime);
			printf("\nhere is start time: %lld\n", firstTime);
			
			// Set the start value for blacklist into index corresponding with blacklisted site
			blackListedStartTime[blackListIndex] = firstTime;
			
			long long secondTime = 0.0;
			sscanf(endTime, "%lld", &secondTime);
			printf("\nhere is end time: %lld\n", secondTime);
			
			// Set the end value for blacklist into index corresponding with blacklisted site
			blackListedEndTime[blackListIndex] = secondTime;
			
			// Get the current time and calculate integer comparable
			currentTimeLong = 1.0;
			currentTimeLong = currentTimeLong * (dateTime.tm_year + 1900);
			//printf("\n1 : here is: %lld\n   dt: %d\n", currentTimeLong, dateTime.tm_year);

			currentTimeLong = currentTimeLong * 10000000000; // Add year to left most			
			//printf("\n2 : here is: %lld\n", currentTimeLong);

			currentTimeLong = currentTimeLong + ((dateTime.tm_mon + 1) * 100000000); // Add month to left most			
			//printf("\n3 : here is: %lld\n", currentTimeLong);

			currentTimeLong = currentTimeLong + (dateTime.tm_mday * 1000000); // Add day to left most
			//printf("\n4 : here is: %lld\n", currentTimeLong);
			
			
			currentTimeLong = currentTimeLong + (dateTime.tm_hour * 10000); // Add hour to left most
			//printf("\n5 : here is: %lld\n", currentTimeLong);

			currentTimeLong = currentTimeLong + (dateTime.tm_min * 100); // Add min to left most
			//printf("\n6 : here is: %lld\n", currentTimeLong);

			currentTimeLong = currentTimeLong + (dateTime.tm_sec); // Add hour to left most
			//printf("\n7 : here is: %lld\n", currentTimeLong);

			strcat(buffer, line);
			lineCount++;
			blackListIndex++;
	   }
	}
}

void setBlackList(){
	printf("\nPlease enter the blackList file name with extension. For example: blackList.txt\n");
	setUserRequestedFile(); // Function handles prompting user for fileName
	printf("\nHere is the input: %s\n", currentlyUsedFileName);
	getBlackListFileContents();
}

void requestURL(){
	char temp[2000];
	const char cache[10] = "";
	const char wget[8] = "wget ";
	const char apost[3] = "\"";
	const char fileNameCommand[5] = "-O ";
	const char html[7] = ".html";
	const char output[19] = " 2> wgetOutput.txt";
	bzero(temp, 2000);
	// Build the command...
	strcat(temp, wget);
	strcat(temp, fileNameCommand);
	  
	sprintf(currentTimeString, "%lld", currentTimeLong);
	
	//Add to cache
	cachedFileList = fopen(cacheFileName, "a");
	
	if(!cachedFileList){
		printf("\nCached file does not exist...\n");
	} else {
		fprintf(cachedFileList, "\n%s", currentlyRequestedIP);
		fprintf(cachedFileList, " %s\n", currentTimeString);
	}

	strcat(currentTimeString, ".html");	
	strcat(temp, currentTimeString);
	strcat(temp, " ");

	strcat(temp, currentlyRequestedIP);
	strcat(temp, output);		  
	  
	printf("\nHere is the command: %s\n\nRequesting URL now...\n", temp);
	system(temp);
	
	strcpy(fileToOpen, currentTimeString);

	system("sleep 5");
}

void listenForConnection(){
	 /* AF_INET - IPv4 IP , Type of socket, protocol*/
    listen_fd = socket(AF_INET, SOCK_STREAM, 0);
 
    bzero(&servaddr, sizeof(servaddr));
 
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htons(INADDR_ANY);
    servaddr.sin_port = htons(22006);
 
    /* Binds the above details to the socket */
	bind(listen_fd,  (struct sockaddr *) &servaddr, sizeof(servaddr));
	/* Start listening to incoming connections */
	listen(listen_fd, 10);
}
 
int main(){
    bzero(buffer, 2000000);
	setDateAndTime();	 // Sets current Date and Time 
	setBlackList();
	  
	printf("\nListening for connection...\n");
	listenForConnection(); 
	  
    // Accepts an incoming connection 
	conn_fd = accept(listen_fd, (struct sockaddr*)NULL, NULL);
	  
	printf("\n\nAccepted connection, waiting for message...\n\n");
	readMessage();
	//printf("\n\nHere is the url: %s\n\n", buffer);
	  
	strcpy(currentlyRequestedIP, buffer);
	//printf("\nHere is currentlyRequestedIP: %s\n", currentlyRequestedIP);

	if(isCurrentlyRequestedIPCached() == 0){
		printf("\n\nRequested URL was: %s, this URL is cached.\n\n", currentlyRequestedIP);
		getFileContents();
		writeMessage();
	} else {
	  
		if(compareRequestedIpWithBlackList() == 0){
			requestURL();
			getFileContents();
			writeMessage();
			
		} else {
			printf("\nAttempted to connect to a blacklisted site.\n");
			strcpy(buffer, "Attempted to connect to a blacklisted site. Permission denied.");
			writeMessage();
		}
	}
    // strcpy(str, "Hello world \n\r\0");
    // write(conn_fd, str, strlen(str)); // write to the client
    
	close (conn_fd); //close the connection
   
}