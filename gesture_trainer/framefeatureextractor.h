#ifndef KEYEXTRACTOR_H
#define KEYEXTRACTOR_H
#include <QObject>
#include "qrineuronlib.h"
#include <vector>

class FrameFeatureExtractor: public QObject{
  Q_OBJECT
public:
  enum Feature{
    FeatureMean=0,
    FeatureAbsMean=1,
    FeatureEnergy,
    FeatureCrossRate,
    Feature25th,
    Feature50th,
    Feature75th,
    FeatureNum
  };
  static constexpr int RAW_ROWS=100;
  static constexpr int RAW_DELTA=80;
  static constexpr int RAW_COLS=2;//FIXED!! only X and Y are used
  static constexpr int FEATURE_MAXNUM=FeatureNum*3;

  static constexpr float FEATURE_GAP_MIN=50.0f;//preprocess delta
  static constexpr float FEATURE_GAP_MAX=2000.0f;//preprocess delta
  static constexpr int   FEATURE_GAP_AXIS=1;

  static constexpr float FEATURE_SCALE_ENERGY=1.0f/800.0f; //energy extraction
  static constexpr float FEATURE_SCALE_CROSSRATE=800.0f;
  static constexpr float FEATURE_SCALE_CORRELATION= 500.0f;
  explicit FrameFeatureExtractor(QObject* parent=NULL);
  /*purpose: extracted features from raw data
   *input:
   * @raw_frame, the raw data frame
   * @key_col, the key column index to extract features
   *output:
   * @feature, the extracted feature
   * @feature_size, the max size of this array
   *return:
   * the length of the feature
   */
  virtual int ExtractFeatures(qri_neuron_lib::DataFrame * raw_frame,
                              int key_col,
                              float feature[],
                              int feature_size);

private:
  //feature extraction
  qri_neuron_lib::FeatureAverageEnergy feature_energy_;
  qri_neuron_lib::FeatureCorrelation feature_correlation_;
  qri_neuron_lib::FeaturePercentile feature_percent_;
  qri_neuron_lib::FeatureMeanCorssingRate feature_crossrate_;
  qri_neuron_lib::FeatureThresholdWaveFinder feature_finder_;
};


#endif // KEYEXTRACTOR_H
