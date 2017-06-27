#ifndef SWEEPFEATURECLASSIFIER_H
#define SWEEPFEATURECLASSIFIER_H
#include "qrineuronlib.h"
#include <vector>
class SweepFeatureClassifier{
public:
  typedef std::vector<qri_neuron_lib::DataFrame*> DataFrameList;

  static constexpr int RAW_ROWS=100;
  static constexpr int RAW_COLS=2;
  static constexpr int RAW_COL={0,1};
  enum Feature{
    FeatureMean=0,
    FeautreAbsMean=1,
    FeautreCorssrate=2,
    FeatureCorrelation,
    FeatureKurtosis,
    FeatureSkewness,
    FeatureEnergy,
    Feature25th,
    Feature50th,
    Feature75th,
    FeatureNum
  };
  static constexpr float FEATURE_ENERGY_SCALE=1000.0f;
  static constexpr int SAMPLE_COLS=FeatureNum;
  static constexpr int SAMPLE_ROWS=1;

  typedef struct classified_cat_s{

  }classified_cat_t;

  SweepFeatureClassifier(NeuronEngineFloat* engine);
  virtual ~SweepFeatureClassifier();

  virtual void PushToClassify(float row_data[],int row_len,);

protected:
  void DisposeSampleList();
  inline float ComputeGap(float a, float b);

protected:
  qri_neuron_lib::DataFrame *current_frame_;
  NeuronEngineFloat * engine_;
public:
  DataFrameList smaple_list_;
};


int SweepFeaturePipelineClassifier::PostProcessFrameForFeatures(const DataFrame *dataframe, DataFrameList &sample_list){
  //step1.extract the threshold for wave finder
  feature_percent_.Process(key_data,data_len);
  feature_finder_.threshold_state_.SetThreshold(feature_percent_.Percentile(FeaturePercentile::PERCENTILE_25));

  //step2. find the waves
  FeatureThresholdWaveFinder::wave_index_t wave_list[SAMPLE_ROWS];
  int wave_num = feature_finder_.Process(key_data,data_len,wave_list,SAMPLE_ROWS);
  for(int i=0;i<wave_num;++i){
    if(wave_list[i].length<FEATURE_MIN_WAVELEN)continue;

    int wave_len = wave_list[i].length;
    int wave_offset = wave_list[i].start_index;

  }

}
int SweepFeaturePipelineClassifier::ExtractFeaturesFromWave(const float  wave_key[], const float wave_ref[],int wave_len,
                                                                                           float feature_array[],int feature_size){
  if(wave_key==NULL || wave_ref==NULL || wave_len<=0)return 0;
  if(feature_array==NULL || feature_size<=F_NUM)return 0;

  //step1.extract the threshold for wave finder
  feature_percent_.Process(wave_key,wave_len);
  feature_finder_.threshold_state_.SetThreshold(feature_percent_.Percentile(FeaturePercentile::PERCENTILE_25));

  //step2. find the waves
  FeatureThresholdWaveFinder::wave_index_t wave_list[SAMPLE_ROWS];
  int wave_num = feature_finder_.Process(wave_key,wave_len,wave_list,SAMPLE_ROWS);
  for(int i=0;i<wave_num;++i){
    if(wave_list[i].length<FEATURE_MIN_WAVELEN)continue;

    int wave_len = wave_list[i].length;
    int wave_offset = wave_list[i].start_index;

    feature_array[F_MEAN] = FeatureMSE::ComputeMean(wave_key+wave_offset,wave_len);
    feature_array[F_ABSMEAN] = FeatureMSE::ComputeAbsoluteMean(wave_key+wave_offset,wave_len);
    feature_array[F_CROSSRATE] = feature_rate_.Process(wave_key+wave_offset,wave_len);
    feature_array[F_CORRELATION] = feature_correlation_.Process(wave_key+wave_offset,wave_ref+wave_offset,wave_len);
    feature_array[F_KURTOSIS] = feature_kurtosis_.Process(wave_key+wave_offset,wave_len,feature_array[F_MEAN]);
    feature_array[F_SKEWNESS] = feature_skewness_.Process(wave_key+wave_offset,wave_len,feature_array[F_MEAN]);
    feature_array[F_ENERGY] = feature_energy_.Process(wave_key+wave_offset,wave_len)/ENERGY_SCALE;
    feature_percent_.Process(wave_key+wave_offset,wave_len);
    feature_array[F_25th] = feature_percent_.Percentile(FeaturePercentile::PERCENTILE_25);
    feature_array[F_50th] = feature_percent_.Percentile(FeaturePercentile::PERCENTILE_50);
    feature_array[F_75th] = feature_percent_.Percentile(FeaturePercentile::PERCENTILE_75);
  }

}

void SweepFeaturePipelineClassifier::DisposeSampleList(){
  for(auto iter=sample_framelist_.begin(); iter!=sample_framelist_.end();++iter){
    DataFrame* df = *iter;
    if(df) delete df;
  }
  sample_framelist_.clear();
}
#endif // SWEEPFEATURECLASSIFIER_H
