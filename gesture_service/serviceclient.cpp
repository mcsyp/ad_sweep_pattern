#include "serviceclient.h"
#include <qdebug.h>
#include <stdio.h>
#include <QTextStream>
#include <QDateTime>
#include "global_service.h"
ServiceClient::ServiceClient(QObject *parent) :QTcpSocket(parent)
{
  connect(this,SIGNAL(connected()),this,SLOT(onConnected()));
  connect(this,SIGNAL(disconnected()),this,SLOT(onDisconnected()));
  connect(this,SIGNAL(readyRead()),this,SLOT(onReadyRead()));

  connect(&timer_retry_,SIGNAL(timeout()),this,SLOT(onTimeoutRetry()));
  connect(&protocol_,SIGNAL(payloadReady(int,QByteArray&)),
          this,SLOT(onPayloadReady(int,QByteArray&)));
}
ServiceClient::~ServiceClient(){
  //reset client
  this->disconnectFromHost();
  SweepThread::ReleaseAll();
}

bool ServiceClient::Setup(QString name, int port){
  //step1.init sweep & load engines
  for(int i=0;i<SWEEP_MAX_THREADS;++i){
    QString path_sweep_key = g_configs[SWEEP_CONFIG_KEY];
    QString path_sweep_feature = g_configs[SWEEP_CONFIG_FEATURE];
    SweepThread * sweep_thread = new SweepThread;

    if(0==LoadEngine(path_sweep_key,sweep_thread->engine_key_)){
      qDebug()<<tr("Fail to load neurons from:%1").arg(path_sweep_key);
      return false;
    }
    if(0==LoadEngine(path_sweep_feature,sweep_thread->engine_feature_)){
      qDebug()<<tr("Fail to load neurons from:%1").arg(path_sweep_feature);
      return false;
    }

    connect(sweep_thread,SIGNAL(resultReady(QString,int,int,int)),this,SLOT(onResultReady(QString,int,int,int)));
  }
  qDebug()<<tr("%1 sweep instance created.").arg(SweepThread::sweep_list.size());

  //setp2. setup server
  host_name_ = name;
  host_port_ = port;
  timer_retry_.start(RETRY_TIMEOUT);
  return true;
}

void ServiceClient::onTimeoutRetry(){
  this->connectToHost(host_name_,host_port_);
  qDebug()<<tr("Try to connect to server %1:%2.").arg(host_name_).arg(host_port_);
}

void ServiceClient::onConnected(){
  qDebug()<<tr("Server %1:%2 connected.").arg(this->peerName()).arg(this->peerPort());
  timer_retry_.stop();
}

void ServiceClient::onDisconnected(){
  qDebug()<<tr("Server %1:%2 disconnected.").arg(this->peerName()).arg(this->peerPort());
  timer_retry_.start(RETRY_TIMEOUT);
}

void ServiceClient::onReadyRead(){
  QByteArray rx_raw = read(RX_MAX_SIZE);
  //qDebug()<<tr("[%1,%2]rx size:%3").arg(__FILE__).arg(__LINE__).arg(rx_raw.size());
  protocol_.PushToProtocol(rx_raw);
}
void ServiceClient::onPayloadReady(int cmdid, QByteArray &payload){
  QTextStream stream(&payload);

  //step1.extract the signature
  QStringList first_line = stream.readLine().split(',');
  if(first_line.size()<SIGNATURE_COLS){
    return;
  }
  QString signature = first_line[0];
  qint64 start = first_line[1].toULongLong();
  qint64 end = first_line[2].toULongLong();
#if 0
  qDebug()<<tr("[%1,%2]cmdid:%3,sign:%4,start:%5,end:%6")
            .arg(__FILE__).arg(__LINE__)
            .arg(cmdid)
            .arg(signature)
            .arg(start)
            .arg(end);
#endif
  //step2.extraxt data
  payload.remove(0,signature.size());
  switch(cmdid){
  case SERVER_PATTERN_REQ:
      //step1. find a valid thread
      SweepThread * t = SweepThread::Available();
      //qDebug()<<tr("[%1,%2]Thread_%3 is ready to serve.").arg(__FILE__).arg(__LINE__).arg(t->Index());
      t->StartTask(signature,start,end,payload);
      break;
  }
}
void ServiceClient::onResultReady(QString sign,int type,int total,int cat){
  //qDebug()<<tr("[%1,%2] signature:%3 type:%4 total:%5  cat:%6").arg(__FILE__).arg(__LINE__).arg(sign).arg(type).arg(total).arg(cat)<<endl;
}
int ServiceClient::LoadEngine(const QString& src_path,NeuronEngineFloat & engine){
  if(src_path.isEmpty())return 0;
  QFile src_file(src_path);
  if(!src_file.open(QFile::ReadOnly)){
    qDebug()<<"Fail to open file:"<<src_path;
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
