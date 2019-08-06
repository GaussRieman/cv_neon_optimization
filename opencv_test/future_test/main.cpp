//
//  main.cpp
//  future_test
//
//  Created by Frank on 2018/7/30.
//  Copyright © 2018年 Frank. All rights reserved.
//

#include <iostream>
#include <future>
#include <queue>
#include <vector>
using namespace std;

int main(int argc, const char * argv[]) {
    // insert code here...
    queue<shared_future<void>> qq;
    
    for (int i = 0; i != 3; i++) {
        shared_future<void> sf = async([]{
            printf("here\n");
        });
        qq.push(sf);
    }
    
    
    while (!qq.empty()) {
        qq.front().wait();
        qq.pop();
    }
    
    std::cout << "Hello, World!\n";
    return 0;
}
