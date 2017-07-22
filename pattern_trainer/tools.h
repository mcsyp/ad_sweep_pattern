#ifndef TOOLS_H
#define TOOLS_H
#include "featureclassifier.h"
#include "featuretrainer.h"
#include "csvsamples.h"

int   LoadEngine(const char* src_path,NeuronEngineFloat & engine);
int   PushFileToSamples(const QString & src_path,int cat, feature_engine::FeatureTrainer * trainer);
float PushFileToClassify(const QString &  src_path, int target_cat ,feature_engine::FeatureClassifier * classifier);

void  PrintEngine(const NeuronEngineFloat &  engine);
void  PrintNeuron(const NeuronFloat *nf);

#endif // TOOLS_H
