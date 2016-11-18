#ifndef RPATCH2TPATCHSOLVER_H
#define RPATCH2TPATCHSOLVER_H

#include "bezierpatch_rectangle.h"
#include "bezierpatch_triangle.h"

class RPatch2TPatchSolver
{

public:
    typedef enum {Solver_UpperTriangle=0, Solver_LowerTriangle=1} trianglePart_t;

    RPatch2TPatchSolver();
    RPatch2TPatchSolver(size_t nOrder, size_t mOrder);

    BezierPatch_Triangle solveFrom(const BezierPatch_Rectangle& rectanglePatch, trianglePart_t triangle);

private:
    std::vector<double> m_tCoefMatrix;
};

#endif // RPATCH2TPATCHSOLVER_H
