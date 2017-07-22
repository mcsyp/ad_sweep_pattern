#ifndef KEYAXSELECTORCLASSIFIER_H
#define KEYAXSELECTORCLASSIFIER_H
#include <QObject>

#include "framefeatureextractor.h"
#include "featureclassifier.h"
#include "qrineuronlib.h"
#include <vector>

class FrameFeatureClassifier: public feature_engine::FeatureClassifier
{
  Q_OBJECT
public:
  explicit FrameFeatureClassifier(const param_wave_t & wave_len);
  explicit FrameFeatureClassifier(const param_wave_t & wave_len,NeuronEngineFloat & engine);
  virtual ~FrameFeatureClassifier();
};



#endif // KEYAXSELECTORCLASSIFIER_H
