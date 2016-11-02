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

//////////////////////PRIVATE////////////////////////////

void BezierPatch_Rectangle::makeVBOfromPatch()
{
    if(m_points.size()>1)
    {
        int k=0;
        for(size_t i=0; i<m_sizeM; ++i)
        {
            for(size_t j=0; j<m_sizeN; ++j)
            {
                m_VBOData[k++]=getPoint(i,j);
            }
        }

        for(size_t j=0; j<m_sizeN; ++j)
        {
            for(size_t i=0; i<m_sizeM; ++i)
            {
                m_VBOData[k++]=getPoint(i,j);
            }
        }
    }
}

/////////////////////////////////////////////////////////


BezierPatch_Square::BezierPatch_Square() : BezierPatch_Rectangle()
{}

BezierPatch_Square::BezierPatch_Square(size_t size) : BezierPatch_Rectangle(size, size)
{}

BezierPatch_Square::~BezierPatch_Square()
{}
