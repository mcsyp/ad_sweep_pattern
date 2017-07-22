#include "framefeatureextractor.h"
#include <stdio.h>
#include <math.h>
#include <qdebug.h>
using namespace std;
using namespace qri_neuron_lib;

FrameFeatureExtractor::FrameFeatureExtractor(const param_wave_t & wave_filter):FeatureExtractor(NULL){
  this->wave_filter_ = wave_filter;
}

void FrameFeatureExtractor::ExtractFeatures(const DataFrame &raw_frame, feature_engine::FeatureExtractor::FeatureList &out_features)
{
  if(raw_frame.Empty())return;

  //step0.find key axis
  int key_col=FindKeyAxis(raw_frame);

  //step1. extract a max wave len
  int wave_start_offset=0;
  int wave_max_len=raw_frame.RowLength();

  QVector<FeatureThresholdWaveFinder::wave_index_t> valid_peaks;
  do{
    feature_percent_.Process(raw_frame.ReadColumnData(key_col),raw_frame.RowLength());
    feature_finder_.threshold_state_.SetThreshold(feature_percent_.Percentile(FeaturePercentile::PERCENTILE_30));
    FeatureThresholdWaveFinder::wave_index_t wave_list[raw_frame.RowLength()];
    int wave_num = feature_finder_.Process(raw_frame.ReadColumnData(key_col),raw_frame.RowLength(),wave_list,raw_frame.RowLength());

    //find out valid peaks
    for(int i=0;i<wave_num;++i){
      if(wave_list[i].length>=this->wave_filter_.min_wave_len && wave_list[i].length<=this->wave_filter_.max_wave_len){
        valid_peaks.push_back(wave_list[i]);
      }
    }
    if(valid_peaks.size()){
      //the peak length
      wave_start_offset = valid_peaks.first().start_index;
      wave_max_len = valid_peaks.last().end_index-wave_start_offset;
    }else{
      return;
    }
  }while(0);

  if(wave_max_len<=0)return;

  FeatureExtractor::feature_sample_t out_sample;
  for(int i=0;i<RAW_COLS;++i){
    //float nomean_buffer[raw_frame.RowLength()];
    //feature_nomean_.Process(nomean_buffer,raw_frame.ReadColumnData(i),raw_frame.RowLength());
    //const float* wave_data = nomean_buffer+wave_start_offset;
    const float* wave_data = raw_frame.ReadColumnData(i)+wave_start_offset;

    out_sample.features.push_back(FeatureMSE::ComputeMean(wave_data,wave_max_len));
    out_sample.features.push_back(FeatureMSE::ComputeMean(wave_data,wave_max_len));
    out_sample.features.push_back(FeatureMSE::ComputeAbsoluteMean(wave_data,wave_max_len));
    out_sample.features.push_back(feature_energy_.Process(wave_data,wave_max_len)*FEATURE_SCALE_ENERGY);
    out_sample.features.push_back(feature_crossrate_.Process(wave_data,wave_max_len)*FEATURE_SCALE_CROSSRATE);
    feature_percent_.Process(wave_data,wave_max_len);
    out_sample.features.push_back(feature_kurtosis_.Process(wave_data,wave_max_len)*FEATURE_SCALE_KUR);
    out_sample.features.push_back(feature_skewness_.Process(wave_data,wave_max_len)*FEATURE_SCALE_SKEW);
    out_sample.features.push_back(feature_percent_.Percentile(FeaturePercentile::PERCENTILE_25));
    out_sample.features.push_back(feature_percent_.Percentile(FeaturePercentile::PERCENTILE_50));
    out_sample.features.push_back(feature_percent_.Percentile(FeaturePercentile::PERCENTILE_75));
  }
  out_sample.cat = raw_frame.category_;
  out_sample.samples = wave_max_len;//save the sample frame len
  out_sample.features.push_back(feature_correlation_.Process(raw_frame.ReadColumnData(0)+wave_start_offset,
                                                             raw_frame.ReadColumnData(1)+wave_start_offset,
                                                             wave_max_len)*FEATURE_SCALE_CORRELATION);
  float c_energy =feature_c_energy_.Process(raw_frame.ReadColumnData(0)+wave_start_offset,
                                            raw_frame.ReadColumnData(1)+wave_start_offset,
                                            wave_max_len)*FEATURE_SCALE_ENERGY;
  out_sample.features.push_back(c_energy);
  out_sample.peaks = valid_peaks.size();

  out_features.push_back(out_sample);//save sample to the sample list
}

int FrameFeatureExtractor::FindKeyAxis(const DataFrame &frame)
{
  if(frame.Empty())return 0;
  float cx = feature_mse_.Process(frame.ReadColumnData(0),frame.RowLength(),frame.Mean(0));
  float cy = feature_mse_.Process(frame.ReadColumnData(1),frame.RowLength(),frame.Mean(1));
  int key_col = 0;
  if(cy>cx){
    key_col = 1;
  }
  return key_col;
}

