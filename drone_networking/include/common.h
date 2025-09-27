// include/common.h
#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>
#include <pthread.h>
#include <time.h>

// Network constants
#define MAX_BUFFER_SIZE 8192
#define MAX_DRONE_ID 32
#define MAX_MESSAGE_TYPE 32
#define DEFAULT_PORT 8888
#define MAX_DRONES 50

// Message types
typedef enum {
    MSG_STATUS_UPDATE,
    MSG_COMMAND,
    MSG_BUILDING_PLAN,
    MSG_HEARTBEAT,
    MSG_ERROR,
    MSG_ACKNOWLEDGMENT
} message_type_t;

// Network modes
typedef enum {
    NETWORK_MODE_CLIENT,
    NETWORK_MODE_SERVER,
    NETWORK_MODE_PEER
} network_mode_t;

// Return codes
typedef enum {
    NET_SUCCESS = 0,
    NET_ERROR = -1,
    NET_TIMEOUT = -2,
    NET_DISCONNECTED = -3
} network_result_t;

#endif