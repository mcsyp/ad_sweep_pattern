#ifndef KEYAXISFEAURETRAINER_H
#define KEYAXISFEAURETRAINER_H
#include <QObject>
#include "framefeatureextractor.h"
#include "framefeatureclassifier.h"
#include "featuretrainer.h"
#include <vector>
class FrameFeatureTrainer:public feature_engine::FeatureTrainer
{
  Q_OBJECT
public:
  explicit FrameFeatureTrainer(const param_wave_t & wave);
  virtual ~FrameFeatureTrainer();
};

#endif // KEYAXISFEAURETRAINER_H
