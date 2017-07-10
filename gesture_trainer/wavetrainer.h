#ifndef SWEEPFEATURETRAINER_H
#define SWEEPFEATURETRAINER_H
#include "wavefeatureextractor.h"

class WaveTrainer: public WaveFeatureExtractor{
  Q_OBJECT
public:
  static constexpr int DEFUALT_ITERATIONS=8;

  explicit WaveTrainer(QObject* parent=NULL);

  /*purpose: push row data to extract samples
   *input:
   * @cat, the category of this row
   * @row_data, one row of raw_data
   * @row_len, the length of this row, no samller than RAW_COLS
   *return:
   * if the input row data is accepted.
   */
  bool PushToSample(int cat, const float *row_data, int row_len);

  /*purpose: start training the raw data
   *input:
   * @engine, the category of this row
   * @iterations, repeat the training
   *return:
   * how many neurons are used
   */
  int StartTraining(NeuronEngineFloat& engine, int iterations=DEFUALT_ITERATIONS);

  void Reset();
protected:
  //generate random index
  void GenerateRandomIndex(int array[], int len);

private:
  qri_neuron_lib::DataFrame * raw_frame_;

public:
  SampleList sample_list_;//saves extracted features
};

#endif // SWEEPFEATURETRAINER_H
