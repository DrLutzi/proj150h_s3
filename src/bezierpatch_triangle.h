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

    size_t getSizeM() const;
    size_t getSizeN() const;
    size_t getSizeP() const;

    const glm::vec3 &getPoint(int i, int j, int k) const;

    //set

    void setPoint(int i, int j, int k, const glm::vec3& cp);

protected:

    void makeVBOfromPatch() override;

private:

    size_t m_sizeN;
};

#endif // BEZIERPATCH_TRIANGLE_H
