#include <stdlib.h>
#include <stdio.h>
#include <string.h>
//------------------------------------------------------------------------
//#defines and #includes for everything needed or define
//------------------------------------------------------------------------
#define MAX_NAME 127
#define MAX_SIZE 120
#define SLIPS_RATE 12.5
#define LAND_RATE 14.0
#define TRAILOR_RATE 25.0
#define STORAGE_RATE 11.2
//------------------------------------typedefs for the enums, unions, and struct


typedef char String[MAX_NAME];

typedef enum {
	
	slip,
	land,
	trailor,
	storage,
	error,	
} boatType;

typedef union{
	int slip_number;
	char bay_letter;
	char trailor_tag[10];
	int storage_number;
} extra_info;

typedef struct {
	String name;
	int length;
	boatType Type;
	extra_info info;
	double dues;
} boat;

typedef boat boat_list[MAX_SIZE];

//-----------------------------------------------------------------------------
void * Malloc(size_t Size) {

    void * Memory;

    if ((Memory = malloc(Size)) == NULL) {
        perror("Cannot malloc");
        exit(EXIT_FAILURE);
    } else {
        return(Memory);
    }
}
//-----------------------------------------------------------------------------

void* Realloc(void *old, size_t size){
	if((old = realloc(old, size)) == NULL){
		perror("Cannot Realloc More Space!");
		exit(EXIT_FAILURE);
	} else {
		return old;
	}

}

void Free(void ** Memory) {

    free(*Memory);
    *Memory = NULL;
}
//-----------------------------------------------------------------------------
//------------------------------------Funtion to print boat type
char* get_boatType(boatType place){
	
	switch(place){

		case slip:
			return "slip";
		case land:
			return "land";
		case trailor:
			return "trailor";
		case storage:
			return "storage";
		case error:
			return "No such storage type!";
		default:
			printf("Something is off here...");
			exit(EXIT_FAILURE);
			break;
	}
}
//------------------------------------Function to get Boat Type from string
boatType String_to_boatType(String place){

	if (!strcasecmp(place, "slip")){
		return slip;
	} else if (!strcasecmp(place, "land")){
		return land;
	} else if (!strcasecmp(place, "trailor")){
		return trailor;
	}	else if (!strcasecmp(place, "storage")){
		return storage;
	} else{
		return error;
	}

}
//------------------------------------ Function to edit the union
void add_to_union(extra_info *info, boatType place,char * additional_info){
	switch(place){

		case slip:
			info->slip_number = atoi(additional_info);
			return;
		case land:
			info->bay_letter = *additional_info;
			return;
		case trailor:
			strcpy(info->trailor_tag, additional_info);
			return;
		case storage:
			info->storage_number = atoi(additional_info);
			return;
		case error:
			printf("No such storage type!");
			return;
		default:
			printf("Something is off here...");
			exit(EXIT_FAILURE);
			break;
	}
}

//------------------------------------Functin which parses CSV format
void extract_data(boat ** curr_boat, char * data, int count){
	
	char * name = strtok(data, ",");
//iterate by using strtok to seperate by the commas
//------------------------------------
	int length = atoi((strtok(NULL, ",")));
	if(length > 100){
		printf("Your boat is too long for this marina!\n");
		count--;
		return;
	}
	char * place_str = strtok(NULL, ",");
	boatType place = String_to_boatType(place_str);
	char *additional_info = strtok(NULL, ",");
	extra_info info;
	add_to_union(&info, place, additional_info);
	double owed = atof(strtok(NULL, ","));
	
	boat * new_boat = (boat*) Malloc(sizeof(boat));
	strncpy(new_boat->name, name, MAX_NAME);
	new_boat->length = length;
	new_boat->Type = place;
	new_boat->info = info;
	new_boat->dues = owed;

	curr_boat[count] = new_boat;
}
//------------------------------------By iterating through the strtok, the boat
//can be added. It is also checked that the boat is not too long
//------------------------------------Function to read in the CSV file

int read_boats(boat ** curr_boat, char *file_name){
	FILE *fp;
	char line[200];
	int count = 0;

	if((fp = fopen(file_name, "r")) == NULL){
		perror("Error");
	}
	fp = fopen(file_name, "r");

	if(fp == NULL){
		perror("Error");
	}

	while(fgets(line, 200, fp) != NULL && count < MAX_SIZE){
		extract_data(curr_boat, line, count);
		count += 1;
//--utilizing extractdata again to parse the data
	}

	fclose(fp);

	return count;
}
//Function used for the qsort comparisons
int boat_name_comparison(const void *a, const void *b){

	boat * boat_a = *(boat **)a;
    boat * boat_b = *(boat **)b;
    return strcasecmp(boat_a->name, boat_b->name);
}
//--Function to print the whole struct of a boat
void print_data(boat ** curr_boat,int count){

	qsort(curr_boat, count, sizeof(boat*), boat_name_comparison);

//qsort to sort in alphabetical order first

	for (int i = 0; i < count; i++) {
        
		boat * boat = curr_boat[i];
        
		printf("%s\t %d\' \t %s\t", boat->name, boat->length, get_boatType(boat->Type));
//using a switch statement to print the union variable       
		switch (boat->Type) {
            case slip:
                printf("%d\t", boat->info.slip_number);
                break;
            case land:
                printf("%c\t", boat->info.bay_letter);
                break;
            case trailor:
                printf("%s\t", boat->info.trailor_tag);
                break;
            case storage:
                printf("%d\t,", boat->info.storage_number);
                break;
			default:
			printf("%s", "n/a");
        }
        
		printf("Owes $%.2lf\n", boat->dues);
    }

}
//--Function to go about adding a boat to the inventory
void add_boat(boat ** curr_boat, char * data, int * count){
	if(*count >= MAX_SIZE){
		printf("We could not add this boat. We are at maximum capacity!\n");
		return;
	}	

	extract_data(curr_boat, data, *count);
}
//-Function to go about removing a boat from inventory
void remove_boat(boat ** curr_boats, char * boat_name, int count){
	
	boat * curr_boat;

	for (int i = 0; i < count; i++) {
		curr_boat = curr_boats[i];
        if(strcasecmp(boat_name, curr_boat->name) == 0) {

			*curr_boats[i] = *curr_boats[count - 1];
			Free((void**)(&curr_boats[count - 1]));
            return;
        }
    }
}//-Remembering to free up the memory from the no longer there boat


//------------------------------------function to pay for a boat's dues
void file_payment(boat ** curr_boats, char * boat_name, double payment, int count){

	boat * curr_boat;

	for (int i = 0; i < count; i++) {
		curr_boat = curr_boats[i];
        if(strcasecmp(boat_name, curr_boat->name) == 0) {
			if(payment > curr_boat->dues){
				printf("That is more than the amount owed: %f\n",curr_boat->dues);
				return;
			} else if(payment > 0 && payment <= curr_boat->dues){
				curr_boat->dues -= payment;
				return;
			} else{
				printf("That is an invalid figure!\n");
			}
        }
    }

	printf("No boat with that name\n");

}
//--function to add a month (30 days) of dues to every boat
 void add_month_dues(boat ** curr_boats, int count){
      boat *curr_boat;
 
      for (int i = 0; i < count; i++){
          curr_boat = curr_boats[i];
          switch (curr_boat->Type) {
              case slip:
                  curr_boat->dues += (curr_boat->length)*30*SLIPS_RATE;
                  break;
              case land:
                  curr_boat->dues += (curr_boat->length)*30*LAND_RATE;
                  break;
              case trailor:
                  curr_boat->dues += (curr_boat->length)*30*TRAILOR_RATE;
                  break;
              case storage:
                  curr_boat->dues += (curr_boat->length)*30*STORAGE_RATE;
                  break;
              default:
                  printf("Something is awfully wrong here\n");
                  break;
          }
      }//iterates by switch case for each different type of boat
  }
//-Function that saves changed inventories to the csv file
void save_to_csv(char* file_name, boat ** curr_boat, int count){
	FILE *file = fopen(file_name, "w");

	if(file == NULL){
		perror("ERROR! COULD NOT OPEN FILE\n");
	}

	for (int i = 0; i < count; i++) {
        
		boat * boat = curr_boat[i];
        
		fprintf(file, "%s,%d,%s,", boat->name, boat->length, get_boatType(boat->Type));
        
		switch (boat->Type) {
            case slip:
                fprintf(file, "%d,", boat->info.slip_number);
                break;
            case land:
                fprintf(file, "%c,", boat->info.bay_letter);
                break;
            case trailor:
                fprintf(file, "%s,", boat->info.trailor_tag);
                break;
            case storage:
                fprintf(file, "%d,", boat->info.storage_number);
                break;
			default:
			fprintf(file, "%s", "n/a");
        }
        
		fprintf(file, "%.2lf\n", boat->dues);
    }
    
	fclose(file);
}

void prompt(void){
	printf("(I)nventory, (A)dd, (R)emove, (P)ayment, (M)onth, e(X)it : ");
}

int main(int argc, char* argv[]){
	
	boat ** the_boats = (boat**) Malloc(MAX_SIZE * sizeof(boat*));
	
	if(argc < 2){
		printf("You did not send a file in as a command line argument!\n");
		return(EXIT_FAILURE);
	}

	int count = read_boats(the_boats, argv[1]); // loads the boats into the structs
	
	printf("Welcome to the Boat Management System\n");
	printf("-------------------------------------\n");
	
	char user_selection[10];
	char input[100];

	prompt();
	fgets(user_selection, 10, stdin);
	user_selection[strcspn(user_selection, "\n")] = '\0';


	while(strcasecmp(user_selection, "x") != 0){
	
		if(strcasecmp(user_selection, "i") == 0){
			print_data(the_boats, count);
		
		} else if(strcasecmp(user_selection, "a") == 0){		
			printf("Please enter the boat data in CSV format: ");
			fgets(input,100, stdin);
			input[strcspn(input, "\n")] = '\0';
			
			add_boat(the_boats, input, &count);
			
			count++;
			
		} else if(strcasecmp(user_selection, "r") == 0){
			printf("Enter the info for the boat you wanna remove here: ");
			fgets(input,100, stdin);
			input[strcspn(input, "\n")] = '\0'; //deal with carriage
			remove_boat(the_boats, input, count);
			count--;

		} else if(strcasecmp(user_selection, "p") == 0){
			char boat_name[100];
			printf("Enter the boat name: ");
			fgets(boat_name,100, stdin);
			fflush(stdin); //--clean out input
			boat_name[strcspn(boat_name, "\n")] = '\0';

			double pmt;
			printf("Enter the payment: ");
			scanf("%lf", &pmt);

			file_payment(the_boats, boat_name, pmt, count);		
			fflush(stdin);
		} else if(strcasecmp(user_selection, "m") == 0){
			
			add_month_dues(the_boats, count);

		} else{

			printf("Invalid input: %s", user_selection);
		}
		
		prompt();
		fgets(user_selection, 10, stdin);
		user_selection[strcspn(user_selection, "\n")] = '\0';

	}

	printf("Exitting the Boat Management System\n");

	save_to_csv(argv[1], the_boats, count); //final save

// Free the memory, iteratively through the array, then the array itself
    for (int i = 0; i < count; i++) {
		Free((void**)(&the_boats[i]));
	}
    Free((void**)(&the_boats));

	return(EXIT_SUCCESS);

}
