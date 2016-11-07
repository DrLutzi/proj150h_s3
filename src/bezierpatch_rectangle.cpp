#include "bezierpatch_rectangle.h"

BezierPatch_Rectangle::BezierPatch_Rectangle() : BezierPatch()
{
}

BezierPatch_Rectangle::BezierPatch_Rectangle(size_t m, size_t n) : BezierPatch(m*n, m*n*2), m_sizeM(m), m_sizeN(n)
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

void BezierPatch_Rectangle::drawLines() const
{
    for(int i=0; i<getNumberOfPoints(); i+=getSizeN())
        glDrawArrays(GL_LINE_STRIP, i, getSizeN());

    for(int j=0; j<getNumberOfPoints(); j+=getSizeM())
        glDrawArrays(GL_LINE_STRIP, getNumberOfPoints()+j, getSizeM());
}

void BezierPatch_Rectangle::drawBezier() const
{
    glDrawArrays(GL_POINTS, getSizeVBOLines_GPU(), m_VBOBezier.size());
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
                m_VBOLines[k++]=getPoint(i,j);
            }
        }

        for(size_t j=0; j<m_sizeN; ++j)
        {
            for(size_t i=0; i<m_sizeM; ++i)
            {
                m_VBOLines[k++]=getPoint(i,j);
            }
        }
    }
}

const glm::vec3& BezierPatch_Rectangle::casteljau(float u, float v)
{
    //first fill tmp tab with all the control points (slower, but easier to read)

    for(int i=0; i<m_sizeM; ++i)
    {
        for(int j=0; j<m_sizeN; ++j)
        {
            getTmpCasteljau(i, j)=getPoint(i, j);
        }
    }

    int cSize;
    //approche produit tensoriel en montant en v
    for(int j=0; j<m_sizeN; ++j)
    {
        for(cSize=m_sizeM-1; cSize>0; --cSize)
        {
            for(int i=0; i<cSize; ++i)
            {
                getTmpCasteljau(i, j)= (1-u)*getTmpCasteljau(i, j) + (u)*getTmpCasteljau(i+1, j);
            }
        }
    }

    //B^m_0 créés, 1 casteljau sur v

    for(cSize=m_sizeN-1; cSize>0; --cSize)
    {
        for(int j=0; j<cSize; ++j)
        {
            getTmpCasteljau(0, j)= (1-v)*getTmpCasteljau(0, j) + (v)*getTmpCasteljau(0, j+1);
        }
    }
    return getTmpCasteljau(0, 0);
}

void BezierPatch_Rectangle::makeVBOBezierDeCasteljau()
{
    m_VBOBezier.resize(0);
    if(m_resolutionChanged)
    {
        m_VBOBezier.reserve(std::max(size_t(0), m_resolution*m_resolution));
    }
    for(int i=0; i<std::max(size_t(0), m_resolution-1); ++i)
        for(int j=0; j<m_resolution-1; ++j)
            m_VBOBezier.push_back(casteljau((float)i/m_resolution, (float)j/m_resolution));
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
