#include "sweepfeatureclassifier.h"
#include <stdlib.h>

using namespace std;
using namespace qri_neuron_lib;
/***********************************
 * SweepFeatureClassifier
 * for pattern classifying
 * 20170626
 * song yunpeng
 ***********************************/
SweepFeatureClassifier::SweepFeatureClassifier(NeuronEngineFloat *engine,KeyAxSelectorClassifier* key):SweepFeatureExtractor(key) {
  //init raw frame
  raw_frame_= new DataFrame(RAW_ROWS,RAW_COLS);
  this->engine_feature_ = engine;
}

SweepFeatureClassifier::~SweepFeatureClassifier(){
  if(raw_frame_) delete raw_frame_;
  sample_list_.clear();
}

int SweepFeatureClassifier::PushToClassify(float row_data[], int row_len, result_cat_t result[], int result_size){
  float last_raw=0.0f;
  if(row_data==NULL ||row_len<RAW_COLS)return -1;
  if(result==NULL || result_size<=0)return -1;

  //step1. check delta
  float gap = fabs(row_data[FEATURE_GAP_AXIS]-last_raw);
  if(gap<FEATURE_GAP_MIN || gap>FEATURE_GAP_MAX){
    return -1;
  }

  //step2.save the data in the frame
  raw_frame_->Push(row_data,RAW_COLS);
  if(raw_frame_->Full()){
    int ret = Classify(raw_frame_,result,result_size);
    raw_frame_->Pop(RAW_DELTA);
    return ret;
  }
  return 0;
}

int SweepFeatureClassifier::Classify(DataFrame * raw_frame,result_cat_t result[],int result_size){
  if(raw_frame==NULL || raw_frame->Columns()<RAW_COLS)return 0;
  if(engine_feature_==NULL) return 0;

  //step1.extract features
  sample_list_.clear();
  ExtractFeatures(raw_frame,sample_list_);

  if(result==NULL || result_size<=0)return 0;

  int result_len=0;
  for(int i=0;i<sample_list_.size();++i){
    sample_t  sample = sample_list_.at(i);
    int cat = engine_feature_->Classify(sample.feature,sample.feature_len);
    if(cat>0){
      result[result_len].cat = cat;
      result[result_len].frame_len = sample.frame_len;
      result[result_len].frame_offset = sample.frame_offset;
      ++result_len;
    }
  }
  return result_len;
}
