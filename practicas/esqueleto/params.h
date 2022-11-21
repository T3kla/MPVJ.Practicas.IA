#pragma once

#include <vector>

struct Params
{
    float max_velocity;
    float max_acceleration;
    float dest_radius;
    float arrive_radius;
    USVec2D targetPosition;

    float max_angular_velocity;
    float max_angular_acceleration;
	
    float angular_arrive_radius;
    float angular_dest_radius;
	
    float targetRotation;
	
    float look_ahead;
    float time_ahead;
};

bool ReadParams(const char* filename, Params& params);

std::vector<USVec2D>* ReadPath(const char* filename);
