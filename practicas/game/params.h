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

// NavMesh stuff

struct Polygon
{
    int id = -1;
    vector<USVec2D> vertex;
};

struct Edge
{
    int id = -1;
    int str = -1;
    int end = -1;
};

struct Link
{
    Edge a;
    Edge b;
};

struct NavMesh
{
    vector<Polygon> polygons;
    vector<Link> links;
};

NavMesh ReadNavMesh(const char *filename);
