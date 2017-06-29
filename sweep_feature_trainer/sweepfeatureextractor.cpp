#include "sweepfeatureextractor.h"
#include <stdlib.h>

using namespace std;
using namespace qri_neuron_lib;
/***********************************
 * SweepFeatureClassifier
 * for pattern classifying
 * 20170626
 * song yunpeng
 ***********************************/
SweepFeatureExtractor::SweepFeatureExtractor(KeyAxSelectorClassifier * selector){
  //init selector
  this->key_selector_ = selector;
}

int SweepFeatureExtractor::ExtractFeatures(DataFrame *raw_frame, SampleList &list){
  if(raw_frame==NULL || key_selector_==NULL)return 0;
  if(raw_frame->Columns()<RAW_COLS)return 0;

  //input frame is [delta] processed
  //step1. find the best axis for the input frame
  int cat = key_selector_->Classify(raw_frame);
  if(cat<=0){
    return 0;
  }
  int key_col = key_selector_->CategoryToAxis(cat);

  //step2. no mean process
  const int nomean_size = raw_frame->RowLength();
  float nomean_data[RAW_COLS][nomean_size];
  for(int i=0;i<RAW_COLS;++i){
    feature_nomean_.Process(nomean_data[i],raw_frame->Read(i),nomean_size);
  }

  //step3 extract features from key col
  do{
    //step1. extract threshold for wave finder
    feature_percent_.Process(nomean_data[key_col],nomean_size);
    feature_finder_.threshold_state_.SetThreshold(feature_percent_.Percentile(FeaturePercentile::PERCENTILE_25));

    //step2. extract waves
    FeatureThresholdWaveFinder::wave_index_t wave_list[nomean_size];
    int wave_num = feature_finder_.Process(nomean_data[key_col],nomean_size,wave_list,nomean_size);
    for(int i=0;i<wave_num;++i){
      if(wave_list[i].length<FEATURE_WAVELEN_MIN) continue;

      int wave_len = wave_list[i].length;
      int wave_offset = wave_list[i].start_index;
      //init sampe
      sample_t sample;
      sample.cat=cat;
      sample.feature_len = F_Num;
      sample.frame_offset = wave_offset+raw_frame->offset_;
      sample.frame_len = wave_len*3/2;

      sample.feature[F_Mean] = FeatureMSE::ComputeMean(nomean_data[key_col]+wave_offset,wave_len);
      sample.feature[F_AbsMean] = FeatureMSE::ComputeAbsoluteMean(nomean_data[key_col]+wave_offset,wave_len);
      sample.feature[F_Corssrate] = feature_crossrate_.Process(nomean_data[key_col]+wave_offset,wave_len);
      sample.feature[F_Correlation] = feature_correlation_.Process(nomean_data[0]+wave_offset,nomean_data[1]+wave_offset,wave_len);
      sample.feature[F_Kurtosis] = feature_kurtosis_.Process(nomean_data[key_col]+wave_offset,wave_len,sample.feature[F_Mean]);
      sample.feature[F_Skewness] = feature_skewness_.Process(nomean_data[key_col]+wave_offset,wave_len,sample.feature[F_Mean]);
      sample.feature[F_Energy] = feature_energy_.Process(nomean_data[key_col]+wave_offset,wave_len)/FEATURE_ENERGY_SCALE;
      feature_percent_.Process(nomean_data[key_col]+wave_offset,wave_len);
      sample.feature[F_25th] = feature_percent_.Percentile(FeaturePercentile::PERCENTILE_25);
      sample.feature[F_50th] = feature_percent_.Percentile(FeaturePercentile::PERCENTILE_50);
      sample.feature[F_75th] = feature_percent_.Percentile(FeaturePercentile::PERCENTILE_75);

      //push to samplelist
      list.push_back(sample);
    }

  }while(0);
  return list.size();
}
