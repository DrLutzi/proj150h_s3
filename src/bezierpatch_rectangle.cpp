#include "bezierpatch_rectangle.h"

BezierPatch_Rectangle::BezierPatch_Rectangle() : BezierPatch()
{
}

BezierPatch_Rectangle::BezierPatch_Rectangle(size_t m, size_t n) :
    BezierPatch(m*n),
    m_sizeM(m),
    m_sizeN(n),
    m_tmpCasteljau(m_points.size())
{}

BezierPatch_Rectangle::~BezierPatch_Rectangle()
{}

//////////////////////PUBLIC////////////////////////////
//get

const glm::vec3 &BezierPatch_Rectangle::getPoint(size_t i, size_t j) const
{
    return m_points[i*m_sizeN+j];
}

size_t BezierPatch_Rectangle::sizeM() const
{
    return m_sizeM;
}

size_t BezierPatch_Rectangle::sizeN() const
{
    return m_sizeN;
}

//set

void BezierPatch_Rectangle::setPoint(size_t i, size_t j, const glm::vec3 &cp)
{
    m_points[i*m_sizeN+j]=cp;
    notifyPatchChanged();
}


void BezierPatch_Rectangle::makePatch()
{
    m_EBOPoints.resize(sizeM()*sizeN()*2);
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

void BezierPatch_Rectangle::makeSurfaceDeCasteljau()
{
    m_surface.resize(0);
    m_EBOSurface.resize(0);
    size_t cappedResolution=std::max(size_t(2), m_resolution);
    m_surface.reserve(cappedResolution*cappedResolution);
    m_EBOSurface.reserve(cappedResolution*cappedResolution*2);

    //We will draw this VBO using GL_TRIANGLE_STRIP, so we need to store alternative sides indexes for each iteration into the EBO to draw triangles

    for(size_t i=0; i<cappedResolution; ++i)
        for(size_t j=0; j<cappedResolution; ++j)
        {
            m_surface.push_back(casteljau((float)(i)/(cappedResolution-1), (float)j/(cappedResolution-1)));
            if(i+1<cappedResolution)
            {
                m_EBOSurface.push_back(i*cappedResolution+j);
                m_EBOSurface.push_back((i+1)*cappedResolution+j);
            }
        }
}

BezierPatch_Rectangle& BezierPatch_Rectangle::operator=(const BezierPatch_Rectangle &other)
{
    if(&other!=this)
    {
        m_sizeM=other.sizeM();
        m_sizeN=other.sizeN();
        BezierPatch::operator=(other);
        m_tmpCasteljau.resize(sizeM()*sizeN());
    }
    return (*this);
}

//////////////////////PROTECTED////////////////////////////

//others

void BezierPatch_Rectangle::drawPatch() const
{
    if(!m_drawPatch)
        return;

    //first draw all the horizontal lines
    for(size_t i=0; i<getNumberOfPointsPatch(); i+=sizeN())
        glDrawElementsBaseVertex(GL_LINE_STRIP, sizeN(), GL_UNSIGNED_INT,
                                 (GLvoid*)(m_firstEBO + i*sizeof(unsigned int)), m_baseVertexEBO);

    //do the same with the vertical lines
    for(size_t j=0; j<getNumberOfPointsPatch(); j+=sizeM())
        glDrawElementsBaseVertex(GL_LINE_STRIP, sizeM(), GL_UNSIGNED_INT,
                                 (GLvoid*)(m_firstEBO + (getNumberOfPointsPatch()+j)*sizeof(unsigned int)), m_baseVertexEBO); //<=> m*n+j
}

void BezierPatch_Rectangle::drawSurface() const
{
    if(!m_drawSurface)
        return;

    size_t cappedResolution=std::max(size_t(2), m_resolution);

    for(size_t i=0; i<cappedResolution-1; ++i)
        glDrawElementsBaseVertex(GL_TRIANGLE_STRIP, 2*cappedResolution, GL_UNSIGNED_INT,
                                 (GLvoid*)(m_firstEBO + getSizeEBOPosition() + (i*2*cappedResolution)*sizeof(unsigned int)), m_points.size()+m_baseVertexEBO);
}

//////////////////////PRIVATE////////////////////////////

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

glm::vec3 &BezierPatch_Rectangle::getTmpCasteljau(size_t i, size_t j)
{
    return m_tmpCasteljau[i*m_sizeN+j];
}
