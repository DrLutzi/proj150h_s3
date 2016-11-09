#include "bezierpatch_rectangle.h"

BezierPatch_Rectangle::BezierPatch_Rectangle() : BezierPatch()
{
}

BezierPatch_Rectangle::BezierPatch_Rectangle(size_t m, size_t n) : BezierPatch(m*n, m*n*2), m_sizeM(m), m_sizeN(n), m_tmpCasteljau(m*n)
{}

BezierPatch_Rectangle::~BezierPatch_Rectangle()
{}

//////////////////////PUBLIC////////////////////////////
//get

const glm::vec3 &BezierPatch_Rectangle::getPoint(size_t i, size_t j) const
{
    return m_points[i*m_sizeN+j];
}

size_t BezierPatch_Rectangle::getSizeM() const
{
    return m_sizeM;
}

size_t BezierPatch_Rectangle::getSizeN() const
{
    return m_sizeN;
}

//set

void BezierPatch_Rectangle::setPoint(size_t i, size_t j, const glm::vec3 &cp)
{
    m_points[i*m_sizeN+j]=cp;
}

//others

void BezierPatch_Rectangle::drawLines(GLint first, GLint baseVertex) const
{
    for(size_t i=0; i<getNumberOfPoints(); i+=getSizeN())
        glDrawElementsBaseVertex(GL_LINE_STRIP, getSizeN(), GL_UNSIGNED_INT,
                                 (GLvoid*)((first + i)*sizeof(unsigned int)), baseVertex);

    for(size_t j=0; j<getNumberOfPoints(); j+=getSizeM())
        glDrawElementsBaseVertex(GL_LINE_STRIP, getSizeM(), GL_UNSIGNED_INT,
                                 (GLvoid*)((first + getNumberOfPoints()+j)*sizeof(unsigned int)), baseVertex); //<=> m*n+j
}

void BezierPatch_Rectangle::drawBezier(GLint first, GLint baseVertex) const
{
    size_t cappedResolution=std::max(size_t(2), m_resolution);

    for(size_t i=0; i<cappedResolution; ++i)
        glDrawElementsBaseVertex(GL_TRIANGLE_STRIP, 2*cappedResolution, GL_UNSIGNED_INT,
                                 (GLvoid*)(getSizeEBOPoints_GPU()+(first + i*2*cappedResolution)*sizeof(unsigned int)), m_points.size()+baseVertex);
}

//////////////////////PRIVATE////////////////////////////

void BezierPatch_Rectangle::makeVBOLines()
{
    if(m_points.size()>1)
    {
        int k=0;
        for(size_t i=0; i<m_sizeM; ++i)
        {
            for(size_t j=0; j<m_sizeN; ++j)
            {
                m_EBOPoints[k++]=i*m_sizeN+j;
            }
        }

        for(size_t j=0; j<m_sizeN; ++j)
        {
            for(size_t i=0; i<m_sizeM; ++i)
            {
                m_EBOPoints[k++]=i*m_sizeN+j;
            }
        }
    }
}

const glm::vec3& BezierPatch_Rectangle::casteljau(float u, float v)
{
    //first fill tmp tab with all the control points (slower, but easier to read)

    for(size_t i=0; i<m_sizeM; ++i)
    {
        for(size_t j=0; j<m_sizeN; ++j)
        {
            getTmpCasteljau(i, j)=getPoint(i, j);
        }
    }

    size_t setSize;
    //approche produit tensoriel en montant en v
    for(size_t j=0; j<m_sizeN; ++j)
    {
        for(setSize=m_sizeM-1; setSize>0; --setSize)
        {
            for(size_t i=0; i<setSize; ++i)
            {
                getTmpCasteljau(i, j)= (1-u)*getTmpCasteljau(i, j) + (u)*getTmpCasteljau(i+1, j);
            }
        }
    }

    //B^m_0 créés, 1 casteljau sur v

    for(setSize=m_sizeN-1; setSize>0; --setSize)
    {
        for(size_t j=0; j<setSize; ++j)
        {
            getTmpCasteljau(0, j)= (1-v)*getTmpCasteljau(0, j) + (v)*getTmpCasteljau(0, j+1);
        }
    }
    return getTmpCasteljau(0, 0);
}

void BezierPatch_Rectangle::makeVBOBezierDeCasteljau()
{
    m_bezier.resize(0);
    m_EBOBezier.resize(0);
    size_t cappedResolution=std::max(size_t(2), m_resolution);
    m_bezier.reserve(cappedResolution*cappedResolution);
    m_EBOBezier.reserve(cappedResolution*cappedResolution*2);

    //We will draw this VBO using GL_TRIANGLE_STRIP, so we need to store alternative sides indexes for each iteration into the EBO to draw triangles

    for(size_t i=0; i<cappedResolution; ++i)
        for(size_t j=0; j<cappedResolution; ++j)
        {
            m_bezier.push_back(casteljau((float)(i)/(cappedResolution-1), (float)j/(cappedResolution-1)));
            if(i+1<cappedResolution)
            {
                m_EBOBezier.push_back(i*cappedResolution+j);
                m_EBOBezier.push_back((i+1)*cappedResolution+j);
            }
        }
}

glm::vec3 &BezierPatch_Rectangle::getTmpCasteljau(size_t i, size_t j)
{
    return m_tmpCasteljau[i*m_sizeN+j];
}

/////////////////////////////////////////////////////////


BezierPatch_Square::BezierPatch_Square() : BezierPatch_Rectangle()
{}

BezierPatch_Square::BezierPatch_Square(size_t size) : BezierPatch_Rectangle(size, size)
{}

BezierPatch_Square::~BezierPatch_Square()
{}
