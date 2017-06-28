#ifndef KEYAXISFEAURETRAINER_H
#define KEYAXISFEAURETRAINER_H

#include "keyaxselectorclassifier.h"
#include <vector>
class KeyAxSelectorTrainer:public KeyAxFeatureExtractor
{
public:
  typedef struct sample_s{
    int cat;
    int feature_len;
    float feature[FEATURE_MAXNUM];
  }sample_t;
  typedef std::vector<sample_t> SampleList;
  static constexpr int DEFUALT_ITERATIONS=10;

  KeyAxSelectorTrainer();

  /*purpose:push one row of raw data to sample
   *input:
   * @cat, the category of this row
   * @row_data, one row of raw_data
   * @row_len, length of the row data
   *return:
   * how many samples in the sample list
   */
  int PushToSample(int cat,float row_data[],int row_len);

  /*purpose: start training the raw data
   *input:
   * @engine, the engine for training
   * @iterations, repeat the training
   *return:
   * how many neurons are used
   */
  int StartTraining(NeuronEngineFloat& engine, int iterations=DEFUALT_ITERATIONS);

  void Reset();
protected:
  int PushFeaturesToSample(int cat, const float features[], int feature_len);

  //generate random index
  void GenerateRandomIndex(int array[], int len);
private:
  qri_neuron_lib::DataFrame * raw_frame_;
public:
  SampleList sample_list_;

};

#endif // KEYAXISFEAURETRAINER_H
