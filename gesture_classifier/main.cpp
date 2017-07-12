#include <QCoreApplication>
#include <QFile>
#include <QTextStream>

#include "qrineuronlib.h"
#include "frameclassifier.h"
#include "waveclassifier.h"

int LoadEngine(const char* src_path,NeuronEngineFloat & engine);
void PushToClassifyFeature(const char *src_path, WaveClassifier &classifier);
int main(int argc, char *argv[])
{
  QCoreApplication a(argc, argv);
  NeuronEngineFloat engine;
  WaveClassifier classifier(engine);
  LoadEngine("../feature_neurons.csv",engine);
  printf("loaded neuros:%d\n",engine.NeuronCount());

  PushToClassifyFeature("../../863137003291495_2017-07-12 19-28-00_2017-07-12 19-35-00.csv",classifier);
  printf("Classification done.");
  return 0;
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
void PushToClassifyFeature(const char *src_path, WaveClassifier &classifier){
  QFile src_file(src_path);
  if(!src_file.open(QFile::ReadOnly)){
    printf("Fail to open %s.\n",src_path);
    return;
  }
  QTextStream src_text(&src_file);

  float row_data[FrameFeatureExtractor::RAW_COLS];

  while(!src_text.atEnd()){
    QString str_line = src_text.readLine();

    //step1. analyze the row data
    str_line.replace('\"',"");
    QStringList str_data = str_line.split(",");
    for(int i=0;i<FrameFeatureExtractor::RAW_COLS;++i){
      row_data[i] = str_data.at(i).toFloat();
    }
    WaveClassifier::ResultList result_list;
    if(classifier.PushToClassify(row_data,FrameFeatureExtractor::RAW_COLS,result_list)){
      for(int i=0;i<result_list.size();++i){
        WaveClassifier::result_cat_t result  =  result_list[i];
        printf("[%d] cat:%d samples:%d\n",i,result.cat,result.frame_len);
      }
    }
  }

  src_file.close();
}
