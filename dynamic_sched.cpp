#include <iostream>
#include <cmath>
#include <cstdlib>
#include <chrono>
#include <pthread.h>
#include <math.h>
#include <string.h>
#include <tuple>

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
int sentinel;
int nbthreads;
int granularity;
int intensity;
int choice;
int nextPair;
int parts;
float a;
float b;
float n;
float x;
float result;

//Used to get the next set of bounds
std::tuple<int, int> getNext(){
  pthread_mutex_lock;
  if(nextPair == parts){
    sentinel = 0;
    return std::make_tuple(0, 0);
  }
  int start = granularity * nextPair;
  int end = start + granularity - 1;
  if(n - granularity < 0){
    end = n;
  }
  nextPair++;
  return std::make_tuple(start, end);
  pthread_mutex_unlock;
}

//Thread called by each thread to increment the result. Is protected with a mutex.
void incrementResult(float x){
  pthread_mutex_lock;
  result += x;
  pthread_mutex_unlock;
}

//This function is called when "iteration" is the argument.
void *iterationFunc(void* none) {
  while(sentinel){
    std::tuple<int, int> data = getNext();
    int start = std::get<0>(data);
    int end = std::get<1>(data);
    if(choice == 1){
      for(int i = start; i < end + 1; i++){
        incrementResult(f1(a+((i+.5)*x), intensity) * x); //Increment each time a portion is calculated.
      }
    }
    else if(choice == 2){
      for(int i = start; i < end + 1; i++){
        incrementResult(f2(a+((i+.5)*x), intensity) * x);
      }
    }
    else if(choice == 3){
      for(int i = start; i < end + 1; i++){
        incrementResult(f3(a+((i+.5)*x), intensity) * x);
      }
    }
    else{
      for(int i = start; i < end + 1; i++){
        incrementResult(f4(a+((i+.5)*x), intensity) * x);
      }
    }
  }
}

//This function is called when "thread" is the argument.
void *threadFunc(void* none) {
  float temp = 0;
  while(sentinel){
    std::tuple<int, int> data = getNext();
    int start = std::get<0>(data);
    int end = std::get<1>(data);
    if(choice == 1){
      for(int i = start; i < end + 1; i++){
        temp += f1(a+((i+.5)*x), intensity) * x; //Increment each time a portion is calculated.
      }
    }
    else if(choice == 2){
      for(int i = start; i < end + 1; i++){
        temp += f2(a+((i+.5)*x), intensity) * x;
      }
    }
    else if(choice == 3){
      for(int i = start; i < end + 1; i++){
        temp += f3(a+((i+.5)*x), intensity) * x;
      }
    }
    else{
      for(int i = start; i < end + 1; i++){
        temp += f4(a+((i+.5)*x), intensity) * x;
      }
    }
  }
  incrementResult(temp);
}

void *chunkFunc(void* none) {
  while(sentinel){
    std::tuple<int, int> data = getNext();
    int start = std::get<0>(data);
    int end = std::get<1>(data);
    float temp = 0;
    if(choice == 1){
      for(int i = start; i < end + 1; i++){
        temp += f1(a+((i+.5)*x), intensity) * x; //Increment each time a portion is calculated.
      }
      incrementResult(temp);
    }
    else if(choice == 2){
      for(int i = start; i < end + 1; i++){
        temp += f2(a+((i+.5)*x), intensity) * x;
      }
      incrementResult(temp);
    }
    else if(choice == 3){
      for(int i = start; i < end + 1; i++){
        temp += f3(a+((i+.5)*x), intensity) * x;
      }
      incrementResult(temp);
    }
    else{
      for(int i = start; i < end + 1; i++){
        temp += f4(a+((i+.5)*x), intensity) * x;
      }
      incrementResult(temp);
    }
  }
}

  
int main (int argc, char* argv[]) {

  if (argc < 9) {
    std::cerr<<"usage: "<<argv[0]<<" <functionid> <a> <b> <n> <intensity> <nbthreads> <sync> <granularity>"<<std::endl;
    return -1;
  }

  strcpy(sync, argv[7]);
  nbthreads = atoi(argv[6]);
  intensity = atoi(argv[5]);
  choice = atoi(argv[1]);
  granularity = atoi(argv[8]);
  a = atof(argv[2]);
  b = atof(argv[3]);
  n = atof(argv[4]);
  x = (b-a)/n;
  result = 0;
  nextPair = 0;
  sentinel = 1;

  int temp = 0;
  parts = ceil(n/granularity); //Equal parts to calculate.
  pthread_t threads[nbthreads];

  std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();
  
  if(strcmp(sync, "iteration") == 0){
    for(int i = 0; i < nbthreads; i++){
      //Create the threads.
      pthread_create(&threads[i], NULL, iterationFunc, NULL);
    }
    for(int i = 0; i < nbthreads; i++){
      //Wait for threads to finish.
      pthread_join(threads[i], NULL);
    }
  } else if(strcmp(sync, "thread") == 0){
    for(int i = 0; i < nbthreads; i++){
      pthread_create(&threads[i], NULL, threadFunc, NULL);
    }
    for(int i = 0; i < nbthreads; i++){
      pthread_join(threads[i], NULL);
    } 
  } else if(strcmp(sync, "chunk") == 0){
    for(int i = 0; i < nbthreads; i++){
      pthread_create(&threads[i], NULL, chunkFunc, NULL);
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
