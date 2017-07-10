#ifndef SWEEPSERVICE_H
#define SWEEPSERVICE_H
#include <QThread>
#include <QVector>
#include "qrineuronlib.h"
#include <configparser.h>

#include "frameclassifier.h"
#include "waveclassifier.h"
class PatternThread : public QThread{
  Q_OBJECT
public:
  typedef QVector<PatternThread*> SweepList;
  static SweepList sweep_list;

  static PatternThread* Available();//get an available thread

  explicit PatternThread(QObject *parent = 0);
  virtual ~PatternThread();
  virtual void run();

  void StartTask(QString& sign,qint64 start,qint64 end,QByteArray& raw_data);
signals:
  void resultReady(QString sign,int type,int total_samples,int cat_samples);

public:
  NeuronEngineFloat engine_key_;
  NeuronEngineFloat engine_feature_;
protected:
  WaveClassifier *feature_;
  FrameClassifier *frame_;

  //gesture basic info
  QString signature_;//the task signature
  qint64 start_time_;
  qint64 end_time_;
  QByteArray raw_data_;
};

#endif // SWEEPSERVICE_H
