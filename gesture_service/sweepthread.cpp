#include "sweepthread.h"
#include <QTextStream>
#include <qdebug.h>
#include "global_service.h"
SweepThread::SweepList SweepThread::sweep_list = SweepThread::SweepList();
void SweepThread::Remove(int index){
  for(auto iter = sweep_list.begin();iter!=sweep_list.end();++iter){
    SweepThread * t = *iter;
    if(t && t->index_==index){
      sweep_list.erase(iter);
      break;
    }
  }
}
SweepThread* SweepThread::Available(){
  for(auto iter=sweep_list.begin();iter!=sweep_list.end();++iter){
    SweepThread * t = *iter;
    if(t && t->isRunning()==false){
      return t;
    }
  }
  return NULL;
}

void SweepThread::ReleaseAll(){
  for(auto iter = sweep_list.begin();iter!=sweep_list.end();++iter){
    SweepThread * t = *iter;
    if(t)delete t;
  }
  sweep_list.clear();
}

SweepThread::SweepThread(QObject *parent) : QThread(parent){
  signature_ = QString();
  index_ = sweep_list.size();
  sweep_list.push_back(this);//save this to the global list

  engine_key_ = NeuronEngineFloat(NeuronEngineFloat::MODE_KNN);
  key_ = new FrameClassifier(&engine_key_);

  //step2.inti feature classifier
  feature_ = new WaveClassifier(&engine_feature_,key_);
}
SweepThread::~SweepThread(){
  if(key_) delete key_;
  if(feature_) delete feature_;
  Remove(index_);
}
void SweepThread::StartTask(QString& sign, qint64 start, qint64 end, QByteArray& raw_data){
  signature_ = sign;
  raw_data_ = raw_data;
  start_time_ = start;
  end_time_ = end;
  this->start();
}

void SweepThread::run(){
  if(raw_data_.size()<=0)return;

  QTextStream stream(&raw_data_);
  const int row_size = 3;
  float row_buffer[row_size];
  const int result_size = WaveClassifier::RAW_ROWS;
  int cat_samples=0;
  int total_samples=0;
  int accepted_samples=0;
  int cat_counter[5]={0,0,0,0,0};
  WaveClassifier::result_cat_t result_array[result_size];

  while(!stream.atEnd()){
    QString str_line = stream.readLine();
    QStringList str_val_list = str_line.split(',');
    if(str_val_list.size()<3)continue;

    row_buffer[0] = str_val_list[0].toFloat();
    row_buffer[1] = str_val_list[1].toFloat();
    row_buffer[2] = str_val_list[2].toFloat();

    int result_len = feature_->PushToClassify(row_buffer,row_size,result_array,result_size);
    if(result_len>=0){
      for(int i=0;i<result_len;++i){
        cat_samples+=result_array[i].frame_len;
        ++cat_counter[result_array[i].cat];
      }
      ++accepted_samples;
    }
    ++total_samples;
  }
  if(cat_samples){
    qDebug()<<endl;
    qDebug()<<tr("[%1,%2] signature:%3 thread_%4 total:%5 accepted:%6 cat:%7")
              .arg(__FILE__).arg(__LINE__)
              .arg(signature_)
              .arg(index_)
              .arg(total_samples)
              .arg(accepted_samples)
              .arg(cat_samples);
    for(int i=1;i<3;++i){
      qDebug()<<tr("cat:%1 counter:%2").arg(i).arg(cat_counter[i]);
    }
  }
  emit resultReady(signature_,G_SWEEP,total_samples,cat_samples);
}

