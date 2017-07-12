#include "waveclassifier.h"
#include <stdlib.h>

using namespace std;
using namespace qri_neuron_lib;
/***********************************
 * SweepFeatureClassifier
 * for pattern classifying
 * 20170626
 * song yunpeng
 ***********************************/
WaveClassifier::WaveClassifier(NeuronEngineFloat &engine,QObject* parent):WaveFeatureExtractor(parent) {
  //init raw frame
  raw_frame_= new DataFrame(RAW_ROWS,RAW_COLS);
  this->ptr_engine_ = &engine;
}

WaveClassifier::~WaveClassifier(){
  if(raw_frame_) delete raw_frame_;
  sample_list_.clear();
}

bool WaveClassifier::PushToClassify(float row_data[], int row_len, ResultList& result_list){
  static float last_raw=0.0f;
  if(row_data==NULL ||row_len<RAW_COLS)return false;

  //step1. check delta
  float gap = fabs(row_data[FEATURE_GAP_AXIS]-last_raw);
  if(gap<FEATURE_GAP_MIN || gap>FEATURE_GAP_MAX){
    return false;
  }
  last_raw = row_data[FEATURE_GAP_AXIS];

  //step2.save the data in the frame
  raw_frame_->Push(row_data,RAW_COLS);
  if(raw_frame_->Full()){
    Classify(raw_frame_,result_list);
    raw_frame_->Pop(RAW_DELTA);
    return true;
  }
  return false;
}

bool WaveClassifier::Classify(DataFrame * raw_frame,ResultList& result_list){
  if(raw_frame==NULL || raw_frame->Columns()<RAW_COLS)return false;
  if(ptr_engine_==NULL) return false;

  //step1.extract features
  sample_list_.clear();
  ExtractFeatures(raw_frame,sample_list_);

  for(int i=0;i<sample_list_.size();++i){
    sample_t  sample = sample_list_.at(i);
    int cat = ptr_engine_->Classify(sample.feature,sample.feature_len);
    if(cat>0){
      result_cat_t r;
      r.cat = cat;
      r.frame_len = sample.frame_len;
      r.frame_offset = sample.frame_offset;
      result_list.push_back(r);
    }
  }
  return (result_list.size()>0);
}
