#include "keyaxselectorclassifier.h"
#include <stdio.h>
using namespace std;
using namespace qri_neuron_lib;

KeyAxSelectorClassifier::KeyAxSelectorClassifier(NeuronEngineFloat* engine){
  this->engine_ = engine;
  raw_frame_ = new DataFrame(RAW_ROWS,RAW_COLS);
}
int KeyAxSelectorClassifier::PushToClassify(float row_data[], int row_len){
  float last_raw=0.0f;
  if(row_data==NULL ||row_len<RAW_COLS)return 0;

  //step1. check delta
  int ret=-1;
  float gap = FeaturePeaks::ComputeGap(row_data[FEATURE_GAP_AXIS],last_raw);
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

int KeyAxSelectorClassifier::Classify(DataFrame *raw_frame){
  if(raw_frame==NULL || raw_frame->Columns()<RAW_COLS)return 0;
  if(engine_==NULL) return 0;

  //step1.extract features
  float feature[FEATURE_MAXNUM];
  int feature_len = ExtractFeatures(raw_frame,feature,FEATURE_MAXNUM);
  return engine_->Classify(feature,feature_len);
}

int KeyAxFeatureExtractor::ExtractFeatures(qri_neuron_lib::DataFrame * raw_frame, float feature[],int feature_size){
  if(raw_frame==NULL || raw_frame->Columns()<RAW_COLS )return 0;
  if(feature==NULL|| feature_size<=0)return 0;

  int feature_len = FeatureNum*RAW_COLS+1;
  for(int i=0;i<RAW_COLS;++i){
    int feature_start_index=i*FeatureNum;
    feature[feature_start_index+FeatureMean] = FeatureMSE::ComputeMean(raw_frame->Read(i),raw_frame->RowLength());
    feature[feature_start_index+FeatureEnergy] = feature_energy_.Process(raw_frame->Read(i),raw_frame->RowLength())/FEATURE_ENERGY_SCALE;
    feature_percent_.Process(raw_frame->Read(i),raw_frame->RowLength());
    feature[feature_start_index+Feature25th] = feature_percent_.Percentile(FeaturePercentile::PERCENTILE_25);
    feature[feature_start_index+Feature50th] = feature_percent_.Percentile(FeaturePercentile::PERCENTILE_50);
    feature[feature_start_index+Feature75th] = feature_percent_.Percentile(FeaturePercentile::PERCENTILE_75);
  }
  feature[RAW_COLS*FeatureNum] = feature_correlation_.Process(raw_frame->Read(0),
                                                              raw_frame->Read(1),
                                                              raw_frame->RowLength(),
                                                              raw_frame->Mean(0),
                                                              raw_frame->Mean(1));
  return feature_len;
}

int KeyAxSelectorClassifier::CategoryToAxis(int cat){
  int ret=0;
  switch(cat){
  case AxisX:
      ret=0;
      break;
  case AxisY:
      ret=1;
      break;
  }
  return ret;
}
