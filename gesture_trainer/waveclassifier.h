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

  enum Exception{
    ExceptionFailDataFrame=-1,
    ExceptionNullSelector=-2,
    ExceptionNullEngine=-3,
  };
  explicit WaveClassifier(NeuronEngineFloat &engine,QObject* parent=NULL);
  virtual ~WaveClassifier();

  /*purpose: push to classifiy the input raw data row by row
   *input:
   * @row_data, on row of raw data
   * @row_len, the length of the raw data
   *output:
   * @result, the result array with classified results
   * @reuslt_size, the maximum size of the result array
   *return:
   * the length of the result array
   */
  virtual int PushToClassify(float row_data[],int row_len,result_cat_t result[],int result_size);

  /*purpose: classifiy the input Dataframe
   *input:
   * @frame, the dataframe with raw data
   *output:
   * @result, the result array with classified results
   * @reuslt_size, the maximum size of the result array
   *return:
   * the length of the result array
   */
  virtual int Classify(qri_neuron_lib::DataFrame * frame,result_cat_t result[],int result_size);

protected:
  //raw data frame
  qri_neuron_lib::DataFrame *raw_frame_;
  NeuronEngineFloat *ptr_engine_;
  SampleList sample_list_;//saves extracted features
};

#endif // SWEEPFEATURECLASSIFIER_H
