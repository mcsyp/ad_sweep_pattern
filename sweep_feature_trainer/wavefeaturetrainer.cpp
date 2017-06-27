#include "wavefeaturetrainer.h"
#include <stdlib.h>
using namespace std;
using namespace qri_neuron_lib;
int WaveFeatureTrainer::Train(NeuronEngineFloat& engine,const DataFrameList& framelist){
  if(framelist.empty()) return 0;

  engine.ResetEngine();

  for(int iter=0;iter<DEFUALT_INTERATIONS;++iter){
    //step1. gerenate rand array
    const int rand_size = framelist.size();
    int rand_array[rand_size];
    GenerateRandomIndex(rand_array,rand_size);

    //step2.train the frame
    for(int i=0;i<rand_size;++i){
      int index = rand_array[i];
      DataFrame* df =  framelist.at(index);
      if(!df) continue;

      //train the vector
      engine.Learn(df->category_,df->Reference(),df->FrameSize());
    }
  }
  return engine.NeuronCount();
}

void WaveFeatureTrainer::GenerateRandomIndex(int array[], int len){
  static unsigned long rand_counter=0;


  for(int i=0;i<len;i++){
      array[i]=i;
  }
  for(int i=len-1;i>0;i--){
      srand(rand_counter+time(NULL));
      rand_counter+=1;
      int index=rand()%(i+1);
      if(index!=i){
          uint8_t tmp = array[i];
          array[i] = array[index];
          array[index] = tmp;
      }
  }
}
