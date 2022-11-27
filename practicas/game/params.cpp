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

// NavMesh stuff

NavMesh ReadNavMesh(const char *filename)
{
    NavMesh nav;

    TiXmlDocument doc(filename);
    if (!doc.LoadFile())
    {
        fprintf(stderr, "Couldn't read params from %s", filename);
        return nav;
    }

    TiXmlHandle hDoc(&doc);
    TiXmlElement *pElem;

    pElem = hDoc.FirstChildElement().Element();
    if (!pElem)
    {
        fprintf(stderr, "Invalid format for %s", filename);
        return nav;
    }

    TiXmlHandle hRoot(pElem);

    // Extract points

    TiXmlHandle xPolygons = hRoot.FirstChildElement("polygons");

    auto *xPoly = xPolygons.FirstChild().ToElement();

    int polyCounter = 0;

    while (xPoly != 0)
    {
        Polygon poly;
        poly.id = polyCounter;

        auto* xPoint = xPoly->FirstChildElement();

        while (xPoint != 0)
        {
            auto p = USVec2D(0, 0);
            xPoint->Attribute("x", &p.mX);
            xPoint->Attribute("y", &p.mY);
            poly.vertex.push_back(p);
            xPoint = xPoint->NextSiblingElement();
        }

        polyCounter++;
        nav.polygons.push_back(poly);
        xPoly = xPoly->NextSiblingElement();
    }

    // Extract links

    TiXmlHandle xLinks = hRoot.FirstChildElement("links");

    auto* xLink = xLinks.FirstChild().ToElement();

    while (xLink != 0)
    {
        Link link;

        auto* xEdge = xLink->FirstChildElement();

        auto p = USVec2D(0, 0);
        xEdge->Attribute("polygon", &link.a.id);
        xEdge->Attribute("edgestart", &link.a.str);
        xEdge->Attribute("edgeend", &link.a.end);
        xEdge = xEdge->NextSiblingElement();
        xEdge->Attribute("polygon", &link.b.id);
        xEdge->Attribute("edgestart", &link.b.str);
        xEdge->Attribute("edgeend", &link.b.end);

        nav.links.push_back(link);
        xLink = xLink->NextSiblingElement();
    }

    return nav;
}
