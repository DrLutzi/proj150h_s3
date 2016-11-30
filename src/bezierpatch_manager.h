#ifndef BEZIERPATCH_MANAGER_H
#define BEZIERPATCH_MANAGER_H

#include "bezierpatch_rectangle.h"
#include "bezierpatch_triangle.h"
#include "rpatch2tpatchsolver.h"

class BezierPatch_Manager
{
public:
    BezierPatch_Manager(GLint vaoId, GLint vboPositionId, GLint eboId, GLint uColorLocation);

    //iterator

    typedef std::vector<BezierPatch*>::iterator iterator;
    typedef std::vector<BezierPatch*>::const_iterator const_iterator;

    iterator begin(){return m_patchs.begin();}
    const_iterator begin() const{return m_patchs.begin();}

    iterator end(){return m_patchs.end();}
    const_iterator end() const{return m_patchs.end();}


    /**
     * @brief attaches a patch to the manager.
     * @param patch patch to attach
     * @param reallocate specifies if the function is authorized to call glBindBuffer again if needed.
     * It can be efficient to avoid reallocating when several patches are attached to the manager at once.
     */
    void append(BezierPatch* patch, bool reallocate=true);

    /**
     * @brief removes a patch from the manager and returns it (often for deletion).
     * @param position iterator's position where a patch should be removed
     */
    BezierPatch *remove(iterator position);

    /**
     * @brief allocate the memory necessary to transfer class datas to VBO datas, done by remakeScene().
     */
    void allocateScene();

    /**
     * @brief transfer class datas to VAO/VBO/EBO datas.
     * It updates any needed information about the OpenGL state. It also perform a reallocation if necessary.
     * This function must be called whenever the size of the VBOs or the EBOs increases.
     * If you know it doesn't, you probably prefer to call updateScene instead.
     */
    void remakeScene();

    /**
     * @brief transfer class datas to VAO/VBO/EBO datas.
     * This shouldn't be used if the organisation of the VBO or the EBO changed. Use remakeScene() if it did.
     */
    void updateScene();


    /**
     * @brief draw the scene, using VBO/EBO datas.
     */
    void drawScene();

    bool rayIntersectsCP(const glm::vec3& origin, const glm::vec3& direction, float r, float& distance);
    void updateSelectedCP(const glm::vec3& newPosition);
    inline void releaseSelectedCP(){m_selectedCP=NULL; m_selectedPatch=NULL;}

    //BezierPatch *&operator[](size_t i){return m_patchs[i];} //doesn't work??
    void set(size_t index, BezierPatch* patch){m_patchs[index]=patch;}

    //

private:

    /**
     * @brief allocates the VBO/EBO using glBufferData
     */
    void allocateVBOPosition();
    void allocateEBO();

    /// A vector of patches to be managed
    std::vector<BezierPatch*> m_patchs;

    /// OpenGL objects
    GLint m_VAOId;
    GLint m_VBOPositionId;
    GLint m_EBOId;
    GLint m_uColorLocation;

    /// VBO/EBO sizes
    GLsizeiptr m_VBOPositionSize;
    GLsizeiptr m_EBOSize;
    GLint m_currentBaseVertex;

    GLsizeiptr m_VBOCapacity;
    GLsizeiptr m_EBOCapacity;

    ///Selected CP for
    glm::vec3 *m_selectedCP;
    ///Pointer to the patch that has a selectedCP
    BezierPatch *m_selectedPatch;
};

#endif // BEZIERPATCH_MANAGER_H
