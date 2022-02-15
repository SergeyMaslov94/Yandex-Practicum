// Урок 7: Стандартные алгоритмы — рекурсия
#include <iostream>
#include <stdexcept>
#include <vector>

uint64_t Factorial(int num) {
    int factorial = 1;
    cout << "Текущее число = "s << num << endl;
    if (num != 0) {
        factorial = Factorial(num - 1) * num;
    }
    cout << "Значение факториала "s << factorial << " для числа "s << num << endl;
    return factorial;
}

int Fibonacci(int num) {
    if(num <= 1)
        return num;
    uint64_t value = Fibonacci(num-1) + Fibonacci((num-2));
    return value;
}

bool IsPowOfTwo(int num) {
    if(num < 1)
        return false;
    if(num == 1)
        return true;
    if(num%2)
        return false;

    return IsPowOfTwo(num/2);
}
//-----------------------------------------------------------------------
class Tower {
public:
    // конструктор и метод SetDisks нужны, чтобы правильно создать башни
    Tower(int disks_num) {
        FillTower(disks_num);
    }

    int GetDisksNum() const {
        return disks_.size();
    }

    void SetDisks(int disks_num) {
        FillTower(disks_num);
    }

    // добавляем диск на верх собственной башни
    // обратите внимание на исключение, которое выбрасывается этим методом
    void AddToTop(int disk) {
        int top_disk_num = disks_.size() - 1;
        if (0 != disks_.size() && disk >= disks_[top_disk_num]) {
            throw invalid_argument("Невозможно поместить большой диск на маленький");
        } else {
            // допишите этот метод и используйте его в вашем решении
            disks_.push_back(disk);
        }
    }

    void Move(Tower& source, Tower& destination) {
        auto disk = source.disks_.back();
        destination.AddToTop(disk);
        source.disks_.pop_back();
    }

    // вы можете дописывать необходимые для вашего решения методы
    // disks_num - количество перемещаемых дисков
    // destination - конечная башня для перемещения
    // buffer - башня, которую нужно использовать в качестве буфера для дисков
    void MoveDisks(int disks_num, Tower& destination, Tower& buffer) {
            if(disks_num == 0)
                return;

            (*this).MoveDisks(disks_num - 1, buffer, destination);
            Move(*this, destination);
            buffer.MoveDisks(disks_num - 1, destination, (*this));
    }

private:
    vector<int> disks_;

    // используем приватный метод FillTower, чтобы избежать дубликации кода
    void FillTower(int disks_num) {
        for (int i = disks_num; i > 0; i--) {
            disks_.push_back(i);
        }
    }
};

void SolveHanoi(vector<Tower>& towers) {
    int disks_num = towers[0].GetDisksNum();
    // запускаем рекурсию
    // просим переложить все диски на последнюю башню
    // с использованием средней башни как буфера
    towers[0].MoveDisks(disks_num, towers[2], towers[1]); //TODO::
    int u = 0;
}

int top_1_lesson_7() {
    //----------------------------------
    cout << IsPowOfTwo(20) << endl;
    cout << Fibonacci(9) << endl;
    cout << Factorial(4) << endl;
    //----------------------------------
    int towers_num = 3;
    int disks_num = 3;
    vector<Tower> towers;
    // добавим в вектор три пустые башни
    for (int i = 0; i < towers_num; ++i) {
        towers.push_back(0);
    }
    // добавим на первую башню три кольца
    towers[0].SetDisks(disks_num);

    SolveHanoi(towers);
    return 0;
}