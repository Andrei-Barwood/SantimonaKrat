// src/communication/udp_comm.h
#ifndef UDP_COMM_H
#define UDP_COMM_H

#include "common.h"
#include "../protocols/message_protocol.h"

// UDP communication context
typedef struct {
    network_config_t* net_config;
    int broadcast_enabled;
    char broadcast_address[16];
    int broadcast_port;
} udp_context_t;

// Initialize UDP communication
udp_context_t* udp_init(const char* server_ip, int port);

// Send message via UDP
network_result_t udp_send_message(udp_context_t* context, 
                                 const message_header_t* message, 
                                 size_t message_size);

// Receive message via UDP
network_result_t udp_receive_message(udp_context_t* context, 
                                   void* buffer, 
                                   size_t buffer_size);

// Enable/disable broadcast
void udp_set_broadcast(udp_context_t* context, int enabled);

// Cleanup UDP context
void udp_cleanup(udp_context_t* context);

#endif