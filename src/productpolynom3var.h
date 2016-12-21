#ifndef PRODUCTPOLYNOM3VAR_H
#define PRODUCTPOLYNOM3VAR_H

#include <vector>
#include <cstddef>
#include <boost/math/special_functions/binomial.hpp>

/**
 * @brief represents a^i1*b^i2*c^i3 in any equation, with only the essential tools necessary for the problematic answered by this program.
 */
class ProductPolynom3Var
{
public:
    typedef enum {PP3V_A=0, PP3V_B=1, PP3V_C=2} var_t;

    ProductPolynom3Var();
    ProductPolynom3Var(const ProductPolynom3Var& other);
    ProductPolynom3Var(double coef, unsigned int dA, unsigned int dB, unsigned int dC);

    double coef() const;
    unsigned int degree(var_t var) const;
    unsigned int operator[](var_t var) const; //same as degree(var)

    void setDegree(var_t var, unsigned int degree);

    /**
     * @brief product between two polynomials of form a^i1*b^i2*c^i3
     * 2 ab^2 * 3 bc^2 = 6 ab^3c^2
     */
    ProductPolynom3Var operator*(const ProductPolynom3Var& other) const;
    ProductPolynom3Var& operator*=(const ProductPolynom3Var& other);

    /**
     * @brief product between a polynomial of form a^i1*b^i2*c^i3 and a double
     * ab^2 * 3 = 3 ab^2
     */
    ProductPolynom3Var operator*(double value) const;
    ProductPolynom3Var& operator*=(double value);

    //same than above but for division
    ProductPolynom3Var operator/(double value) const;
    ProductPolynom3Var& operator/=(double value);


    ProductPolynom3Var& operator=(const ProductPolynom3Var& other);

private:

    double m_coef;
    unsigned int m_tab[3];

};

/**
 * @brief represents a full sum of a^i1*b^i2*c^i3 in any equation, with only the essential tools necessary for the problematic answered by this program.
 * It provides for example an implementation of remarkable identities.
 */

class Sum_ProductPolynom3Var
{
public:

    Sum_ProductPolynom3Var(size_t reserveSize=0);
    Sum_ProductPolynom3Var(const Sum_ProductPolynom3Var& other);

    typedef std::vector<ProductPolynom3Var>::iterator iterator;
    typedef std::vector<ProductPolynom3Var>::const_iterator const_iterator;

    inline iterator begin() {return m_expression.begin();}
    inline const_iterator begin() const {return m_expression.begin();}

    inline iterator end() {return m_expression.end();}
    inline const_iterator end() const {return m_expression.end();}

    /**
     * @brief reserve reserveSize in the current sum array
     */
    void reserve(size_t reserveSize);

    size_t size() const;

    /**
     * @brief add a polynomial of form a^i1*b^i2*c^i3 to the array representing their sum
     * Note: polynoms DO NOT merge automatically when they are identical; this might become a feature if it turns out to be useful
     * @param product represents an a^i1*b^i2*c^i3 polynomial
     */
    inline void append(const ProductPolynom3Var& product);

    /**
     * @brief shortcut for append, but returns a reference to this
     * @param product represents an a^i1*b^i2*c^i3 polynomial
     */
    Sum_ProductPolynom3Var& operator+=(const ProductPolynom3Var& product);

    /**
     * @brief product of a polynomial sum with another polynomial sum
     * @param other represents the other polynomial sum
     */
    Sum_ProductPolynom3Var operator*(const Sum_ProductPolynom3Var& other) const;
    Sum_ProductPolynom3Var& operator*=(const Sum_ProductPolynom3Var& other);

    /**
     * @brief product of a polynomial sum with a single a^i1*b^i2*c^i3 expression (distribution of other to this)
     * @param product represents an a^i1*b^i2*c^i3 polynomial
     */
    Sum_ProductPolynom3Var operator*(const ProductPolynom3Var& product) const;
    Sum_ProductPolynom3Var& operator*=(const ProductPolynom3Var& product);

    /**
     * @brief deep copy of the content of other to this
     * @param other represents the other polynomial sum
     */
    Sum_ProductPolynom3Var& operator=(const Sum_ProductPolynom3Var& other);

    /**
     * @brief computes a vector from a remarkable identity (a+b)^n
     * @param first represents a
     * @param second represents b
     * @param n represents n (power of the sum)
     */
    static Sum_ProductPolynom3Var remarkable_identity_plus(ProductPolynom3Var::var_t first, ProductPolynom3Var::var_t second, unsigned int n);

private:

    std::vector<ProductPolynom3Var> m_expression;
};

#endif // PRODUCTPOLYNOM3VAR_H
