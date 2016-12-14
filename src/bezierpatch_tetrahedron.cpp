#include "bezierpatch_tetrahedron.h"

BezierPatch_Tetrahedron::BezierPatch_Tetrahedron() :
    BezierPatch(),
    m_size(0),
    mc_constValue(0)
{
}

BezierPatch_Tetrahedron::BezierPatch_Tetrahedron(size_t n) :
    BezierPatch((n*(n+1)*(n+2))/6, "Tetrahedron"),
    m_size(n),
    m_tmpCasteljau(m_points.size()),
    mc_constValue(3*m_size*m_size + 6*m_size + 2)
{
}

BezierPatch_Tetrahedron::~BezierPatch_Tetrahedron()
{
}

//get

size_t BezierPatch_Tetrahedron::size() const
{
    return m_size;
}

const glm::vec3 &BezierPatch_Tetrahedron::getPoint(size_t i, size_t j, size_t k, size_t l) const
{
    if(i+j+k+l!=m_size-1); //TODO
    return m_points[indexOf(j,k,l)];
}

//set

void BezierPatch_Tetrahedron::setPoint(size_t i, size_t j, size_t k, size_t l, const glm::vec3& cp)
{
    if(i+j+k+l!=m_size-1); //TODO
    m_points[indexOf(j,k,l)]=cp;
}

void BezierPatch_Tetrahedron::makePatch()
{
    int eboIndex=0;

    //A similar approach to making a triangle patch, yet, again, 4 times.
    //We are not going to show any of the internal links of the patch.
    //This might be useful in another context, such as cutting a tetrahedron.
    m_EBOPoints.resize(std::max(  (size_t)0, (( ((size())*(size()-1)) /2 )) * 3 * 4)  );

    //remember that one of the coordinates is useless because dependant on the others. We chose i
    //in the get/set functions, might as well do the same here.
    size_t j,k,l;

    //first step: l=0, AKA front triangle.
    //this is the easiest, because we filled our VBO with the front triangle first.
    //Blatently copy the code from the triangle patch.
    l=0;
    for(size_t k=0; k<size()-1; ++k)
    {
        for(size_t j=0; j<size()-k-1; ++j)
        {
            //the point iterated, his right neighbor, and top neighbor
            m_EBOPoints[eboIndex++]=indexOf(j,k,l);
            m_EBOPoints[eboIndex++]=indexOf(j+1,k,l);
            m_EBOPoints[eboIndex++]=indexOf(j,k+1,l);
        }
    }

    //second step: k=0, AKA bottom triangle.
    //This is kind of harder, as the points of a line are not set in a continuous fashion.
    //However, with the accessValue fonction, this is still nothing really difficult.
    //We only need to pay attention to which triangles we want to draw and what this implies for their coordinates.
    k=0;
    for(size_t l=0; l<size()-1; ++l)
    {
        for(size_t j=0; j<size()-l-1; ++j)
        {
            m_EBOPoints[eboIndex++]=indexOf(j,k,l);
            m_EBOPoints[eboIndex++]=indexOf(j+1,k,l);
            m_EBOPoints[eboIndex++]=indexOf(j,k,l+1);
        }
    }

    //third step: j=0, AKA left triangle.
    j=0;
    for(size_t k=0; k<size()-1; ++k)
    {
        for(size_t l=0; l<size()-k-1; ++l)
        {
            m_EBOPoints[eboIndex++]=indexOf(j,k,l);
            m_EBOPoints[eboIndex++]=indexOf(j,k,l+1);
            m_EBOPoints[eboIndex++]=indexOf(j,k+1,l);
        }
    }

    //final step: i=0, AKA right triangle.
    //Since i doesn't interfer in the calculation, we need to introduce j to have 3 variables.
    for(size_t k=0; k<size()-1; ++k)
    {
        for(size_t l=0; l<size()-k-1; ++l)
        {
            size_t j=size()-k-l-1;
            m_EBOPoints[eboIndex++]=indexOf(j,k,l);
            m_EBOPoints[eboIndex++]=indexOf(j-1,k,l+1);
            m_EBOPoints[eboIndex++]=indexOf(j-1,k+1,l);
        }
    }
}

void BezierPatch_Tetrahedron::makeSurfaceDeCasteljau()
{
    size_t cappedResolution=std::max(size_t(2), m_resolution);

    m_surface.resize(0);
    m_surface.reserve(cappedResolution*cappedResolution*2);

    m_EBOSurface.resize(0);
    m_EBOSurface.reserve(cappedResolution*cappedResolution*8);

    //we will only calculate the external surface. More options could be added.
    //To do this, we need to succesfully display the 4 external triangles of the thetraedron surface.
    //These faces all have a common point : i,j,k or l is always equal to 0,
    //hence we will calculate a triangle bezier surface for l=0 (front), then k=0, then j=0 and finally i=0.
    //Each triangle will be proprely oriented to have a calculation similar to the one of BezierPatch_Triangle.

    int eboIndex=0;
    size_t i,j,k,l;

    //First step: l=0 (front)
    l=0;
    for(k=0; k<cappedResolution; ++k) //"from bottom to top"
    {
        for(j=0; j<cappedResolution-k; ++j) //also read "from left to right"
        {
            i=cappedResolution-k-j-1;
            m_surface.push_back(casteljau((float)(i)/(cappedResolution-1),
                                         (float)(j)/(cappedResolution-1),
                                         (float)(k)/(cappedResolution-1),
                                        (float)(l)/(cappedResolution-1)));
            //the point created, his right neighbor, and top neighbor.
            //Here we need to be careful with the borders since we can't use the precomputed vector.
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

    //Second step: k=0 (bottom)
    //You'll note that the bottom line of the triangle was already drawn.
    //Though, starting l at 0 is a more convenient way to draw the VAO afterwards.
    k=0;
    for(l=0; l<cappedResolution; ++l)
    {
        for(i=0; i<cappedResolution-l; ++i)
        {
            j=cappedResolution-l-i-1;
            m_surface.push_back(casteljau((float)(i)/(cappedResolution-1),
                                         (float)(j)/(cappedResolution-1),
                                         (float)(k)/(cappedResolution-1),
                                        (float)(l)/(cappedResolution-1)));
            if(j!=0)
            {
                m_EBOSurface.push_back(eboIndex);
                m_EBOSurface.push_back(cappedResolution-l+eboIndex);
                m_EBOSurface.push_back(++eboIndex);
            }
            else
                ++eboIndex;
        }
    }

    //Third step: j=0 (left)
    j=0;
    for(l=0; l<cappedResolution; ++l)
    {
        for(k=0; k<cappedResolution-l; ++k)
        {
            i=cappedResolution-l-k-1;
            m_surface.push_back(casteljau((float)(i)/(cappedResolution-1),
                                         (float)(j)/(cappedResolution-1),
                                         (float)(k)/(cappedResolution-1),
                                        (float)(l)/(cappedResolution-1)));
            if(i!=0)
            {
                m_EBOSurface.push_back(eboIndex);
                m_EBOSurface.push_back(cappedResolution-l+eboIndex);
                m_EBOSurface.push_back(++eboIndex);
            }
            else
                ++eboIndex;
        }
    }

    //Final step: i=0 (right)
    i=0;
    for(l=0; l<cappedResolution; ++l)
    {
        for(j=0; j<cappedResolution-l; ++j)
        {
            k=cappedResolution-l-j-1;
            m_surface.push_back(casteljau((float)(i)/(cappedResolution-1),
                                         (float)(j)/(cappedResolution-1),
                                         (float)(k)/(cappedResolution-1),
                                        (float)(l)/(cappedResolution-1)));
            if(k!=0)
            {
                m_EBOSurface.push_back(eboIndex);
                m_EBOSurface.push_back(cappedResolution-l+eboIndex);
                m_EBOSurface.push_back(++eboIndex);
            }
            else
                ++eboIndex;
        }
    }

    return;
}

//operators

BezierPatch_Tetrahedron& BezierPatch_Tetrahedron::operator=(const BezierPatch_Tetrahedron& other)
{
    if(&other!=this)
    {
        m_size=other.size();
        BezierPatch::operator=(other);
        m_tmpCasteljau.resize(m_points.size());
        mc_constValue=3*m_size*m_size + 6*m_size + 2;
    }
    return (*this);
}

//others

void BezierPatch_Tetrahedron::drawPatch() const
{
    if(!m_drawPatch)
        return;

    GLint polygonMode[2];
    glGetIntegerv(GL_POLYGON_MODE, polygonMode);
    glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);

    size_t quarterSize=m_EBOPoints.size()/4;

    for(unsigned int i=0; i<4; ++i)
    glDrawElementsBaseVertex(GL_TRIANGLES, quarterSize, GL_UNSIGNED_INT, (GLvoid*)(m_firstEBO+i*quarterSize*sizeof(unsigned int)), 0+m_baseVertexEBO);

    glPolygonMode(polygonMode[0], polygonMode[1]);
}

void BezierPatch_Tetrahedron::drawSurface() const
{
    if(!m_drawSurface)
        return;

    size_t quarterSurface=m_EBOSurface.size()/4;
    for(unsigned int i=0; i<4; ++i)
        glDrawElementsBaseVertex(GL_TRIANGLES, quarterSurface, GL_UNSIGNED_INT, (GLvoid*)(m_firstEBO+getSizeEBOPosition() + quarterSurface*i*sizeof(unsigned int) ), m_points.size()+m_baseVertexEBO);
}

const glm::vec3 &BezierPatch_Tetrahedron::casteljau(float u, float v, float w, float t)
{
    //first, as usual, fill the tmp vector with the current control points in m_points
    for(size_t i=0; i<m_points.size(); ++i)
        m_tmpCasteljau[i]=m_points[i];

    //then compute a new set of control points by linear interpolation, smaller than the previous one
    //do that until the size of the set is 1
    size_t setSize;
    size_t i=0, j=0, k=0, l=0;
    for(setSize=m_size-1; setSize>0; --setSize)
    {
        for(l=0; l<setSize; ++l)
        {
            for(k=0; k<setSize-l; ++k)
            {
                size_t sizeMinusKL=setSize-l-k;
                for(j=0; j<sizeMinusKL; ++j)
                {
                    i=sizeMinusKL-j-1;
                    getTmpCasteljau(i,j,k,l)= u*getTmpCasteljau(i+1, j, k, l) + v*getTmpCasteljau(i, j+1, k, l) + w*getTmpCasteljau(i, j, k+1, l) + t*getTmpCasteljau(i, j, k, l+1);
                }
            }
        }
    }
    return m_tmpCasteljau[0];
}

glm::vec3 &BezierPatch_Tetrahedron::getTmpCasteljau(size_t i, size_t j, size_t k, size_t l)
{
    return m_tmpCasteljau[accessValue(k, l)+j];
}

size_t BezierPatch_Tetrahedron::indexOf(size_t j, size_t k, size_t l) const
{
    return accessValue(k, l)+j;
}

unsigned int BezierPatch_Tetrahedron::accessValue(unsigned int k, unsigned int l) const
{
    //When making a function to access a simple vector using 4 values, we can first test what it is to try it using 3, then 2.
    //In a Bezier tetrahedron, one of the variables is useless.
    //In Bezier Triangles, we showed what it was like to use 2 variables to access a vector.
    //By dividing the tetrahedron patch as a set of triangles, we can access the Bezier Tetrahedron CPs by
    //multiplying the number of points in the triangle by value by the last coordinate.
    //Unfortunately the access value of the triangle is dependant on the size of the triangle,
    //and the size of the triangles behind the front triangle (l=0) decreases for each l value >0. We had to perform the expansion of a sum of a sum dependant on
    //the first sum parameter on MATLAB to find a more convenient way to access the tetrahedron than iterating everytime to compute the sum.

    //We also use mc_constValue, which pre-computed a part of the equation which is only dependant on the size of the patch.
    unsigned int lDepth = l!=0 ? (l*(l*(l - 3*(m_size+1)) + mc_constValue))/6 : 0;

    unsigned int kHeight = k!=0 ? (m_size-l)*k - (k*(k-1))/2 : 0;
    return lDepth+kHeight;
}
