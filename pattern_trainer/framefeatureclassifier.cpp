#include "framefeatureclassifier.h"
#include <stdio.h>
#include <math.h>
using namespace std;
using namespace qri_neuron_lib;
using namespace feature_engine;


FrameFeatureClassifier::FrameFeatureClassifier(const param_wave_t & wave)
  :FeatureClassifier({FrameFeatureExtractor::RAW_ROWS,FrameFeatureExtractor::RAW_COLS,FrameFeatureExtractor::RAW_DELTA},
                     {FrameFeatureExtractor::FEATURE_GAP_MIN,FrameFeatureExtractor::FEATURE_GAP_MAX},
                     NULL)
{
  this->ptr_extractor_ = new FrameFeatureExtractor(wave);
}

FrameFeatureClassifier::FrameFeatureClassifier(const param_wave_t & wave, NeuronEngineFloat &engine)
  :FeatureClassifier(engine,
                    {FrameFeatureExtractor::RAW_ROWS,FrameFeatureExtractor::RAW_COLS,FrameFeatureExtractor::RAW_DELTA},
                    {FrameFeatureExtractor::FEATURE_GAP_MIN,FrameFeatureExtractor::FEATURE_GAP_MAX},
                     NULL)
{
  this->ptr_extractor_ = new FrameFeatureExtractor(wave);
}
FrameFeatureClassifier::~FrameFeatureClassifier(){
  if(this->ptr_extractor_) delete this->ptr_extractor_;
}
