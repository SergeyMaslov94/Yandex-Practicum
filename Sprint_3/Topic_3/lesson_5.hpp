// Раскрутка стека. Исключения в конструкторе и деструкторе.

/*
Задание
Класс Rational уже умеет сообщать об ошибках деления на ноль, но всё ещё разрешает создавать дроби с нулевым знаменателем.
Доработайте конструктор класса Rational так, чтобы при попытке сконструировать дробь с нулевым знаменателем,
выбрасывалось исключение типа domain_error.
 */

#include <iostream>
#include <numeric>

using namespace std;

class Rational {
public:
    Rational() = default;

    Rational(int value)
            : numerator_(value)
            , denominator_(1) {
    }

    Rational(int numerator, int denominator)
            : numerator_(numerator)
            , denominator_(denominator)
    {
        if(denominator_ == 0)
            throw domain_error("denominator is zero value");

        Normalize();
    }

    int Numerator() const {
        return numerator_;
    }

    int Denominator() const {
        return denominator_;
    }

    Rational& operator+=(Rational right) {
        numerator_ = numerator_ * right.denominator_ + right.numerator_ * denominator_;
        denominator_ *= right.denominator_;
        Normalize();
        return *this;
    }

    Rational& operator-=(Rational right) {
        numerator_ = numerator_ * right.denominator_ - right.numerator_ * denominator_;
        denominator_ *= right.denominator_;
        Normalize();
        return *this;
    }

    Rational& operator*=(Rational right) {
        numerator_ *= right.numerator_;
        denominator_ *= right.denominator_;
        Normalize();
        return *this;
    }

    Rational& operator/=(Rational right) {
        if(right.numerator_ == 0)
            throw invalid_argument("value is zero"s);

        numerator_ *= right.denominator_;
        denominator_ *= right.numerator_;
        Normalize();
        return *this;
    }

private:
    void Normalize() {
        if (denominator_ < 0) {
            numerator_ = -numerator_;
            denominator_ = -denominator_;
        }
        int n = gcd(numerator_, denominator_);
        numerator_ /= n;
        denominator_ /= n;
    }

    int numerator_ = 0;
    int denominator_ = 1;
};

ostream& operator<<(ostream& output, Rational rational) {
    return output << rational.Numerator() << '/' << rational.Denominator();
}

istream& operator>>(istream& input, Rational& rational) {
    int numerator;
    int denominator;
    char slash;
    if ((input >> numerator) && (input >> slash) && (slash == '/') && (input >> denominator)) {
        rational = Rational{numerator, denominator};
    }
    return input;
}

// Unary plus and minus

Rational operator+(Rational value) {
    return value;
}

Rational operator-(Rational value) {
    return {-value.Numerator(), value.Denominator()};
}

// Binary arithmetic operations

Rational operator+(Rational left, Rational right) {
    return left += right;
}

Rational operator-(Rational left, Rational right) {
    return left -= right;
}

Rational operator*(Rational left, Rational right) {
    return left *= right;
}

Rational operator/(Rational left, Rational right) {
    return left /= right;
}

// Comparison operators

bool operator==(Rational left, Rational right) {
    return left.Numerator() == right.Numerator() &&
           left.Denominator() == right.Denominator();
}

bool operator!=(Rational left, Rational right) {
    return !(left == right);
}

bool operator<(Rational left, Rational right) {
    return left.Numerator() * right.Denominator() <
           left.Denominator() * right.Numerator();
}

bool operator>(Rational left, Rational right) {
    return left.Numerator() * right.Denominator() >
           left.Denominator() * right.Numerator();
}

bool operator>=(Rational left, Rational right) {
    return !(left < right);
}

bool operator<=(Rational left, Rational right) {
    return !(left > right);
}

int top_3_lesson_5()
{
    try {
        // При попытке сконструировать дробь с нулевым знаменателем
        // должно выброситься исключение domain_error
        const Rational invalid_value{1, 0};
        // Следующая строка не должна выполниться
        cout << invalid_value << endl;
    } catch (const domain_error& e) {
        cout << "Ошибка: "s << e.what() << endl;
    }

    return 0;
}