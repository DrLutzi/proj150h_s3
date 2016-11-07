#include "bezierpatch.h"

//////////////////////CONSTR////////////////////////////

BezierPatch::BezierPatch() : m_points(0), m_tmpCasteljau(0), m_VBOLines(0), m_VBOBezier(0), m_resolution(0)
{
}

BezierPatch::BezierPatch(size_t sizePatch, size_t sizeVBOLines) : m_points(sizePatch), m_tmpCasteljau(sizePatch), m_VBOLines(sizeVBOLines), m_VBOBezier(), m_resolution(0)
{}

BezierPatch::~BezierPatch()
{}

//////////////////////PUBLIC////////////////////////////

void BezierPatch::elevation(int degree)
{

}

size_t BezierPatch::getNumberOfPoints() const
{
    return m_points.size();
}

void BezierPatch::makeVBO(GLint vboId)
{
    glBindBuffer(GL_ARRAY_BUFFER, vboId);
    glBufferData(GL_ARRAY_BUFFER, getSizeVBOLines_GPU() + getSizeVBOBezier_GPU(), NULL, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void BezierPatch::updateVBO_CP(GLint vboId)
{
    makeVBOLines();
    glBindBuffer(GL_ARRAY_BUFFER, vboId);
    glBufferSubData(GL_ARRAY_BUFFER, 0, getSizeVBOLines_GPU(), getVBOLinesData());
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void BezierPatch::setResolution(size_t resolution)
{
    m_resolutionChanged=m_resolution != resolution;
    m_resolution=resolution;
}

void BezierPatch::updateVBO_Bezier(GLint vboId)
{
    makeVBOBezierDeCasteljau();
    if(m_resolutionChanged)
    {
        makeVBO(vboId);
        updateVBO_CP(vboId); m_resolutionChanged=false;
    }
    glBindBuffer(GL_ARRAY_BUFFER, vboId);
    //offset starts after the vbo datas for lines set into the actual VBO
    glBufferSubData(GL_ARRAY_BUFFER, getSizeVBOLines_GPU(), getSizeVBOBezier_GPU(), getVBOBezierData());
    glBindBuffer(GL_ARRAY_BUFFER, 0);
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

void BezierPatch::drawControlPoints() const
{
    glDrawArrays(GL_POINTS, 0, getNumberOfPoints());
}

//////////////////////PROTECTED////////////////////////////

const glm::vec3* BezierPatch::getVBOLinesData() const
{
    return &m_VBOLines[0];
}

const glm::vec3* BezierPatch::getVBOBezierData() const
{
    return &m_VBOBezier[0];
}

GLsizeiptr BezierPatch::getSizeVBOLines_GPU() const
{
    return 3 * m_VBOLines.size() * sizeof(GLfloat);
}

GLsizeiptr BezierPatch::getSizeVBOBezier_GPU() const
{
    return 3 * m_VBOBezier.size() * sizeof(GLfloat);
}
