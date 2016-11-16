#ifndef PRODUCTPOLYNOM3VAR_H
#define PRODUCTPOLYNOM3VAR_H

#include <vector>
#include <cstddef>
#include <boost/math/special_functions/binomial.hpp>

/**
 * @brief represents a^j1*b^j2*c^j3 in any equation, with only the essential tools necessary for the problematic answered by this program.
 */
class ProductPolynom3Var
{
public:
    typedef enum {PP3V_A=0, PP3V_B=1, PP3V_C=2} var_t;

    ProductPolynom3Var();
    ProductPolynom3Var(double coef, unsigned int dA, unsigned int dB, unsigned int dC);

    double coef() const;
    unsigned int degree(var_t var) const;
    unsigned int operator[](var_t var) const; //same as degree(var)

    void setDegree(var_t var, unsigned int degree);

    ProductPolynom3Var operator*(const ProductPolynom3Var& other) const;

private:

    double m_coef;
    unsigned int m_tab[3];

};

/**
 * @brief represents a full sum of a^j1*b^j2*c^j3 in any equation, with only the essential tools necessary for the problematic answered by this program.
 * It provides for example an implementation of remarkable identities.
 */

class Sum_ProductPolynom3Var
{
public:

    Sum_ProductPolynom3Var(size_t polynomSize=0);

    typedef std::vector<ProductPolynom3Var>::iterator iterator;
    typedef std::vector<ProductPolynom3Var>::const_iterator const_iterator;

    iterator begin();
    const_iterator begin() const;

    iterator end();
    const_iterator end() const;

    void append(const ProductPolynom3Var& product);

    /**
     * @brief product of a polynomial sum with another polynomial sum
     */
    Sum_ProductPolynom3Var operator*(const Sum_ProductPolynom3Var& other) const;

    /**
     * @brief product of a polynomial sum with a single a^j1*b^j2*c^j3 expression (distribution of other to this)
     */
    Sum_ProductPolynom3Var operator*(const ProductPolynom3Var& other) const;
    Sum_ProductPolynom3Var& operator*=(const ProductPolynom3Var& other);

    /**
     * @brief computes a vector from a remarkable identity (a+b)^n
     * @param first represents a
     * @param second represents b
     * @param n represents n (power of the sum)
     */
    static Sum_ProductPolynom3Var remarkable_identity_plus(ProductPolynom3Var::var_t first, ProductPolynom3Var::var_t second, unsigned int n);

    size_t size() const;

private:

    std::vector<ProductPolynom3Var> m_expression;
};

#endif // PRODUCTPOLYNOM3VAR_H
