#include "wavefeatureextractor.h"
#include <stdlib.h>
#include <QDebug>

using namespace std;
using namespace qri_neuron_lib;
/***********************************
 * SweepFeatureClassifier
 * for pattern classifying
 * 20170626
 * song yunpeng
 ***********************************/
WaveFeatureExtractor::WaveFeatureExtractor(QObject* parent):QObject(parent){
}

int WaveFeatureExtractor::ExtractFeatures(DataFrame *raw_frame,SampleList &list)
{
  if(raw_frame==NULL || raw_frame->Columns()<RAW_COLS)return 0;

  int key_col=0;
  int ref_col=1;
  float c_k = feature_mse_.Process(raw_frame->ReadColumnData(key_col),raw_frame->RowLength(),raw_frame->Mean(key_col));
  float c_r = feature_mse_.Process(raw_frame->ReadColumnData(ref_col),raw_frame->RowLength(),raw_frame->Mean(ref_col));
  if(c_k>c_r){
    key_col = 0;
    ref_col = 1;
  }else{
    key_col = 1;
    ref_col = 0;
  }
#if 0
  qDebug()<<tr("[%1,%2] ck=%3,cr=%4, k_col=%5, r_col=%6")
            .arg(__FILE__).arg(__LINE__)
            .arg(c_k).arg(c_r)
            .arg(key_col).arg(ref_col);
#endif

  //step3 extract features from key col
  do{
    const float * source_key_data = raw_frame->ReadColumnData(key_col);
    const float * source_ref_data = raw_frame->ReadColumnData(ref_col);
    const int source_len = raw_frame->RowLength();
    //step1. extract threshold for wave finder
    feature_percent_.Process(source_key_data,source_len);
    feature_finder_.threshold_state_.SetThreshold(feature_percent_.Percentile(FeaturePercentile::PERCENTILE_30));

    //step2. extract waves
    FeatureThresholdWaveFinder::wave_index_t wave_list[source_len];
    int wave_num = feature_finder_.Process(source_key_data,source_len,wave_list,source_len);
    for(int i=0;i<wave_num;++i){
      if(wave_list[i].length<FEATURE_WAVELEN_MIN) continue;

      int wave_len = wave_list[i].length;
      int wave_offset = wave_list[i].start_index;
      //init sampe
      sample_t sample;
      sample.cat=raw_frame->category_;
      sample.feature_len = F_Num;
      sample.frame_offset = wave_offset+raw_frame->offset_;
      sample.frame_len = wave_len*3/2;

      const float * key_col_data = source_key_data+wave_offset;
      const float * ref_col_data = source_ref_data+wave_offset;
      sample.feature[F_Mean] = FeatureMSE::ComputeMean(key_col_data,wave_len);
      sample.feature[F_AbsMean] = FeatureMSE::ComputeAbsoluteMean(key_col_data,wave_len);
      sample.feature[F_Corssrate] = feature_crossrate_.Process(key_col_data,wave_len)*FEATURE_SCALE_CROSSRATE;
      sample.feature[F_Correlation] = feature_correlation_.Process(key_col_data,ref_col_data,wave_len)*FEATURE_SCALE_CORRELATION;
      sample.feature[F_Kurtosis] = feature_kurtosis_.Process(key_col_data,wave_len,sample.feature[F_Mean])*FEATURE_SCALE_KUR;
      sample.feature[F_Skewness] = feature_skewness_.Process(key_col_data,wave_len,sample.feature[F_Mean])*FEATURE_SCALE_SKEW;
      sample.feature[F_Energy] = feature_energy_.Process(key_col_data,wave_len)*FEATURE_SCALE_ENERGY;
      sample.feature[F_CEnergy] = feature_c_energy_.Process(key_col_data,ref_col_data,wave_len)*FEATURE_SCALE_ENERGY;
      feature_percent_.Process(key_col_data,wave_len);
      sample.feature[F_25th] = feature_percent_.Percentile(FeaturePercentile::PERCENTILE_25);
      sample.feature[F_50th] = feature_percent_.Percentile(FeaturePercentile::PERCENTILE_50);
      sample.feature[F_75th] = feature_percent_.Percentile(FeaturePercentile::PERCENTILE_75);

      //push to samplelist
      list.push_back(sample);
    }

  }while(0);
  return list.size();
}

