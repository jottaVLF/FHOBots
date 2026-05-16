#ifndef FHOBOTS_VECTOR2D_HPP
#define FHOBOTS_VECTOR2D_HPP

#include <ostream>

class Vector2D {

    public:
        /// -----------------------------------
        /// Construtores
        /// -------------------------------------
        Vector2D();
        Vector2D(double _x, double _y);
        Vector2D(double angle);

        /// ----------------------------------------------------------------
        /// Sobrecarga de operadores, para somar, subtrair, multiplicar, etc
        /// ----------------------------------------------------------------
        Vector2D operator+(Vector2D& other);
        Vector2D operator-(Vector2D& other);
        double operator*(Vector2D& other);
        Vector2D& operator+=(Vector2D& other);
        Vector2D& operator-=(Vector2D& other);

        Vector2D operator*(double scalar);
        Vector2D operator*=(double scalar);
        Vector2D operator*(int scalar);
        Vector2D operator*=(int scalar);

        Vector2D operator/(double scalar);
        Vector2D operator/=(double scalar);
        Vector2D operator/(int scalar);
        Vector2D operator/=(int scalar);

        friend  std::ostream& operator<<(std::ostream& os, Vector2D& v);
        /// ------------------------------------------------------------------

        /// Sobregarga Que Calcula a Diferença de Angulo entre dois Vetores, a ordem
        /// Nesse Caso Importa, Retorna em Rad
        double operator||(Vector2D& other);

        /// Sobrecarga Que Calcula o Produto Vetorial
        double operator&&(Vector2D& other);

        /// Calcula o módulo ou a hipotenusa, como queira chamar
        double magnitude();

        /// Retorna o angulo do vetor, em Rad
        double angle();

        /// Normaliza o vetor, o que transforma o módulo do vetor igual a 1
        void normalize();

        ///Retorna o angulo entre 2 vetores
        double angleBetween(Vector2D other);

        /// Define o X e o Y do vetor
        void set(double _x, double _y);

        double x; /// X
        double y; /// Y
};


#endif //FHOBOTS_VECTOR2D_HPP
