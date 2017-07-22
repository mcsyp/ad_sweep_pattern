#include "framefeaturetrainer.h"
#include "featuretrainer.h"
#include "featureclassifier.h"

using namespace std;
using namespace qri_neuron_lib;
using namespace feature_engine;

FrameFeatureTrainer::FrameFeatureTrainer(const param_wave_t & wave)
  :FeatureTrainer({FrameFeatureExtractor::RAW_ROWS,FrameFeatureExtractor::RAW_COLS,FrameFeatureExtractor::RAW_DELTA},
                  {FrameFeatureExtractor::FEATURE_GAP_MIN,FrameFeatureExtractor::FEATURE_GAP_MAX},NULL)
{
  this->ptr_extractor_ = new FrameFeatureExtractor(wave);
}

FrameFeatureTrainer::~FrameFeatureTrainer(){
  if(this->ptr_extractor_) delete this->ptr_extractor_;
}


