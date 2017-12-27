/*
 * bluetooth_packet_handler.h
 *
 *  Created on: Dec 27, 2017
 *      Author: james
 */

#ifndef BLUETOOTH_PACKET_HANDLER_H_
#define BLUETOOTH_PACKET_HANDLER_H_

#include <stdint.h>

void handle_new_packet(char* pkt, uint32_t len);

#endif /* BLUETOOTH_PACKET_HANDLER_H_ */
