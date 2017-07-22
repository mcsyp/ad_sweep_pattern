#ifndef FEATURETRAINER_H
#define FEATURETRAINER_H

#include "featureclassifier.h"

namespace feature_engine {

  class FeatureTrainer : public FeatureBasic
  {
    Q_OBJECT
  public:
    static constexpr int DEFAULT_TRAINING_ITERATIONS=8;
    explicit FeatureTrainer(const param_window_t & window, const param_gap_filter_t & gap,FeatureExtractor * extractor);

    /*purpose:push one row of raw data to sample
     *input:
     * @cat, the category of this row
     * @row_data, one row of raw_data
     * @row_len, length of the row data
     *return:
     * if the new sample is accepted
     */
    virtual bool PushToSample(int cat,float row_data[],int row_len);

    /*purpose: start training the raw data
     *input:
     * @engine, the engine for training
     * @iterations, repeat the training
     *return:
     * how many neurons are used
     */
    virtual int StartTraining(NeuronEngineFloat& engine, int iterations=DEFAULT_TRAINING_ITERATIONS);
    virtual void Reset();

  protected:
    //generate random index
    void GenerateRandomIndex(int array[], int len);
  public:
    FeatureExtractor::FeatureList sample_list_;
  };

}

#endif // FEATUREENGINETRAINER_H
