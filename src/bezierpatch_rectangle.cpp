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

const glm::vec3 &BezierPatch_Rectangle::getPoint(int i, int j) const
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

void BezierPatch_Rectangle::setPoint(int i, int j, const glm::vec3 &cp)
{
    m_points[i*m_sizeN+j]=cp;
}


//////////////////////PRIVATE////////////////////////////

void BezierPatch_Rectangle::makeVBOfromPatch()
{
    if(getVBOSize()>1)
    {
        int k=0;
        for(int i=0; i<m_sizeM; ++i)
        {
            for(int j=0; j<m_sizeN; ++j)
            {
                m_VBOData[k++]=getPoint(i,j);
            }
        }

        for(int j=0; j<m_sizeN; ++j)
        {
            for(int i=0; i<m_sizeM; ++i)
            {
                m_VBOData[k++]=getPoint(i,j);
            }
        }
    }
}
