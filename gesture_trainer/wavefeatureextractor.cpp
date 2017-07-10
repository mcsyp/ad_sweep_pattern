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
      sample.feature[F_Corssrate] = feature_crossrate_.Process(key_col_data,wave_len);
      sample.feature[F_Correlation] = feature_correlation_.Process(key_col_data,ref_col_data,wave_len);
      sample.feature[F_Kurtosis] = feature_kurtosis_.Process(key_col_data,wave_len,sample.feature[F_Mean]);
      sample.feature[F_Skewness] = feature_skewness_.Process(key_col_data,wave_len,sample.feature[F_Mean]);
      sample.feature[F_Energy] = feature_energy_.Process(key_col_data,wave_len)/FEATURE_ENERGY_SCALE;
      sample.feature[F_CEnergy] = feature_c_energy_.Process(key_col_data,ref_col_data,wave_len)/FEATURE_ENERGY_SCALE;
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

int WaveFeatureExtractor::NormalizeSampleFeatures(const WaveFeatureExtractor::SampleList &src_list, WaveFeatureExtractor::SampleList &out_list)
{
  if(src_list.size()<=1)return 0;
  const int feature_size = src_list.size();
  DataFrame * ptr_frame = new DataFrame(feature_size,F_Num);

  //step1. duplicate all feature data to ptr_frame
  for(auto iter = src_list.begin();iter!=src_list.end();++iter){
    sample_t src_sample = *iter;

    //step1. copy the dst sample
    sample_t dst_sample;
    CopySample(src_sample,dst_sample);
    out_list.push_back(dst_sample);
    //step2. save to the dst frame
    ptr_frame->Push(src_sample.feature,src_sample.feature_len);
  }

  //step2. scale all rows
  for(int i=0;i<ptr_frame->Columns();++i){
    float min, max;
    const float * col_data = ptr_frame->ReadColumnData(i);
    int col_data_len = ptr_frame->RowLength();
    feature_minmax_.Process(col_data,col_data_len);
    feature_minmax_.MinMaxValue(&min,&max);

    if(min==max)continue;
    for(int k=0;k<col_data_len;++k){
      out_list[k].feature[i] = (src_list[k].feature[i]-min)*FEATURE_SCALE_RANGE/(max-min);
    }
  }

  //final step
  delete ptr_frame;
  return out_list.size();
}

void WaveFeatureExtractor::CopySample(const WaveFeatureExtractor::sample_t &src_sample, WaveFeatureExtractor::sample_t &dst_sample){
  dst_sample.cat = src_sample.cat;
  dst_sample.feature_len = src_sample.feature_len;
  dst_sample.frame_len = src_sample.frame_len;
  dst_sample.frame_offset = src_sample.frame_offset;
  memcpy((uint8_t*)dst_sample.feature,(uint8_t*)src_sample.feature,src_sample.feature_len*sizeof(float));
}
