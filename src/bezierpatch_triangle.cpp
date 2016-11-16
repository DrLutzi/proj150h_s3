#include "bezierpatch_triangle.h"

BezierPatch_Triangle::BezierPatch_Triangle() : BezierPatch(), m_size(0)
{
}

BezierPatch_Triangle::BezierPatch_Triangle(size_t n) : BezierPatch(((n)*(n+1))/2, std::max((size_t)0, (( ((n)*(n-1))/2))*3)), m_size(n), m_precomputedSums_NMinusK(n), m_tmpCasteljau(m_points.size())
{
    fillSums_NMinusK();
}

BezierPatch_Triangle::~BezierPatch_Triangle()
{}

//////////////////////PUBLIC////////////////////////////
//get

const glm::vec3 &BezierPatch_Triangle::getPoint(size_t i, size_t j, size_t k) const
{
    if(i+j+k!=m_size-1); //TODO
    return m_points[m_precomputedSums_NMinusK[k]+j];
}

size_t BezierPatch_Triangle::getSize() const
{
    return m_size;
}

//set

void BezierPatch_Triangle::setPoint(size_t i, size_t j, size_t k, const glm::vec3 &cp)
{
    if(i+j+k!=m_size-1); //TODO
    m_points[m_precomputedSums_NMinusK[k]+j]=cp;
}

//others



//draw functions

void BezierPatch_Triangle::drawLines(GLint first, GLint baseVertex) const
{
    GLint polygonMode[2];
    glGetIntegerv(GL_POLYGON_MODE, polygonMode);
    glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
    glDrawElementsBaseVertex(GL_TRIANGLES, m_EBOPoints.size(), GL_UNSIGNED_INT, (GLvoid*)(first*sizeof(unsigned int)), 0+baseVertex);
    glPolygonMode(polygonMode[0], polygonMode[1]);
}

void BezierPatch_Triangle::drawBezier(GLint first, GLint baseVertex) const
{
    glDrawElementsBaseVertex(GL_TRIANGLES, m_EBOBezier.size(), GL_UNSIGNED_INT, (GLvoid*)(getSizeEBOPoints_GPU()+first*sizeof(unsigned int)), m_points.size()+baseVertex);
}


//////////////////////PROTECTED////////////////////////////

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

void BezierPatch_Triangle::makeVBOLines()
{
    int eboIndex=0;

    //construction triangle par triangle
    for(size_t k=0; k<getSize()-1; ++k) //"from bottom to top"
    {
        for(size_t j=0; j<getSize()-k-1; ++j) //also read "from left to right"
        {
            //the point iterated, his right neighbor, and top neighbor
            //this isn't easy to maintain, I might change it later
            m_EBOPoints[eboIndex++]=m_precomputedSums_NMinusK[k]+j;
            m_EBOPoints[eboIndex++]=m_precomputedSums_NMinusK[k]+j+1;
            m_EBOPoints[eboIndex++]=m_precomputedSums_NMinusK[k+1]+j;
        }
    }
}

void BezierPatch_Triangle::makeVBOBezierDeCasteljau()
{
    size_t cappedResolution=std::max(size_t(2), m_resolution);

    m_bezier.resize(0);
    m_bezier.reserve(cappedResolution*cappedResolution);

    m_EBOBezier.resize(0);
    m_EBOBezier.reserve(cappedResolution*cappedResolution*3);

    //calculate entire VBO, from top to bottom, left to right (because why not?)
    int eboIndex=0;
    for(size_t k=0; k<cappedResolution; ++k) //"from bottom to top"
    {
        for(size_t j=0; j<cappedResolution-k; ++j) //also read "from left to right"
        {
            size_t i=cappedResolution-k-j-1;
            m_bezier.push_back(casteljau((float)(i)/(cappedResolution-1),
                                         (float)(j)/(cappedResolution-1),
                                         (float)(k)/(cappedResolution-1)));
            //the point created, his right neighbor, and top neighbor.
            //Here we need to be careful with the borders since we can't use the precomputed vector.
            if(i!=0)
            {
                m_EBOBezier.push_back(eboIndex);
                m_EBOBezier.push_back(cappedResolution-k+eboIndex);
                m_EBOBezier.push_back(++eboIndex);
            }
            else
                ++eboIndex;
        }
    }
}

glm::vec3 &BezierPatch_Triangle::getTmpCasteljau(size_t i, size_t j, size_t k)
{
    //this might seem odd, but we can show on the paper that it works for casteljau
    //as straight forward as this as long as the vector has the size of m_points
    return m_tmpCasteljau[m_precomputedSums_NMinusK[k]+j];
}

//////////////////////PRIVATE////////////////////////////

void BezierPatch_Triangle::fillSums_NMinusK()
{
    unsigned int currentValue=0;
    for(size_t k=0; k<m_size; ++k)
    {
        m_precomputedSums_NMinusK[k]=currentValue;
        currentValue+=(unsigned int)m_size-k;
    }
}
