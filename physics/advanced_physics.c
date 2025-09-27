// advanced_physics.c
#include "advanced_physics.h"
#include "drone_firmware.h"
#include <math.h>

// Initialize physics state
void init_physics_state(physics_state_t* state, physics_environment_t env, geometry_mode_t geom) {
    state->position.x = 0.0;
    state->position.y = 0.0;
    state->position.z = 0.0;
    state->velocity.x = 0.0;
    state->velocity.y = 0.0;
    state->velocity.z = 0.0;
    state->acceleration.x = 0.0;
    state->acceleration.y = 0.0;
    state->acceleration.z = 0.0;
    state->forces.x = 0.0;
    state->forces.y = 0.0;
    state->forces.z = 0.0;
    
    state->environment = env;
    state->geometry_mode = geom;
    
    // Set default properties
    state->properties.mass = 50.0; // 50kg drone
    state->properties.thrust_force = 100.0; // Newtons
    state->properties.drag_coefficient = 0.1; // Low drag in space
    state->properties.cross_sectional_area = 0.5; // m²
    state->properties.max_velocity = 50.0; // m/s
    state->properties.acceleration_limit = 10.0; // m/s²
    state->properties.precision_level = 0.95; // High precision
    state->gravitational_parameter = 0.0; // Will be set based on celestial body
}

// Calculate distance using appropriate geometry
double calculate_distance(physics_state_t* state, position_t target) {
    switch (state->geometry_mode) {
        case PLANE_GEOMETRY:
            // 2D distance (ignoring z-axis)
            return sqrt(pow(target.x - state->position.x, 2) + 
                       pow(target.y - state->position.y, 2));
            
        case SPHERICAL_GEOMETRY:
            // Great circle distance on spherical surface
            double lat1 = state->position.y * PI / 180.0;
            double lat2 = target.y * PI / 180.0;
            double delta_lat = (target.y - state->position.y) * PI / 180.0;
            double delta_lon = (target.x - state->position.x) * PI / 180.0;
            
            double a = sin(delta_lat/2) * sin(delta_lat/2) +
                      cos(lat1) * cos(lat2) *
                      sin(delta_lon/2) * sin(delta_lon/2);
            double c = 2 * atan2(sqrt(a), sqrt(1-a));
            
            // Assuming radius of 1 unit for simplicity (scale as needed)
            return c; // Distance in radians
            
        case SPACE_GEOMETRY:
        default:
            // 3D Euclidean distance
            return sqrt(pow(target.x - state->position.x, 2) + 
                       pow(target.y - state->position.y, 2) + 
                       pow(target.z - state->position.z, 2));
    }
}

// Calculate gravitational force
force_vector_t calculate_gravitational_force(physics_state_t* state, 
                                          position_t planet_center, 
                                          double planet_mass) {
    force_vector_t gravity_force = {0, 0, 0};
    
    if (state->environment == SPACE_VACUUM) {
        return gravity_force; // No gravity in deep space
    }
    
    // Calculate distance vector
    double dx = planet_center.x - state->position.x;
    double dy = planet_center.y - state->position.y;
    double dz = planet_center.z - state->position.z;
    double distance = sqrt(dx*dx + dy*dy + dz*dz);
    
    if (distance < 1.0) distance = 1.0; // Prevent division by zero
    
    // Calculate gravitational force magnitude
    double force_magnitude = (GRAVITY_CONSTANT * state->properties.mass * planet_mass) / (distance * distance);
    
    // Normalize direction vector and scale by force magnitude
    gravity_force.x = (dx / distance) * force_magnitude;
    gravity_force.y = (dy / distance) * force_magnitude;
    gravity_force.z = (dz / distance) * force_magnitude;
    
    return gravity_force;
}

// Calculate drag force (important for atmospheric environments)
force_vector_t calculate_drag_force(physics_state_t* state) {
    force_vector_t drag_force = {0, 0, 0};
    
    double velocity_magnitude = sqrt(state->velocity.x * state->velocity.x +
                                   state->velocity.y * state->velocity.y +
                                   state->velocity.z * state->velocity.z);
    
    if (velocity_magnitude < 0.001) {
        return drag_force; // No significant velocity
    }
    
    // Drag force opposes motion
    double drag_magnitude = 0.5 * state->properties.drag_coefficient * 
                           state->properties.cross_sectional_area * 
                           velocity_magnitude * velocity_magnitude;
    
    drag_force.x = -(state->velocity.x / velocity_magnitude) * drag_magnitude;
    drag_force.y = -(state->velocity.y / velocity_magnitude) * drag_magnitude;
    drag_force.z = -(state->velocity.z / velocity_magnitude) * drag_magnitude;
    
    return drag_force;
}

// Calculate thrust force based on desired direction
force_vector_t calculate_thrust_force(physics_state_t* state, position_t target) {
    force_vector_t thrust = {0, 0, 0};
    
    // Calculate direction to target
    double dx = target.x - state->position.x;
    double dy = target.y - state->position.y;
    double dz = target.z - state->position.z;
    
    double distance = sqrt(dx*dx + dy*dy + dz*dz);
    
    if (distance < 0.1) {
        return thrust; // Already at target
    }
    
    // Normalize direction vector
    double norm_x = dx / distance;
    double norm_y = dy / distance;
    double norm_z = dz / distance;
    
    // Calculate required acceleration
    double current_speed = sqrt(state->velocity.x * state->velocity.x +
                              state->velocity.y * state->velocity.y +
                              state->velocity.z * state->velocity.z);
    
    // Negative acceleration (deceleration) calculation
    double distance_to_stop = (current_speed * current_speed) / (2.0 * state->properties.acceleration_limit);
    double safety_distance = distance_to_stop * 1.2; // 20% safety margin
    
    double thrust_factor = 1.0;
    if (distance < safety_distance) {
        // Need to decelerate
        thrust_factor = -0.8; // Negative acceleration for deceleration
    } else if (distance < safety_distance * 2) {
        // Moderate acceleration
        thrust_factor = 0.5;
    } else {
        // Full acceleration toward target
        thrust_factor = 1.0;
    }
    
    // Apply precision level to thrust
    thrust_factor *= state->properties.precision_level;
    
    thrust.x = norm_x * state->properties.thrust_force * thrust_factor;
    thrust.y = norm_y * state->properties.thrust_force * thrust_factor;
    thrust.z = norm_z * state->properties.thrust_force * thrust_factor;
    
    // Limit thrust based on maximum acceleration
    double thrust_magnitude = sqrt(thrust.x * thrust.x + 
                                 thrust.y * thrust.y + 
                                 thrust.z * thrust.z);
    
    if (thrust_magnitude > state->properties.thrust_force) {
        double scale = state->properties.thrust_force / thrust_magnitude;
        thrust.x *= scale;
        thrust.y *= scale;
        thrust.z *= scale;
    }
    
    return thrust;
}

// Update physics state using Newton's laws
void update_physics_state(physics_state_t* state, position_t target, double time_step) {
    // Calculate all forces acting on the drone
    force_vector_t gravity = calculate_gravitational_force(state, 
                                                         (position_t){0, 0, 0}, // Planet center
                                                         5.972e24); // Earth mass as example
    force_vector_t drag = calculate_drag_force(state);
    force_vector_t thrust = calculate_thrust_force(state, target);
    
    // Sum all forces
    state->forces.x = thrust.x + gravity.x + drag.x;
    state->forces.y = thrust.y + gravity.y + drag.y;
    state->forces.z = thrust.z + gravity.z + drag.z;
    
    // Calculate acceleration using F = ma
    state->acceleration.x = state->forces.x / state->properties.mass;
    state->acceleration.y = state->forces.y / state->properties.mass;
    state->acceleration.z = state->forces.z / state->properties.mass;
    
    // Limit acceleration
    double accel_magnitude = sqrt(state->acceleration.x * state->acceleration.x +
                                state->acceleration.y * state->acceleration.y +
                                state->acceleration.z * state->acceleration.z);
    
    if (accel_magnitude > state->properties.acceleration_limit) {
        double scale = state->properties.acceleration_limit / accel_magnitude;
        state->acceleration.x *= scale;
        state->acceleration.y *= scale;
        state->acceleration.z *= scale;
    }
    
    // Update velocity (v = v0 + a*t)
    state->velocity.x += state->acceleration.x * time_step;
    state->velocity.y += state->acceleration.y * time_step;
    state->velocity.z += state->acceleration.z * time_step;
    
    // Limit velocity
    double vel_magnitude = sqrt(state->velocity.x * state->velocity.x +
                              state->velocity.y * state->velocity.y +
                              state->velocity.z * state->velocity.z);
    
    if (vel_magnitude > state->properties.max_velocity) {
        double scale = state->properties.max_velocity / vel_magnitude;
        state->velocity.x *= scale;
        state->velocity.y *= scale;
        state->velocity.z *= scale;
    }
    
    // Update position (s = s0 + v*t)
    state->position.x += state->velocity.x * time_step;
    state->position.y += state->velocity.y * time_step;
    state->position.z += state->velocity.z * time_step;
}

// Calculate negative acceleration (deceleration) for precision landing
double calculate_negative_acceleration(physics_state_t* state, position_t target) {
    double distance = calculate_distance(state, target);
    double current_speed = sqrt(state->velocity.x * state->velocity.x +
                              state->velocity.y * state->velocity.y +
                              state->velocity.z * state->velocity.z);
    
    if (current_speed < 0.001) return 0.0; // Not moving
    
    // Time to reach target at current speed
    double time_to_target = distance / current_speed;
    
    // Required deceleration to stop at target
    double required_deceleration = current_speed / time_to_target;
    
    // Add safety factor based on precision level
    required_deceleration *= (2.0 - state->properties.precision_level); // Higher precision = more deceleration
    
    return -required_deceleration; // Negative = deceleration
}

// Spherical geometry navigation (for planetary surfaces)
position_t spherical_navigation_correction(physics_state_t* state, position_t target) {
    position_t corrected_target = target;
    
    // For spherical geometry, we might need to account for the curvature
    // This is a simplified example - real implementation would be more complex
    double earth_radius = 6371000.0; // meters
    
    // Convert to radians for spherical calculations
    double lat1 = state->position.y * PI / 180.0;
    double lon1 = state->position.x * PI / 180.0;
    double lat2 = target.y * PI / 180.0;
    double lon2 = target.x * PI / 180.0;
    
    // Calculate intermediate point on great circle
    double d = calculate_distance(state, target); // This would return angular distance
    double A = sin((1 - 0.5) * d) / sin(d);
    double B = sin(0.5 * d) / sin(d);
    
    double x = A * cos(lat1) * cos(lon1) + B * cos(lat2) * cos(lon2);
    double y = A * cos(lat1) * sin(lon1) + B * cos(lat2) * sin(lon2);
    double z = A * sin(lat1) + B * sin(lat2);
    
    double lat3 = atan2(z, sqrt(x*x + y*y));
    double lon3 = atan2(y, x);
    
    corrected_target.y = lat3 * 180.0 / PI;
    corrected_target.x = lon3 * 180.0 / PI;
    
    return corrected_target;
}