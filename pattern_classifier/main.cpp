#include <QCoreApplication>
#include <QFile>
#include <QIODevice>
#include <QDir>
#include <QTextStream>
#include <qdebug.h>

#include "configparser.h"
#include "csvsamples.h"
#include "multipatternservice.h"

void PushFileToService(QString path, MultiPatternService& patter_service){
  QFile file(path);
  if(!file.open(QIODevice::ReadOnly)){
    qDebug()<<"Fail to open the data source file."<<endl;
    return;
  }
  QTextStream text(&file);
  //load text
  QString file_content = text.readAll();

  MultiPatternService::ResultMap out_map;
  patter_service.Classify(file_content,out_map);
  QList<int> keys = out_map.keys();
  for(int i=0;i<keys.size();++i){
    int type = keys[i];
    qDebug()<<QString("Type[%1], peaks:%2, samples:%3, percent:%4")
              .arg(type)
              .arg(out_map[type].peaks)
              .arg(out_map[type].samples)
              .arg(out_map[type].percentile);
  }

  file.close();
}

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

  //step2.load qfile
  MultiPatternService service;
  service.Setup(work_dir,configs);

#if 1
  PushFileToService(QString(PROCESSED_0),service);
  qDebug()<<"======================================================================"<<endl;

  PushFileToService(QString(PROCESSED_61),service);
  qDebug()<<"======================================================================"<<endl;

  PushFileToService(QString(PROCESSED_60),service);
  qDebug()<<"======================================================================"<<endl;

  PushFileToService(QString(PROCESSED_91),service);
  qDebug()<<"======================================================================"<<endl;

  PushFileToService(QString(PROCESSED_70),service);
  qDebug()<<"======================================================================"<<endl;
#else
  QString path("../../2017-07-22-8-10/863137003290778_2017-07-22 08-00-00_2017-07-22 10-00-00.csv");
  PushFileToService(path,service);
  qDebug()<<"======================================================================"<<endl;
#endif
  return 0;
}
