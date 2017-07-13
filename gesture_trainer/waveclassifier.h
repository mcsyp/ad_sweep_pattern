#ifndef SWEEPFEATURECLASSIFIER_H
#define SWEEPFEATURECLASSIFIER_H
#include "qrineuronlib.h"
#include "frameclassifier.h"
#include "wavefeatureextractor.h"
#include <vector>
class WaveClassifier : public WaveFeatureExtractor{
public:
  typedef struct result_cat_s{
    int cat;
    int frame_offset;
    int frame_len;
  }result_cat_t;
  typedef std::vector<result_cat_t> ResultList;

  explicit WaveClassifier(NeuronEngineFloat &engine,QObject* parent=NULL);
  virtual ~WaveClassifier();

  /*purpose: push to classifiy the input raw data row by row
   *input:
   * @row_data, on row of raw data
   * @row_len, the length of the raw data
   *output:
   * @result_list, the result array with classified results
   *return:
   * if there returns any result
   */
  virtual bool PushToClassify(float row_data[],int row_len,ResultList & result_list);

  /*purpose: classifiy the input Dataframe
   *input:
   * @frame, the dataframe with raw data
   *output:
   * @result_list, the result array with classified results
   *return:
   * if there is a valid result
   */
  virtual bool Classify(qri_neuron_lib::DataFrame * frame,ResultList & result_list);

protected:
  //raw data frame
  qri_neuron_lib::DataFrame *raw_frame_;
  NeuronEngineFloat *ptr_engine_;
  SampleList sample_list_;//saves extracted features
};

#endif // SWEEPFEATURECLASSIFIER_H
