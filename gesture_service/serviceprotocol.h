#ifndef SERVICEPROTOCOL_H
#define SERVICEPROTOCOL_H


#include <QByteArray>
#include <QTimer>

#include <stdint.h>
#include <vector>

class ServiceProtocol: public QObject{
  Q_OBJECT
public:
  /**************************************
   **SERVER PROTOCOL RELATED**
   **************************************/
  //magic number
  static constexpr int  MAGIC_NUM1=0x62;
  static constexpr int  MAGIC_NUM2=0x65;
  static constexpr int  MAGIC_NUM3=0x6e;
  static constexpr int  MAGIC_NUM4=0x7a;
  static constexpr int  MAGIC_NUM_LEN=4;
  //magic struct design
#pragma pack (1)
  typedef struct message_head_s{
    uint8_t magic_num1;
    uint8_t magic_num2;
    uint8_t magic_num3;
    uint8_t magic_num4;
    uint8_t cmdId;
    uint32_t len;//this is the length of the total pack include head size.
                 //e.g. len=HEAD_SIZE+PAYLOAD_SIZE
  }message_head;
#pragma pack()
  static constexpr int MESSAGE_HEAD_LEN = sizeof(message_head);

  ServiceProtocol(QObject* parent=NULL);

  void PushToProtocol(QByteArray & raw_data);

  void Reset();

  /*purpose: find if the input data is a message header
   *input:
   * @rx_buffer, the rx buffer from the client
   * @rx_len ,the length of the rx buffer
   *return:
   * returns true if success
   */
  static bool CheckMessageHead(uint8_t* rx_buffer, int rx_len);

  /*purpose:fill the message head of the buffer
   *input:
   * @cmdid, the command id
   * @payload_len, the length of payload, HEAD size not included
   *output:
   * @head, the output filled message head
   */
  static void FillHead(uint16_t cmdid, uint16_t payload_len, message_head & head);

signals:
  void foundHead(int cmdid, int payload_size);
  void payloadReady(int cmdid, QByteArray & payload);

private:
  enum RxState{
    RxStateIdle=0, //no package is procesing
    RxStateProcessing=1, //processing a package
  };
  message_head rx_message_head_;//rx message head
  enum RxState rx_state_;

  //rx buffer & message
  QByteArray rx_payload_;//rx queue, waiting to be processed
  int rx_payload_size_;
};

#endif // SERVICEPROTOCOL_H
