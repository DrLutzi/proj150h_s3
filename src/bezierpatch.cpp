#include "bezierpatch.h"

//////////////////////CONSTR////////////////////////////

unsigned int BezierPatch::ms_currentId = 0;

BezierPatch::BezierPatch() :
    m_id                (ms_currentId++),
    m_points            (0),
    m_EBOPoints         (0),
    m_surface           (0),
    m_resolution        (0),
    m_drawPatch         (false),
    m_drawSurface       (false),
    m_firstVBO          (0),
    m_firstEBO          (0),
    m_baseVertexEBO     (0),
    m_patchChanged      (true),
    m_surfaceChanged    (true)
{
}

BezierPatch::BezierPatch(size_t sizePatch, const QString& name) :
    m_id                (ms_currentId++),
    m_name              (name),
    m_points            (sizePatch),
    m_EBOPoints         (),
    m_surface           (),
    m_EBOSurface        (),
    m_resolution        (0),
    m_drawPatch         (false),
    m_drawSurface       (false),
    m_firstVBO          (0),
    m_firstEBO          (0),
    m_baseVertexEBO     (0),
    m_patchChanged      (true),
    m_surfaceChanged    (true)
{}

BezierPatch::~BezierPatch()
{}

//////////////////////PUBLIC////////////////////////////

BezierPatch& BezierPatch::operator=(const BezierPatch& other)
{
    if(&other!=this)
    {
        m_id=ms_currentId++;
        m_name=other.name();
        m_points.resize(0);
        m_points.reserve(other.getNumberOfPointsPatch());
        for(const_iterator it=other.begin(); it!=other.end(); ++it)
        {
            m_points.push_back(*it);
        }

        setFirstVBO(other.firstVBO());
        setFirstEBO(other.firstEBO());
        setBaseVertexEBO(other.baseVertexEBO());

        setDrawPatch(other.isDrawPatch());
        setDrawSurface(other.isDrawSurface());

        setControlPointColor(other.controlPointColor());
        setPatchColor(other.patchColor());
        setSurfaceColor(other.surfaceColor());

        notifyPatchChanged();
    }
    return (*this);
}

void BezierPatch::updateVBOPatch(GLint vboId, GLint eboId)
{
    if(!m_drawPatch || !m_patchChanged)
        return;

    glBindBuffer(GL_ARRAY_BUFFER, vboId);
    //offset starts at where we told the bezier patch to start putting datas in the VBO. Copy m_points.
    glBufferSubData(GL_ARRAY_BUFFER, m_firstVBO, getSizeVBOPosition(), &m_points[0]);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eboId);
    //offset starts at where we told the bezier patch to start putting indexes in the VBO. Copy m_EBOPoints.
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, m_firstEBO, getSizeEBOPosition() , &m_EBOPoints[0]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    m_patchChanged=false;
}

void BezierPatch::updateVBOSurface(GLint vboId, GLint eboId)
{
    if(!m_drawSurface || !m_surfaceChanged)
        return;

    //We need to rebuild the surface often because it is dependant on the control points.
    makeSurfaceDeCasteljau();

    glBindBuffer(GL_ARRAY_BUFFER, vboId);
    //offset starts after the VBO datas where points have been input
    glBufferSubData(GL_ARRAY_BUFFER, m_firstVBO+getSizeVBOPosition(), getSizeVBOSurface(), &m_surface[0]);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eboId);
    //offset starts after the EBO datas where point indexes have been input
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, m_firstEBO+getSizeEBOPosition(), getSizeEBOSurface(), &m_EBOSurface[0]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    m_surfaceChanged=false;
}

void BezierPatch::setResolution(size_t resolution)
{
    m_resolution=resolution;
    notifySurfaceChanged();
}

void BezierPatch::clear()
{
    for(iterator it=m_points.begin(); it!=m_points.end(); ++it)
    {
        (*it)=glm::vec3(0,0,0);
    }
}

void BezierPatch::draw(GLint uColorLocation)
{
    setDrawingColor(m_surfaceColor, uColorLocation);
    drawSurface();

    setDrawingColor(m_patchColor, uColorLocation);
    drawPatch();

    setDrawingColor(m_controlPointColor, uColorLocation);
    drawControlPoints();
}

glm::vec3 *BezierPatch::rayIntersectsCP(const glm::vec3& origin, const glm::vec3& direction, float &r, float& distance) const
{
    float oldR=r;
    size_t iIndex;
    for(size_t i=0; i<m_points.size(); ++i)
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

//////////////////////PROTECTED////////////////////////////

void BezierPatch::setDrawingColor(const glm::vec4& color, GLint uColorLocation)
{
    glUniform4f(uColorLocation, color.x, color.y, color.z, color.a);
}

void BezierPatch::drawControlPoints() const
{
    if(m_drawPatch)
    {
        glDrawElementsBaseVertex(GL_POINTS, m_EBOPoints.size(), GL_UNSIGNED_INT, (GLvoid*)(m_firstEBO), m_baseVertexEBO);
    }
}

void BezierPatch::drawSurface() const
{
    if(m_drawSurface)
    {
        glDrawElementsBaseVertex(GL_POINTS, m_EBOSurface.size(), GL_UNSIGNED_INT, (GLvoid*)(getSizeEBOPosition()+m_firstEBO), m_baseVertexEBO+m_points.size());
    }
}
