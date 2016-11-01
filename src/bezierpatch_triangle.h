#ifndef BEZIERPATCH_TRIANGLE_H
#define BEZIERPATCH_TRIANGLE_H

#include "bezierpatch.h"

class BezierPatch_Triangle : BezierPatch
{
public:
    BezierPatch_Triangle();

    BezierPatch_Triangle(size_t n);
    virtual ~BezierPatch_Triangle();

    //get

    size_t getSize() const;

    const glm::vec3 &getPoint(int i, int j, int k) const;

    //set

    void setPoint(int i, int j, int k, const glm::vec3& cp);

protected:

    void makeVBOfromPatch() override;

private:

    /**
     * @brief remplit m_precomputedSums_NMinusK avec \sum_{i=0}^{k-1}(n-k); utilisé pour accéder aux CPs avec un 3-uplet (i,j,k).
     */
    void fillSums_NMinusK();

    size_t m_size;
    std::vector<unsigned int> m_precomputedSums_NMinusK;
};

#endif // BEZIERPATCH_TRIANGLE_H
