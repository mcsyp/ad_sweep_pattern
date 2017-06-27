#ifndef WAVEFEATURETRAINER_H
#define WAVEFEATURETRAINER_H
#include "qrineuronlib.h"
#include "vector"

class WaveFeatureTrainer
{
public:
  static constexpr int DEFUALT_INTERATIONS=10;
  typedef std::vector<qri_neuron_lib::DataFrame*> DataFrameList;

  /*purpose: train the frame list according to sequence array.
   *input:
   * @framelist, the dataframe array
   * @engine, the neuron engine
   *return:
   * @the neuron number of the engine
   */
  virtual int Train(NeuronEngineFloat& engine,const DataFrameList& framelist);

  //generate random index
  static void GenerateRandomIndex(int array[], int len);
};

#endif // WAVEFEATURETRAINER_H
