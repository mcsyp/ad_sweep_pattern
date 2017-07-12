#include "framefeatureextractor.h"
#include <stdio.h>
#include <math.h>
using namespace std;
using namespace qri_neuron_lib;

FrameFeatureExtractor::FrameFeatureExtractor(QObject *parent):QObject(parent){

}

int FrameFeatureExtractor::ExtractFeatures(qri_neuron_lib::DataFrame * raw_frame,sample_t & out_sample)
{
  if(raw_frame==NULL || raw_frame->Columns()<RAW_COLS )return 0;

  int key_col=FindKeyAxis(raw_frame);

  //step1. extract a max wave len
  int wave_start_offset=0;
  int wave_max_len=raw_frame->RowLength();
  do{
    feature_percent_.Process(raw_frame->ReadColumnData(key_col),raw_frame->RowLength());
    feature_finder_.threshold_state_.SetThreshold(feature_percent_.Percentile(FeaturePercentile::PERCENTILE_25));
    FeatureThresholdWaveFinder::wave_index_t wave_list[raw_frame->RowLength()];
    int wave_num = feature_finder_.Process(raw_frame->ReadColumnData(key_col),raw_frame->RowLength(),wave_list,raw_frame->RowLength());
    if(wave_num>0){
      wave_start_offset = wave_list[0].start_index;
      wave_max_len = wave_list[wave_num-1].end_index-wave_start_offset;
    }
  }while(0);
  if(wave_max_len<=0)return 0;

  int feature_len = FeatureNum*RAW_COLS+1;
  for(int i=0;i<RAW_COLS;++i){
    const float* wave_data = raw_frame->ReadColumnData(i)+wave_start_offset;

    int feature_start_index=i*FeatureNum;
    out_sample.feature[feature_start_index+FeatureMean] = FeatureMSE::ComputeMean(wave_data,wave_max_len);
    out_sample.feature[feature_start_index+FeatureAbsMean] = FeatureMSE::ComputeAbsoluteMean(wave_data,wave_max_len);
    out_sample.feature[feature_start_index+FeatureEnergy] = feature_energy_.Process(wave_data,wave_max_len)*FEATURE_SCALE_ENERGY;
    out_sample.feature[feature_start_index+FeatureCrossRate] = feature_crossrate_.Process(wave_data,wave_max_len)*FEATURE_SCALE_CROSSRATE;
    feature_percent_.Process(wave_data,wave_max_len);
    out_sample.feature[feature_start_index+Feature25th] = feature_percent_.Percentile(FeaturePercentile::PERCENTILE_25);
    out_sample.feature[feature_start_index+Feature50th] = feature_percent_.Percentile(FeaturePercentile::PERCENTILE_50);
    out_sample.feature[feature_start_index+Feature75th] = feature_percent_.Percentile(FeaturePercentile::PERCENTILE_75);
  }
  out_sample.cat = raw_frame->category_;
  out_sample.feature_len = feature_len;
  out_sample.frame_len = wave_max_len;//save the sample frame len
  out_sample.feature[RAW_COLS*FeatureNum] = feature_correlation_.Process(raw_frame->ReadColumnData(0),
                                                                          raw_frame->ReadColumnData(1),
                                                                          raw_frame->RowLength(),
                                                                          raw_frame->Mean(0),
                                                                          raw_frame->Mean(1))*FEATURE_SCALE_CORRELATION;
  return feature_len;
}

int FrameFeatureExtractor::FindKeyAxis(DataFrame *frame)
{
  if(frame==NULL || frame->Columns()<RAW_COLS )return 0;
  float cx = feature_mse_.Process(frame->ReadColumnData(0),frame->RowLength(),frame->Mean(0));
  float cy = feature_mse_.Process(frame->ReadColumnData(1),frame->RowLength(),frame->Mean(1));
  int key_col = 0;
  if(cy>cx){
    key_col = 1;
  }
  return key_col;
}
