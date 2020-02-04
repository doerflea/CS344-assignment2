#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include<fcntl.h>

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
	 //printf("%s", filePath);

	 FILE* room_file;
	 char* line = NULL;
	 size_t length = 0;
	 ssize_t read;
	 //char* room_type = "MID_ROOM";

	 room_file = fopen(filePath, "r");
	 if(room_file == NULL){
	    printf("NULL");
	    return;
	 }
      }
   }
   closedir(dir);

}




void readRoomFile(char* newestDirName, const char* rooms[7],int i){
   DIR* dir;
   struct dirent *room_file;

   dir = opendir(newestDirName);
   char filePath[50];
   memset(filePath,'\0',50);

   sprintf(filePath, "%s/%s",newestDirName,rooms[i]);
   //printf("%s", filePath);

   FILE* room;
   char* line = NULL;
   size_t length = 0;
   ssize_t read;

   room = fopen(filePath, "r");
   if(room == NULL){
      printf("NULL");
      return;
   }
   printf("POSSIBLE CONNECTIONS: ");
   while((read = getline(&line, &length,room)) != -1){
      //printf("line length: %zu\n", read);
      //printf("line: %s", line);
      char buf_name[100];
      memset(buf_name, '\0',100);
      char buf_connection[100];
      memset(buf_connection,'\0',100);
      char buf_room[100];
      memset(buf_room,'\0',100);

      
      sscanf(line,"%s%s%s",buf_connection, buf_name,buf_room);
      if(strstr(buf_connection,"CONNECTION") !=NULL){
	 printf("%s, ", buf_room);
      }
      else{
	 printf(".\n");
      }

   }
   closedir(dir);
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
   readRoomFile(newestDirName,rooms, 1);

   int start_index = getStartRoom(newestDirName,rooms);

   char* curr_room = "START_ROOM";

   if(strcmp(curr_room,"END_ROOM") !=0){
      printf("CURRENT LOCATION:%s\n", rooms[start_index]);
   }
   /
   /*for(i = 0; i < 7; i++){
     printf("rooms %s\n",rooms[i]);
     }*/



}





