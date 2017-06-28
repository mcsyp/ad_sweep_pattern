#ifndef KEYAXSELECTORCLASSIFIER_H
#define KEYAXSELECTORCLASSIFIER_H

#include "qrineuronlib.h"
#include <vector>

class KeyAxFeatureExtractor{
public:
  enum AxisCategory{
    AxisX=1,
    AxisY=2,
  };

  enum Feature{
    FeatureMean=0,
    FeatureEnergy,
    Feature25th,
    Feature50th,
    Feature75th,
    FeatureNum
  };
  static constexpr int RAW_ROWS=300;
  static constexpr int RAW_DELTA=280;
  static constexpr int RAW_COLS=2;
  static constexpr int FEATURE_MAXNUM=FeatureNum*3;

  static constexpr float FEATURE_GAP_MIN=50.0f;//preprocess delta
  static constexpr float FEATURE_GAP_MAX=2000.0f;//preprocess delta
  static constexpr int    FEATURE_GAP_AXIS=1;

  /*purpose: extracted features from raw data
   *input:
   * @raw_frame, the raw data frame
   *output:
   * @feature, the extracted feature
   * @feature_size, the max size of this array
   *return:
   * the length of the feature
   */
  virtual int ExtractFeatures(qri_neuron_lib::DataFrame * raw_frame, float feature[],int feature_size);

private:
  //feature extraction
  qri_neuron_lib::FeatureAverageEnergy feature_energy_;
  qri_neuron_lib::FeatureCorrelation feature_correlation_;
  qri_neuron_lib::FeaturePercentile feature_percent_;
};

class KeyAxSelectorClassifier: public KeyAxFeatureExtractor
{
public:
  KeyAxSelectorClassifier(NeuronEngineFloat* engine);

  /*purpose: extracted features from raw data
   *input:
   * @row_data, one row raw data
   * @row_len, length of the row
   *return:
   * return -1 not valid
   * reurn classified result
   */
  int PushToClassify(float row_data[],int row_len);

  /*purpose: extracted features from raw data
   *input:
   * @raw_frame, the raw data frame
   *output:
   * @feature, the extracted feature
   * @feature_size, the max size of this array
   *return:
   * the length of the feature
   */
  int Classify(qri_neuron_lib::DataFrame * frame);


  /*purpose: convert the input category into axis
   *input:
   * @cat, category classfied by selector
   *return:
   * the axis id [0,1,2,3 ....]
   */
  int CategoryToAxis(int cat);

protected:
  NeuronEngineFloat* engine_;
  qri_neuron_lib::DataFrame* raw_frame_;
};



#endif // KEYAXSELECTORCLASSIFIER_H
