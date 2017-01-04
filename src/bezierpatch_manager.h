#ifndef BEZIERPATCH_MANAGER_H
#define BEZIERPATCH_MANAGER_H

#include "bezierpatch_rectangle.h"
#include "bezierpatch_triangle.h"
#include "bezierpatch_tetrahedron.h"
#include "bezierpatch_hexaedron.h"
#include "errorsHandler.hpp"
#include "patchdependencysolver.h"

#include <map>

class BezierPatch_Manager
{

public:
    BezierPatch_Manager(GLint vaoId, GLint vboPositionId, GLint eboId, GLint uColorLocation);

    //iterator

    typedef std::map<unsigned int, BezierPatch*>::iterator iterator;
    typedef std::map<unsigned int, BezierPatch*>::const_iterator const_iterator;

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
     * @param id id of the patch to be removed
     */
    BezierPatch *remove(unsigned int id);

    /**
     * @brief removeDependency remove a dependency between patches.
     * @param id id of the patch.
     */
    void removeDependency(unsigned int id);

    /**
     * @brief addDependency adds a dependency to a patch.
     * @param id id of the patch.
     * @return reference to the created dependency
     */
    PatchDependencySolver &addDependency(unsigned int id);

    /**
     * @brief addDependency updates the dependency of a patch.
     * @param id id of the patch.
     */
    void updateDependency(unsigned int id);

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

    ///
    /// \brief rayIntersectsCP computes a best intersection with all the drawn patches, using a shot ray.
    /// \param origin origin of the ray
    /// \param direction of the ray
    /// \param r hitBox of the control points
    /// \param distance it hit, used to update the CP later on
    /// \return if an intersection was or not found
    ///
    bool rayIntersectsCP(const glm::vec3& origin, const glm::vec3& direction, float r, float &distance);
    void updateSelectedCP(const glm::vec3& newPosition);
    void releaseSelectedCP();

    BezierPatch *operator[](unsigned int i);

    void setPatch(unsigned int index, BezierPatch* patch);
    BezierPatch* getPatch(unsigned int index);

private:

    /**
     * @brief allocates the VBO/EBO using glBufferData
     */
    void allocateVBOPosition();
    void allocateEBO();

    /// A vector of patches to be managed
    std::map<unsigned int, BezierPatch*>               m_patchs;
    std::map<unsigned int, PatchDependencySolver>      m_dependencies;

    /// OpenGL objects
    GLint                           m_VAOId;
    GLint                           m_VBOPositionId;
    GLint                           m_EBOId;
    GLint                           m_uColorLocation;

    /// VBO/EBO sizes
    GLsizeiptr                      m_VBOPositionSize;
    GLsizeiptr                      m_EBOSize;
    GLint                           m_currentBaseVertex;

    GLsizeiptr                      m_VBOCapacity;
    GLsizeiptr                      m_EBOCapacity;

    ///Selected CP for
    BezierPatch::RayHit             m_selectionHitProperties;
};

#endif // BEZIERPATCH_MANAGER_H
