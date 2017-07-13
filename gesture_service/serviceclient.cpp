#include "serviceclient.h"
#include <qdebug.h>
#include <stdio.h>
#include <QTextStream>
#include <QDateTime>
#include "global_service.h"
ServiceClient* ServiceClient::ptr_instance_ = NULL;
ServiceClient::ServiceClient(QObject *parent) :QTcpSocket(parent)
{
  connect(this,SIGNAL(connected()),this,SLOT(onConnected()));
  connect(this,SIGNAL(disconnected()),this,SLOT(onDisconnected()));
  connect(this,SIGNAL(readyRead()),this,SLOT(onReadyRead()));

  connect(&timer_retry_,SIGNAL(timeout()),this,SLOT(onTimeoutRetry()));
  connect(&protocol_,SIGNAL(payloadReady(int,QByteArray&)),
          this,SLOT(onPayloadReady(int,QByteArray&)));
  connect(&protocol_,SIGNAL(foundHead(int,int)),this,SLOT(onProtocolFoundHead(int,int)));
}

ServiceClient *ServiceClient::Singleton(){
  if(ptr_instance_==NULL){
    ptr_instance_ = new ServiceClient;
  }
  return ptr_instance_;
}

ServiceClient::~ServiceClient(){
  //reset client
  this->disconnectFromHost();
  for(auto iter = PatternThread::pattern_list.begin();iter!=PatternThread::pattern_list.end();++iter){
    PatternThread* thread = *iter;
    if(thread){
      thread->deleteLater();
    }
  }
  QThread::msleep(100);
}

bool ServiceClient::Setup(ConfigParser::ConfigMap & configs){
  if(configs.size()==0)return false;
  if(!configs.contains(HOST_NAME) ||
     !configs.contains(HOST_PORT) ||
     !configs.contains(PATTERN_THREADS)){
    qDebug()<<tr("[%1,%2]Fail to find the key words in configs.").arg(__FILE__).arg(__LINE__);
    return false;
  }

  //step1.init sweep & load engines
  int pattern_threads =  configs[PATTERN_THREADS].remove('\"').toInt();
  if(pattern_threads<=0 ||  pattern_threads>PatternThread::MAX_THREADS){
    qDebug()<<tr("[%1,%2]Invalid pattern threads [%3]. Max number is %4")
              .arg(__FILE__).arg(__LINE__)
              .arg(pattern_threads)
              .arg(PatternThread::MAX_THREADS);
    return false;
  }

  for(int i=0;i<pattern_threads;++i){
    PatternThread * ptr_thread = new PatternThread;
    if(!ptr_thread->Setup(configs)){
      qDebug()<<tr("[%1,%2] Fail to create the [%3]th thread.\n").arg(__FILE__).arg(__LINE__).arg(i);
      return false;
    }
    qDebug()<<tr("[%1] pattern thread created.").arg(PatternThread::pattern_list.size());
    connect(ptr_thread,SIGNAL(reportCsvReady(QString)),this,SLOT(onReportReady(QString)));
  }

  //setp2. setup server
  host_name_ = configs[HOST_NAME];
  host_port_ = configs[HOST_PORT].remove('\"').toInt();
  qDebug()<<"Connecting to host["<<host_name_<<":"<<host_port_<<"] ...";
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
  QByteArray rx_raw = readAll();
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

  //step2.extraxt data
  payload.remove(0,signature.size());
  switch(cmdid){
  case SERVER_PATTERN_REQ:
      //step1. find a valid thread
      PatternThread * t = PatternThread::Available();
      t->pack_start = pack_start_;
      t->pack_end = QDateTime::currentSecsSinceEpoch();
      qDebug()<<tr("[%1,%2] it takes %3 secs to receive this pack.").arg(__FILE__).arg(__LINE__).arg(t->pack_end-t->pack_start);
      t->StartTask(signature,start,end,payload);
      break;
  }
}

void ServiceClient::onProtocolFoundHead(int cmdid, int payload_size)
{
  switch(cmdid){
  case SERVER_PATTERN_REQ:
      pack_mutex_.lock();
      pack_start_ = QDateTime::currentSecsSinceEpoch();
      pack_mutex_.unlock();
      qDebug()<<endl<<tr("[%1,%2] found [DATA HEAD]. Payload size is:%3").arg(__FILE__).arg(__LINE__).arg(payload_size);
      break;
  }

}
void ServiceClient::onReportReady(QString report){
  qDebug()<<tr("[%1,%2] report:").arg(__FILE__).arg(__LINE__);
  qDebug()<<report;

  socket_mutex_.lock();
  ServiceProtocol::message_head head;
  ServiceProtocol::FillHead(SERVER_PATTERN_ACK,report.size(),head);
  QByteArray tx_pack;
  tx_pack.append((char*)&head,sizeof(head));
  tx_pack.append(report);
  write(tx_pack);
  socket_mutex_.unlock();
}

