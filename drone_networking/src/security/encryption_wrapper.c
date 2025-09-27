// src/security/encryption_wrapper.c
#include "encryption_wrapper.h"

security_context_t* security_init(const char* key, const char* iv) {
    security_context_t* ctx = (security_context_t*)malloc(sizeof(security_context_t));
    if (!ctx) return NULL;
    
    memset(ctx, 0, sizeof(security_context_t));
    
    // Initialize Mithril context
    ctx->mithril_ctx = mithril_init();
    if (!ctx->mithril_ctx) {
        free(ctx);
        return NULL;
    }
    
    // Set encryption key
    if (key) {
        strncpy(ctx->encryption_key, key, 31);
        ctx->encryption_key[31] = '\0';
    } else {
        // Generate random key (in real implementation)
        strcpy(ctx->encryption_key, "DEFAULT_DRONE_KEY_32_CHARS");
    }
    
    // Set IV
    if (iv) {
        strncpy(ctx->iv, iv, 15);
        ctx->iv[15] = '\0';
    } else {
        // Generate random IV (in real implementation)
        strcpy(ctx->iv, "DEFAULT_IV_16_CH");
    }
    
    ctx->encryption_enabled = 1;
    ctx->authentication_enabled = 1;
    
    printf("Security context initialized\n");
    return ctx;
}

void security_cleanup(security_context_t* ctx) {
    if (ctx) {
        if (ctx->mithril_ctx) {
            mithril_cleanup(ctx->mithril_ctx);
        }
        free(ctx);
    }
}

int encrypt_message(security_context_t* ctx, void* data, size_t length) {
    if (!ctx || !data || !ctx->encryption_enabled) return -1;
    
    // Use Mithril's encryption function
    return Mithril_Framework(ctx->mithril_ctx, 
                          (unsigned char*)data, 
                          length,
                          (unsigned char*)ctx->encryption_key,
                          (unsigned char*)ctx->iv);
}

int decrypt_message(security_context_t* ctx, void* data, size_t length) {
    if (!ctx || !data || !ctx->encryption_enabled) return -1;
    
    // Use Mithril's decryption function
    return Mithril_Framework(ctx->mithril_ctx, 
                          (unsigned char*)data, 
                          length,
                          (unsigned char*)ctx->encryption_key,
                          (unsigned char*)ctx->iv);
}

int authenticate_message(security_context_t* ctx, const void* message, size_t length) {
    if (!ctx || !message || !ctx->authentication_enabled) return -1;
    
    // Generate authentication tag using Mithril
    unsigned char auth_tag[32];
    int result = mithril_authenticate(ctx->mithril_ctx,
                                     (const unsigned char*)message,
                                     length,
                                     auth_tag,
                                     sizeof(auth_tag));
    
    return result;
}

int add_security_headers(security_context_t* ctx, message_header_t* header) {
    if (!ctx || !header) return -1;
    
    // Add security flags to message header
    // This is a simplified example - adjust based on Mithril's capabilities
    header->payload_length += 32; // Add space for authentication tag
    
    return 0;
}

int verify_security_headers(security_context_t* ctx, const message_header_t* header) {
    if (!ctx || !header) return -1;
    
    // Verify security in message header
    return 0; // Simplified for now
}

network_result_t secure_send_message(security_context_t* ctx, 
                                   network_config_t* net_config,
                                   const void* message, 
                                   size_t length) {
    if (!ctx || !net_config || !message) return NET_ERROR;
    
    // Create a copy of the message for encryption
    void* encrypted_buffer = malloc(length + 1024); // Extra space for security overhead
    if (!encrypted_buffer) return NET_ERROR;
    
    memcpy(encrypted_buffer, message, length);
    
    // Encrypt the message if security is enabled
    if (ctx->encryption_enabled) {
        int result = encrypt_message(ctx, encrypted_buffer, length);
        if (result != 0) {
            free(encrypted_buffer);
            return NET_ERROR;
        }
    }
    
    // Authenticate the message
    if (ctx->authentication_enabled) {
        int result = authenticate_message(ctx, encrypted_buffer, length);
        if (result != 0) {
            free(encrypted_buffer);
            return NET_ERROR;
        }
    }
    
    // Send the secured message
    network_result_t send_result = network_send_data(net_config, encrypted_buffer, length);
    
    free(encrypted_buffer);
    return send_result;
}

network_result_t secure_receive_message(security_context_t* ctx,
                                     network_config_t* net_config,
                                     void* buffer,
                                     size_t buffer_size) {
    if (!ctx || !net_config || !buffer) return NET_ERROR;
    
    // Receive the message
    network_result_t result = network_receive_data(net_config, buffer, buffer_size);
    if (result != NET_SUCCESS) {
        return result;
    }
    
    // Authenticate the received message
    if (ctx->authentication_enabled) {
        int auth_result = authenticate_message(ctx, buffer, buffer_size);
        if (auth_result != 0) {
            printf("Message authentication failed\n");
            return NET_ERROR;
        }
    }
    
    // Decrypt the message if security is enabled
    if (ctx->encryption_enabled) {
        int decrypt_result = decrypt_message(ctx, buffer, buffer_size);
        if (decrypt_result != 0) {
            printf("Message decryption failed\n");
            return NET_ERROR;
        }
    }
    
    return NET_SUCCESS;
}