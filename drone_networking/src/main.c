// src/main.c
#include "common.h"
#include "core/network_core.h"
#include "protocols/message_protocol.h"
#include "communication/udp_comm.h"
#include "security/encryption_wrapper.h"

void demonstrate_secure_networking() {
    printf("=== Secure Drone Networking Demonstration ===\n");
    
    // Initialize security context
    security_context_t* security = security_init("MY_SECURE_DRONE_KEY_32_CHARS", "MY_IV_16_CHARS");
    if (!security) {
        fprintf(stderr, "Failed to initialize security\n");
        return;
    }
    
    // Initialize UDP communication
    udp_context_t* udp = udp_init("127.0.0.1", 8888);
    if (!udp) {
        fprintf(stderr, "Failed to initialize UDP\n");
        security_cleanup(security);
        return;
    }
    
    // Create a status update message
    status_update_message_t status_msg;
    init_message_header(&status_msg.header, MSG_STATUS_UPDATE, "DRONE_01_SECURE");
    status_msg.header.payload_length = sizeof(status_update_message_t) - sizeof(message_header_t);
    
    // Fill in status data
    status_msg.x = 100.0;
    status_msg.y = 200.0;
    status_msg.z = 300.0;
    status_msg.battery_level = 85.5;
    status_msg.state = 1; // IDLE
    status_msg.construction_progress = 25;
    
    printf("Sending SECURE status update message...\n");
    
    // Use secure send function
    network_result_t result = secure_send_message(security, 
                                                udp->net_config,
                                                &status_msg, 
                                                sizeof(status_update_message_t));
    
    if (result == NET_SUCCESS) {
        printf("Secure message sent successfully!\n");
    } else {
        printf("Failed to send secure message: %d\n", result);
    }
    
    // Receive a message securely
    char secure_receive_buffer[MAX_BUFFER_SIZE];
    printf("Attempting to receive secure message...\n");
    
    result = secure_receive_message(security, 
                                  udp->net_config, 
                                  secure_receive_buffer, 
                                  sizeof(secure_receive_buffer));
    
    if (result == NET_SUCCESS) {
        printf("Secure message received and verified successfully!\n");
    } else {
        printf("Failed to receive secure message: %d\n", result);
    }
    
    // Cleanup
    udp_cleanup(udp);
    security_cleanup(security);
    printf("Secure networking demonstration complete.\n");
}

int main() {
    printf("Interplanetary Construction Drone Secure Networking System\n");
    printf("Mithril Security API Integration\n\n");
    
    demonstrate_secure_networking();
    
    return 0;
}