#include "tools.h"
#include <QFile>
#include <QTextStream>
#include <QString>
#include <QStringList>
#include <QVector>
#include <QDebug>
using namespace feature_engine;

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

int PushFileToSamples(const QString & src_path,int cat, feature_engine::FeatureTrainer * trainer){
  QFile src_file(src_path);
  if(!src_file.open(QFile::ReadOnly)){
    qDebug()<<"Fail to open "<<src_path;
    return 0;
  }
  QTextStream src_text(&src_file);

  QVector<float> row_data;
  int sample_size=0;
  //step1. read and push adll data to the frame list
  src_text.readLine();
  while(!src_text.atEnd()){
    QString str_line = src_text.readLine();
    str_line.replace('\"',"");
    QStringList str_data = str_line.split(",");
    if(str_data.size()==0)continue;

    //step1 push raw data to engine
    row_data.clear();
    for(int i=0;i<str_data.size();++i){
      row_data.push_back(str_data.at(i).toFloat());
    }
    //step2. push to trainer;
    sample_size = trainer->PushToSample(cat,&(row_data[0]),row_data.size());
  }

  src_file.close();
  return sample_size;
}

float PushFileToClassify(const QString &  src_path, int target_cat ,feature_engine::FeatureClassifier * classifier){
  QFile src_file(src_path);
  if(!src_file.open(QFile::ReadOnly)){
    qDebug()<<"Fail to open "<<src_path;
    return 0;
  }
  QTextStream src_text(&src_file);

  QVector<float> row_data;
  int correct_samples=0;

  classifier->BeginStatistic();
  while(!src_text.atEnd()){
    QString str_line = src_text.readLine();
    str_line.replace('\"',"");
    QStringList str_data = str_line.split(",");
    if(str_data.size()==0)continue;


    //step1. push to data
    row_data.clear();
    for(int i=0;i<str_data.size();++i){
      row_data.push_back(str_data.at(i).toFloat());
    }

    if(classifier->PushToClassify(&row_data[0],row_data.size())){
      const FeatureClassifier::ResultList & result_list  = classifier->LastResult();
      for(int i=0;i<result_list.size();++i){
        const FeatureClassifier::result_t & sample = result_list.at(i);
        if(sample.cat==target_cat){
          correct_samples+=sample.samples;
#if 0
          qDebug()<<QString("[%1,%2] cat:%3, percentile:%4, nid:%5,peaks:%6")
                    .arg(__FILE__).arg(__LINE__)
                    .arg(sample.cat)
                    .arg(sample.percentile)
                    .arg(sample.nid)
                    .arg(sample.peaks);
#endif
        }
      }
    }
  }
  src_file.close();


  FeatureClassifier::result_statistic_t statistic = classifier->EndStatistic();
  float accuracy = 0.0f;
  if(statistic.accpeted_samples){
    accuracy = (float)correct_samples / (float)statistic.accpeted_samples;
  }
#if 0
  qDebug()<<QString("[%1,%2] correct samples:%3, accepted samples:%4, totol samples: %5. classified_frames:%6, total_frames:%7")
            .arg(__FILE__).arg(__LINE__)
            .arg(correct_samples)
            .arg(statistic.accpeted_samples)
            .arg(statistic.total_samples)
            .arg(statistic.classified_frames)
            .arg(statistic.total_frames);
#endif

  return accuracy;
}
int LoadEngine(const char* src_path,NeuronEngineFloat & engine){
  if(src_path==NULL)return 0;
  QFile src_file(src_path);
  if(!src_file.open(QFile::ReadOnly)){
    printf("[%s,%d]Fail to open file %s",__FILE__,__LINE__,src_path);
    return 0;
  }
  QTextStream src_text(&src_file);

  //step1.reset engine
  engine.ResetEngine();

  engine.BeginRestoreMode();
  while(!src_text.atEnd()){
    QString str_record = src_text.readLine();
    QStringList str_list = str_record.split(',');
    if(str_list.size()<3)continue;

    //read cat,min_aif, aif
    int cat =  str_list[0].toInt();
    int min_aif = str_list[1].toFloat();
    int aif = str_list[2].toFloat();

    //read buffer
    const int neuron_size=1024;
    float neuron_buffer[neuron_size];
    int neuron_len=0;
    for(int i=0;i<str_list.size()-3;++i){
      neuron_buffer[i]=str_list[3+i].toFloat();
      ++neuron_len;
    }

    //restore
    engine.RestoreNeuron(neuron_buffer,neuron_len,cat,aif,min_aif);
  }
  engine.EndRestoreMode();
  return engine.NeuronCount();
}
