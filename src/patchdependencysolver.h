#ifndef PATCHDEPENDENCYSOLVER_H
#define PATCHDEPENDENCYSOLVER_H

#include "productpolynom3var.h"
#include "bezierpatch_rectangle.h"
#include "bezierpatch_triangle.h"
#include "bezierpatch_hexaedron.h"
#include "bezierpatch_tetrahedron.h"

/**
 * @brief represents a single dependency between a patch and two other patchs, between the following combinations:
 * Rectangle -> 2 Triangles
 * Hexaedron -> 2 Tetrahedrons
 */

class BezierPatch_Manager;

class PatchDependencySolver
{
public:
    PatchDependencySolver(BezierPatch_Manager &mgr);
    ~PatchDependencySolver();

    /**
     * @brief createR2TDependency creates a rectangle -> 2 triangles dependency relation
     * @param id id of the rectangle patch inside the manager
     * @return true if the dependency was correctly created
     */
    bool createR2TDependency(unsigned int id);

    /**
     * @brief createH2TDependency creates a hexaedron -> 2 tetrahedrons dependency relation
     * @param id id of the hexaedron patch inside the manager
     * @param face face of the hexaedron we want to set a dependency to
     * @return true if the dependency was correctly created
     */
    bool createH2TDependency(unsigned int id, BezierPatch_Hexaedron::Face_t face);

    /**
     * @brief updateDependency updates the child patches with the current dependency set
     */
    void updateDependency();

private:

    /**
     * @brief copies some of the content of parent into ch1 and ch2.
     */
    void copyParent(BezierPatch *parent, BezierPatch *ch1, BezierPatch *ch2);

    /**
     * @brief indexOf returns a single index to access the vector like a matrix using j1 and j2.
     */
    inline size_t indexOf(size_t j1, size_t j2) {return j1*m_sizeN+j2;}

    /**
     * @brief the size of the matrix (m,n) deduced from the patch
     */
    size_t m_sizeM;
    size_t m_sizeN;

    typedef enum {NO_DEPENDENCY, R2T, H2T} dependency_t;

    /**
     * @brief m_dependencyType what type of dependency the instance holds
     */
    dependency_t            m_dependencyType;

    /**
     * @brief m_patchInfos adds infos for the parent patch dependency
     * (only needed for knowing the hexaedron face yet)
     */
    BezierPatch_Hexaedron::Face_t   m_parentPatchInfos;

    /**
     * @brief m_manager a link to the program's bezier patch manager
     */
    BezierPatch_Manager     &m_manager;
    /**
     * @brief m_parentId id of the parent patch inside the manager
     */
    unsigned int            m_parentId;

    unsigned int            m_child1Id;
    unsigned int            m_child2Id;

    std::vector<Sum_ProductPolynom3Var> m_dependencyMatrix;
};

#endif // PATCHDEPENDENCYSOLVER_H
