#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <pthread.h>
#include <time.h>

typedef enum { false, true } bool; //https://stackoverflow.com/questions/1921539/using-boolean-values-in-c

char* room_names[10] = { "LIVING_ROOM", "KITCHEN", "DINING_ROOM", "BEDROOM", "BATHROOM", "GARAGE", "CLOSET", "LAUNDRY_ROOM", "TORTURE_ROOM ", "ATTIC"};
char* room_types[3] = { "START_ROOM", "MID_ROOM", "END_ROOM" };

struct ROOM{
   char* name;
   char* type;
   int num_connections;
   struct ROOM* connections[6];
};

struct ROOM* all_rooms[7];

void CreateRoomDirectory()
{
   char* directory_name = malloc(sizeof(char)*20);
   char* static_dirname = "doerflea.rooms.";
   int pid = getpid();
   sprintf(directory_name, "%s%d", static_dirname, pid);
   mkdir(directory_name, 0755);
   free(directory_name);
}
void CreateRoomFiles()
{
   srand(time(NULL));

   int rooms_made[7];
   int rooms_made_num = 0;
   int room_match_check = 0;
   memset(rooms_made, -1, 7);
   int room_num = -1;
   int i = 0;
  
   while(rooms_made_num < 7){
      int room_num = rand() % 10;
      for(i = 0; i < rooms_made_num; i++){
	 if(room_num == rooms_made[i]){
	    room_match_check++;
	 }
      }
      if(room_match_check == 0){
	 rooms_made[rooms_made_num] = room_num;
	 SingleRoomFile(rooms_made_num);
	 rooms_made_num++;
      }
      room_match_check = 0;
   }
}

void SingleRoomFile(int number)
{
   FILE *roomFile;
   char pathFile[256];
   memset(pathFile,'\0',256);
   sprintf(pathFile, "doerflea.rooms.%d/%s",getpid(),room_names[number]);
   fopen(pathFile, "w");
   //fclose(roomFile);
}



      
/*
// Create all connections in graph
while (IsGraphFull() == false)
{
   AddRandomConnection();
}

// Returns true if all rooms have 3 to 6 outbound connections, false otherwise
bool IsGraphFull()  
{
   int i = 0;
   int full = 0;
   for(i = 0; i < 7; i++){
      if(all_rooms[i]->num_connections >= 3){
	 full++;
      }
   }
   if(full == 7){
      return true;
   }
   return false;

}

// Adds a random, valid outbound connection from a Room to another Room
void AddRandomConnection()  
{
   Room A;  // Maybe a struct, maybe global arrays of ints
   Room B;

   while(true)
   {
      A = GetRandomRoom();

      if (CanAddConnectionFrom(A) == true)
	 break;
   }

   do
   {
      B = GetRandomRoom();
   }
   while(CanAddConnectionFrom(B) == false || IsSameRoom(A, B) == true || ConnectionAlreadyExists(A, B) == true);

   ConnectRoom(A, B);  // TODO: Add this connection to the real variables, 
   ConnectRoom(B, A);  //  because this A and B will be destroyed when this function terminates
}

// Returns a random Room, does NOT validate if connection can be added
Room GetRandomRoom()
{
   ...
}

// Returns true if a connection can be added from Room x (< 6 outbound connections), false otherwise
bool CanAddConnectionFrom(Room x) 
{
   ...
}
// Returns true if a connection from Room x to Room y already exists, false otherwise
bool ConnectionAlreadyExists(struct ROOM* a, struct ROOM* b)
{
   int i = 0;
   for(i = 0; i < a->num_connections; i++){
      if(a->connections[i]->name == b->name){
	 return true;
      }
   }
   return false;
}

// Connects Rooms x and y together, does not check if this connection is valid
void ConnectRoom(Room x, Room y) 
{
   ...
}

// Returns true if Rooms x and y are the same Room, false otherwise
bool IsSameRoom(Room x, Room y) 
{
   ...
}

*/
int main(){
   CreateRoomDirectory();
   CreateRoomFiles();
}
