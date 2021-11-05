/*************************************************************************
    > File Name: chrono_test4.cpp
    > Author: Feng
    > Created Time: 2021-11-05 18:04
    > Content: 
 ************************************************************************/

 #include <iostream>
 #include <chrono>

 int main(int argc, char* argv[]) {
     using clock = std::chrono::steady_clock;
     std::chrono::steady_clock::time_point now = std::chrono::time_point<clock>();
     if (now == std::chrono::time_point<clock>()) {
         std::cout << "test\n";
         std::cout << now.time_since_epoch().count() << std::endl;

         now = clock::now();
         if (now == std::chrono::time_point<clock>()) {
             std::cout << "xxxx\n";
         } else {
             std::cout << now.time_since_epoch().count() << std::endl;
         }
     }
     return 0;
 }
