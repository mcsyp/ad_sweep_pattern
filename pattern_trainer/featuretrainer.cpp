#include "featuretrainer.h"
#include "qrineuronlib.h"
#include <QDebug>

using namespace std;
using namespace feature_engine;
using namespace qri_neuron_lib;
FeatureTrainer::FeatureTrainer(const param_window_t &window, const param_gap_filter_t &gap,FeatureExtractor * extractor) :
  FeatureBasic(window,gap,extractor)
{
}

bool FeatureTrainer::PushToSample(int cat,float row_data[],int row_len){
  if(cat<=0 || row_data==NULL || row_len<this->window_.cols) return false;
  if(ptr_raw_frame_==NULL)return false;
  if(this->ptr_extractor_==NULL) return false;

  //step1. check the category
  if(cat!=ptr_raw_frame_->category_)
  {//if there is a new cat data enters, means the old data is useless, clean and reset the frame
    //step1. process the features
    if(!ptr_raw_frame_->Empty()){
      ptr_extractor_->ExtractFeatures(*ptr_raw_frame_,sample_list_);
    }
    //step2. update cat info and reset frame
    ptr_raw_frame_->category_ = cat;
    ptr_raw_frame_->Clear();
  }

  //delta check
  if(!this->CheckDataAccpeted(row_data,row_len)){
    return false;
  }

  //step2. save the input data
  ptr_raw_frame_->Push(row_data,window_.cols);
  if(ptr_raw_frame_->Full()){
     ptr_raw_frame_->category_ = cat;
    //if the frame is full , extract features from it and push to sample list
     ptr_extractor_->ExtractFeatures(*ptr_raw_frame_,sample_list_);
     //pop the data tail
     ptr_raw_frame_->Pop(window_.delta);//pop raw data for next process
  }
  return true;
}
int FeatureTrainer::StartTraining(NeuronEngineFloat &engine, int iterations){
  if(iterations<=0)return 0;

  //step1. reset engine
  engine.ResetEngine();


  //step3. start iterations
  for(int iter=0;iter<iterations;++iter){
    //step1. gerenate rand array
    const int rand_size = sample_list_.size();
    int rand_array[rand_size];
    GenerateRandomIndex(rand_array,rand_size);

    //step2.train the frame
    for(int i=0;i<rand_size;++i){
      int index = rand_array[i];
      FeatureExtractor::feature_sample_t sample = sample_list_.at(index);

      const int feature_size = sample.features.size();
      float feature_array[feature_size];
      int feature_len = FeatureExtractor::toArray(sample.features,feature_array,feature_size);
      //train the vector
      engine.Learn(sample.cat,feature_array,feature_len);
    }
  }

  return engine.NeuronCount();
}
void FeatureTrainer::GenerateRandomIndex(int array[], int len){
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
void FeatureTrainer::Reset(){
  sample_list_.clear();
  if(ptr_raw_frame_) ptr_raw_frame_->Clear();
}



