#include "sweepfeaturetrainer.h"
#include <stdlib.h>

using namespace qri_neuron_lib;
/***********************************
 * SweepFeaturePipelineTrainer
 * for data training
 * 20170626
 * song yunpeng
 ***********************************/
SweepFeatureTrainer::SweepFeatureTrainer(KeyAxSelectorClassifier *selector):SweepFeatureExtractor(selector){
  //init raw frame
  raw_frame_=new DataFrame(RAW_ROWS,RAW_COLS);
  raw_frame_->category_=0;
}

void SweepFeatureTrainer::Reset(){
  sample_list_.clear();
  raw_frame_->Clear();
}

bool SweepFeatureTrainer::PushToSample(int cat, const float *row_data, int row_len){
  static float last_data=0.0f;
  if(cat<=0 || row_data==NULL || row_len<RAW_COLS) return false;

  //step1. check the category
  if(cat!=raw_frame_->category_)
  {//if there is a new cat data enters, means the old data is useless, clean and reset the frame
    //step1. process the features
    if(!raw_frame_->Empty()){
      ExtractFeatures(raw_frame_,sample_list_);
    }
    //step2. update cat info and reset frame
    raw_frame_->category_ = cat;
    raw_frame_->Clear();
    last_data=0.0f;//reset the last_data
  }

  //delta check
  float gap = FeaturePeaks::ComputeGap(row_data[FEATURE_GAP_AXIS],last_data);
  if(gap<FEATURE_GAP_MIN || gap>FEATURE_GAP_MAX){
    return false;
  }
  last_data =  row_data[FEATURE_GAP_AXIS];//if passed save the data

  //step2. save the input data
  raw_frame_->Push(row_data,RAW_COLS);
  if(raw_frame_->Full()){
    //if the frame is full , extract features from it and push to sample list
     ExtractFeatures(raw_frame_,sample_list_);

     //pop the data tail
     raw_frame_->Pop(RAW_DELTA);//pop raw data for next process
  }

  return true;
}
int SweepFeatureTrainer::StartTraining(NeuronEngineFloat &engine, int iterations){
  if(iterations<=0)return 0;

  //step1. reset engine
  engine.ResetEngine();

  //step2. start iterations
  for(int iter=0;iter<iterations;++iter){
    //step1. gerenate rand array
    const int rand_size = sample_list_.size();
    int rand_array[rand_size];
    GenerateRandomIndex(rand_array,rand_size);

    //step2.train the frame
    for(int i=0;i<rand_size;++i){
      int index = rand_array[i];
      sample_t sample = sample_list_.at(index);

      //train the vector
      engine.Learn(sample.cat,sample.feature,sample.feature_len);
    }
  }

  return engine.NeuronCount();
}

void SweepFeatureTrainer::GenerateRandomIndex(int array[], int len){
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


