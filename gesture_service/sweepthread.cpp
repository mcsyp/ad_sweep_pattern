#include "sweepthread.h"
#include <QTextStream>
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
  selector_ = new KeyAxSelectorClassifier(&engine_key_);
  //step2.inti feature classifier
  feature_ = new SweepFeatureClassifier(&engine_feature_,selector_);
}
SweepThread::~SweepThread(){
  if(selector_) delete selector_;
  if(feature_) delete feature_;
  Remove(index_);
}
void SweepThread::StartTask(QString& sign, QByteArray& raw_data){
  signature_ = sign;
  raw_data_ = raw_data;
  this->start();
}

void SweepThread::run(){
  if(raw_data_.size()<=0)return;

  QTextStream stream(&raw_data_);
  while(!stream.atEnd()){
    QString str_line = stream.readLine();

  }
}

