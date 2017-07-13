#ifndef ServerOpti_H
#define ServerOpti_H
#include <QTcpSocket>
#include <QByteArray>
#include <QTimer>
#include <QVector>

#include <QMutex>

#include <stdint.h>
#include <vector>

#include "serviceprotocol.h"
#include "patternthread.h"

#define HOST_NAME "host"
#define HOST_PORT "port"
#define PATTERN_THREADS "pattern_threads"
class ServiceClient : public QTcpSocket
{
  Q_OBJECT
protected:
  explicit ServiceClient(QObject *parent = 0);

public:
  static constexpr int RETRY_TIMEOUT=3000;
  static constexpr int SIGNATURE_COLS=3;

  enum SERVER_CMDID {
    SERVER_PATTERN_REQ=0x1,
    SERVER_PATTERN_ACK=0x2,
    SERVER_STATUS_ACK=0x3,
  };

  static ServiceClient* Singleton();
  virtual ~ServiceClient();

  bool Setup(ConfigParser::ConfigMap & map);

protected slots:
  void onConnected();
  void onDisconnected();
  void onReadyRead();

  void onTimeoutRetry();

  void onPayloadReady(int cmdid, QByteArray & payload);
  void onProtocolFoundHead(int cmdid,int payload_size);

  void onReportReady(QString report);
protected:
  static ServiceClient*  ptr_instance_;

protected:
  QString host_name_;
  int host_port_;

  QTimer timer_retry_;

  ServiceProtocol protocol_;
  QVector<PatternThread*> sweep_list_;
  QMutex socket_mutex_;
  QMutex pack_mutex_;

  qint64 pack_start_;
};

#endif // ServerOpti_H
