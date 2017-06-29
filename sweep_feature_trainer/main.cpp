#include <QCoreApplication>
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QStringList>

#include <stdio.h>
#include <math.h>

#include <qrineuronlib.h>
#include "keyaxselectorclassifier.h"
#include "keyaxselectortrainer.h"
#include "sweepfeaturetrainer.h"
#include "sweepfeatureclassifier.h"
#include "sweepfeatureextractor.h"

#include <tooldebug.h>

using namespace qri_neuron_lib;


#define CAT_LEFT_HIGH KeyAxFeatureExtractor::AxisX
#define CAT_LEFT_LOW KeyAxFeatureExtractor::AxisY

#define PROCESSED_0    "../../5.data_source/20170624-nanjing/gao_left_high.csv"
#define PROCESSED_1    "../../5.data_source/20170624-nanjing/gao_left_low.csv"
#define PROCESSED_2    "../../5.data_source/20170624-nanjing/shidai_left_high.csv"
#define PROCESSED_3    "../../5.data_source/20170624-nanjing/shidai_left_low.csv"
#define PROCESSED_4    "../../5.data_source/20170624-nanjing/zhipeng_left_high.csv"
#define PROCESSED_5    "../../5.data_source/20170624-nanjing/zhipeng_left_low.csv"


#define PROCESSED_6   "../../5.data_source/20170626-nanjing-validate/correct_s_gao_left_high.csv"
#define PROCESSED_7   "../../5.data_source/20170626-nanjing-validate/correct_s_gao_left_low.csv"
#define PROCESSED_8   "../../5.data_source/20170626-nanjing-validate/correct_s_shidai_left_high.csv"
int PushToTrainSelector(const char* src_path,int cat, KeyAxSelectorTrainer & trainer);
float PushToClassifySelector(const char * src_path, int target_cat ,KeyAxSelectorClassifier & classifier);

int PushToTrainFeature(const char* src_path,int cat, SweepFeatureTrainer & trainer);
float PushToClassifyFeature(const char * src_path, int target_cat ,SweepFeatureClassifier & classifier);
float Possibility(int target_cat,SweepFeatureClassifier::result_cat_t * result_array,int result_len);

void PrintEngine(const NeuronEngineFloat &  engine);
void PrintNeuron(const NeuronFloat *nf);

int main(int argc, char *argv[])
{
  (void)argc;
  (void)argv;
  NeuronEngineFloat engine_selector_(NeuronEngineFloat::MODE_KNN);
  KeyAxSelectorTrainer key_trainer;
  KeyAxSelectorClassifier key_classifier(&engine_selector_);

  do{
    //step1. push samples to the df list
    PushToTrainSelector(PROCESSED_0,CAT_LEFT_HIGH,key_trainer);
    PushToTrainSelector(PROCESSED_2,CAT_LEFT_HIGH,key_trainer);
    PushToTrainSelector(PROCESSED_4,CAT_LEFT_HIGH,key_trainer);
    PushToTrainSelector(PROCESSED_1,CAT_LEFT_LOW,key_trainer);
    PushToTrainSelector(PROCESSED_3,CAT_LEFT_LOW,key_trainer);
    PushToTrainSelector(PROCESSED_5,CAT_LEFT_LOW,key_trainer);

    printf("%d samples are in the list.\n",key_trainer.sample_list_.size());


    int neurons = key_trainer.StartTraining(engine_selector_);
    printf("used neurons=%d\n",neurons);

    //step3.validate
    //positive test
    PushToClassifySelector(PROCESSED_0,CAT_LEFT_HIGH,key_classifier);
    PushToClassifySelector(PROCESSED_2,CAT_LEFT_HIGH,key_classifier);
    PushToClassifySelector(PROCESSED_5,CAT_LEFT_LOW,key_classifier);
    PushToClassifySelector(PROCESSED_3,CAT_LEFT_LOW,key_classifier);
    PushToClassifySelector(PROCESSED_6,CAT_LEFT_HIGH,key_classifier);
    PushToClassifySelector(PROCESSED_7,CAT_LEFT_LOW,key_classifier);
  }while(0);

  printf("=====================================Selector training done=====================================\n\n");
  printf("=====================================Start training Sweep trainer=====================================\n");


  NeuronEngineFloat engine_feature_;

  SweepFeatureTrainer feature_trainer(&key_classifier);
  SweepFeatureClassifier feature_classifier(&engine_feature_,&key_classifier);

  do{
    //push high left
    PushToTrainFeature(PROCESSED_0,CAT_LEFT_HIGH,feature_trainer);
    PushToTrainFeature(PROCESSED_2,CAT_LEFT_HIGH,feature_trainer);
    PushToTrainFeature(PROCESSED_4,CAT_LEFT_HIGH,feature_trainer);
    //push low left
    PushToTrainFeature(PROCESSED_1,CAT_LEFT_LOW,feature_trainer);
    PushToTrainFeature(PROCESSED_3,CAT_LEFT_LOW,feature_trainer);
    PushToTrainFeature(PROCESSED_5,CAT_LEFT_LOW,feature_trainer);

    PushToTrainFeature(PROCESSED_7,CAT_LEFT_LOW,feature_trainer);

    printf("%d samples are in the list.\n",feature_trainer.sample_list_.size());

    int neurons = feature_trainer.StartTraining(engine_feature_,20);
    printf("used neurons=%d\n",neurons);


    PushToClassifyFeature(PROCESSED_0,CAT_LEFT_HIGH,feature_classifier);
    PushToClassifyFeature(PROCESSED_2,CAT_LEFT_HIGH,feature_classifier);
    PushToClassifyFeature(PROCESSED_5,CAT_LEFT_LOW,feature_classifier);
    PushToClassifyFeature(PROCESSED_3,CAT_LEFT_LOW,feature_classifier);
    PushToClassifyFeature(PROCESSED_6,CAT_LEFT_HIGH,feature_classifier);
    PushToClassifyFeature(PROCESSED_8,CAT_LEFT_HIGH,feature_classifier);

  }while(0);
  SaveEngineFloat(&engine_selector_,"neurons_selector.csv");
  SaveEngineFloat(&engine_feature_,"neurons_feature.csv");
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

int PushToTrainSelector(const char* src_path,int cat, KeyAxSelectorTrainer & trainer){
  QFile src_file(src_path);
  if(!src_file.open(QFile::ReadOnly)){
    printf("Fail to open %s.\n",src_path);
    return 0;
  }
  QTextStream src_text(&src_file);

  float row_data[KeyAxSelectorTrainer::RAW_COLS];
  int sample_size=0;
  //step1. read and push adll data to the frame list
  src_text.readLine();
  while(!src_text.atEnd()){
    QString str_line = src_text.readLine();

    //step1. analyze the row data
    str_line.replace('\"',"");
    QStringList str_data = str_line.split(",");
    for(int i=0;i<KeyAxSelectorTrainer::RAW_COLS;++i){
      row_data[i] = str_data.at(i).toFloat();
    }

    //step2. push to trainer;
    sample_size = trainer.PushToSample(cat,row_data,KeyAxSelectorTrainer::RAW_COLS);
  }

  src_file.close();
  return sample_size;
}

float PushToClassifySelector(const char *src_path, int target_cat, KeyAxSelectorClassifier &classifier){
  QFile src_file(src_path);
  if(!src_file.open(QFile::ReadOnly)){
    printf("Fail to open %s.\n",src_path);
    return 0;
  }
  QTextStream src_text(&src_file);

  float row_data[KeyAxFeatureExtractor::RAW_COLS];
  int total_frames=0;
  int correct_frames=0;
  while(!src_text.atEnd()){
    QString str_line = src_text.readLine();

    //step1. analyze the row data
    str_line.replace('\"',"");
    QStringList str_data = str_line.split(",");
    for(int i=0;i<KeyAxFeatureExtractor::RAW_COLS;++i){
      row_data[i] = str_data.at(i).toFloat();
    }
    int cat = classifier.PushToClassify(row_data,KeyAxFeatureExtractor::RAW_COLS);
    if(cat>=0){
      ++total_frames;
      if(cat==target_cat){
        ++correct_frames;
      }
    }

  }
  src_file.close();
  if(total_frames<=0)return 0.0f;

  float accuracy = (float)correct_frames / (float)total_frames;

#if 1
  printf("Validating [%s] with target_cat [%2d]\n",src_path,target_cat);
  printf("[%3d/%3d] samples are correct with target [%2d]. Accuracy is [%.2f]\n",
         correct_frames,
         total_frames,
         target_cat,
         accuracy);
  printf("----------------------------------------------------------------------------------------------\n");
#endif

  return accuracy;
}

int PushToTrainFeature(const char* src_path,int cat, SweepFeatureTrainer & trainer){
  QFile src_file(src_path);
  if(!src_file.open(QFile::ReadOnly)){
    printf("Fail to open %s.\n",src_path);
    return 0;
  }
  QTextStream src_text(&src_file);

  const int row_size =SweepFeatureExtractor::RAW_COLS;
  float row_data[row_size];
  int sample_size=0;
  //step1. read and push adll data to the frame list
  src_text.readLine();
  while(!src_text.atEnd()){
    QString str_line = src_text.readLine();

    //step1. analyze the row data
    str_line.replace('\"',"");
    QStringList str_data = str_line.split(",");
    for(int i=0;i<KeyAxSelectorTrainer::RAW_COLS;++i){
      row_data[i] = str_data.at(i).toFloat();
    }

    //step2. push to trainer;
    sample_size = trainer.PushToSample(cat,row_data,row_size);
  }

  src_file.close();
  return sample_size;
}

float PushToClassifyFeature(const char *src_path, int target_cat, SweepFeatureClassifier &classifier){
  QFile src_file(src_path);
  if(!src_file.open(QFile::ReadOnly)){
    printf("Fail to open %s.\n",src_path);
    return 0;
  }
  QTextStream src_text(&src_file);

  float row_data[KeyAxFeatureExtractor::RAW_COLS];
  int total_frames=0;
  int correct_frames=0;

  const int result_size = SweepFeatureExtractor::RAW_ROWS;
  SweepFeatureClassifier::result_cat_t result[result_size];

  while(!src_text.atEnd()){
    QString str_line = src_text.readLine();

    //step1. analyze the row data
    str_line.replace('\"',"");
    QStringList str_data = str_line.split(",");
    for(int i=0;i<KeyAxFeatureExtractor::RAW_COLS;++i){
      row_data[i] = str_data.at(i).toFloat();
    }
    int result_len = classifier.PushToClassify(row_data,KeyAxFeatureExtractor::RAW_COLS,result,result_size);
    if(result_len>=0){
      ++total_frames;
      if(result_len>0 && Possibility(target_cat,result,result_len)>0.5f){
        ++correct_frames;
      }
#if 0
      printf("valid results:");
      for(int i=0;i<result_len;++i){
        printf("%d ",result[i].cat);
      }
      printf("");
#endif
    }

  }
  src_file.close();
  if(total_frames<=0)return 0.0f;

  float accuracy = (float)correct_frames / (float)total_frames;

#if 1
  printf("Validating [%s] with target_cat [%2d]\n",src_path,target_cat);
  printf("[%3d/%3d] samples are correct with target [%2d]. Accuracy is [%.2f]\n",
         correct_frames,
         total_frames,
         target_cat,
         accuracy);
  printf("----------------------------------------------------------------------------------------------\n");
#endif

  return accuracy;
}
float Possibility(int target_cat,SweepFeatureClassifier::result_cat_t * result_array,int result_len){
  int sum=0;
  if(result_len==0)return 0.0f;
  for(int i=0;i<result_len;++i){
    int cat = result_array[i].cat;
    if(cat==target_cat){
      ++sum;
    }
  }
  float accuracy = static_cast<float>(sum)/static_cast<float>(result_len);
  printf("Correct rate with target [%d] is [%.2f]\n",target_cat,accuracy);
  return accuracy;
}
