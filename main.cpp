#include "MPMC.hpp"
#include <thread>



int main() {
    std::thread p[10]{};
    std::thread c[10]{};

    MPMC<int> buffer{10};


    for ( int i = 0; i < 10; i++ ){
        p[i] = std::thread(producer<int>, std::ref(buffer), i);
        c[i] = std::thread(consumer<int>, std::ref(buffer));
    }

    for ( int i = 0; i < 10; i++ ){
        p[i].join();
        c[i].join();
    }



    return 0;
}