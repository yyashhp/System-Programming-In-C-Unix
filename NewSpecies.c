#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define INITIAL_SIZE 1
#define STRING_LENGTH 128
#include <ctype.h>

//include necessary libraries, define constants
//----------------------------------------------------------------------------

typedef char String[STRING_LENGTH];

typedef enum {
  fish,
  insect,
  mammal,
  bird,
  error
} SpeciesType;

//-an enum to catch invalid types as well

typedef struct {
  String Name;
  double Size;
  SpeciesType Species;
} NewSpecies;
  
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

//-A realloc function that is similar to Malloc, which will back out if a null
//-pointer is produced by the realloc function

//----------------------------------------------------------------------------

void * Realloc(void *old,size_t size){
  if((old = realloc(old,size)) == NULL) {
    perror("Cannot Realloc More Space");
    exit(EXIT_FAILURE);
  }
    else{
    return(old);
  }
}
//----------------------------------------------------------------------------
//Returns the string of the species type so that it can be printed properly

char * GetSpeciesType(SpeciesType animal){
  switch (animal) {
    case fish: return("fish");
    break;
    case insect: return("insect");
    break;
    case mammal: return("mammal");
    break;
    case bird: return("bird");
    break;
    default:
    return("INVALID ANIMAL TYPE");
    break;
  }
}
//-----------------------------------------------------------------------------
void Free(void ** Memory) {

    free(*Memory);
    *Memory = NULL;
}
//-----------------------------------------------------------------------------
//-converts an inputted string into the SpeciesType object

SpeciesType GetSpecies(String type){
  String temp;
  strcpy(temp,type);
  for(int letter = 0; temp[letter]; letter++){
    temp[letter] = tolower(temp[letter]);
  }
  if(!strcmp(type,"mammal")) {
    return(mammal);
  }
  if(!strcmp(type,"insect")) {
    return(insect);
  }
  if(!strcmp(type,"fish")) {
    return(fish);
  }
  if(!strcmp(type,"bird")) {
    return(bird);
  }
  return(error);
}
  
//-----------------------------------------------------------------------------
//-takes in the user's input to return an entry into the Book of species'

NewSpecies CreateSpecies(SpeciesType type, double size, String name){
  NewSpecies NewEntry;
  strcpy(NewEntry.Name,name);
  NewEntry.Size = size;
  NewEntry.Species = type;
  return(NewEntry);
}
//----------------------------------------------------------------------------
//-prints all attributes of a species

void PrintSpecies(NewSpecies Entry){
  printf("%s\t has size %0.2f and is a %s\n",Entry.Name,Entry.Size,GetSpeciesType(Entry.Species));
}
//----------------------------------------------------------------------------

int main() {
  int size = INITIAL_SIZE;
  int count = 0;
  String name;
  double Entrysize;
  SpeciesType type;
  String typeEntry;
  NewSpecies * Book; //--this will hold the user's input so it can be converted
  Book = (NewSpecies*)Malloc(sizeof(NewSpecies));
  printf("Enter animal information(\"exit\" to exit)");
  scanf("%s",name);
  while((strcmp(name,"exit"))){
    if (count == size){
      size*=2;   //-this will adjust size dynamically, as needed
      Book = (NewSpecies*)Realloc(Book,size * sizeof(NewSpecies));
      }
    printf("What is the size?");
    scanf("%lf",&Entrysize);
    printf("What is the type?");
    scanf("%s",typeEntry);
    type = GetSpecies(typeEntry);
    Book[count] = CreateSpecies(type,Entrysize,name);
    count++;
    printf("Enter animal information(\"exit\" to exit)");
    scanf("%s",name);
    }
  printf("The following new species were found:\n");
  for  (int printcounter = 0; printcounter<count; printcounter++){
    PrintSpecies(Book[printcounter]);
  }
  Free((void **)(&Book));  //--free the array
  return(EXIT_SUCCESS);
} 
  
