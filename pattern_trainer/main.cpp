#include <QCoreApplication>
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QStringList>
#include <QTime>

#include <stdio.h>
#include <math.h>

#include <qrineuronlib.h>
#include "framefeatureextractor.h"
#include "framefeatureclassifier.h"
#include "framefeaturetrainer.h"

#include <tooldebug.h>
#include <QDebug>

#include "featuretrainer.h"
#include "csvsamples.h"
#include "tools.h"
#include "pattern_config.h"

using namespace feature_engine;
using namespace qri_neuron_lib;

void  TrainCase(FeatureTrainer * trainer,
                FeatureClassifier* classifier,
                const QString & dst_path,
                training_case_t train_array[], int train_size,
                training_case_t validate_array[],int validate_size);

int main(int argc, char *argv[])
{
  (void)argc;
  (void)argv;
  do{
    param_wave_t sweep_wave={SWEEP_MIN_WAVE,SWEEP_MAX_WAVE};
    FrameFeatureTrainer sweep_trainer(sweep_wave);
    FrameFeatureClassifier sweep_classifier(sweep_wave);
    TrainCase(&sweep_trainer,
              &sweep_classifier,
              "../neurons_sweep.csv",
              g_sweep_array,g_sweep_size,
              g_sweep_validate_array,g_sweep_validate_size);
  }while(0);

  do{
    param_wave_t garbage_wave={GARBAGE_MIN_WAVE,GARBAGE_MAX_WAVE};
    FrameFeatureTrainer garbage_trainer(garbage_wave);
    FrameFeatureClassifier garbage_classifier(garbage_wave);
    TrainCase(&garbage_trainer,
              &garbage_classifier,
              "../neurons_garbage.csv",
              g_garbage_array,g_garbage_size,
              g_garbage_validate_array,g_garbage_validate_size);
  }while(0);

  do{
    param_wave_t wash_wave={WASH_MIN_WAVE,WASH_MAX_WAVE};
    FrameFeatureTrainer wash_trainer(wash_wave);
    FrameFeatureClassifier wash_classifier(wash_wave);
    TrainCase(&wash_trainer,
              &wash_classifier,
              "../neurons_wash.csv",
              g_wash_array,g_wash_size,
              g_wash_validate_array,g_wash_validate_size);
  }while(0);
}

void  TrainCase(FeatureTrainer * trainer,
                FeatureClassifier* classifier,
                const QString & dst_path,
                training_case_t train_array[], int train_size,
                training_case_t validate_array[],int validate_size){
  //push high left
  QTime time;
  time.start();
  for(int i=0;i<train_size;++i){
    PushFileToSamples(train_array[i].path,train_array[i].cat,trainer);
  }
  qDebug()<<endl<<QString("%3 ms to push %4 samples to trainer.").arg(time.elapsed()).arg(trainer->sample_list_.size());

  const int engine_size=10;
  NeuronEngineFloat engine_array[engine_size];
  float max_accuracy=0;
  int max_index=0;
  for(int i=0;i<engine_size;++i){
    //step1. training
    time.restart();
    int neurons = trainer->StartTraining(engine_array[i],20);
    int training_time = time.elapsed();
    time.restart();

    float accuracy=0.0f;//step2. verifying
    classifier->SetEngine(engine_array[i]);
    for(int k=0;k<validate_size;++k){
      accuracy += PushFileToClassify(validate_array[k].path,validate_array[k].cat, classifier);
    }
    accuracy = accuracy/validate_size;

    qDebug()<<QString("[%1]Training phase: %2 ms with %3 neurons. Verifying phase: %4 ms with accuracy: %5")
              .arg(i).arg(training_time).arg(neurons).arg(time.elapsed()).arg(accuracy);
    if(accuracy>max_accuracy){
      max_accuracy = accuracy;
      max_index = i;
    }
  }

  qDebug()<<QString("Engine_%1's accuracy is %2. %3 neurons are used.")
            .arg(max_index).arg(max_accuracy).arg(engine_array[max_index].NeuronCount());

  SaveEngineFloat(&engine_array[max_index],dst_path.toLatin1().data());

  do{
    classifier->SetEngine(engine_array[max_index]);
    float accuracy=0.0f;

    for(int k=0;k<g_nagtive_size;++k){
      accuracy += PushFileToClassify(g_nagtive_array[k].path,g_nagtive_array[k].cat, classifier);
    }
    accuracy = accuracy/g_nagtive_size;
    qDebug()<<QString("Negtive accuracy is %1").arg(accuracy);
  }while(0);
  printf("----------------------------------------------------------------------------------------\n\n");
}

