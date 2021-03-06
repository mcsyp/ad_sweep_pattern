#include "serviceprotocol.h"
#include <QDebug>
ServiceProtocol::ServiceProtocol(QObject* parent):QObject(parent){
  rx_state_ = RxStateIdle;
  rx_payload_size_=0;
}

void ServiceProtocol::Reset(){
  //reset rx message state
  rx_payload_.clear();
  rx_state_ = RxStateIdle;
  rx_payload_size_=0;
}

void ServiceProtocol::PushToProtocol(QByteArray & raw_data){
  int i=0;
  while(i<raw_data.size()){
    uint8_t * current = (uint8_t*)(raw_data.data()+i);
    int delta=1;
    switch(rx_state_){
    case RxStateProcessing:
        //if there is a previous head
        if(rx_payload_.size()<rx_payload_size_){
          rx_payload_.push_back(raw_data.at(i));
        }
        break;
    case RxStateIdle:
        //step1. check if there is are head since this byte
        if(i<=raw_data.size()-MESSAGE_HEAD_LEN &&
           CheckMessageHead(current,MAGIC_NUM_LEN)){

          //step1.fill the head
           memcpy((uint8_t*)&rx_message_head_,current,sizeof(rx_message_head_));
           rx_payload_size_ = rx_message_head_.len-MESSAGE_HEAD_LEN;
           rx_payload_.clear();//clear the rx payload
           emit foundHead(rx_message_head_.cmdId,rx_payload_size_);
           //step2.update  the state
           rx_state_ = RxStateProcessing;

           //step3. i jump to payload
           delta = MESSAGE_HEAD_LEN;
        }
        break;
    }

    if(rx_payload_.size()>=rx_payload_size_){
      //check if a package is full
      emit payloadReady(rx_message_head_.cmdId,rx_payload_);
      rx_state_ = RxStateIdle;
      rx_payload_.clear();
    }

    //increase
    i=i+delta;
  }
}

bool ServiceProtocol::CheckMessageHead(uint8_t * rx_buffer, int rx_len){
  if(rx_buffer==NULL || rx_len<MAGIC_NUM_LEN){
    return false;
  }
  return (rx_buffer[0]==MAGIC_NUM1 &&
          rx_buffer[1]==MAGIC_NUM2 &&
          rx_buffer[2]==MAGIC_NUM3 &&
          rx_buffer[3]==MAGIC_NUM4);
}

void ServiceProtocol::FillHead(uint16_t cmdid, uint16_t payload_len, message_head & head){
  head.cmdId = cmdid;
  head.len = MESSAGE_HEAD_LEN + payload_len;
  head.magic_num1  = MAGIC_NUM1;
  head.magic_num2  = MAGIC_NUM2;
  head.magic_num3  = MAGIC_NUM3;
  head.magic_num4  = MAGIC_NUM4;
}

