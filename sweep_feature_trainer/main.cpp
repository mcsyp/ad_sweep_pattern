#include <QCoreApplication>
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QStringList>

#include <stdio.h>
#include <math.h>

#include <qrineuronlib.h>
#include "wavefeaturetrainer.h"
using namespace qri_neuron_lib;

#define SAMPLE_COLS   13
#define SAMPLE_OFFSET 2

#define CAT_LEFT_HIGH 1
#define CAT_LEFT_LOW 2

#define PROCESSED_0    "../../5.data_source/20170625-nanjing-feature/wave_gao_left_high.csv"
#define PROCESSED_1    "../../5.data_source/20170625-nanjing-feature/wave_gao_left_low.csv"
#define PROCESSED_2    "../../5.data_source/20170625-nanjing-feature/wave_shidai_left_high.csv"
#define PROCESSED_3    "../../5.data_source/20170625-nanjing-feature/wave_shidai_left_low.csv"
#define PROCESSED_4    "../../5.data_source/20170625-nanjing-feature/wave_zhipeng_left_high.csv"
#define PROCESSED_5    "../../5.data_source/20170625-nanjing-feature/wave_zhipeng_left_low.csv"

float PushToClassify(const char * src_path, int target_cat,NeuronEngineFloat & engine);

int PushToSampleList(const char* src_path,int cat, WaveFeatureTrainer::DataFrameList & df_list);
void DisposeSampleList(WaveFeatureTrainer::DataFrameList& list);

void PrintEngine(const NeuronEngineFloat &  engine);
void PrintNeuron(const NeuronFloat *nf);

int main(int argc, char *argv[])
{
  (void)argc;
  (void)argv;
  WaveFeatureTrainer trainer;
  NeuronEngineFloat engine;
  WaveFeatureTrainer::DataFrameList df_list;
  //step1. push samples to the df list
  PushToSampleList(PROCESSED_0,CAT_LEFT_HIGH,df_list);
  PushToSampleList(PROCESSED_2,CAT_LEFT_HIGH,df_list);
  PushToSampleList(PROCESSED_4,CAT_LEFT_HIGH,df_list);
  PushToSampleList(PROCESSED_1,CAT_LEFT_LOW,df_list);
  PushToSampleList(PROCESSED_3,CAT_LEFT_LOW,df_list);
  printf("%d samples are in the list.\n",df_list.size());

  for(int i=0;i<10;++i){
    printf("[%3d] iteration:\n",i);
    float positive_acc=0.0f;
    float negtive_acc=0.0f;

    //step2.train and dump neurons
    int neurons = trainer.Train(engine,df_list);
    printf("[%3d] neurons are used\n",neurons);

    //step3.validate
    //positive test
    positive_acc += PushToClassify(PROCESSED_0,CAT_LEFT_HIGH,engine);
    positive_acc += PushToClassify(PROCESSED_2,CAT_LEFT_HIGH,engine);
    positive_acc += PushToClassify(PROCESSED_5,CAT_LEFT_LOW,engine);
    positive_acc += PushToClassify(PROCESSED_3,CAT_LEFT_LOW,engine);

    //negtive test
    negtive_acc += PushToClassify(PROCESSED_1,CAT_LEFT_HIGH,engine);
    negtive_acc += PushToClassify(PROCESSED_3,CAT_LEFT_HIGH,engine);
    negtive_acc += PushToClassify(PROCESSED_0,CAT_LEFT_LOW,engine);
    negtive_acc += PushToClassify(PROCESSED_4,CAT_LEFT_LOW,engine);

    printf("Positive accuracy is [%.2f]\n", positive_acc/4);
    printf("Negtive accuracy is [%.2f]\n", 1.0f-negtive_acc/4);

    printf("==========================================================================================\n\n");
  }
  //step FINAL dispose samples.
  DisposeSampleList(df_list);
  return 0;
}


void PrintEngine(const NeuronEngineFloat &  engine){
  //dump all neurons
  printf("Neuron number is %d\n", engine.NeuronCount());
  for(int i=0;i<engine.NeuronCount();++i){
    const NeuronFloat * nf = engine.ReadNeuron(i);
    PrintNeuron(nf);
  }
}

void PrintNeuron(const NeuronFloat* nf){
  const int max_buffer_size=1024;
  float nf_buffer[max_buffer_size];
  printf("cat=%d, min_aif=%.2f, aif=%.2f, vector:",nf->Cat(),nf->MinAIF(),nf->Aif());
  int len = nf->ReadNeuronMem(nf_buffer,max_buffer_size);
  for(int k=0;k<len;++k){
    printf(" %.2f ",nf_buffer[k]);
  }
  printf("]\n");
}

void DisposeSampleList(WaveFeatureTrainer::DataFrameList& list){
  for(auto iter = list.begin();iter!=list.end();++iter){
    DataFrame* df = *iter;
    if(df){
      delete df;
    }
  }
  list.clear();
}

int PushToSampleList(const char* src_path,int cat, WaveFeatureTrainer::DataFrameList & df_list){
  QFile src_file(src_path);
  if(!src_file.open(QFile::ReadOnly)){
    printf("Fail to open %s.\n",src_path);
    return 0;
  }
  QTextStream src_text(&src_file);

  //step1. read and push adll data to the frame list
  src_text.readLine();
  while(!src_text.atEnd()){
    QString str_line = src_text.readLine();

    DataFrame* current_frame  =  DataFrame::Create(SAMPLE_COLS-SAMPLE_OFFSET,1);
    //step1. analyze the row data
    QStringList str_data = str_line.split(",");
    for(int i=0;i<SAMPLE_COLS;++i){
      float temp = str_data.at(i).toFloat();
      if(i>=SAMPLE_OFFSET){
        current_frame->Push(&temp,1);
      }
    }
    current_frame->category_ = cat;
    df_list.push_back(current_frame);
  }

  src_file.close();
  return df_list.size();
}
float PushToClassify(const char *src_path, int target_cat, NeuronEngineFloat &engine){
  QFile src_file(src_path);
  if(!src_file.open(QFile::ReadOnly)){
    printf("Fail to open %s.\n",src_path);
    return 0;
  }
  QTextStream src_text(&src_file);



  int total_frames=0;
  int correct_frames=0;
  float sample_val[SAMPLE_COLS];
  while(!src_text.atEnd()){
    QString str_line = src_text.readLine();

    //step1. analyze the row data
    QStringList str_data = str_line.split(",");
    for(int i=0;i<SAMPLE_COLS;++i){
      sample_val[i] = str_data.at(i).toFloat();
    }
    int cat = engine.Classify(sample_val+SAMPLE_OFFSET,SAMPLE_COLS-SAMPLE_OFFSET);
    if(cat==target_cat){
      ++correct_frames;
    }
    ++total_frames;
  }
  src_file.close();
  if(total_frames<=0)return 0.0f;

  float accuracy = (float)correct_frames / (float)total_frames;

#if 0
  printf("Validating [%s] with target_cat [%2d]\n",src_path,target_cat);
  printf("[%3d/%3d] samples are correct with target [%2d]. Accuracy is [%.2f]\n",
         correct_frames,
         total_frames,
         target_cat,
         accuracy);
  printf("-----------------------------------------------------------------------------------------------------------------------------------\n");
#endif

  return accuracy;
}
