#include "InfantryInfo.h"

void pc_data_handler(u8 *p_frame, send_pc_t &pc_send_mesg)
{
  frame_header_t *p_header = (frame_header_t*)p_frame;
  memcpy(p_header, p_frame, HEADER_LEN);

  u8 data_length = p_frame[1];//10

  unsigned char cmd_id = p_frame[5] ;//17

  u8 *data_addr = p_frame + HEADER_LEN + CMD_LEN-1; //-1

  //taskENTER_CRITICAL();

  switch (cmd_id)
  {
//    case CHASSIS_DATA_ID:
//      memcpy(&pc_send_mesg.chassis_information, data_addr, data_length);
//      break;

//    case GIMBAL_DATA_ID:
//      memcpy(&pc_send_mesg.gimbal_information, data_addr, data_length);
//      break;

//    case SHOOT_TASK_DATA_ID:
//      memcpy(&pc_send_mesg.shoot_task_information, data_addr, data_length);
//     break;

//    case INFANTRY_ERR_ID:
//      memcpy(&pc_send_mesg.bottom_error_data, data_addr, data_length);
//    break;

//    case CONFIG_RESPONSE_ID:
//      memcpy(&pc_send_mesg.structure_config_data, data_addr, data_length);
//    break;

//    case CALI_RESPONSE_ID:
//      memcpy(&pc_send_mesg.cali_response_data, data_addr, data_length);
//    break;

//    case REMOTE_CTRL_INFO_ID:
//      memcpy(&pc_send_mesg.remote_ctrl_data, data_addr, data_length);
//     break;

//    case BOTTOM_VERSION_ID:
//      memcpy(&pc_send_mesg.version_info_data, data_addr, data_length);
//    break;

  case SENTRY_DATA_ID:
    memcpy(&pc_send_mesg.stm32_info_data, data_addr, data_length);
    break;
  }
  return;
}