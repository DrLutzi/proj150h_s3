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

const glm::vec3 &BezierPatch_Triangle::getPoint(int i, int j, int k) const
{
    if(i+j+k!=m_size-1); //TODO
    return m_points[m_precomputedSums_NMinusK[k]+j];
}

size_t BezierPatch_Triangle::getSize() const
{
    return m_size;
}

//set

void BezierPatch_Triangle::setPoint(int i, int j, int k, const glm::vec3 &cp)
{
    if(i+j+k!=m_size-1); //TODO
    m_points[m_precomputedSums_NMinusK[k]+j]=cp;
}


//////////////////////PROTECTED////////////////////////////

void BezierPatch_Triangle::makeVBOfromPatch()
{
    //construction du VBO ligne par ligne en trois passes, où chaque passe construit ses lignes en montant progressivement dans un indice.
    int vboIndex=0;

    //première passe triviale : les points sont ordonnés correctement dans le vecteur m_points. Ici on monte selon k
    for(int i=0; i<m_points.size(); ++i)
    {
        m_VBOData[vboIndex++]=m_points[i];
    }

    //deuxièmes et troisièmes passes, on monte selon i, puis selon j.

    for(int i=0; i<m_size-1; ++i)
    {
        int sizeMinusI=m_size-i;
        for(int k=0; k<sizeMinusI; ++k)
        {
            int j=sizeMinusI-k-1;
            m_VBOData[vboIndex++]=getPoint(i,j,k);
        }
    }

    for(int j=0; j<m_size-1; ++j)
    {
        int sizeMinusJ=m_size-j;
        for(int i=0; i<sizeMinusJ; ++i)
        {
            int k=sizeMinusJ-i-1;
            m_VBOData[vboIndex++]=getPoint(i,j,k);
        }
    }
}

//////////////////////PRIVATE////////////////////////////

void BezierPatch_Triangle::fillSums_NMinusK()
{
    unsigned int currentValue=0;
    for(int k=0; k<m_size; ++k)
    {
        m_precomputedSums_NMinusK[k]=currentValue;
        currentValue+=(unsigned int)m_size-k;
    }
}
