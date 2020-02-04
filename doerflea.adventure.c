#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

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

void getRooms(char* newestDirName, const char* rooms[7]){
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
      }
   }
     closedir(dir);
}
void readRoomFile(char* newestDirName, const char* rooms[7]){
   char filePath[50];
   memset(filePath,'\0',50);
   sprintf(filePath, "%s",rooms[0]);

   FILE* room_file;
   chdir(newestDirName);
   room_file = fopen(filePath, "r");
   fclose(room_file);
}

void main()
{
   char* newestDirName = getNewestRoomDir();
   const char *rooms[7];
   getRooms(newestDirName, rooms);
   readRoomFile(newestDirName, rooms);
}

   

   

