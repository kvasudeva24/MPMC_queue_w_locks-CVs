#include <mutex>
#include <condition_variable>
#include <iostream>


template <typename T>
class MPMC {

private:
    T* data = nullptr;
    size_t capacity = 100;
    size_t size = 0;
    size_t fill_ptr = 0;
    size_t take_ptr = 0;
    std::mutex mtx;
    std::condition_variable fill, empty;

public:
    MPMC() {
        data = new T[capacity];
    }
    MPMC(size_t c) : capacity(c){
        data = new T[capacity];
    }

    ~MPMC() { delete data; data = nullptr; }

    MPMC(const MPMC<T>& rhs) = delete;
    MPMC<T>& operator=(const MPMC<T>& rhs) = delete;

    //implement move operators?


    void put(T value) {
        for ( size_t i{}; i < capacity; i++ ) {
            std::unique_lock<std::mutex> lock(mtx);
            while ( size == capacity ) {
                empty.wait(lock);
            }
            data[fill_ptr] = value;
            size++;
            fill_ptr = (fill_ptr + 1) % capacity;
            fill.notify_one();
            lock.unlock();
        }
    }

    void get() {
        for ( size_t i{}; i < capacity; i++ ) {
            std::unique_lock<std::mutex> lock(mtx);
            while ( size == 0 ) {
                fill.wait(lock);
            }
            T buffer_val = data[take_ptr];
            size--;
            take_ptr = ( take_ptr + 1 ) % capacity;
            empty.notify_one();
            std::cout << "Value taken off of buffer: " << buffer_val << std::endl; //assumes that T has a << overload
            lock.unlock();
        }
    }
};


template <typename T> 
void producer(MPMC<T>& buffer, T value) {
    buffer.put(value);
}

template<typename T>
void consumer(MPMC<T>& buffer){
    buffer.get();
}