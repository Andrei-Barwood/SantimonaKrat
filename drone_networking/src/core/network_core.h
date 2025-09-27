// src/core/network_core.h
#ifndef NETWORK_CORE_H
#define NETWORK_CORE_H

#include "common.h"
#include "../../include/common.h"

// Network configuration structure
typedef struct {
    char server_ip[16];
    int server_port;
    int local_port;
    network_mode_t mode;
    int socket_fd;
    struct sockaddr_in server_addr;
    struct sockaddr_in local_addr;
    pthread_t network_thread;
    int is_connected;
    int max_retries;
    int retry_delay;
} network_config_t;

// Network statistics
typedef struct {
    unsigned long bytes_sent;
    unsigned long bytes_received;
    unsigned long messages_sent;
    unsigned long messages_received;
    unsigned long errors;
    time_t last_activity;
} network_stats_t;

// Function prototypes
network_config_t* network_init(const char* server_ip, int port, network_mode_t mode);
void network_cleanup(network_config_t* config);
network_result_t network_connect(network_config_t* config);
network_result_t network_disconnect(network_config_t* config);
network_result_t network_send_data(network_config_t* config, const void* data, size_t length);
network_result_t network_receive_data(network_config_t* config, void* buffer, size_t max_length);
void network_get_stats(network_config_t* config, network_stats_t* stats);
void network_print_stats(network_config_t* config);

#endif