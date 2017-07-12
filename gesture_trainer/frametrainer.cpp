#include "frametrainer.h"

using namespace std;
using namespace qri_neuron_lib;

FrameTrainer::FrameTrainer(QObject* parent):FrameFeatureExtractor(parent){
  raw_frame_ = new DataFrame(RAW_ROWS,RAW_COLS);
  raw_frame_->category_=0;
}
FrameTrainer::~FrameTrainer(){
  if(raw_frame_)delete raw_frame_;
}

int FrameTrainer::PushToSample(int cat,float row_data[],int row_len){
  float last_raw=0.0f;
  if(cat<=0 || row_data==NULL || row_len<RAW_COLS) return 0;

  //step1. check the category
  if(cat!=raw_frame_->category_ )
  {//if there is a new cat data enters, means the old data is useless, clean and reset the frame
    //step1. process the features
    if(!raw_frame_->Empty()){
      sample_t my_sample;
      ExtractFeatures(raw_frame_,my_sample);
      PushFeaturesToSample(raw_frame_->category_,my_sample.feature,my_sample.feature_len);
    }

    //step2. update cat info and reset frame
    raw_frame_->category_ = cat;
    raw_frame_->Clear();
    last_raw=0;
  }

  //step2. check the delta
  float gap = fabs(row_data[FEATURE_GAP_AXIS]-last_raw);
  if(gap>FEATURE_GAP_MAX || gap<FEATURE_GAP_MIN){
    return 0;//not accepted
  }

  //step2. save the input data
  raw_frame_->Push(row_data,RAW_COLS);
  if(raw_frame_->Full()){
      sample_t my_sample;
    //if the frame is full , extract features from it and push to sample list
      ExtractFeatures(raw_frame_,my_sample);
      PushFeaturesToSample(raw_frame_->category_,my_sample.feature,my_sample.feature_len);

      //pop the data tail
      raw_frame_->Pop(RAW_DELTA);//pop raw data for next process
  }
  return sample_list_.size();
}

int FrameTrainer::StartTraining(NeuronEngineFloat &engine, int iterations){
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
      sample_t sample =  sample_list_.at(index);

      //train the vector
      engine.Learn(sample.cat,sample.feature,sample.feature_len);
    }
  }

  return engine.NeuronCount();
}

int FrameTrainer::PushFeaturesToSample(int cat, const float features[], int feature_len){
  if(cat<=0 || features==NULL || feature_len<=0)return 0;
  sample_t s;
  s.cat = cat;
  s.feature_len = feature_len;
  memcpy((char*)s.feature,(char*)features,sizeof(float)*feature_len);
  sample_list_.push_back(s);
  return sample_list_.size();
}

void FrameTrainer::GenerateRandomIndex(int array[], int len){
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


