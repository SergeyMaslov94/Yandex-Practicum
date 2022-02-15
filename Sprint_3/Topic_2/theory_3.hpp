// Арифметические операции с разными типами аргументов
//----------------------------------------------------

// Здесь мы объявляем структуру, а не класс, так как поля структуры
// могут принимать произвольные значения
struct Vector2D {
    // Такая запись сообщает компилятору, чтобы он сгенерировал конструктор по умолчанию,
    // в котором поля x и y проинициализировал значениями, заданными при их объявлении
    Vector2D() = default;

    Vector2D(double x0, double y0)
            : x(x0), y(y0) {
    }

    // Задаем значения по умолчанию для полей структуры
    double x = 0.0;
    double y = 0.0;
};

// Операция умножения вектора на скаляр
Vector2D operator*(Vector2D vector, double scalar) {
    return {vector.x * scalar, vector.y * scalar};
}

// Операция умножения скаляра на вектор
Vector2D operator*(double scalar, Vector2D vector) {
    // Благодаря коммутативности операции умножения, мы можем выразить
    // умножение скаляра на вектор через операцию умножения вектора на скаляр
    return vector * scalar;
}

int top_2_theory_3()
{
    double scalar = 3;
    Vector2D vector(4,5);

    auto mul1 = scalar * vector;
    auto mul2 = vector * scalar;

    return 0;
}