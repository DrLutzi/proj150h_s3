#include "rpatch2tpatchsolver.h"

RPatch2TPatchSolver::RPatch2TPatchSolver()
{
}

RPatch2TPatchSolver::RPatch2TPatchSolver(size_t nOrder, size_t mOrder) : m_tCoefMatrix((nOrder+mOrder+1)*(nOrder+mOrder+1))
{
}

