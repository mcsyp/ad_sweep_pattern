#include <QCoreApplication>
#include <QFile>
#include <QTextStream>

#include "serviceclient.h"
#include "qrineuronlib.h"
#include "configparser.h"

#define CONFIG_FILE "../config.txt"

ConfigParser::ConfigMap g_configs;
int main(int argc, char *argv[])
{
  QCoreApplication a(argc, argv);

  //step1. load the config file
  qDebug()<<"Parsing the config file...";
  ConfigParser parser;
  if(!parser.Parse(CONFIG_FILE,g_configs)){
    qDebug()<<"Fail to load the config file:"<<CONFIG_FILE<<endl;
    exit(0);
  }

  //step2.setup and connet to host
  QString hostname = g_configs["host"];
  int hostport = g_configs["port"].remove('\"').toInt();
  qDebug()<<"Connecting to the server:"<<hostname<<":"<<hostport;
  ServiceClient client;
  client.Setup(hostname,hostport);

  return a.exec();
}
