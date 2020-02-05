#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <pthread.h>
#include <time.h>

typedef enum { false, true } bool; //https://stackoverflow.com/questions/1921539/using-boolean-values-in-c

char* room_names[10] = { "LIVING_ROOM", "KITCHEN", "DINING_ROOM", "BEDROOM", "BATHROOM", "GARAGE", "CLOSET", "LAUNDRY_ROOM", "TORTURE_ROOM", "ATTIC"};

char* room_types[3] = { "START_ROOM", "MID_ROOM", "END_ROOM"};

struct ROOM{
   char* name;
   char* type;
   int num_connections;
   struct ROOM* connections[6]; 
};

struct ROOM* all_rooms[7];

void InitializeRoom(int room_num, int rooms_made)
{
   struct ROOM* new_room = malloc(sizeof(struct ROOM)); //Allocate memory for new room
   new_room->name = room_names[room_num]; //Take room name from names array using random index passed by CreateRoom function
   new_room->num_connections = 0;
   all_rooms[rooms_made] = new_room;

}
void CreateRoomType(){
   int i;
   all_rooms[0]->type = room_types[0]; //Assign one room as start room
   all_rooms[6]->type = room_types[2]; //Assign one room as end room
   //Assign remaining rooms as mid rooms
   for(i = 1; i < 6; i++){
      all_rooms[i]->type = room_types[1];
   }
}

void CreateRoomDirectory()
{
   char* directory_name = malloc(sizeof(char)*20);
   char* static_dirname = "doerflea.rooms.";
   int pid = getpid();
   sprintf(directory_name, "%s%d", static_dirname, pid); //Create directory name using onid user name and process id
   mkdir(directory_name, 0755);
   free(directory_name);
}

void CreateRoomFiles()
{
   int i,j;
   for(i = 0; i < 7; i++){
      FILE* room_file;
      char pathFile[256];
      memset(pathFile,'\0',256);
      sprintf(pathFile, "doerflea.rooms.%d/%s",getpid(),all_rooms[i]->name);
      room_file = fopen(pathFile, "w");
      
      //print room name to file
      fprintf(room_file, "ROOM NAME: %s\n", all_rooms[i]->name);

      //print connections
      for(j = 0; j < all_rooms[i]->num_connections; j++){
	fprintf(room_file, "CONNECTION %d: %s\n", j+1, all_rooms[i]->connections[j]->name);
      }
      //print room type
   	fprintf(room_file, "ROOM TYPE: %s\n", all_rooms[i]->type);
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

   while(rooms_made_num < 7){ //Make 7 rooms
      int room_num = rand() % 10; //Get random index number to take from room name array
      for(i = 0; i < 7; i++){
	 if(room_num == rooms_made[i]){ //If index already matches one made, increment match check variable
	    room_match_check++;
	 }
      }
      if(room_match_check == 0){ //If no matches were found, create new room
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

   ConnectRoom(A, B);  // TODO: Add this connection to the real variables
   if(ConnectionAlreadyExists(B,A) == false) 
   	ConnectRoom(B, A);  //  because this A and B will be destroyed when this function terminates
}


int main(){
   srand(time(NULL));
   CreateRoomDirectory();
   CreateRooms();
   // Create all connections in graph
   while (IsGraphFull() == false)
   {
      //printf("Graph full loop");
      AddRandomConnection();
   }
   CreateRoomType();
   CreateRoomFiles();

   //Free memory allocated for room structs
   int i;
   for(i = 0; i < 7; i++){
      free(all_rooms[i]);
   }

}
