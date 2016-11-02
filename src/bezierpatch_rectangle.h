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

    const glm::vec3 &getPoint(size_t i, size_t j) const;

    //set

    void setPoint(size_t i, size_t j, const glm::vec3& cp);

    //others

    void drawLines() const;

protected:

    void makeVBOfromPatch();

private:

    size_t m_sizeM;
    size_t m_sizeN;
};

class BezierPatch_Square : public BezierPatch_Rectangle
{
    BezierPatch_Square();
    BezierPatch_Square(size_t size);
    virtual ~BezierPatch_Square();
};

#endif // BEZIERPATCH_RECTANGLE_H
