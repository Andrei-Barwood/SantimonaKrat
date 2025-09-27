// src/protocols/message_protocol.h
#ifndef MESSAGE_PROTOCOL_H
#define MESSAGE_PROTOCOL_H

#include "common.h"

// Message header structure
typedef struct {
    uint32_t magic_number;      // Magic number for validation
    uint16_t message_type;      // Type of message
    uint16_t message_id;        // Unique message ID
    uint32_t payload_length;    // Length of payload data
    uint32_t checksum;          // Message checksum
    char drone_id[MAX_DRONE_ID]; // Source drone ID
    uint64_t timestamp;         // Message timestamp
} message_header_t;

// Status update message
typedef struct {
    message_header_t header;
    double x, y, z;             // Position
    double vx, vy, vz;          // Velocity
    double battery_level;       // Battery percentage
    int state;                  // Drone state
    int construction_progress;  // Construction progress percentage
} status_update_message_t;

// Command message
typedef struct {
    message_header_t header;
    int command_type;           // Type of command
    double target_x, target_y, target_z; // Target coordinates
    int priority;               // Command priority
    char payload[1024];         // Additional command data
} command_message_t;

// Building plan message
typedef struct {
    message_header_t header;
    int total_components;       // Total building components
    int completed_components;   // Completed components
    int building_type;          // Type of building
    char building_name[64];     // Building name
    // Components would be stored separately
} building_plan_message_t;

// Initialize message header
void init_message_header(message_header_t* header, message_type_t type, 
                        const char* drone_id);

// Calculate message checksum
uint32_t calculate_checksum(const void* data, size_t length);

// Validate message integrity
int validate_message(const void* message, size_t length);

// Serialize message to buffer
int serialize_message(const void* message, void* buffer, size_t buffer_size);

// Deserialize message from buffer
int deserialize_message(const void* buffer, size_t length, void* message);

#endif