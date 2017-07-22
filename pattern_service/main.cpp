#include <QCoreApplication>
#include <QFile>
#include <QTextStream>
#include <QDir>

#include "serviceclient.h"
#include "qrineuronlib.h"
#include "configparser.h"

int main(int argc, char *argv[])
{
  QCoreApplication a(argc, argv);

  //step1.check input
  if(argc<2){
    qDebug()<<"Invalid parameters.";
    qDebug()<<"usage: pattern_service config.txt";
    exit(0);
  }
  //step2. load the config file
  qDebug()<<"Parsing the config file...";
  ConfigParser::ConfigMap configs;
  ConfigParser parser;
  if(!parser.Parse(argv[1],configs)){
    qDebug()<<"Fail to load the config file:"<<argv[1]<<endl;
    exit(0);
  }

  //step3. find the work_path
  QString config_path(argv[1]);
  QDir work_path(config_path);
  QString work_dir = work_path.absolutePath();
  work_dir.remove(work_dir.split('/').last());
  qDebug()<<work_dir;


#if 1
  ServiceClient * ptr_client = ServiceClient::Singleton();
  if(!ptr_client->Setup(work_dir,configs)){
    qDebug()<<"Fail to setup the service client."<<endl;
    exit(-1);
  }
#endif

  return a.exec();
}
