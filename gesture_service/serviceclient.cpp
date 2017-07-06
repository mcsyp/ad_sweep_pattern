#include "serviceclient.h"
#include <qdebug.h>
#include <stdio.h>
#include <QTextStream>

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
  protocol_.PushToProtocol(rx_raw);
}
void ServiceClient::onPayloadReady(int cmdid, QByteArray &payload){
  QTextStream stream(&payload);

  //step1.extract the signature
  QString signature = stream.readLine();
  qDebug()<<tr("[%1,%2]cmdid:%3,sign:%4").arg(__FILE__).arg(__LINE__).arg(cmdid).arg(signature);

  //step2.extraxt data
  payload.remove(0,signature.size());
  switch(cmdid){
  case SERVER_PATTERN_REQ:
      //step1. find a valid thread
      break;
  }
}
