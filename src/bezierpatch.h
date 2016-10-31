#ifndef BEZIERPATCH_H
#define BEZIERPATCH_H

#include "glm/glm.hpp"
#include <vector>

class BezierPatch
{
public:
    BezierPatch();
    BezierPatch(size_t sizePatch, size_t sizeVBO);
    ~BezierPatch();

    virtual void elevation(int degree);

    size_t getNumberOfPoints() const;

    const glm::vec3* getVBOData();
    size_t getVBOSize() const;

    /**
     * @brief compute intersection between a control point and a ray
     * @param origin origin of the ray
     * @param direction direction of the ray
     * @param r maximum distance of the control point sphere hitbox
     * @param iIndex index in i
     * @param jIndex index in j
     * @param distance distance between origin and point found
     * @return true if intersection was computed, otherwise false.
     */
    glm::vec3* rayIntersectsCP(const glm::vec3& origin, const glm::vec3& direction, float r, float &distance) const;

protected:

    /**
     * @brief computes the best VBO datas from the patch into m_VBOData
     */
    virtual void makeVBOfromPatch()=0;

    std::vector<glm::vec3> m_points;
    std::vector<glm::vec3> m_VBOData;
};


/*

Le patch est construit selon les indices suivants :

0,0 --------> N-1, j
  |
  |
  |
  v
M-1, i

VBOData permet au GPU de construire rapidement le patch, colonnes par colonnes puis lignes par lignes.

*/

#endif // BEZIERPATCH_H
