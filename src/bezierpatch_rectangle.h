#ifndef BEZIERPATCH_RECTANGLE_H
#define BEZIERPATCH_RECTANGLE_H

#include "bezierpatch.h"

class BezierPatch_Rectangle : public BezierPatch
{
public:
    BezierPatch_Rectangle();
    BezierPatch_Rectangle(size_t m, size_t n);
    virtual ~BezierPatch_Rectangle();

    //get

    size_t getSizeM() const;
    size_t getSizeN() const;

    const glm::vec3 &getPoint(int i, int j) const;

    //set

    void setPoint(int i, int j, const glm::vec3& cp);

protected:

    void makeVBOfromPatch() override;

private:

    size_t m_sizeM;
    size_t m_sizeN;
};

#endif // BEZIERPATCH_RECTANGLE_H
