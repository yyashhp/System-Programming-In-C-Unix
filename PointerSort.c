//-------Program to generate random ints in an array, and an array of
// respective pointers, and sort the pointer array. Then sort the 
// int array and see how the pointers change in reaction---------------------
//---------------------------------------------------------------------------
//--------------------Include essentials first, including library for getpid()--

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

//---------------------define variables, including int pointer array ----------

#define ARRAY_SIZE 5
typedef int * IntPointer;
typedef int IntArray[ARRAY_SIZE];
typedef IntPointer PointerArray[ARRAY_SIZE];

//------------------Function to initialize int array, setting each index
//---of the intPointer array to the random ints. First seeding the generator

void init(int * IntArr, IntPointer * PointerArr){
  srand(getpid());
  for (int Num = 0; Num < ARRAY_SIZE; Num++){
    IntArr[Num] = rand();
    PointerArr[Num] = &IntArr[Num];
  }
}
//--------Methods to print the array entries  by going through all indices
void printArr(int * IntArr) {
  for(int Num = 0; Num < ARRAY_SIZE; Num++){
  printf("%i: %i\n",Num+1,IntArr[Num]);
  }
}

void PrintPointers(IntPointer * PointerArr){
  for(int Num = 0; Num < ARRAY_SIZE; Num++) {
  printf("%i: %i\n",Num+1,*PointerArr[Num]);
  }
}

//-----------Using Bubble Sort to sort the int array and the intPointer Array
//------Double de-referencing in the SortPointers method as the array
//------entries themselves are pointers to ints

void SortInts(int * IntArr) {
  for(int Num = 0; Num < ARRAY_SIZE-1; Num++){
    for(int Run = Num; Run < ARRAY_SIZE; Run++){
      if(IntArr[Num] < IntArr[Run]){
        int temp = IntArr[Num];
        IntArr[Num] = IntArr[Run];
        IntArr[Run] = temp;
      }
    }
  }
}
void SortPointers(IntPointer * PointerArr) {
  for(int Num = 0; Num < ARRAY_SIZE-1; Num++) {
    for(int Run = Num; Run < ARRAY_SIZE; Run++){
      if(*PointerArr[Num] < *PointerArr[Run]){
        IntPointer temp = PointerArr[Num];
        PointerArr[Num] = PointerArr[Run];
        PointerArr[Run] = temp;
      }
    }
  }
}

//-------Main method which runs the methods and prints results accordingly

int main(void){
  IntArray intList;
  PointerArray pointList;
  init(intList, pointList);
  printf("----- Initialized array of integers -----\n");
  printArr(intList);
  SortPointers(pointList);
  printf("----- Sorted array of pointers -----\n");
  PrintPointers(pointList);
  SortInts(intList);
  printf("----- Sorted array of integers -----\n");
  printArr(intList);
  printf("----- Array of pointers -----\n");
  PrintPointers(pointList);
  return(EXIT_SUCCESS);
}
