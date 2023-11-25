#ifndef FHOBOTS_CONTROL_HPP
#define FHOBOTS_CONTROL_HPP

#include "../model/Vector2D.hpp"

class Control {

    public:

        /// Contrutores
        Control(const double kp = 0.0, const double kd = 0.0, const double basePwmValue = 80.0);
        ~Control();

        /// Define o Kp e o Kd
        void setPD(const double kp, const double kd);

        ///Define o valor do PwmMaximo
        void setMaxPwm(const int maxPwm);

        /// Define o Valor Base do Pwm
        void setBasePwmValue(const double pwm);

        /// Define o LastErros
        void setLastError(const double lerror);

        /// Retorna o Valor Base do Pwm
        double getBasePwmValue();

        /// Calcula o Pwm das Rodas
        void calculatePwm(Vector2D& robotToDestiny, Vector2D orientationRobot);

        /// Calcula o Pwm das Rodas
        void calculatePwm(Vector2D& robotToDestiny, const double& angleRobot);

        /// Define Pwm da Roda Esquerda
        void setPwmLeftWheel(const int pwm);

        /// Define Pwm da Roda Direita
        void setPwmRightWheel(const int pwm);

        /// Retorna o Pwm da Roda Esquerda
        int getPwmLeftWheel();

        /// Retorna o Pwm da Roda Direita
        int getPwmRightWheel();

    private:

        /// Calcula o PD
        double calculatePD(Vector2D& robotToDestiny, Vector2D& orientarionRobot);

        /// Calcula o PD
        double calculatePD(Vector2D& robotToDestiny, const double& angleRobot);

        double _kp;
        double _kd;
        double _lastError;

        double _basePwmValue;
        double _maxPwmValue;

        int _pwmLeftWheel;
        int _pwmRightWheel;
};


#endif //FHOBOTS_CONTROL_HPP
