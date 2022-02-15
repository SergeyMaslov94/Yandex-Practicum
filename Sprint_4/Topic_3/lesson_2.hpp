#include <stack>
#include <iostream>
#include <vector>

using namespace std;

template <typename It>
void PrintRange(It range_begin, It range_end) {
    for (auto it = range_begin; it != range_end; ++it) {
        cout << *it << " "s;
    }
    cout << endl;
}

template <typename Type>
class Queue {
public:
    void Push(const Type& element) {
        // просто вставим элемент в первый стек
        stack1_.push(element);
    }
    void Pop() {
        // переложим все элементы из первого стека во второй,
        // так что первый элемент очереди окажется на вершине второго стека
        MoveElements();
        // вытащим верхний элемент второго стека
        stack2_.pop();
    }
    Type& Front() {
        // логика аналогична методу Pop
        MoveElements();
        return stack2_.top();
    }
    uint64_t Size() const {
        return stack1_.size() + stack2_.size();
    }
    bool IsEmpty() const {
        return (stack1_.empty() && stack2_.empty());
    }

private:
    stack<Type> stack1_;
    stack<Type> stack2_;

    void MoveElements() {
        // Перекладывать во второй стек из первого
        // необходимо только тогда, когда во втором стеке
        // элементов не осталось. Иначе порядок очереди будет нарушен.
        if (stack2_.empty()) {
            while (!stack1_.empty()) {
                stack2_.push(stack1_.top());
                stack1_.pop();
            }
        }
    }
};

int top_3_lesson_2()
{
    Queue<int> queue;
    vector<int> values{5, 2, 4, 3, 1};

    PrintRange(values.begin(), values.end());

    cout << "Заполняем очередь"s << endl;

    // заполняем очередь и выводим элемент в начале очереди
    for (int i = 0; i < 5; ++i) {
        queue.Push(values[i]);
        cout << "Вставленный элемент "s << values[i] << endl;
        cout << "Первый элемент очереди "s << queue.Front() << endl;
    }

    cout << "Вынимаем элементы из очереди"s << endl;

    // выводим элемент в начале очереди и вытаскиваем элементы по одному
    while (!queue.IsEmpty()) {
        // сначала будем проверять начальный элемент, а потом вытаскивать,
        // так как операция Front на пустой очереди не определена
        cout << "Будем вынимать элемент "s << queue.Front() << endl;
        queue.Pop();
    }
    return 0;
}