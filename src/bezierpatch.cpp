#include "bezierpatch.h"

//////////////////////CONSTR////////////////////////////

BezierPatch::BezierPatch() : m_points(0)
{
    m_points[0]=glm::vec3();
}

BezierPatch::BezierPatch(size_t sizePatch, size_t sizeVBO) : m_points(sizePatch), m_VBOData(sizeVBO)
{}

BezierPatch::~BezierPatch()
{}

//////////////////////PUBLIC////////////////////////////

void BezierPatch::elevation(int degree)
{

}

const glm::vec3* BezierPatch::getVBOData()
{
    makeVBOfromPatch();
    return &m_VBOData[0];
}

size_t BezierPatch::getVBOSize() const
{
    return m_VBOData.size();
}

size_t BezierPatch::getNumberOfPoints() const
{
    return m_points.size();
}

glm::vec3 *BezierPatch::rayIntersectsCP(const glm::vec3& origin, const glm::vec3& direction, float r, float& distance) const
{
    float oldR=r;
    int iIndex;
    //TODO mais pas obligatoire : rajouter une bounding box sur le patch plus large de "r" que le patch
    for(int i=0; i<m_points.size(); ++i)
    {
        float dotPD, length;
        glm::vec3 toPoint=m_points[i]-origin;

        if((dotPD=glm::dot(toPoint, direction)) > 0) //point devant la caméra
        {
            glm::vec3 projectionPoint=glm::dot(toPoint, direction)*direction + origin; //projection sur le rayon
            if((length=glm::length(projectionPoint-m_points[i]))<r) //si le point projeté du rayon est dans la sphère il y a intersection
            {
                r=length;
                iIndex=i;
            }
        }
    }
    if(r!=oldR)
        distance=glm::length(m_points[iIndex]-origin);
    return r!=oldR ? (glm::vec3 *)&m_points[iIndex] : NULL;
}
