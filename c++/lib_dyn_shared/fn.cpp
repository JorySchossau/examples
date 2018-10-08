#include <iostream>
using namespace std;

void (*update)() = NULL;
extern "C" void register_update( void (*main_update)())
{
  cout << "in fn::register_update" << endl;
  update = reinterpret_cast<void(*)()>(main_update);
  cout << "leaving fn::register_update" << endl;
}

extern "C" void fn_main() {
  cout << "in fn::fn_main" << endl;
  cout << "calling main::update" << endl;
  update();
  update();
  update();
  cout << "leaving fn::fn_main" << endl;
}
