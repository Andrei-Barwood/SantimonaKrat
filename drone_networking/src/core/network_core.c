// src/core/network_core.c
#include "network_core.h"
#include <fcntl.h>

network_config_t* network_init(const char* server_ip, int port, network_mode_t mode) {
    network_config_t* config = (network_config_t*)malloc(sizeof(network_config_t));
    if (!config) {
        fprintf(stderr, "Failed to allocate network config\n");
        return NULL;
    }
    
    // Initialize structure
    memset(config, 0, sizeof(network_config_t));
    
    // Set configuration values
    strncpy(config->server_ip, server_ip, 15);
    config->server_ip[15] = '\0';
    config->server_port = port;
    config->local_port = 0; // Will be assigned by system
    config->mode = mode;
    config->socket_fd = -1;
    config->is_connected = 0;
    config->max_retries = 5;
    config->retry_delay = 2;
    
    // Initialize addresses
    memset(&config->server_addr, 0, sizeof(struct sockaddr_in));
    memset(&config->local_addr, 0, sizeof(struct sockaddr_in));
    
    config->server_addr.sin_family = AF_INET;
    config->server_addr.sin_port = htons(port);
    config->server_addr.sin_addr.s_addr = inet_addr(server_ip);
    
    config->local_addr.sin_family = AF_INET;
    config->local_addr.sin_port = htons(config->local_port);
    config->local_addr.sin_addr.s_addr = INADDR_ANY;
    
    printf("Network initialized: %s:%d (Mode: %d)\n", server_ip, port, mode);
    return config;
}

network_result_t network_connect(network_config_t* config) {
    if (!config) return NET_ERROR;
    
    // Create socket
    config->socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (config->socket_fd < 0) {
        perror("Socket creation failed");
        return NET_ERROR;
    }
    
    // Bind socket for server mode or set up for client
    if (config->mode == NETWORK_MODE_SERVER) {
        if (bind(config->socket_fd, (struct sockaddr*)&config->local_addr, 
                 sizeof(config->local_addr)) < 0) {
            perror("Bind failed");
            close(config->socket_fd);
            return NET_ERROR;
        }
        printf("Server bound to port %d\n", ntohs(config->local_addr.sin_port));
    }
    
    config->is_connected = 1;
    printf("Network connection established\n");
    return NET_SUCCESS;
}

network_result_t network_disconnect(network_config_t* config) {
    if (!config || config->socket_fd < 0) return NET_ERROR;
    
    close(config->socket_fd);
    config->socket_fd = -1;
    config->is_connected = 0;
    printf("Network disconnected\n");
    return NET_SUCCESS;
}

network_result_t network_send_data(network_config_t* config, const void* data, size_t length) {
    if (!config || !data || !config->is_connected) return NET_ERROR;
    
    ssize_t bytes_sent = sendto(config->socket_fd, data, length, 0,
                               (struct sockaddr*)&config->server_addr,
                               sizeof(config->server_addr));
    
    if (bytes_sent < 0) {
        perror("Send failed");
        return NET_ERROR;
    }
    
    printf("Sent %zd bytes of data\n", bytes_sent);
    return NET_SUCCESS;
}

network_result_t network_receive_data(network_config_t* config, void* buffer, size_t max_length) {
    if (!config || !buffer || !config->is_connected) return NET_ERROR;
    
    socklen_t addr_len = sizeof(struct sockaddr_in);
    ssize_t bytes_received = recvfrom(config->socket_fd, buffer, max_length, 0,
                                     (struct sockaddr*)&config->server_addr, &addr_len);
    
    if (bytes_received < 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            return NET_TIMEOUT;
        }
        perror("Receive failed");
        return NET_ERROR;
    }
    
    printf("Received %zd bytes of data\n", bytes_received);
    return NET_SUCCESS;
}

void network_cleanup(network_config_t* config) {
    if (config) {
        if (config->is_connected) {
            network_disconnect(config);
        }
        free(config);
    }
}

void network_get_stats(network_config_t* config, network_stats_t* stats) {
    if (!config || !stats) return;
    
    // This would be populated in a real implementation
    stats->bytes_sent = 0;
    stats->bytes_received = 0;
    stats->messages_sent = 0;
    stats->messages_received = 0;
    stats->errors = 0;
    stats->last_activity = time(NULL);
}

void network_print_stats(network_config_t* config) {
    network_stats_t stats;
    network_get_stats(config, &stats);
    
    printf("=== Network Statistics ===\n");
    printf("Bytes Sent: %lu\n", stats.bytes_sent);
    printf("Bytes Received: %lu\n", stats.bytes_received);
    printf("Messages Sent: %lu\n", stats.messages_sent);
    printf("Messages Received: %lu\n", stats.messages_received);
    printf("Errors: %lu\n", stats.errors);
    printf("Last Activity: %s", ctime(&stats.last_activity));
}