#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>

typedef enum { false, true } bool; //https://stackoverflow.com/questions/1921539/using-boolean-values-in-c

pthread_mutex_t mutex;


void* writeTime(){

   //https://www.epochconverter.com/programming/c
   //https://stackoverflow.com/questions/13644182/wrong-time-with-localtime

   time_t curr_time; //Create time datatype object

   //Open currentTime.txt if already created, if not, the create file
   FILE* time_file;
   time_file = fopen("currentTime.txt","w");

   struct tm* time_info;
   time(&curr_time);
   time_info = localtime(&curr_time);

   char time_str[100];
   memset(time_str, '\0',100); 
   strftime(time_str,100, "%I:%M%P %A, %B %d, %Y", time_info); // format string: hour, weekday, month, day, year

   fprintf(time_file, "%s",time_str); //Write time to file

   //Close file
   fclose(time_file); 
   return NULL;
}

void* readTime(){

   //Open time file
   FILE* time_file;
   time_file = fopen("currentTime.txt", "r");

   char buf[100];
   memset(buf,'\0',100);

   while(fgets(buf,100,time_file) != NULL){ //read in time file info
      printf("%s\n",buf); //print to screen
   }
   //Close time file
   fclose(time_file);
   return NULL;
}



void writerThread(){

   pthread_mutex_init(&mutex, NULL);
   pthread_mutex_lock(&mutex);

   pthread_t writet_thread;
   int wthread = pthread_create(&writet_thread, NULL, writeTime, NULL); //id_new_thread, flags, start_routine, args

   pthread_mutex_unlock(&mutex);
   pthread_mutex_destroy(&mutex);

   pthread_join(writet_thread,NULL);
   return;
}

void readerThread(){
   pthread_mutex_init(&mutex, NULL);
   pthread_mutex_lock(&mutex);

   pthread_t readt_thread;
   int rthread = pthread_create(&readt_thread, NULL, readTime, NULL); //id_new_thread, flags, start_routine, args

   pthread_mutex_unlock(&mutex);
   pthread_mutex_destroy(&mutex);

   pthread_join(readt_thread,NULL);
   return;


}




char* getNewestRoomDir(){
   int newestDirTime = -1; // Modified timestamp of newest subdir examined
   char targetDirPrefix[32] = "doerflea.rooms."; // Prefix we're looking for
   static char newestDirName[256]; // Holds the name of the newest dir that contains prefix
   memset(newestDirName, '\0', sizeof(newestDirName));

   DIR* dirToCheck; // Holds the directory we're starting in
   struct dirent *fileInDir; // Holds the current subdir of the starting dir
   struct stat dirAttributes; // Holds information we've gained about subdir

   dirToCheck = opendir("."); // Open up the directory this program was run in

   if (dirToCheck > 0) // Make sure the current directory could be opened
   {
      while ((fileInDir = readdir(dirToCheck)) != NULL) // Check each entry in dir
      {
	 if (strstr(fileInDir->d_name, targetDirPrefix) != NULL) // If entry has prefix
	 {
	    stat(fileInDir->d_name, &dirAttributes); // Get attributes of the entry

	    if ((int)dirAttributes.st_mtime > newestDirTime) // If this time is bigger
	    {
	       newestDirTime = (int)dirAttributes.st_mtime;
	       memset(newestDirName, '\0', sizeof(newestDirName));
	       strcpy(newestDirName, fileInDir->d_name);
	    }
	 }
      }
   }
   closedir(dirToCheck); // Close the directory we opened
   return newestDirName;
}



void getRooms(char* newestDirName,const char* rooms[7]){
   char* dot = ".";
   char* dotdot = "..";
   DIR* dir;
   struct dirent *room_file;
   int i = 0;

   dir = opendir(newestDirName);
   while ((room_file = readdir(dir)) != NULL) {
      if(strcmp(room_file->d_name, dot) !=0 && strcmp(room_file->d_name,dotdot) != 0){
	 rooms[i] = room_file->d_name;
	 i++;

	 char filePath[50];
	 memset(filePath,'\0',50);

	 sprintf(filePath, "%s/%s",newestDirName,room_file->d_name);

	 FILE* room_file;
	 char* line = NULL;
	 size_t length = 0;
	 ssize_t read;

	 room_file = fopen(filePath, "r");
	 if(room_file == NULL){
	    printf("NULL");
	    return;
	 }
      }
   }
   closedir(dir);

}




void readRoomFile(char* newestDirName, const char* rooms,int steps,char path[40][15]){

   printf("CURRENT LOCATION: %s\n", rooms);

   char possible_connections[6][15];

   strcpy(path[steps],rooms);

   int i = 0;

   DIR* dir;
   struct dirent *room_file;

   dir = opendir(newestDirName);
   char filePath[50];
   memset(filePath,'\0',50);

   sprintf(filePath, "%s/%s",newestDirName,rooms);

   FILE* room;
   char* line = NULL;
   size_t length = 0;
   ssize_t read;

   room = fopen(filePath, "r");
   if(room == NULL){
      printf("NULL");
      return;
   }
   char connections[100];
   memset(connections, '\0',100);
   while((read = getline(&line, &length,room)) != -1){

      char buf_connection[100];
      memset(buf_connection,'\0',100);

      char buf_room[100];
      memset(buf_room,'\0',100);

      sscanf(line,"%s%*s%s",buf_connection,buf_room);

      if(strstr(buf_room,"END_ROOM") !=NULL){
	 printf("YOU HAVE FOUND THE END ROOM. CONGRATULATIONS\n");
	 printf("YOU TOOK %d STEPS. YOUR PATH TO VICTORY WAS:\n",steps);
	 int k;
	 for(k = 1; k <= steps; k++){
	    printf("%s\n",path[k]);
	 }
	 return;
      }

      if(strstr(buf_connection,"CONNECTION") !=NULL){
	 strcpy(possible_connections[i],buf_room);
	 i++;
      }
   }
   int k;
   for(k = 0; k < i-1; k++){
      sprintf(connections,"%s%s, ",connections,possible_connections[k]);
   }
   sprintf(connections,"%s%s.",connections,possible_connections[i-1]);

   char answer[100];
   memset(answer, '\0',100);
   bool valid_answer = false; 

   printf("POSSIBLE CONNECTIONS: %s\n", connections);

   do{
      printf("WHERE TO?>");
      scanf("%s",&answer);

      if(strcmp(answer,"time") == 0){
	 printf("\n");
	 writerThread();
	 readerThread();
	 printf("\n");
      }

   }while(strcmp(answer,"time") == 0);


   int j;
   for(j = 0; j < i; j++){
      if(strcmp(answer,possible_connections[j]) == 0){
	 valid_answer = true;
      }
   }
   if(valid_answer == false){
      printf("\n");
      printf("HUH? I DON'T UNDERSTAND THAT ROOM. TRY AGAIN.\n\n");
      
      printf("CURRENT LOCATION: %s\n", rooms);
      printf("POSSIBLE CONNECTIONS: %s\n", connections);
      printf("WHERE TO?>");
      scanf("%s",&answer);

      while(strcmp(answer,"time") == 0){

         printf("\n");
	 writerThread();
	 readerThread();
	 printf("\n");

         printf("WHERE TO?>");
         scanf("%s",&answer);

      }

      for(j = 0; j < i; j++){
	 if(strcmp(answer,possible_connections[j]) == 0){
	    valid_answer = true;
	 }
      }

   }
   steps++;
   printf("\n\n");
   readRoomFile(newestDirName, answer,steps,path);
}


int getStartRoom(char* newestDirName, const char* rooms[7]){
   DIR* dir;
   struct dirent *room_file;


   int i;
   for(i = 0; i < 7; i++){
      dir = opendir(newestDirName);
      char filePath[50];
      memset(filePath,'\0',50);

      sprintf(filePath, "%s/%s",newestDirName,rooms[i]);

      FILE* room_file;
      char* line = NULL;
      size_t length = 0;
      ssize_t read;

      room_file = fopen(filePath, "r");
      if(room_file == NULL){
	 printf("NULL");
	 return;
      }

      while((read = getline(&line, &length,room_file )) != -1){

	 char buf_room[100];
	 memset(buf_room,'\0',100);

	 sscanf(line,"%*s%*s%s",buf_room);
	 if(strstr(buf_room, "START") != NULL){
	    return i;
	 }
      }
      fclose(room_file);
      closedir(dir);
   }
   return -1;
}



int main()
{
   char* newestDirName = getNewestRoomDir();
   const char* rooms[7];
   char path[40][15];
   getRooms(newestDirName,rooms);
   int start_index = getStartRoom(newestDirName,rooms);
   int steps = 0;
   readRoomFile(newestDirName,rooms[start_index],steps,path);
   return 0;


}





