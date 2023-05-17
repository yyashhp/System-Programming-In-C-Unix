#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//----------------------------------
//---Include Neccessities
//----------------------------------
//----Define Important Things
#define STRING_LENGTH 20
typedef char String[STRING_LENGTH];


typedef struct Country{
  String Name;
  struct Country* Ally;
  struct Country* Next;
} Country;

typedef Country * COUNTRYPOINTER;

//----------------function to add
//------------a country to the linked list
void addCountry(COUNTRYPOINTER * head, String Name){
  COUNTRYPOINTER NewCount = (COUNTRYPOINTER)malloc(sizeof(Country));	
  strcpy(NewCount->Name,Name);
  NewCount->Next = *head;
  *head = NewCount;
}

//----------------function to find ally in the linked list

COUNTRYPOINTER Search(COUNTRYPOINTER head, String Name){ 
  while(head){
  if(strcmp(head->Name,Name)==0){
    return head;
  }
  head = head->Next;
  }
}

//----------------enter best ally names------------------------------------------------
void Ally(COUNTRYPOINTER head){
  String Name;
  COUNTRYPOINTER current = head;
  while(current){
    printf("Enter best ally name for %s : ",current->Name);
    scanf("%s",Name);
    current->Ally = Search(head,Name); //set ally to country
    current = current->Next;
  }
}

//----------------print best allies-----------------------------------------------------
void printAllies(COUNTRYPOINTER head){ //function to print allies   while(head){
    printf("The best ally of %s is %s\n", head->Name, head->Ally->Name);
    head = head->Next;
  }
}

int main(){
  COUNTRYPOINTER head = NULL;
  String Name;
  int Add = 1;
  while(Add>=1){
    printf("Enter nation name : ");
    fgets(Name, sizeof(Name), stdin);
    Name[strcspn(Name, " \n")] = '\0';
    if(strcmp(Name, "") == 0){ //terminate when user enters a blank space
      Add = 0;
    }
    addCountry(&head,Name);
  }
  Ally(head);
  printf("\n");
  printAllies(head);
	
//-------free malloced memory-----------------------
  COUNTRYPOINTER current = head;
  while(current != NULL){
    COUNTRYPOINTER temp  = current;
    current = NULL;
    current = current->Next;
    free(temp);
  }
return(EXIT_SUCCESS);
}
