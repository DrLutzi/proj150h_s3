#include "bezierpatch_hexaedron.h"

BezierPatch_Hexaedron::BezierPatch_Hexaedron():
    BezierPatch(),
    m_sizeM(0),
    m_sizeN(0),
    m_sizeP(0)
{
}

BezierPatch_Hexaedron::BezierPatch_Hexaedron(size_t m, size_t n, size_t p):
    BezierPatch(m*n*p, "Hexaedron"),
    m_sizeM(m),
    m_sizeN(n),
    m_sizeP(p),
    m_tmpCasteljau(m_points.size())
{}

BezierPatch_Hexaedron::~BezierPatch_Hexaedron()
{

}

//get

const glm::vec3 &BezierPatch_Hexaedron::getPoint(size_t i, size_t j, size_t k) const
{
    return m_points[indexOf(i,j,k)];
}

//set

void BezierPatch_Hexaedron::setPoint(size_t i, size_t j, size_t k, const glm::vec3& cp)
{
    m_points[indexOf(i,j,k)]=cp;
}

void BezierPatch_Hexaedron::makePatch()
{
    //Since we decided to only display the outside surface, we have to compute a surface
    //for each face of the hexaedron. There are six.
    //2 faces of size (m,n), 2 of size (m,p), 2 of size (n,p)
    //and each EBO will contain twice the amount of points on a face. We factorize the expression.
    m_EBOPoints.resize(4*(sizeM()*(sizeN()+sizeP()) + sizeN()*sizeP()));

    size_t i,j,k;
    unsigned int eboIndex=0;

    //To draw these surface we copy the code for bezierpatch_rectangle and we adapt it. Six times, of course.
    //We will use a lambda function for that.
    //First and Second have a reference either equal to i, j or k.
    auto fillEBO=[&](size_t &first, size_t &second, size_t firstLimit, size_t secondLimit)
    {
        for(first=0; first<firstLimit; ++first)
        {
            for(second=0; second<secondLimit; ++second)
            {
                m_EBOPoints[eboIndex++]=indexOf(i,j,k);
            }
        }

        for(second=0; second<secondLimit; ++second)
        {
            for(first=0; first<firstLimit; ++first)
            {
                m_EBOPoints[eboIndex++]=indexOf(i,j,k);
            }
        }
    };

    //Step 1: k=0 (front face)
    k=0;
    fillEBO(i,j,sizeM(),sizeN());

    //Step 2: k=P (back face)
    k=sizeP()-1;
    fillEBO(i,j,sizeM(),sizeN());

    //Step 3: j=0 (upper face)
    j=0;
    fillEBO(i,k,sizeM(),sizeP());

    //Step 4: j=N (lower face)
    j=sizeN()-1;
    fillEBO(i,k,sizeM(),sizeP());

    //Step 5: i=0 (left face)
    i=0;
    fillEBO(j,k,sizeN(),sizeP());

    //Step 6: i=M (right face)
    i=sizeM()-1;
    fillEBO(j,k,sizeN(),sizeP());
}

void BezierPatch_Hexaedron::makeSurfaceDeCasteljau()
{
    size_t cappedResolution=std::max(size_t(2), m_resolution);

    m_surface.resize(0);
    m_surface.reserve(cappedResolution*cappedResolution*6);

    m_EBOSurface.resize(0);
    m_EBOSurface.reserve(cappedResolution*cappedResolution*12);

    size_t i,j,k, EBOSize=0;
    //We have to do the same thing 6 times again! Just do BezierPatch_rectangle for 6 faces.
    auto fillVBOnEBO=[&](size_t &first, size_t &second)
    {
        for(first=0; first<cappedResolution; ++first)
            for(second=0; second<cappedResolution; ++second)
            {
                m_surface.push_back(casteljau((float)(i)/(cappedResolution-1), (float)j/(cappedResolution-1), (float)k/(cappedResolution-1)));
                if(first+1<cappedResolution)
                {
                    m_EBOSurface.push_back(EBOSize + first*cappedResolution+second);
                    m_EBOSurface.push_back(EBOSize + (first+1)*cappedResolution+second);
                }
            }

        EBOSize=m_surface.size();
    };

    //Step 1: k=0 (front face)
    k=0;
    fillVBOnEBO(i,j);

    //Step 2: k=P (back face)
    k=cappedResolution-1;
    fillVBOnEBO(i,j);

    //Step 3: j=0 (upper face)
    j=0;
    fillVBOnEBO(i,k);

    //Step 4: j=N (lower face)
    j=cappedResolution-1;
    fillVBOnEBO(i,k);

    //Step 5: i=0 (left face)
    i=0;
    fillVBOnEBO(j,k);

    //Step 6: i=M (right face)
    i=cappedResolution-1;
    fillVBOnEBO(j,k);
}

//operators

BezierPatch_Hexaedron& BezierPatch_Hexaedron::operator=(const BezierPatch_Hexaedron &other)
{
    if(&other!=this)
    {
        m_sizeM=other.sizeM();
        m_sizeN=other.sizeN();
        m_sizeP=other.sizeP();
        BezierPatch::operator=(other);
        m_tmpCasteljau.resize(sizeM()*sizeN()*sizeP());
    }
    return (*this);
}

//others

void BezierPatch_Hexaedron::drawPatch() const
{
    if(!m_drawPatch)
        return;

    size_t drawnSize=0;

    //You should be used by this copypasta from BezierPatch_Rectangle from now
    auto drawRectangularPatch=[&](size_t firstLimit, size_t secondLimit)
    {
        size_t fullLimitSize=firstLimit*secondLimit;
        //first draw all the horizontal lines
        for(size_t i=0; i<fullLimitSize; i+=secondLimit)
            glDrawElementsBaseVertex(GL_LINE_STRIP, secondLimit, GL_UNSIGNED_INT,
                                     (GLvoid*)(m_firstEBO + (i+drawnSize)*sizeof(unsigned int)), m_baseVertexEBO);

        //do the same with the vertical lines
        for(size_t j=0; j<fullLimitSize; j+=firstLimit)
            glDrawElementsBaseVertex(GL_LINE_STRIP, firstLimit, GL_UNSIGNED_INT,
                                     (GLvoid*)(m_firstEBO + (fullLimitSize+j+drawnSize)*sizeof(unsigned int)), m_baseVertexEBO);

        drawnSize+=2*fullLimitSize;
    };

    //Front and back
    drawRectangularPatch(sizeM(), sizeN());
    drawRectangularPatch(sizeM(), sizeN());

    //Up and down
    drawRectangularPatch(sizeM(), sizeP());
    drawRectangularPatch(sizeM(), sizeP());

    //Left and Right
    drawRectangularPatch(sizeN(), sizeP());
    drawRectangularPatch(sizeN(), sizeP());
}

void BezierPatch_Hexaedron::drawSurface() const
{
    if(!m_drawSurface)
        return;

    size_t cappedResolution=std::max(size_t(2), m_resolution);

    for(size_t i=0; i<(cappedResolution-1)*6; ++i)
        glDrawElementsBaseVertex(GL_TRIANGLE_STRIP, 2*cappedResolution, GL_UNSIGNED_INT,
                                 (GLvoid*)(m_firstEBO + getSizeEBOPosition() + (i*2*cappedResolution)*sizeof(unsigned int)), m_points.size()+m_baseVertexEBO);
}

const glm::vec3 &BezierPatch_Hexaedron::casteljau(float u, float v, float w)
{
    //first fill tmp tab with all the control points, like always...
    for(size_t i=0; i<m_points.size(); ++i)
        m_tmpCasteljau[i]=m_points[i];

    //tensor product in w
    size_t setSize;

    //this will create a rectangular patch fixed in w
    for(setSize=sizeP()-1; setSize>0; --setSize)
    {
        for(size_t k=0; k<setSize; ++k)
        {
            for(size_t j=0; j<sizeN(); ++j)
            {
                for(size_t i=0; i<sizeM(); ++i)
                    getTmpCasteljau(i, j, k)= (1-w)*getTmpCasteljau(i, j, k) + (w)*getTmpCasteljau(i, j, k+1);
            }
        }
    }

    //From this point on, this is just pure rectangular patch copy pasting
    //This will create a curve fixed in u
    for(setSize=sizeM()-1; setSize>0; --setSize)
    {
        for(size_t i=0; i<setSize; ++i)
        {
            for(size_t j=0; j<sizeN(); ++j)
            {
                getTmpCasteljau(i, j, 0)= (1-u)*getTmpCasteljau(i, j, 0) + (u)*getTmpCasteljau(i+1, j, 0);
            }
        }
    }

    //1 casteljau on v left to do

    for(setSize=sizeN()-1; setSize>0; --setSize)
    {
        for(size_t j=0; j<setSize; ++j)
        {
            getTmpCasteljau(0, j, 0)= (1-v)*getTmpCasteljau(0, j, 0) + (v)*getTmpCasteljau(0, j+1, 0);
        }
    }

    return m_tmpCasteljau[0];
}

glm::vec3 &BezierPatch_Hexaedron::getTmpCasteljau(size_t i, size_t j, size_t k)
{
    return m_tmpCasteljau[indexOf(i,j,k)];
}

//private

size_t BezierPatch_Hexaedron::indexOf(size_t i, size_t j, size_t k) const
{
    return sizeN()*(sizeM()*k + i) + j;
}
