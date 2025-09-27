// advanced_physics.h
#ifndef ADVANCED_PHYSICS_H
#define ADVANCED_PHYSICS_H

#include <math.h>
#include <stdbool.h>

#define GRAVITY_CONSTANT 6.67430e-11
#define LIGHT_SPEED 299792458.0
#define PI 3.14159265358979323846

// Physics context for different environments
typedef enum {
    SPACE_VACUUM,      // No gravity, no atmosphere
    PLANETARY_SURFACE, // Has gravity, may have atmosphere
    ORBITAL,           // Orbital mechanics apply
    ASTEROID_FIELD     // Complex gravitational fields
} physics_environment_t;

// Geometric calculation modes
typedef enum {
    PLANE_GEOMETRY,     // 2D flat surface calculations
    SPHERICAL_GEOMETRY, // Spherical surface calculations
    SPACE_GEOMETRY      // 3D space calculations
} geometry_mode_t;

// Physics properties structure
typedef struct {
    double mass;                    // Drone mass in kg
    double thrust_force;           // Maximum thrust force
    double drag_coefficient;       // Drag coefficient
    double cross_sectional_area;   // Cross-sectional area
    double max_velocity;           // Maximum velocity
    double acceleration_limit;     // Maximum acceleration
    double precision_level;        // Navigation precision (0.0 to 1.0)
} physics_properties_t;

// Force vector for physics calculations
typedef struct {
    double x, y, z;
} force_vector_t;

// Physics state for drone
typedef struct {
    position_t position;
    force_vector_t velocity;
    force_vector_t acceleration;
    force_vector_t forces;        // Sum of all forces acting on drone
    physics_environment_t environment;
    geometry_mode_t geometry_mode;
    physics_properties_t properties;
    double gravitational_parameter; // For orbital mechanics (GM)
} physics_state_t;

#endif