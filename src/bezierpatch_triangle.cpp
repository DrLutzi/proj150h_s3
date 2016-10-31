#include "bezierpatch_triangle.h"

BezierPatch_Triangle::BezierPatch_Triangle() : BezierPatch()
{
}

BezierPatch_Triangle::BezierPatch_Triangle(size_t n) : BezierPatch(((n)*(n+1))/2, std::max(0, (( ((n)*(n+1))/2) -1)*3)), m_sizeN(n)
{}

BezierPatch_Triangle::~BezierPatch_Triangle()
{}

//////////////////////PUBLIC////////////////////////////
//get

const glm::vec3 &BezierPatch_Triangle::getPoint(int i, int j, int k) const
{
    return; //TODO
}

size_t BezierPatch_Triangle::getSizeN() const
{
    return m_sizeN;
}

//set

void BezierPatch_Triangle::setPoint(int i, int j, int k, const glm::vec3 &cp)
{
    m_points[i*m_sizeN+j]=cp;
}


//////////////////////PRIVATE////////////////////////////

void BezierPatch_Triangle::makeVBOfromPatch()
{

}
