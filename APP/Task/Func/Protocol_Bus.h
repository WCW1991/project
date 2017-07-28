#ifndef __PROTOCOL_BUS_H
#define __PROTOCOL_BUS_H

#include "includes.h"

ErrorStatus send_reset( Terminal_enum terminal );
ErrorStatus report_output(uint8_t state);
ErrorStatus report_record( Record_struct *Record );
ErrorStatus poll_state_board( uint8_t *state );
ErrorStatus poll_data_card( uint8_t card_id, uint8_t **data );
ErrorStatus send_info_card( uint8_t Card_ID );

ErrorStatus charge_addr_card( uint8_t card_id, uint8_t old_addr, uint8_t new_addr );
ErrorStatus auto_info_card( uint8_t card_id );
ErrorStatus get_info_card( uint8_t card_id, uint8_t **data );
ErrorStatus get_data_detector( uint8_t card_id, uint8_t detector_id, uint8_t **data );

ErrorStatus send_cmd_common( Terminal_enum terminal, uint8_t cmd, uint8_t dest );

ErrorStatus get_threshold_detector( uint8_t card_id, uint8_t detector_id, uint8_t channel, uint16_t *value, uint8_t *state );
ErrorStatus set_threshold_detector( uint8_t card_id, uint8_t detector_id, uint8_t channel, uint16_t value, uint8_t *state );
ErrorStatus calibration_detector( uint8_t card_id, uint8_t detector_id, uint8_t channel, uint16_t value, uint8_t *state );
ErrorStatus P_read_threshold_detector( uint8_t card_id, uint8_t detector_id, uint8_t type, uint16_t *ValueL, uint16_t *ValueH, uint8_t *state );
ErrorStatus P_set_threshold_detector( uint8_t card_id, uint8_t detector_id, uint8_t type, uint16_t ValueL, uint16_t ValueH, uint8_t *state );

#endif
