#ifndef SWEEPFEATUREEXTRACTOR_H
#define SWEEPFEATUREEXTRACTOR_H
#include <QObject>

#include "qrineuronlib.h"
#include "frameclassifier.h"

class WaveFeatureExtractor: public QObject
{
  Q_OBJECT
public:
  enum Feature{
    F_Mean=0,
    F_AbsMean=1,
    F_Corssrate=2,
    F_Correlation,
    F_Kurtosis,
    F_Skewness,
    F_Energy,
    F_CEnergy,
    F_25th,
    F_50th,
    F_75th,
    F_Num
  };
  static constexpr int FEATURE_MAXNUM=(int)(F_Num*5/4);
  static constexpr int FEATURE_WAVELEN_MIN=10;//wave extraction

  static constexpr float FEATURE_GAP_MIN=100.0f;//preprocess delta
  static constexpr float FEATURE_GAP_MAX=2000.0f;//preprocess delta
  static constexpr int FEATURE_GAP_AXIS=1;

  static constexpr float FEATURE_SCALE_ENERGY=1.0f/1000.0f; //energy extraction
  static constexpr float FEATURE_SCALE_CROSSRATE=400.0f;
  static constexpr float FEATURE_SCALE_CORRELATION=300.0f;
  static constexpr float FEATURE_SCALE_KUR=200.0f;
  static constexpr float FEATURE_SCALE_SKEW=60.0f;

  static constexpr int SAMPLE_COLS=F_Num;
  static constexpr int SAMPLE_ROWS=1;

  static constexpr int RAW_DELTA=180;
  static constexpr int RAW_ROWS=200;
  static constexpr int RAW_COLS=2;

  typedef struct sample_frame_s{
    int    cat;//the cat of this sample
    int    frame_offset;//the raw data index of this frame
    int    frame_len;//the frame len
    int     feature_len;//how many features in the feature list
    float  feature[FEATURE_MAXNUM];
  }sample_t;
  typedef std::vector<sample_t> SampleList;

  explicit WaveFeatureExtractor(QObject* parent=NULL);

  /*purpose: extracted features from raw data
   *input:
   * @raw_frame, the raw data frame
   *output:
   * @list, the result array with extracted features results
   *return:
   * the length of the samplelist array
   */
  virtual int ExtractFeatures(qri_neuron_lib::DataFrame * raw_frame,SampleList& list);

private:
  //preprocess
  qri_neuron_lib::FeatureRemoveMean feature_nomean_;

  //feature extraction
  qri_neuron_lib::FeatureAverageEnergy feature_energy_;
  qri_neuron_lib::FeatureCorrelation feature_correlation_;
  qri_neuron_lib::FeatureMeanCorssingRate feature_crossrate_;
  qri_neuron_lib::FeatureKurtosis feature_kurtosis_;
  qri_neuron_lib::FeatureSkewness feature_skewness_;
  qri_neuron_lib::FeaturePercentile feature_percent_;
  qri_neuron_lib::FeatureThresholdWaveFinder feature_finder_;
  qri_neuron_lib::FeatureMSE feature_mse_;
  qri_neuron_lib::FeatureCorrelatedEnergy feature_c_energy_;

  qri_neuron_lib::ComputeScaleQuant255 scale_quant255_;
  qri_neuron_lib::FeatureMinMax feature_minmax_;
};

#endif // SWEEPFEATUREEXTRACTOR_H
