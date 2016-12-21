#include "bezierpatch_triangle.h"
#include "errorsHandler.hpp"

BezierPatch_Triangle::BezierPatch_Triangle() :
    BezierPatch(),
    m_size(0)
{
}

BezierPatch_Triangle::BezierPatch_Triangle(size_t n) :
    BezierPatch(((n)*(n+1))/2, "Triangle"),
    m_size(n),
    m_tmpCasteljau(m_points.size())
{
}

BezierPatch_Triangle::~BezierPatch_Triangle()
{}

//////////////////////PUBLIC////////////////////////////
//get

const glm::vec3 &BezierPatch_Triangle::getPoint(size_t i, size_t j, size_t k) const
{
    if(i+j+k!=m_size-1)
        ERROR("BezierPatch_Triangle : the triplet (i,j,k) is not a valid coordinate");
    return m_points[accessValue(k)+j];
}

size_t BezierPatch_Triangle::size() const
{
    return m_size;
}

//set

void BezierPatch_Triangle::setPoint(size_t i, size_t j, size_t k, const glm::vec3 &cp)
{
    if(i+j+k!=m_size-1)
        ERROR("BezierPatch_Triangle : the triplet (i,j,k) is not a valid coordinate");
    m_points[accessValue(k)+j]=cp;

    notifyPatchChanged();
}

void BezierPatch_Triangle::makePatchEBO()
{
    int eboIndex=0;
    //the size of the EBO is 3 times the number of times we draw a triangle while going an axis.
    //the axis are i, j and k, obviously.
    m_EBOPoints.resize(std::max(  (size_t)0, (( ((size())*(size()-1)) /2 )) * 3)  );

    //we construct this triangle by triangle
    for(size_t k=0; k<size()-1; ++k) //"from bottom to top"
    {
        for(size_t j=0; j<size()-k-1; ++j) //also read "from left to right"
        {
            //the point iterated, his right neighbor, and top neighbor
            m_EBOPoints[eboIndex++]=accessValue(k)+j;
            m_EBOPoints[eboIndex++]=accessValue(k)+j+1;
            m_EBOPoints[eboIndex++]=accessValue(k+1)+j;
        }
    }
}

void BezierPatch_Triangle::makeSurfaceVBO()
{
    size_t cappedResolution=std::max(size_t(2), m_resolution);

    m_surface.resize(0);
    m_surface.reserve(cappedResolution*cappedResolution);

    //calculate entire VBO, from top to bottom, left to right (because why not?)
    for(size_t k=0; k<cappedResolution; ++k) //"from bottom to top"
    {
        for(size_t j=0; j<cappedResolution-k; ++j) //also read "from left to right"
        {
            size_t i=cappedResolution-k-j-1;
            m_surface.push_back(casteljau((float)(i)/(cappedResolution-1),
                                         (float)(j)/(cappedResolution-1),
                                         (float)(k)/(cappedResolution-1)));
        }
    }

}

void BezierPatch_Triangle::makeSurfaceEBO()
{
    size_t cappedResolution=std::max(size_t(2), m_resolution);

    m_EBOSurface.resize(0);
    m_EBOSurface.reserve(cappedResolution*cappedResolution*2);

    //calculate entire VBO, from top to bottom, left to right (because why not?)
    int eboIndex=0;
    for(size_t k=0; k<cappedResolution; ++k) //"from bottom to top"
    {
        for(size_t j=0; j<cappedResolution-k; ++j) //also read "from left to right"
        {
            size_t i=cappedResolution-k-j-1;
            //the point created, his right neighbor, and top neighbor.
            if(i!=0)
            {
                m_EBOSurface.push_back(eboIndex);
                m_EBOSurface.push_back(cappedResolution-k+eboIndex);
                m_EBOSurface.push_back(++eboIndex);
            }
            else
                ++eboIndex;
        }
    }
}


//operators

BezierPatch_Triangle& BezierPatch_Triangle::operator=(const BezierPatch_Triangle& other)
{
    if(&other!=this)
    {
        m_size=other.size();
        BezierPatch::operator=(other);
        m_tmpCasteljau.resize(m_points.size());
    }
    return (*this);
}

BezierPatch_Triangle* BezierPatch_Triangle::generate(size_t n, float xStep, float yStep, float max_noise)
{
    //TODO : bug
    BezierPatch_Triangle *bp=new BezierPatch_Triangle(n);

    auto genNoise=[&](){
        return (float((rand()%2000)-1000)/1000)*max_noise;
    };

    float noise;
    glm::vec3 currentCP(0,0,0);
    size_t i,j,k;
    for(k=0; k<n; ++k)
    {
        for(j=0; j<n-k; ++j)
        {
            i=n-k-j-1;
            currentCP.x+=noise;
            bp->setPoint(i,j,k, currentCP);
            noise=genNoise();
            currentCP.x+=xStep+noise;
            noise=genNoise();
            currentCP.y+=noise;
            noise=genNoise();
            currentCP.z+=noise;
        }
        noise=genNoise();
        currentCP.x=(k)*xStep/2 + noise;
        noise=genNoise();
        currentCP.y+=yStep+noise;
    }

    return bp;
}

//////////////////////PROTECTED////////////////////////////

//draw functions

void BezierPatch_Triangle::drawPatch() const
{
    if(!m_drawPatch)
        return;

    GLint polygonMode[2];
    glGetIntegerv(GL_POLYGON_MODE, polygonMode);
    glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
    glDrawElementsBaseVertex(GL_TRIANGLES, m_EBOPoints.size(), GL_UNSIGNED_INT, (GLvoid*)(m_firstEBO), 0+m_baseVertexEBO);
    glPolygonMode(polygonMode[0], polygonMode[1]);
}

void BezierPatch_Triangle::drawSurface() const
{
    if(!m_drawSurface)
        return;

    glDrawElementsBaseVertex(GL_TRIANGLES, m_EBOSurface.size(), GL_UNSIGNED_INT, (GLvoid*)(m_firstEBO+getSizeEBOPosition()), m_points.size()+m_baseVertexEBO);
}

const glm::vec3 &BezierPatch_Triangle::casteljau(float u, float v, float w)
{
    //first, as usual, fill the tmp vector with the current control points in m_points
    for(size_t i=0; i<m_points.size(); ++i)
        m_tmpCasteljau[i]=m_points[i];

    //then compute a new set of control points by linear interpolation, smaller than the previous one
    //do that until the size of the set is 1
    size_t setSize;
    size_t i=0, j=0, k=0;
    for(setSize=m_size-1; setSize>0; --setSize)
    {
        for(k=0; k<setSize; ++k)
        {
            size_t sizeMinusK=setSize-k;
            for(j=0; j<sizeMinusK; ++j)
            {
                i=sizeMinusK-j-1;
                getTmpCasteljau(i,j,k)= u*getTmpCasteljau(i+1, j, k) + v*getTmpCasteljau(i, j+1, k) + w*getTmpCasteljau(i, j, k+1);
            }
        }
    }
    return m_tmpCasteljau[0];
}

glm::vec3 &BezierPatch_Triangle::getTmpCasteljau(size_t i, size_t j, size_t k)
{
    return m_tmpCasteljau[accessValue(k)+j];
}

//////////////////////PRIVATE////////////////////////////

unsigned int BezierPatch_Triangle::accessValue(unsigned int k) const
{
    return k!=0 ? m_size*k - (k*(k-1))/2 : 0;
}
