#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <pthread.h>
#include <time.h>

typedef enum { false, true } bool; //https://stackoverflow.com/questions/1921539/using-boolean-values-in-c

char* room_names[10] = { "LIVING_ROOM", "KITCHEN", "DINING_ROOM", "BEDROOM", "BATHROOM", "GARAGE", "CLOSET", "LAUNDRY_ROOM", "TORTURE_ROOM", "ATTIC"};

enum room_types{ START_ROOM, MID_ROOM, END_ROOM};

struct ROOM{
   char* name;
   int type;
   int num_connections;
   struct ROOM* connections[6];
};

struct ROOM* all_rooms[7];

void InitializeRoom(int room_num, int rooms_made)
{
   struct ROOM* new_room = malloc(sizeof(struct ROOM));
   new_room->name = room_names[room_num];
   new_room->num_connections = 0;
   all_rooms[rooms_made] = new_room;

}

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
   int i;
   for(i = 0; i < 7; i++){
      FILE* room_file;
      char pathFile[256];
      memset(pathFile,'\0',256);
      sprintf(pathFile, "doerflea.rooms.%d/%s",getpid(),all_rooms[i]->name);
      room_file = fopen(pathFile, "w");
      fclose(room_file);
   }
}

void CreateRooms()
{
   int rooms_made[7];
   int rooms_made_num = 0;
   int room_match_check = 0;
   memset(rooms_made, -1, 7);
   int room_num;
   int i = 0;
   int length = 0;

   while(rooms_made_num < 7){
      int room_num = rand() % 10;
      for(i = 0; i < 7; i++){
	 if(room_num == rooms_made[i]){
	    room_match_check++;
	 }
      }
      if(room_match_check == 0){
	 InitializeRoom(room_num, rooms_made_num);
	 rooms_made[rooms_made_num] = room_num;
	 rooms_made_num++;
      }
      room_match_check = 0;
   }
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

// Returns a random Room, does NOT validate if connection can be added
struct ROOM* GetRandomRoom()
{
   int random_room = rand() % 7;
   return all_rooms[random_room];
}


// Connects Rooms x and y together, does not check if this connection is valid
void ConnectRoom(struct ROOM* x, struct ROOM* y) 
{
   x->connections[x->num_connections] = y;
   x->num_connections++;
   y->connections[y->num_connections] = x;
   y->num_connections++;
   
}

// Returns true if Rooms x and y are the same Room, false otherwise
bool IsSameRoom(struct ROOM* x, struct ROOM* y)
{
   if(strcmp(x->name, y->name) == 0){
      return true;
   }
   return false;
}

// Returns true if a connection from Room x to Room y already exists, false otherwise
bool ConnectionAlreadyExists(struct ROOM* a, struct ROOM* b)
{
   int i = 0;
   for(i = 0; i < a->num_connections; i++){
      if(strcmp(a->connections[i]->name, b->name) == 0){
	 return true;
      }
   }
   return false;
}

// Returns true if a connection can be added from Room x (< 6 outbound connections), false otherwise
bool CanAddConnectionFrom(struct ROOM* x) 
{
   if(x->num_connections <= 5){
      return true;
   }
   return false;

}


// Adds a random, valid outbound connection from a Room to another Room
void AddRandomConnection()  
{
   struct ROOM* A = NULL;
   struct ROOM* B = NULL;

   while(true)
   {
      //printf("can add conneciton loop");
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


int main(){
   srand(time(0));
   CreateRoomDirectory();
   CreateRooms();
   // Create all connections in graph
   /*while (IsGraphFull() == false)
   {
      //printf("Graph full loop");
      AddRandomConnection();
   }*/
     CreateRoomFiles();

}
