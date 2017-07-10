#include "framefeatureextractor.h"
#include <stdio.h>
#include <math.h>
using namespace std;
using namespace qri_neuron_lib;

FrameFeatureExtractor::FrameFeatureExtractor(QObject *parent):QObject(parent){

}

int FrameFeatureExtractor::ExtractFeatures(qri_neuron_lib::DataFrame * raw_frame,
                                           int key_col,
                                           float feature[],
                                           int feature_size)
{
  if(raw_frame==NULL || raw_frame->Columns()<RAW_COLS )return 0;
  if(feature==NULL|| feature_size<=0)return 0;

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
    feature[feature_start_index+FeatureMean] = FeatureMSE::ComputeMean(wave_data,wave_max_len);
    feature[feature_start_index+FeatureAbsMean] = FeatureMSE::ComputeAbsoluteMean(wave_data,wave_max_len);
    feature[feature_start_index+FeatureEnergy] = feature_energy_.Process(wave_data,wave_max_len)*FEATURE_SCALE_ENERGY;
    feature[feature_start_index+FeatureCrossRate] = feature_crossrate_.Process(wave_data,wave_max_len)*FEATURE_SCALE_CROSSRATE;
    feature_percent_.Process(wave_data,wave_max_len);
    feature[feature_start_index+Feature25th] = feature_percent_.Percentile(FeaturePercentile::PERCENTILE_25);
    feature[feature_start_index+Feature50th] = feature_percent_.Percentile(FeaturePercentile::PERCENTILE_50);
    feature[feature_start_index+Feature75th] = feature_percent_.Percentile(FeaturePercentile::PERCENTILE_75);
  }
  feature[RAW_COLS*FeatureNum] = feature_correlation_.Process(raw_frame->ReadColumnData(0),
                                                              raw_frame->ReadColumnData(1),
                                                              raw_frame->RowLength(),
                                                              raw_frame->Mean(0),
                                                              raw_frame->Mean(1))*FEATURE_SCALE_CORRELATION;
  return feature_len;
}
