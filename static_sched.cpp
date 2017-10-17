#include <iostream>
#include <cmath>
#include <cstdlib>
#include <chrono>
#include <pthread.h>
#include <math.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

float f1(float x, int intensity);
float f2(float x, int intensity);
float f3(float x, int intensity);
float f4(float x, int intensity);

#ifdef __cplusplus
}
#endif


char sync[10];
int nbthreads;
int intensity;
int choice;
float a;
float b;
float n;
float x;
float result;

//Structure to hold the start and end values for each thread.
struct values{
  int start, end;
};

//Thread called by each thread to increment the result. Is protected with a mutex.
void incrementResult(float x){
  pthread_mutex_lock;
  result += x;
  pthread_mutex_unlock;
}

//This function is called when "iteration" is the argument.
void *iterationFunc(void *pArg) {
  struct values *data = (values *)pArg;
  if(choice == 1){
    for(int i = data -> start; i < data -> end + 1; i++){
      incrementResult(f1(a+((i+.5)*x), intensity) * x); //Increment each time a portion is calculated.
    }
  }
  else if(choice == 2){
    for(int i = data -> start; i < data -> end + 1; i++){
      incrementResult(f2(a+((i+.5)*x), intensity) * x);
    }
  }
  else if(choice == 3){
    for(int i = data -> start; i < data -> end + 1; i++){
      incrementResult(f3(a+((i+.5)*x), intensity) * x);
    }
  }
  else{
    for(int i = data -> start; i < data -> end + 1; i++){
      incrementResult(f4(a+((i+.5)*x), intensity) * x);
    }
  }
}

//This function is called when "thread" is the argument.
void *threadFunc(void *pArg) {
  struct values *data = (values *)pArg;
  float temp = 0;
  if(choice == 1){
    for(int i = data -> start; i < data -> end + 1; i++){
      temp += (f1(a+((i+.5)*x), intensity) * x);
    }
    incrementResult(temp); //Only incrememnt once the entire chunk is calculated.
  }
  else if(choice == 2){
    for(int i = data -> start; i < data -> end + 1; i++){
      temp += f2(a+((i+.5)*x), intensity) * x;
    }
    incrementResult(temp);
  }
  else if(choice == 3){
    for(int i = data -> start; i < data -> end + 1; i++){
      temp += f3(a+((i+.5)*x), intensity) * x;
    }
    incrementResult(temp);
  }
  else{
    for(int i = data -> start; i < data -> end + 1; i++){
      temp += f4(a+((i+.5)*x), intensity) * x;
    }
    incrementResult(temp);
  }
}

int main (int argc, char* argv[]) {

  if (argc < 8) {
    std::cerr<<"usage: "<<argv[0]<<" <functionid> <a> <b> <n> <intensity> <nbthreads> <sync>"<<std::endl;
    return -1;
  }
  
  //nbthreads : an integer indicating the number of threads that should perform perform
  //            the numerical integration.

  //sync      : a string, either "iteration" or "thread" that indicates which
  //            synchronization method to use.

  strcpy(sync, argv[7]);
  nbthreads = atoi(argv[6]);
  intensity = atoi(argv[5]);
  choice = atoi(argv[1]);
  a = atof(argv[2]);
  b = atof(argv[3]);
  n = atof(argv[4]);
  x = (b-a)/n;
  result = 0;

  int equalPart = round(n/nbthreads); //Equal parts to calculate.
  int temp = 0;
  pthread_t threads[nbthreads];
  values valueStruct[nbthreads];

  std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();

  if(strcmp(sync, "iteration") == 0){
    //Initialize the start and end points of each thread.
    for(int i = 0; i < nbthreads; i++){
      valueStruct[i].start = temp;
      temp += equalPart-1;
      if(i == nbthreads - 1){
        temp = n;
      }
      valueStruct[i].end = temp;
      temp++;
    }
    for(int i = 0; i < nbthreads; i++){
      //Create the threads.
      pthread_create(&threads[i], NULL, iterationFunc, (void *)&valueStruct[i]);
    }
    for(int i = 0; i < nbthreads; i++){
      //Wait for threads to finish.
      pthread_join(threads[i], NULL);
    }
  } else if(strcmp(sync, "thread") == 0){
    for(int i = 0; i < nbthreads; i++){
      valueStruct[i].start = temp;
      temp += equalPart-1;
      if(i == nbthreads - 1){
        temp = n;
      }
      valueStruct[i].end = temp;
      temp++;
    }
    for(int i = 0; i < nbthreads; i++){
      pthread_create(&threads[i], NULL, threadFunc, (void *)&valueStruct[i]);
    }
    for(int i = 0; i < nbthreads; i++){
      pthread_join(threads[i], NULL);
    }
  }
  
  std::chrono::time_point<std::chrono::system_clock> end = std::chrono::system_clock::now();

  std::chrono::duration<double> elapsed_seconds = end-start;
  
  std::cout<<result<<std::endl;
  
  std::cerr<<elapsed_seconds.count()<<std::endl;
  
  return 0;
}
