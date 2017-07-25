#ifndef KEYEXTRACTOR_H
#define KEYEXTRACTOR_H
#include <QObject>
#include "qrineuronlib.h"
#include "featureclassifier.h"
#include <vector>

typedef struct param_wave_s{
  int min_wave_len;
  int max_wave_len;
}param_wave_t;
class FrameFeatureExtractor: public feature_engine::FeatureExtractor{
  Q_OBJECT
public:
  static constexpr int RAW_ROWS=180;
  static constexpr int RAW_DELTA=170;
  static constexpr int RAW_COLS=3;//FIXED!! only X and Y are used

  static constexpr float FEATURE_GAP_MIN=50.0f;//preprocess delta
  static constexpr float FEATURE_GAP_MAX=2000.0f;//preprocess delta

  static constexpr float FEATURE_SCALE_ENERGY=1.0f/1000.0f; //energy extraction
  static constexpr float FEATURE_SCALE_CROSSRATE=200.0f;
  static constexpr float FEATURE_SCALE_CORRELATION= 200.0f;
  static constexpr float FEATURE_SCALE_KUR=200.0f;
  static constexpr float FEATURE_SCALE_SKEW=60.0f;
  static constexpr float FEATURE_SCALE_SAMPLE_PER_PEAK=40.0f;

  explicit FrameFeatureExtractor(const param_wave_t & wave_filter);
  /*purpose: extracted features from raw data
   *input:
   * @raw_frame, the raw data frame
   *output:
   * @out_features, all features are saved in this struct
   */
  void ExtractFeatures(const qri_neuron_lib::DataFrame & raw_frame,FeatureList & out_features);

protected:
  int FindKeyAxis(const qri_neuron_lib::DataFrame& frame);
private:
  //feature extraction
  qri_neuron_lib::FeatureRemoveMean feature_nomean_;
  qri_neuron_lib::FeatureAverageEnergy feature_energy_;
  qri_neuron_lib::FeatureCorrelation feature_correlation_;
  qri_neuron_lib::FeaturePercentile feature_percent_;
  qri_neuron_lib::FeatureMeanCorssingRate feature_crossrate_;
  qri_neuron_lib::FeatureThresholdWaveFinder feature_finder_;
  qri_neuron_lib::FeatureCorrelatedEnergy feature_c_energy_;
  qri_neuron_lib::FeatureMSE feature_mse_;
  qri_neuron_lib::FeatureKurtosis feature_kurtosis_;
  qri_neuron_lib::FeatureSkewness feature_skewness_;

  param_wave_t wave_filter_;
};


#endif // KEYEXTRACTOR_H
