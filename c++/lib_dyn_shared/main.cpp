#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>

/*
 * Compile as follows:
 * main: g++ -o main main.cpp -ldl
 * libfn.so: g++ -shared -o libfn.so -fpic fn.cpp
 * run: LD_LIBRARY_PATH=. ./main
 */

/* This is a demonstration of dynamic loading of a library,
 * and passing runtime control to that library by giving the
 * library access to known/expected functions that exist in
 * the main object code, external to the library.
 * Files: main.cpp, fn.cpp
 * fn.cpp represents user code that would like to drive a
 * simulation, of which all the workings for are in main.cpp.
 * main.cpp represents the simulation code with a time counter
 * that steps forward every time update is called.
 * The objective is to only ever recompile user code (fn.cpp)
 * but be able to drive the simulation update fn from fn.cpp.
 * To do this, main.exe needs to expect that fn.cpp is
 * a shared library, libfn.so and that certain callbacks
 * may be passed to it and stored in libfn.so at runtime.
 * Control passes as follows:
 * - main loads the dynamic library using dlopen (os-specific)
 * - main finds 2 methods in the dynlib, and stores their fn ptrs
 * - the first is a fn that stores a fn pointer in libfn.so
 *   so, it stores a pointer to the main simulation update().
 * - the second is a fn that is going to take over for main,
 *   but it exists in libfn.so.
 * - main then calls the first method, passing it a pointer to update()
 * - main then invokes the replacement main in libfn.so
 * - the replacement main in libfn.so uses the saved fn pointer
 *   previously passed by main, and calls the update() a few times.
 */

using namespace std;

static int timet(0);

void update() {
  cout << "in main::update [" << timet++ << "]" << endl;
  cout << "leaving main::update" << endl;
}

void (*register_update)(void(*)());
void (*external_main)();

int main() {
  void *handle = dlopen("libfn.so",RTLD_NOW);
  cout << "in main::main" << endl;
  register_update = reinterpret_cast<void(*)(void(*)())>(dlsym(handle, "register_update"));
  external_main = reinterpret_cast<void(*)()>(dlsym(handle, "fn_main"));
  cout << "calling fn::register_update" << endl;
  register_update(update);
  cout << "calling fn::fn_main" << endl;
  external_main();
  cout << "leaving main::main" << endl;
  return (0);
}
