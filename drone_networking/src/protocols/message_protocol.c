// src/protocols/message_protocol.c
#include "message_protocol.h"
#include <time.h>

#define MESSAGE_MAGIC_NUMBER 0xDEADBEEF

void init_message_header(message_header_t* header, message_type_t type, 
                        const char* drone_id) {
    if (!header) return;
    
    header->magic_number = MESSAGE_MAGIC_NUMBER;
    header->message_type = (uint16_t)type;
    header->message_id = (uint16_t)(time(NULL) & 0xFFFF); // Simple ID generation
    header->payload_length = 0;
    header->checksum = 0;
    strncpy(header->drone_id, drone_id, MAX_DRONE_ID - 1);
    header->drone_id[MAX_DRONE_ID - 1] = '\0';
    header->timestamp = (uint64_t)time(NULL);
}

uint32_t calculate_checksum(const void* data, size_t length) {
    const unsigned char* bytes = (const unsigned char*)data;
    uint32_t checksum = 0;
    
    for (size_t i = 0; i < length; i++) {
        checksum += bytes[i];
    }
    
    return checksum;
}

int validate_message(const void* message, size_t length) {
    if (!message || length < sizeof(message_header_t)) {
        return 0;
    }
    
    const message_header_t* header = (const message_header_t*)message;
    
    // Check magic number
    if (header->magic_number != MESSAGE_MAGIC_NUMBER) {
        printf("Invalid magic number\n");
        return 0;
    }
    
    // Check checksum
    uint32_t calculated_checksum = calculate_checksum(message, length);
    if (calculated_checksum != header->checksum) {
        printf("Checksum mismatch\n");
        return 0;
    }
    
    return 1; // Valid message
}

int serialize_message(const void* message, void* buffer, size_t buffer_size) {
    if (!message || !buffer || buffer_size < sizeof(message_header_t)) {
        return -1;
    }
    
    const message_header_t* header = (const message_header_t*)message;
    size_t message_size = sizeof(message_header_t) + header->payload_length;
    
    if (message_size > buffer_size) {
        return -1;
    }
    
    memcpy(buffer, message, message_size);
    
    // Update checksum
    message_header_t* buf_header = (message_header_t*)buffer;
    buf_header->checksum = calculate_checksum(buffer, message_size);
    
    return (int)message_size;
}

int deserialize_message(const void* buffer, size_t length, void* message) {
    if (!buffer || !message || length < sizeof(message_header_t)) {
        return -1;
    }
    
    if (!validate_message(buffer, length)) {
        return -1;
    }
    
    size_t copy_size = (length < sizeof(message_header_t) + ((message_header_t*)buffer)->payload_length) ? 
                      length : sizeof(message_header_t) + ((message_header_t*)buffer)->payload_length;
    
    memcpy(message, buffer, copy_size);
    return (int)copy_size;
}