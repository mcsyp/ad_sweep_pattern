#include "frameclassifier.h"
#include <stdio.h>
#include <math.h>
using namespace std;
using namespace qri_neuron_lib;

FrameClassifier::FrameClassifier(NeuronEngineFloat &engine, QObject *parent):FrameFeatureExtractor(parent){
  raw_frame_ = new DataFrame(RAW_ROWS,RAW_COLS);
  ptr_engine_ = &engine;
}

FrameClassifier::~FrameClassifier(){
  if(raw_frame_)delete raw_frame_;
}

int FrameClassifier::PushToClassify(float row_data[], int row_len){
  float last_raw=0.0f;
  if(row_data==NULL ||row_len<RAW_COLS)return 0;

  //step1. check delta
  int ret=-1;
  float gap = fabs(row_data[FEATURE_GAP_AXIS]-last_raw);
  if(gap<FEATURE_GAP_MIN || gap>FEATURE_GAP_MAX){
    return -1;
  }

  //step2.save the data in the frame
  raw_frame_->Push(row_data,RAW_COLS);
  if(raw_frame_->Full()){
    ret = Classify(raw_frame_);
    raw_frame_->Pop(RAW_DELTA);
  }
  return ret;
}
int FrameClassifier::Classify(DataFrame *raw_frame){
  if(raw_frame==NULL || raw_frame->Columns()<RAW_COLS)return 0;
  if(ptr_engine_==NULL) return 0;

  //step1.extract features
  float feature[FEATURE_MAXNUM];
  const int nid_size = ptr_engine_->NeuronCount();
  int nid_array[nid_size];
  int min_nid=0;

  int feature_len = ExtractFeatures(raw_frame,1,feature,FEATURE_MAXNUM);
  int nid_len = ptr_engine_->Classify(feature,feature_len,nid_array,nid_size,&min_nid);

  //trigger the signal for more details
  emit resultReady(nid_array,nid_len,min_nid);

  return ptr_engine_->ReadNeuron(min_nid)->Cat();
}

