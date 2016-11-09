#ifndef BEZIERPATCH_TRIANGLE_H
#define BEZIERPATCH_TRIANGLE_H

#include "bezierpatch.h"

class BezierPatch_Triangle : public BezierPatch
{
public:
    BezierPatch_Triangle();

    BezierPatch_Triangle(size_t n);
    virtual ~BezierPatch_Triangle();

    //get

    size_t getSize() const;

    const glm::vec3 &getPoint(size_t i, size_t j, size_t k) const;

    //set

    void setPoint(size_t i, size_t j, size_t k, const glm::vec3& cp);

    //others

    void drawLines(GLint first=0, GLint baseVertex=0) const;
    void drawBezier(GLint first=0, GLint baseVertex=0) const;

protected:

    const glm::vec3 &casteljau(float u, float v, float w);

    void makeVBOLines();

    void makeVBOBezierDeCasteljau();

    glm::vec3 &getTmpCasteljau(size_t i, size_t j, size_t k);

private:

    /**
     * @brief remplit m_precomputedSums_NMinusK avec \sum_{i=0}^{k-1}(n-k); utilisé pour accéder aux CPs avec un 3-uplet (i,j,k).
     */
    void fillSums_NMinusK();

    size_t m_size;
    std::vector<unsigned int> m_precomputedSums_NMinusK;

    std::vector<glm::vec3> m_tmpCasteljau;
};

#endif // BEZIERPATCH_TRIANGLE_H
