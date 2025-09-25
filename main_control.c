// main_control.c
#include "drone_firmware.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

construction_drone_t fleet[MAX_DRONES];
pthread_t drone_threads[MAX_DRONES];

void* drone_thread_function(void* arg) {
    int drone_index = *(int*)arg;
    
    while (1) {
        drone_control_loop(&fleet[drone_index]);
        
        // Small delay to prevent overwhelming CPU
        usleep(100000); // 100ms delay
    }
    return NULL;
}

int main() {
    printf("Interplanetary Construction Fleet Initializing...\n");
    
    // Initialize all drones
    for (int i = 0; i < MAX_DRONES; i++) {
        drone_init(&fleet[i], i);
        
        // Create thread for each drone
        int* drone_id = malloc(sizeof(int));
        *drone_id = i;
        pthread_create(&drone_threads[i], NULL, drone_thread_function, drone_id);
    }
    
    // Load building plan
    create_building_plan(fleet, MAX_DRONES, MINING_BAY);
    
    // Start construction
    for (int i = 0; i < MAX_DRONES; i++) {
        fleet[i].drone_info.state = FLYING_TO_SITE;
        fleet[i].drone_info.target_pos = (position_t){i * 10.0, 0, 50, 0, 0, 0};
    }
    
    printf("Construction fleet deployed! Building mining station...\n");
    
    // Keep main thread alive
    while (1) {
        sleep(5);
        
        // Print status
        int completed = 0;
        for (int i = 0; i < MAX_DRONES; i++) {
            if (fleet[i].drone_info.state == IDLE && 
                fleet[i].completed_components == fleet[i].total_components) {
                completed++;
            }
        }
        
        printf("Construction Progress: %d/%d drones completed\n", completed, MAX_DRONES);
        
        if (completed == MAX_DRONES) {
            printf("Construction complete! Mining station operational!\n");
            break;
        }
    }
    
    return 0;
}