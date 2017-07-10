#ifndef KEYAXSELECTORCLASSIFIER_H
#define KEYAXSELECTORCLASSIFIER_H
#include <QObject>

#include "framefeatureextractor.h"
#include "qrineuronlib.h"
#include <vector>

class FrameClassifier: public FrameFeatureExtractor
{
  Q_OBJECT
public:
  explicit FrameClassifier(NeuronEngineFloat& engine,QObject *parent=NULL);
  ~FrameClassifier();
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
signals:
  void resultReady(int nid_array[],int nid_len,int min_nid);

protected:
  NeuronEngineFloat * ptr_engine_;
  qri_neuron_lib::DataFrame* raw_frame_;
};



#endif // KEYAXSELECTORCLASSIFIER_H
