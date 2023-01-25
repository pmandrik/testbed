#include <iostream>
#include <thread>

using namespace std;

void hello(string n = "N"){
  cout << "Hello # " << std::this_thread:: get_id() << "," << n << endl;
} 

class background_task {
  public:
    void operator()() const {
      hello("B");
    }
};

// Resource Acquisition Is Initialization (RAII) idiom
class thread_guard {
  std::thread & t;
  public:
    explicit thread_guard(std::thread & t_): t(t_){}
    ~thread_guard(){
      if(not t.joinable()) return;
      cout << "guard call" << endl;
      t.join();
    }
    thread_guard(thread_guard const&)=delete;
    thread_guard& operator=(thread_guard const&)=delete;
};

class scoped_thread {
  std::thread t;
  public:
    explicit scoped_thread(std::thread & t_): t( std::move(t_) ){
      if(!t.joinable()) throw std::logic_error("No thread");
    }
    ~scoped_thread(){
      cout << "scoped guard call" << endl;
      t.join();
    }
    scoped_thread(scoped_thread const&)=delete;
    scoped_thread& operator=(scoped_thread const&)=delete;
};

void incrementer(int th, char & c){
  cout << "Incrementer(" << th << "," << c++ << ")" << endl;
} 

class X_incrementer {
  public:
    void do_lengthy_work(int th, char & c){ incrementer(th, c); };
};

int main(){
  std::thread t0(hello, "A"); // "A" is copied!
  cout << "t0 id " << t0.get_id() << endl;

  std::thread t1{ background_task() };

  std::thread t2([]{ hello("C"); } );
  thread_guard t2g( t2 );

  std::thread t3([]{ hello("C"); } );
  scoped_thread t3g( t3 );

  t0.join();
  t1.detach();
  // t2.join();

  char ch = 'A';
  std::thread t10(incrementer, 10, std::ref(ch) );
  std::thread t11(incrementer, 11, std::ref(ch) );
  X_incrementer my_x;
  std::thread t12(&X_incrementer::do_lengthy_work, &my_x, 12, std::ref(ch));  

  std::thread t13=std::move(t12); // copy not possible, only move like unic ptr

  t10.join();
  t11.join();
  // t12.join();
  t13.join();

  int n_threads = std::thread::hardware_concurrency();
  cout << "std::thread::hardware_concurrency = " << n_threads << endl;

/*
Once you’ve started your thread, you need to explicitly decide whether to wait for it
to finish (by joining with it—see section 2.1.2) or leave it to run on its own (by
detaching it—see section 2.1.3). If you don’t decide before the std::thread object
is destroyed, then your program is terminated (the std::thread destructor calls
std::terminate() ). It’s therefore imperative that you ensure that the thread is cor-
rectly joined or detached, even in the presence of exceptions.

Call to join() is liable to
be skipped if an exception is thrown after the thread has been started but before the
call to join().
e.g.
  std::thread t(my_func);
  try {
    do_something_in_current_thread();
  } catch(...) {
    t.join();
    throw;
  }
  t.join();

Detached threads are often called daemon threads.
Moving from temporaries is automatic and implicit.

*/

  

  return 0;
}
