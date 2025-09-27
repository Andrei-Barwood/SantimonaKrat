// src/security/encryption_wrapper.h
#ifndef ENCRYPTION_WRAPPER_H
#define ENCRYPTION_WRAPPER_H

#include "common.h"
#include "../protocols/message_protocol.h"

// Include Mithril headers (adjust path as needed)
#include "../security/mithril/Mithril_Cryptography_API.xcodeproj"

// Security context structure
typedef struct {
    mithril_context_t* mithril_ctx;
    char encryption_key[32];    // AES-256 key
    char iv[16];               // Initialization vector
    int encryption_enabled;
    int authentication_enabled;
} security_context_t;

// Initialize security context
security_context_t* security_init(const char* key, const char* iv);

// Cleanup security context
void security_cleanup(security_context_t* ctx);

// Encrypt message
int encrypt_message(security_context_t* ctx, void* data, size_t length);

// Decrypt message
int decrypt_message(security_context_t* ctx, void* data, size_t length);

// Authenticate message
int authenticate_message(security_context_t* ctx, const void* message, size_t length);

// Add security headers to message
int add_security_headers(security_context_t* ctx, message_header_t* header);

// Verify security headers
int verify_security_headers(security_context_t* ctx, const message_header_t* header);

// Secure send function
network_result_t secure_send_message(security_context_t* ctx, 
                                   network_config_t* net_config,
                                   const void* message, 
                                   size_t length);

// Secure receive function
network_result_t secure_receive_message(security_context_t* ctx,
                                     network_config_t* net_config,
                                     void* buffer,
                                     size_t buffer_size);

#endif