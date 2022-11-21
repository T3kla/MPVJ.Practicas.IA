#include <stdafx.h>
#include "params.h"
#include <tinyxml.h>

bool ReadParams(const char* filename, Params& params)
{
    TiXmlDocument doc(filename);
    if (!doc.LoadFile())
    {
        fprintf(stderr, "Couldn't read params from %s", filename);
        return false;
    }

    TiXmlHandle hDoc(&doc);

    TiXmlElement* pElem;
    pElem = hDoc.FirstChildElement().Element();
    if (!pElem)
    {
        fprintf(stderr, "Invalid format for %s", filename);
        return false;
    }

    TiXmlHandle hRoot(pElem);
    TiXmlHandle hParams = hRoot.FirstChildElement("params");

    TiXmlElement* paramElem = hParams.FirstChildElement("max_velocity").Element();
    if (paramElem)
        paramElem->Attribute("value", &params.max_velocity);

    paramElem = hParams.FirstChildElement("max_acceleration").Element();
    if (paramElem)
        paramElem->Attribute("value", &params.max_acceleration);

    paramElem = hParams.FirstChildElement("dest_radius").Element();
    if (paramElem)
        paramElem->Attribute("value", &params.dest_radius);
    
    paramElem = hParams.FirstChildElement("arrive_radius").Element();
    if (paramElem)
        paramElem->Attribute("value", &params.arrive_radius);

    paramElem = hParams.FirstChildElement("targetPosition").Element();
    if (paramElem)
    {
        paramElem->Attribute("x", &params.targetPosition.mX);
        paramElem->Attribute("y", &params.targetPosition.mY);
    }

    paramElem = hParams.FirstChildElement("max_angular_velocity").Element();
    if (paramElem)
        paramElem->Attribute("value", &params.max_angular_velocity);
    
    paramElem = hParams.FirstChildElement("max_angular_acceleration").Element();
    if (paramElem)
        paramElem->Attribute("value", &params.max_angular_acceleration);
    
    paramElem = hParams.FirstChildElement("angular_arrive_radius").Element();
    if (paramElem)
        paramElem->Attribute("value", &params.angular_arrive_radius);
    
    paramElem = hParams.FirstChildElement("angular_dest_radius").Element();
    if (paramElem)
        paramElem->Attribute("value", &params.angular_dest_radius);
    
    paramElem = hParams.FirstChildElement("targetRotation").Element();
    if (paramElem)
        paramElem->Attribute("value", &params.targetRotation);
    
    paramElem = hParams.FirstChildElement("look_ahead").Element();
    if (paramElem)
        paramElem->Attribute("value", &params.look_ahead);
    
    paramElem = hParams.FirstChildElement("time_ahead").Element();
    if (paramElem)
        paramElem->Attribute("value", &params.time_ahead);

    return true;
}

std::vector<USVec2D>* ReadPath(const char* filename)
{
    TiXmlDocument doc(filename);
    if (!doc.LoadFile())
    {
        fprintf(stderr, "Couldn't read params from %s", filename);
        return false;
    }

    TiXmlHandle hDoc(&doc);

    TiXmlElement* pElem;
    pElem = hDoc.FirstChildElement().Element();
    if (!pElem)
    {
        fprintf(stderr, "Invalid format for %s", filename);
        return false;
    }

    TiXmlHandle hRoot(pElem);
    TiXmlHandle hParams = hRoot.FirstChildElement("points");

    auto paramElem = hParams.FirstChild().Element();

    if (!paramElem)
        return nullptr;

    auto newPath = new std::vector<USVec2D>;
    newPath->reserve(10);

	while (paramElem != 0)
    {
		auto point = USVec2D(0, 0);
        paramElem->Attribute("x", &point.mX);
        paramElem->Attribute("y", &point.mY);
        newPath->push_back(point);
        paramElem = paramElem->NextSiblingElement();
    }

    return newPath;
}