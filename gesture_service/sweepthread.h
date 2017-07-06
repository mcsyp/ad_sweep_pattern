#ifndef SWEEPSERVICE_H
#define SWEEPSERVICE_H
#include <QThread>
#include <QVector>
#include "qrineuronlib.h"
#include <configparser.h>
#include "../sweep_feature_trainer/sweepfeatureclassifier.h"


class SweepThread : public QThread{
  Q_OBJECT
public:

#define SWEEP_KEY "sweep_key"
#define SWEEP_FEATURE "sweep_feature"
  typedef QVector<SweepThread*> SweepList;
  static SweepList sweep_list;

  static void Remove(int index);
  static SweepThread* Available();//get an available thread
  static void ReleaseAll();

  explicit SweepThread(QObject *parent = 0);
  virtual ~SweepThread();
  virtual void run();

  void StartTask(QString& sign,QByteArray& raw_data);

signals:
  void resultReady(QString & sign);

public:
  NeuronEngineFloat engine_key_;
  NeuronEngineFloat engine_feature_;
protected:
  SweepFeatureClassifier *feature_;
  KeyAxSelectorClassifier *selector_;

  //gesture basic info
  QString signature_;//the task signature
  QByteArray raw_data_;

private:
  int index_;

};

#endif // SWEEPSERVICE_H
