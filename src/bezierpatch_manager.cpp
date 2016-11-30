#include "bezierpatch_manager.h"



BezierPatch_Manager::BezierPatch_Manager(GLint vaoId, GLint vboPositionId, GLint eboId, GLint uColorLocation) :
    m_VAOId(vaoId),
    m_VBOPositionId(vboPositionId),
    m_EBOId(eboId),
    m_uColorLocation(uColorLocation),
    m_VBOPositionSize(0),
    m_EBOSize(0),
    m_currentBaseVertex(0),
    m_VBOCapacity(0),
    m_EBOCapacity(0),
    m_selectedCP(NULL),
    m_selectedPatch(NULL)
{
}

void BezierPatch_Manager::append(BezierPatch* patch, bool reallocate)
{
    if(patch!=NULL)
    {
        m_patchs.push_back(patch);

        //the indexes where we finished writting
        GLsizeiptr firstVBO=m_VBOPositionSize;
        GLsizeiptr firstEBO=m_EBOSize;
        GLsizeiptr baseVertex=m_currentBaseVertex;

        m_VBOPositionSize   +=      patch->getSizeVBOPosition()+patch->getSizeVBOSurface();
        m_EBOSize           +=      patch->getSizeEBOPosition()+patch->getSizeEBOSurface();
        m_currentBaseVertex +=      patch->getNumberOfPointsPatch()+patch->getNumberOfPointsSurface();

        if(reallocate && (m_VBOCapacity<m_VBOPositionSize || m_EBOCapacity<m_EBOSize))
        {
            //remake the whole scene
            remakeScene();
        }
        else
        {
            //set first indexes and base vertex only
            patch->setFirstVBO(firstVBO);
            patch->setFirstEBO(firstEBO);
            patch->setBaseVertexEBO(baseVertex);
        }
    }
}

BezierPatch *BezierPatch_Manager::remove(iterator position)
{
    BezierPatch *removedPtr=(*position);
    m_patchs.erase(position);
    remakeScene();
    return removedPtr;
}

void BezierPatch_Manager::allocateScene()
{
    allocateVBOPosition();
    allocateEBO();
}

void BezierPatch_Manager::remakeScene()
{
    //first check if the desired size of the scene is bigger than its current maximum size and update first indexes
    m_VBOPositionSize   =   0;
    m_EBOSize           =   0;
    m_currentBaseVertex =   0;
    for(iterator it=begin(); it!=end(); ++it)
    {
        BezierPatch* patch=(*it);

        if(patch!=NULL)
        {
            //the patches need to be forced to re-fill datas into the VBO/EBO, as intended
            patch->notifyPatchChanged();

            patch->setFirstVBO(m_VBOPositionSize);
            patch->setFirstEBO(m_EBOSize);
            patch->setBaseVertexEBO(m_currentBaseVertex);

            m_VBOPositionSize   +=      patch->getSizeVBOPosition()+patch->getSizeVBOSurface();
            m_EBOSize           +=      patch->getSizeEBOPosition()+patch->getSizeEBOSurface();
            m_currentBaseVertex +=      patch->getNumberOfPointsPatch()+patch->getNumberOfPointsSurface();
        }
    }

    //re-allocate if the new size of the scene is bigger than the old size
    if(m_VBOPositionSize>m_VBOCapacity)
    {
        allocateVBOPosition();
    }

    //do that for the EBO, too
    if(m_EBOSize>m_EBOCapacity)
    {
        allocateEBO();
    }

    //finally, let classes fill whatever information they want in their VBOs or EBOs.
    updateScene();
}

void BezierPatch_Manager::updateScene()
{
    //transfer informations to the VBO and the EBO.
    for(iterator it=begin(); it!=end(); ++it)
    {
        BezierPatch* patch=(*it);

        if(patch!=NULL)
        {
            patch->updateVBOPatch(m_VBOPositionId, m_EBOId);
            patch->updateVBOSurface(m_VBOPositionId, m_EBOId);
        }
    }
}

void BezierPatch_Manager::drawScene()
{
    for(iterator it=begin(); it!=end(); ++it)
    {
        BezierPatch* patch=(*it);

        patch->draw(m_uColorLocation);
    }
}

//CONTROL POINT SELECTION

bool BezierPatch_Manager::rayIntersectsCP(const glm::vec3& origin, const glm::vec3& direction, float r, float& distance)
{
    m_selectedCP=NULL;
    for(iterator it=begin(); it!=end(); ++it)
    {
        BezierPatch* patch=(*it);

        //try to connect with a control point (this function also modifies r)
        glm::vec3 *other=patch->rayIntersectsCP(origin, direction, r, distance);
        if(other!=NULL)
        {
            m_selectedCP=other;
            m_selectedPatch=patch;
        }
    }
    return m_selectedCP!=NULL;
}

void BezierPatch_Manager::updateSelectedCP(const glm::vec3& newPosition)
{
    if(m_selectedCP!=NULL)
    {
        (*m_selectedCP)=newPosition;
        if(m_selectedPatch!=NULL)
        {
            m_selectedPatch->notifyPatchChanged();
        }
    }
}

//////////////////////PRIVATE////////////////////////////

void BezierPatch_Manager::allocateVBOPosition()
{
    //simple memory allocation
    glBindBuffer(GL_ARRAY_BUFFER, m_VBOPositionId);
    glBufferData(GL_ARRAY_BUFFER, m_VBOPositionSize, NULL, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    m_VBOCapacity=m_VBOPositionSize;
}

void BezierPatch_Manager::allocateEBO()
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBOId);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_EBOSize, NULL, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    m_EBOCapacity=m_EBOSize;
}
