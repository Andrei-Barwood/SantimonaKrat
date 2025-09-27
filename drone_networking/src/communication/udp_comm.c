// src/communication/udp_comm.c
#include "udp_comm.h"

udp_context_t* udp_init(const char* server_ip, int port) {
    udp_context_t* context = (udp_context_t*)malloc(sizeof(udp_context_t));
    if (!context) return NULL;
    
    memset(context, 0, sizeof(udp_context_t));
    
    context->net_config = network_init(server_ip, port, NETWORK_MODE_CLIENT);
    if (!context->net_config) {
        free(context);
        return NULL;
    }
    
    strncpy(context->broadcast_address, "255.255.255.255", 15);
    context->broadcast_port = port;
    context->broadcast_enabled = 0;
    
    printf("UDP communication initialized\n");
    return context;
}

network_result_t udp_send_message(udp_context_t* context, 
                                 const message_header_t* message, 
                                 size_t message_size) {
    if (!context || !message) return NET_ERROR;
    
    // Serialize message
    char buffer[MAX_BUFFER_SIZE];
    int serialized_size = serialize_message(message, buffer, sizeof(buffer));
    if (serialized_size < 0) {
        return NET_ERROR;
    }
    
    return network_send_data(context->net_config, buffer, serialized_size);
}

network_result_t udp_receive_message(udp_context_t* context, 
                                   void* buffer, 
                                   size_t buffer_size) {
    if (!context || !buffer) return NET_ERROR;
    
    network_result_t result = network_receive_data(context->net_config, buffer, buffer_size);
    if (result == NET_SUCCESS) {
        // Validate received message
        if (!validate_message(buffer, buffer_size)) {
            return NET_ERROR;
        }
    }
    
    return result;
}

void udp_set_broadcast(udp_context_t* context, int enabled) {
    if (context) {
        context->broadcast_enabled = enabled;
        
        int broadcast = enabled ? 1 : 0;
        if (setsockopt(context->net_config->socket_fd, SOL_SOCKET, SO_BROADCAST, 
                      &broadcast, sizeof(broadcast)) < 0) {
            perror("Set broadcast option failed");
        }
    }
}

void udp_cleanup(udp_context_t* context) {
    if (context) {
        if (context->net_config) {
            network_cleanup(context->net_config);
        }
        free(context);
    }
}