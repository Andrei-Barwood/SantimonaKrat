// src/main.c
#include "common.h"
#include "core/network_core.h"
#include "protocols/message_protocol.h"
#include "communication/udp_comm.h"

void demonstrate_networking() {
    printf("=== Drone Networking Demonstration ===\n");
    
    // Initialize UDP communication
    udp_context_t* udp = udp_init("127.0.0.1", 8888);
    if (!udp) {
        fprintf(stderr, "Failed to initialize UDP\n");
        return;
    }
    
    // Create a status update message
    status_update_message_t status_msg;
    init_message_header(&status_msg.header, MSG_STATUS_UPDATE, "DRONE_01");
    status_msg.header.payload_length = sizeof(status_update_message_t) - sizeof(message_header_t);
    
    // Fill in status data
    status_msg.x = 100.0;
    status_msg.y = 200.0;
    status_msg.z = 300.0;
    status_msg.battery_level = 85.5;
    status_msg.state = 1; // IDLE
    status_msg.construction_progress = 25;
    
    printf("Sending status update message...\n");
    network_result_t result = udp_send_message(udp, (message_header_t*)&status_msg, 
                                              sizeof(status_update_message_t));
    
    if (result == NET_SUCCESS) {
        printf("Message sent successfully!\n");
    } else {
        printf("Failed to send message: %d\n", result);
    }
    
    // Receive a message (this would typically be in a separate thread)
    char receive_buffer[MAX_BUFFER_SIZE];
    printf("Attempting to receive message...\n");
    result = udp_receive_message(udp, receive_buffer, sizeof(receive_buffer));
    
    if (result == NET_SUCCESS) {
        printf("Message received successfully!\n");
    } else {
        printf("Failed to receive message: %d\n", result);
    }
    
    // Cleanup
    udp_cleanup(udp);
    printf("Networking demonstration complete.\n");
}

int main() {
    printf("Interplanetary Construction Drone Networking System\n");
    printf("Template Project - Ready for Cookbook Implementation\n\n");
    
    demonstrate_networking();
    
    return 0;
}