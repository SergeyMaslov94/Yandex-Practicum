#include <future>
#include <mutex>
#include <numeric>
#include <queue>
#include <string>
#include <thread>
#include <vector>

#include "../test_framework.hpp"

using namespace std;

// ���������� ������ Synchronized<T>.
// ����� GetAccess ������ ���������� ���������, � ������� ���� ���� T& ref_to_value.
template <typename T>
class Synchronized {
public:
    explicit Synchronized(T initial = T()) : value_(move(initial)) {};

    struct Access {
        std::lock_guard<std::mutex> lock;
        T& ref_to_value;
    };

    Access GetAccess() {
        return  { std::lock_guard<std::mutex>(value_mutex_), value_ };
    };

private:
    T value_;
    std::mutex value_mutex_;
};


void TestConcurrentUpdate() {
    Synchronized<string> common_string;

    const size_t add_count = 50000;
    auto updater = [&common_string, add_count] {
        for (size_t i = 0; i < add_count; ++i) {
            auto access = common_string.GetAccess();
            access.ref_to_value += 'a';
        }
    };

    auto f1 = async(updater);
    auto f2 = async(updater);

    f1.get();
    f2.get();

    ASSERT_EQUAL(common_string.GetAccess().ref_to_value.size(), 2 * add_count);
}

vector<int> Consume(Synchronized<deque<int>>& common_queue) {
    vector<int> got;

    for (;;) {
        deque<int> q;

        {
            // �� ���������� ��������� ��� ��� ������� � ����������� ������, �����
            // ��������� ������ ����������� ������. �����-����������� �����������
            // �������, ���������� �� ���������� ����� ������� � ����
            // ��������� ���������� � ��������� �������. ����� ����� �� ������������
            // ������� � ������� �� ��������� ����������� ������, ��������
            // ������-������������� ����������� �������� � ������� ����� �������.
            //
            // ������ ����������� ������ ����������� ������ �� ��������������
            // ������������� ��������.
            auto access = common_queue.GetAccess();
            q = move(access.ref_to_value);
        }

        for (int item : q) {
            if (item > 0) {
                got.push_back(item);
            }
            else {
                return got;
            }
        }
    }
}

void TestProducerConsumer() {
    Synchronized<deque<int>> common_queue;

    auto consumer = async(Consume, ref(common_queue));

    const size_t item_count = 100000;
    for (size_t i = 1; i <= item_count; ++i) {
        common_queue.GetAccess().ref_to_value.push_back(i);
    }
    common_queue.GetAccess().ref_to_value.push_back(-1);

    vector<int> expected(item_count);
    iota(begin(expected), end(expected), 1);
    ASSERT_EQUAL(consumer.get(), expected);
}

int lesson_1() {

    TestRunner tr;
    RUN_TEST(tr, TestConcurrentUpdate);
    RUN_TEST(tr, TestProducerConsumer);

    return 0;
}