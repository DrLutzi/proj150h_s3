#include "bezierpatch.h"

//////////////////////CONSTR////////////////////////////

BezierPatch::BezierPatch() : m_points(0), m_EBOPoints(0), m_bezier(0), m_resolution(0)
{
}

BezierPatch::BezierPatch(size_t sizePatch, size_t sizeEBOPoints) : m_points(sizePatch), m_EBOPoints(sizeEBOPoints), m_bezier(), m_EBOBezier(), m_resolution(0)
{}

BezierPatch::~BezierPatch()
{}

//////////////////////PUBLIC////////////////////////////

size_t BezierPatch::getNumberOfPoints() const
{
    return m_points.size();
}

void BezierPatch::makeVBO(GLint vboId, GLint eboId)
{
    glBindBuffer(GL_ARRAY_BUFFER, vboId);
    glBufferData(GL_ARRAY_BUFFER, getSizeVBOPoints_GPU() + getSizeVBOBezier_GPU(), NULL, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eboId);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, getSizeEBOPoints_GPU() + getSizeEBOBezier_GPU(), NULL, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void BezierPatch::updateVBO_CP(GLint vboId, GLint eboId)
{
    makeVBOLines();
    glBindBuffer(GL_ARRAY_BUFFER, vboId);
    glBufferSubData(GL_ARRAY_BUFFER, 0, getSizeVBOPoints_GPU(), &m_points[0]);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eboId);
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, getSizeEBOPoints_GPU() , &m_EBOPoints[0]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void BezierPatch::updateVBO_Bezier(GLint vboId, GLint eboId)
{
    makeVBOBezierDeCasteljau();
    if(m_resolutionChanged)
    {
        makeVBO(vboId, eboId);
        updateVBO_CP(vboId, eboId);
        m_resolutionChanged=false;
    }
    glBindBuffer(GL_ARRAY_BUFFER, vboId);
    //offset starts after the VBO datas where points have been put
    glBufferSubData(GL_ARRAY_BUFFER, getSizeVBOPoints_GPU(), getSizeVBOBezier_GPU(), &m_bezier[0]);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindBuffer(GL_ARRAY_BUFFER, eboId);
    //same remark for the EBO
    glBufferSubData(GL_ARRAY_BUFFER, getSizeEBOPoints_GPU(), getSizeEBOBezier_GPU(), &m_EBOBezier[0]);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void BezierPatch::setResolution(size_t resolution)
{
    m_resolutionChanged=m_resolution != resolution;
    m_resolution=resolution;
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

void BezierPatch::drawControlPoints(GLint first, GLint baseVertex) const
{
    glDrawArrays(GL_POINTS, first, getNumberOfPoints());
}

void BezierPatch::drawBezier(GLint first, GLint baseVertex) const
{
    glDrawElementsBaseVertex(GL_POINTS, m_EBOPoints.size(), GL_UNSIGNED_INT, (GLvoid*)(first*sizeof(unsigned int)), baseVertex);
}

//////////////////////PROTECTED////////////////////////////

GLsizeiptr BezierPatch::getSizeVBOPoints_GPU() const
{
    return 3 * m_points.size() * sizeof(GLfloat);
}

GLsizeiptr BezierPatch::getSizeVBOBezier_GPU() const
{
    return 3 * m_bezier.size() * sizeof(GLfloat);
}

GLsizeiptr BezierPatch::getSizeEBOPoints_GPU() const
{
    return m_EBOPoints.size() * sizeof(unsigned int);
}

GLsizeiptr BezierPatch::getSizeEBOBezier_GPU() const
{
    return m_EBOBezier.size() * sizeof(unsigned int);
}
