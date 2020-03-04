#include <iostream>
#include <thread>
 
void thread_function() {
  for (int i = 0; i < 100; i++)
    std::cout << "thread run : way 1" << std::endl;
}
 
 
class DisplayThread {
 public:
  void my_display(){std::cout << "thread run way 2"<<std::endl}
  void my_display2(int a){std::cout << "thread run way 2 and paramter is :" << a <<std::endl;}
  };

 
int main() {
  std::thread threadObj1(thread_function);
  DisplayThread dt;
  std::thread threadObj2(DisplayThread::my_display, &dt);
  std::thread threadObj3(DisplayThread::my_display, &dt,1);
  auto myLamThread = [] {std::cout << thread run way 3"<<std::endl};
  std::thread threadObj4(myLamThread);
  
  threadObj1.join();
  threadObj2.join();
  threadObj3.join();
  threadObj4.join();
  std::cout << "Exit of Main function" << std::endl;
  return 0;
}