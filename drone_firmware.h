// drone_firmware.h
#ifndef DRONE_FIRMWARE_H
#define DRONE_FIRMWARE_H

#include <stdint.h>
#include <stdbool.h>
#include <math.h>

#define MAX_DRONES 50
#define MAX_BUILDING_COMPONENTS 1000
#define DRONE_ID_LENGTH 16

// Drone state machine
typedef enum {
    IDLE,
    FLYING_TO_SITE,
    SCANNING_TERRAIN,
    CONSTRUCTING,
    REPAIRING,
    RETURNING_HOME,
    EMERGENCY_LANDING
} drone_state_t;

// Position structure for 3D space
typedef struct {
    double x, y, z;
    double pitch, yaw, roll;
} position_t;

// Drone configuration
typedef struct {
    char drone_id[DRONE_ID_LENGTH];
    position_t current_pos;
    position_t target_pos;
    float battery_level;
    float payload_capacity;
    drone_state_t state;
    bool has_payload;
    uint8_t construction_materials[10]; // Different material types
} drone_t;

// Building component structure
typedef struct {
    char component_id[32];
    position_t position;
    char material_type[32];
    bool is_constructed;
    float construction_progress;
} building_component_t;

// Main drone structure
typedef struct {
    drone_t drone_info;
    building_component_t building_plan[MAX_BUILDING_COMPONENTS];
    int total_components;
    int completed_components;
} construction_drone_t;

#endif