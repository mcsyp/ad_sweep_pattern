#ifndef ServerOpti_H
#define ServerOpti_H
#include <QTcpSocket>
#include <QByteArray>
#include <QTimer>
#include <QVector>

#include <stdint.h>
#include <vector>

#include "serviceprotocol.h"
#include "sweepthread.h"

class ServiceClient : public QTcpSocket
{
  Q_OBJECT
public:
  static constexpr int RX_MAX_SIZE=4096;
  static constexpr int RETRY_TIMEOUT=3000;
  enum SERVER_CMDID {
    SERVER_PATTERN_REQ=0x1,
    SERVER_PATTERN_ACK=0x2,
    SERVER_STATUS_ACK=0x3,
  };
#define SWEEP_CONFIG_KEY     "sweep_key"
#define SWEEP_CONFIG_FEATURE "sweep_feature"
#define SWEEP_MAX_THREADS 10

  explicit ServiceClient(QObject *parent = 0);
  virtual ~ServiceClient();

  bool Setup(QString name,int port);
protected slots:
  void onConnected();
  void onDisconnected();
  void onReadyRead();

  void onTimeoutRetry();
  void onPayloadReady(int cmdid, QByteArray & payload);

protected:
  QString host_name_;
  int host_port_;
  QTimer timer_retry_;
  ServiceProtocol protocol_;
  QVector<SweepThread*> sweep_list_;
};

#endif // ServerOpti_H
