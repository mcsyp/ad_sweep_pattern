#include <QCoreApplication>
#include <QFile>
#include <QTextStream>

#include "serviceclient.h"
#include "qrineuronlib.h"
#include "configparser.h"

#define CONFIG_FILE "../config.txt"


int main(int argc, char *argv[])
{
  QCoreApplication a(argc, argv);

  //step1. load the config file
  qDebug()<<"Parsing the config file...";
  ConfigParser::ConfigMap configs;
  ConfigParser parser;
  if(!parser.Parse(CONFIG_FILE,configs)){
    qDebug()<<"Fail to load the config file:"<<CONFIG_FILE<<endl;
    exit(0);
  }

  ServiceClient * ptr_client = ServiceClient::Singleton();
  if(!ptr_client->Setup(configs)){
    qDebug()<<"Fail to setup the service client."<<endl;
    exit(-1);
  }

  return a.exec();
}
