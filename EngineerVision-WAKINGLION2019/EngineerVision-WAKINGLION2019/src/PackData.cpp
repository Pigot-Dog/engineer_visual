#include "PackData.h"

PackData::PackData()
{

}

UnpackData::UnpackData()
{

}

void PackData::processing(Serial *serial)
{
    while(1)
    {
        process(serial);
    }
}

void PackData::process(Serial *serial)
{
    u8 tx_buf[50]={0};
    packData(GIMBAL_CTRL_ID,
             (u8 *)&pc2stm_mesg.chassis_control_data,
             sizeof(pc2stm_mesg.chassis_control_data),tx_buf);
    serial->writing(tx_buf,sizeof(tx_buf));
    //usleep(100);
}

u8* PackData::packData(u16 cmd_id, u8 *p_data, u16 len,u8 *tx_buf)
{
    //memset(tx_buf, 0, 100);
    //static u8 seq;

    u16 frame_length = HEADER_LEN + CMD_LEN + len + CRC_LEN-1;
    frame_header_t *p_header = (frame_header_t*)tx_buf;

    p_header->sof          = UP_REG_ID;
    //p_header->data_length  = len;
    memcpy(&tx_buf[1], (u8*)&len, 1);
    //p_header->data_length  = p_header->data_length<<8;
    //p_header[1] = (u16)len;
    //p_header->seq          = 0;
    p_header->seq++;

    memcpy(&tx_buf[HEADER_LEN-1], (u8*)&cmd_id, CMD_LEN);
    append_crc8_check_sum(tx_buf, HEADER_LEN-1);
    memcpy(&tx_buf[HEADER_LEN + CMD_LEN-1], p_data, len);
    append_crc16_check_sum(tx_buf, frame_length);

    return tx_buf;
}


void UnpackData::processing(Serial *serial)
{
    while(1)
    {
        process(serial);
    }
}

void UnpackData::process(Serial *serial)
{
    u8 buf[UNPACK_DATA_BUFFSIZE];
    u16 size=0;
    serial->reading(buf,size);
//    std::cout<<"size: "<<size<<std::endl;
    blockData(buf,size);
#ifndef USE_WIN
//    unsleep(1000);
#endif // !USE_WIN
}

void UnpackData::blockData(u8 *data, u16 &size)
{
    unpackData(data,size);
}

void UnpackData::unpackData(u8 *data, int size)
{
    // u8 framehearder[5] = {0};
    static u8 * protocol_packet =NULL;
    //unsigned char crc_data[100] = {0};
    int i = 0;
    int start = 0;
    u32 datalen  = 0;

    if(size < 9){
        return;
    }
    for(;;)
    {
        if(start >= size -5) {
            return;
        }
        if(data[start] == 0XA0 ){
            break;
        }
        start++;
    }

    datalen = (u32)data[1+start] + 9;//25 29 17
    protocol_packet = (u8 *)malloc(sizeof(u8)*datalen);
    for(i =0; i< (int)datalen ; i++){
        protocol_packet[i] = data[start+i];
//        std::cout<<i<<":";
//        printf("%X ",protocol_packet[i]);
    }
//    std::cout<<std::endl;

    if( verify_crc8_check_sum(protocol_packet, sizeof(frame_header_t)-1) !=true ){
        return;
    }

    if(verify_crc16_check_sum(protocol_packet, datalen) !=true){
        return;
    }

    pc_data_handler(protocol_packet,stm2pc_mesg);
    free(protocol_packet);
    protocol_packet = NULL;
    start +=datalen;

    if (start + 1 < size){
        unpackData(&data[ start + 1], size - start - 1);
    }
}
