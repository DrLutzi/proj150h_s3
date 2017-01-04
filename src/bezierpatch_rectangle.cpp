#include "bezierpatch_rectangle.h"

BezierPatch_Rectangle::BezierPatch_Rectangle() :
    BezierPatch(),
    m_sizeM(0),
    m_sizeN(0)
{
}

BezierPatch_Rectangle::BezierPatch_Rectangle(size_t m, size_t n) :
    BezierPatch(m*n,"Rectangle"),
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
    return BezierPatch::getPoint(i*m_sizeN+j);
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

void BezierPatch_Rectangle::setPoint(size_t i, size_t j, const glm::vec3 &cp, bool sendToVBO)
{
    BezierPatch::setPoint(i*m_sizeN+j, cp, sendToVBO);
}


void BezierPatch_Rectangle::makePatchEBO()
{
    m_EBOPoints.resize(sizeM()*sizeN()*2);
    if(m_points.size()>1)
    {
        unsigned int eboIndex=0;
        for(size_t i=0; i<m_sizeM; ++i)
        {
            for(size_t j=0; j<m_sizeN; ++j)
            {
                m_EBOPoints[eboIndex++]=i*m_sizeN+j;
            }
        }

        for(size_t j=0; j<m_sizeN; ++j)
        {
            for(size_t i=0; i<m_sizeM; ++i)
            {
                m_EBOPoints[eboIndex++]=i*m_sizeN+j;
            }
        }
    }
}

void BezierPatch_Rectangle::makeSurfaceVBO()
{
    m_surface.resize(0);
    size_t cappedResolution=std::max(size_t(2), m_resolution);
    m_surface.reserve(cappedResolution*cappedResolution);

    //We will draw this VBO using GL_TRIANGLE_STRIP, so we need to store alternative sides indexes for each iteration into the EBO to draw triangles

    for(size_t i=0; i<cappedResolution; ++i)
        for(size_t j=0; j<cappedResolution; ++j)
        {
            m_surface.push_back(casteljau((float)(i)/(cappedResolution-1), (float)j/(cappedResolution-1)));
        }
}

void BezierPatch_Rectangle::makeSurfaceEBO()
{
    m_EBOSurface.resize(0);
    size_t cappedResolution=std::max(size_t(2), m_resolution);
    m_EBOSurface.reserve(cappedResolution*cappedResolution*2);

    //We will draw this VBO using GL_TRIANGLE_STRIP, so we need to store alternative sides indexes for each iteration into the EBO to draw triangles

    for(size_t i=0; i<cappedResolution; ++i)
        for(size_t j=0; j<cappedResolution; ++j)
        {
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

//static random generation

BezierPatch_Rectangle* BezierPatch_Rectangle::generate(size_t m, size_t n, float xStep, float yStep, float max_noise)
{
    BezierPatch_Rectangle *bp=new BezierPatch_Rectangle(m, n);

    auto genNoise=[&](){
        return (float((rand()%2000)-1000)/1000)*max_noise;
    };

    float noise;
    glm::vec3 currentCP(0,0,0);
    size_t i,j;
    for(j=0; j<n; ++j)
    {
        noise=genNoise();
        for(i=0; i<m; ++i)
        {
            currentCP.x+=noise;
            bp->setPoint(i, j, currentCP);
            noise=genNoise();
            currentCP.x+=xStep + noise;
            noise=genNoise();
            currentCP.z=noise;
        }
        noise=genNoise();
        currentCP.y+=yStep+noise;
        currentCP.x=0;
    }
    return bp;
}

//////////////////////PROTECTED////////////////////////////

//others

void BezierPatch_Rectangle::drawPatch() const
{
    if(!m_drawPatch)
        return;

    //first draw all the horizontal lines
    for(size_t i=0; i<sizeN()*sizeM(); i+=sizeN())
        glDrawElementsBaseVertex(GL_LINE_STRIP, sizeN(), GL_UNSIGNED_INT,
                                 (GLvoid*)(m_firstEBO + i*sizeof(unsigned int)), m_baseVertexEBO);

    //do the same with the vertical lines
    for(size_t j=0; j<sizeN()*sizeM(); j+=sizeM())
        glDrawElementsBaseVertex(GL_LINE_STRIP, sizeM(), GL_UNSIGNED_INT,
                                 (GLvoid*)(m_firstEBO + (sizeN()*sizeM()+j)*sizeof(unsigned int)), m_baseVertexEBO);
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
    //first fill tmp tab with all the control points
    for(size_t i=0; i<m_points.size(); ++i)
        m_tmpCasteljau[i]=m_points[i];

    size_t setSize;
    //tensor product in u
    //This will create a curve fixed in u
    for(setSize=m_sizeM-1; setSize>0; --setSize)
    {
        for(size_t i=0; i<setSize; ++i)
        {
            for(size_t j=0; j<m_sizeN; ++j)
            {
                getTmpCasteljau(i, j)= (1-u)*getTmpCasteljau(i, j) + (u)*getTmpCasteljau(i+1, j);
            }
        }
    }

    //B^{m}_{0} créés, 1 casteljau on v left

    for(setSize=m_sizeN-1; setSize>0; --setSize)
    {
        for(size_t j=0; j<setSize; ++j)
        {
            getTmpCasteljau(0, j)= (1-v)*getTmpCasteljau(0, j) + (v)*getTmpCasteljau(0, j+1);
        }
    }
    return m_tmpCasteljau[0];
}

glm::vec3 &BezierPatch_Rectangle::getTmpCasteljau(size_t i, size_t j)
{
    return m_tmpCasteljau[i*m_sizeN+j];
}
