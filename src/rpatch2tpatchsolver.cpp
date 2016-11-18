#include "rpatch2tpatchsolver.h"

RPatch2TPatchSolver::RPatch2TPatchSolver()
{
}

RPatch2TPatchSolver::RPatch2TPatchSolver(size_t nOrder, size_t mOrder) : m_tCoefMatrix((nOrder+mOrder+1)*(nOrder+mOrder+1))
{
}

BezierPatch_Triangle RPatch2TPatchSolver::solveFrom(const BezierPatch_Rectangle& rectanglePatch, trianglePart_t triangle)
{
    BezierPatch_Triangle trianglePatch(rectanglePatch.getNumberOfPoints() > 0 ? rectanglePatch.getSizeM()+rectanglePatch.getSizeN()-1 : 0);

    //here develop the left side equation


    //here develop the right side equation, and complete the left side with points and coefficients

    //finally set all trianglePatch points correctly by iterating on i, j and k

    //free what needs to be freed

    //simply return the patch
    return trianglePatch;
}
