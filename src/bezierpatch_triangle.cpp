#include "bezierpatch_triangle.h"

BezierPatch_Triangle::BezierPatch_Triangle() : BezierPatch(), m_size(0)
{
}

BezierPatch_Triangle::BezierPatch_Triangle(size_t n) : BezierPatch(((n)*(n+1))/2, std::max((size_t)0, (( ((n)*(n+1))/2) -1)*3)), m_size(n), m_precomputedSums_NMinusK(n)
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


//////////////////////PROTECTED////////////////////////////

void BezierPatch_Triangle::makeVBOLines()
{
    if(m_points.size()<3)
        return;
    //construction du VBO ligne par ligne en trois passes, où chaque passe construit ses lignes en montant progressivement dans un indice.
    int vboIndex=0;

    //première passe triviale : les points sont ordonnés correctement dans le vecteur m_points. Ici on monte selon k
    for(size_t i=0; i<m_points.size()-1; ++i)
    {
        m_VBOLines[vboIndex++]=m_points[i];
    }

    //deuxièmes et troisièmes passes, on monte selon i, puis selon j.

    for(size_t i=0; i<m_size-1; ++i)
    {
        size_t sizeMinusI=m_size-i;
        for(size_t k=0; k<sizeMinusI; ++k)
        {
            size_t j=sizeMinusI-k-1;
            m_VBOLines[vboIndex++]=getPoint(i,j,k);
        }
    }

    for(size_t j=0; j<m_size-1; ++j)
    {
        size_t sizeMinusJ=m_size-j;
        for(size_t i=0; i<sizeMinusJ; ++i)
        {
            size_t k=sizeMinusJ-i-1;
            m_VBOLines[vboIndex++]=getPoint(i,j,k);
        }
    }
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

void BezierPatch_Triangle::drawLines() const
{
    int jump=0;
    for(int i=0; i<3; ++i) //dessin des trois passes
    {
        for(int lineSize=getSize(); lineSize>1; --lineSize)
        {
            glDrawArrays(GL_LINE_STRIP, jump, lineSize);
            jump+=lineSize;
        }
    }
}
