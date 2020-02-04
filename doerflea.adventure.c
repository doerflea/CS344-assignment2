#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include<fcntl.h>

typedef enum { false, true } bool; //https://stackoverflow.com/questions/1921539/using-boolean-values-in-c


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




void readRoomFile(char* newestDirName, const char* rooms){


   printf("CURRENT LOCATION: %s\n", rooms);
   char possible_connections[6][15];
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
      char buf_name[100];
      memset(buf_name, '\0',100);
      char buf_connection[100];
      memset(buf_connection,'\0',100);
      char buf_room[100];
      memset(buf_room,'\0',100);


      sscanf(line,"%s%s%s",buf_connection, buf_name,buf_room);
      if(strstr(buf_room,"END_ROOM") !=NULL){
	 printf("YOU HAVE FOUND THE END ROOM. CONGRATULATIONS\n");
	 return;
      }
      if(strstr(buf_connection,"CONNECTION") !=NULL){
	 char name[15];
	 memset(name,'\0',15);
	 sprintf(connections,"%s%s ",connections, buf_room);
	 sprintf(name, "%s",buf_room);
	 strcpy(possible_connections[i],name);
	 i++;
      }

   }
   int j;
   char answer[100];
   memset(answer, '\0',100);
   printf("POSSIBLE CONNECTIONS: %s\n", connections);
   printf("WHERE TO?>");
   scanf("%s",&answer);
   bool valid_answer = false;
   for(j = 0; j < i; j++){
      if(strcmp(answer,possible_connections[j]) == 0){
	 valid_answer = true;
      }
   }
   while(valid_answer == false){
      printf("WHERE TO?>");
      scanf("%s",&answer);
      for(j = 0; j < i; j++){
	 if(strcmp(answer,possible_connections[j]) == 0){
	    valid_answer = true;
	 }
      }

   }
   readRoomFile(newestDirName, answer);
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




void main()
{
   int i;
   char* newestDirName = getNewestRoomDir();
   const char* rooms[7];

   getRooms(newestDirName,rooms);
   int start_index = getStartRoom(newestDirName,rooms);

   readRoomFile(newestDirName,rooms[start_index]);


   // printf("CURRENT LOCATION:%s\n", rooms[start_index]);

   /*for(i = 0; i < 7; i++){
     printf("rooms %s\n",rooms[i]);
     }*/



}





