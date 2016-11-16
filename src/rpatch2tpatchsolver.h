#ifndef RPATCH2TPATCHSOLVER_H
#define RPATCH2TPATCHSOLVER_H

#include "bezierpatch_rectangle.h"

class RPatch2TPatchSolver
{

public:
    typedef enum {Solver_UpperPart=0, Solver_LowerPart=1} part_t;

    RPatch2TPatchSolver();
    RPatch2TPatchSolver(size_t nOrder, size_t mOrder);

private:
    std::vector<double> m_tCoefMatrix;
};

#endif // RPATCH2TPATCHSOLVER_H
