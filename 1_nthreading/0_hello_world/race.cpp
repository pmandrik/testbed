#include <iostream>
#include <thread>

#include <mutex>
#include <list>
#include <algorithm>

using namespace std;


// race
list<int> l1, l2;
mutex mutex_locker;

void add_to_l(int n, int start, list<int> & l, bool lock){
  if(lock) 
    lock_guard<mutex> guard(mutex_locker);
  for(int i = 0; i < n; ++i)
    l.push_back( start + i );
}

bool print(list<int> & l){
  lock_guard<mutex> guard(mutex_locker);
  for(int val : l){
    cout << val << " ";
  }
  cout << endl;
}

//deadlock
class Dlock {
  private:
    mutex m;
  public:

    int data;
    string name;
    friend void swapA(Dlock & a, Dlock & b, bool & done);
    friend void swapB(Dlock & a, Dlock & b, bool & done);
    friend void swapC(Dlock & a, Dlock & b, bool & done);
    friend void swapD(Dlock & a, Dlock & b, bool & done);

    // unick lock
    auto get_unic_lock(){
      std::unique_lock<std::mutex> lk ( m );
      return lk;
    }
};
    void swapA(Dlock & a, Dlock & b, bool & done){
      cout << std::this_thread:: get_id() << " start" << endl;
      cout << std::this_thread:: get_id() << " try to lock " << a.name << endl;
      lock_guard<mutex> g1( a.m );         cout << std::this_thread:: get_id() << " lock " << a.name << endl;
      this_thread::sleep_for(2000ms);
      cout << std::this_thread:: get_id() << " try to lock " << b.name << endl;
      lock_guard<mutex> g2( b.m );         cout << std::this_thread:: get_id() << " lock " << b.name << endl;
      swap( a.data, b.data );
      cout << std::this_thread:: get_id() << " result " << a.data << " " << b.data << endl;
      done = true;
    }

    void swapB(Dlock & a, Dlock & b, bool & done){
      cout << std::this_thread:: get_id() << " start" << endl;
      std::lock( a.m, b.m );
      cout << std::this_thread:: get_id() << " try to lock " << a.name << endl;
      lock_guard<mutex> g1( a.m, std::adopt_lock );         cout << std::this_thread:: get_id() << " lock " << a.name << endl;
      this_thread::sleep_for(2000ms);
      cout << std::this_thread:: get_id() << " try to lock " << b.name << endl;
      lock_guard<mutex> g2( b.m, std::adopt_lock );         cout << std::this_thread:: get_id() << " lock " << b.name << endl;
      swap( a.data, b.data );
      cout << std::this_thread:: get_id() << " result " << a.data << " " << b.data << endl;
      done = true;
    }

    void swapC(Dlock & a, Dlock & b, bool & done){
      std::scoped_lock guard(a.m, b.m);
      swap( a.data, b.data );
      done = true;
    }

    void swapD(Dlock & a, Dlock & b, bool & done){
      std::unique_lock<std::mutex> lock_a(a.m,std::defer_lock);
      std::unique_lock<std::mutex> lock_b(b.m,std::defer_lock);
      std::lock( lock_a, lock_b );
      swap( a.data, b.data );
      done = true;
    }

    void swapE(Dlock & a, Dlock & b, bool & done){
      // race conditions ...
      cout << std::this_thread:: get_id() << " start" << endl;

      cout << std::this_thread:: get_id() << " try to lock " << a.name << endl;
      auto lock_a = a.get_unic_lock();
      int a_data = a.data;
      lock_a.unlock();

      this_thread::sleep_for(1000ms);

      cout << std::this_thread:: get_id() << " try to lock " << b.name << endl;
      auto lock_b = b.get_unic_lock();
      int b_data = b.data;
      b.data = a_data;
      lock_b.unlock();

      this_thread::sleep_for(1000ms);

      cout << std::this_thread:: get_id() << " try to lock " << a.name << endl;
      lock_a.lock();
      a.data = b_data;
      lock_a.unlock();

      cout << std::this_thread:: get_id() << " result " << a.data << " " << b.data << endl;
      done = true;
    }

void master_swap( Dlock & a, Dlock & b, string swap_a ){
  thread t1, t2;  
  bool done1 = false;
  bool done2 = false;
  if( swap_a == "A" ){
    t1 = thread( swapA, std::ref(a), std::ref(b), std::ref(done1) );
    t2 = thread( swapA, std::ref(b), std::ref(a), std::ref(done2) );
  } else if( swap_a == "B" ){ 
    t1 = thread( swapB, std::ref(a), std::ref(b), std::ref(done1) );
    t2 = thread( swapB, std::ref(b), std::ref(a), std::ref(done2) );
  } else if( swap_a == "E" ){ 
    t1 = thread( swapE, std::ref(a), std::ref(b), std::ref(done1) );
    t2 = thread( swapE, std::ref(b), std::ref(a), std::ref(done2) );
  }
  this_thread::sleep_for(5000ms);
  if( not done1 ) { t1.detach(); cout << "t1 still running (deadlock), detach" << endl; } else { t1.join(); }
  if( not done2 ) { t2.detach(); cout << "t2 still running (deadlock), detach" << endl; } else { t2.join(); }
};

// thread safe singleton
class Single {
  public:
  int id;
  Single(int id_) : id(id_) {};
};

Single s1(-1);
std::once_flag s1_flag;

Single & get_s1(bool once){
  if( once )
    std::call_once( s1_flag, []{ s1.id = s1.id + 1; } );
  else s1.id = s1.id + 1;
  return s1;
}

Single & get_s2(int x){
  static Single s2(x);
  return s2;
}

// shared lock - when read-only threads lock data for write-operations, but allow read operations
#include <shared_mutex>
int shared_data = 0;
std::shared_mutex entry_mutex;
void print_shared_data(string N, bool lock){
  if(lock) std::shared_lock<std::shared_mutex> lk(entry_mutex);
  this_thread::sleep_for(1000ms);
  cout << N << " " << shared_data << endl;
}
void set_shared_data(string N, int val){
  this_thread::sleep_for(1000ms);
  std::lock_guard<std::shared_mutex> lk(entry_mutex);
  shared_data = val;
  cout << N << " " << shared_data << endl;
}

int main(){
  // shared lock
  cout << "shared lock" << endl;  
  for(int i = 0; i < 10; i++){
    thread t( print_shared_data, "A" + to_string(i), true );
    t.detach();
  }
  {
    thread t1( set_shared_data, "B", 1000 );
    t1.join();
    thread t2( print_shared_data, "A", false );
    t2.join();
  }
  this_thread::sleep_for(2000ms);
  cout << endl;

  // call once
  cout << "call once" << endl;  
  for(int i = 0; i < 10; i++){
    thread t( [ i ]{ Single & s = get_s1( true ) ; cout << i << " " << s.id << endl;; } );
    t.detach();
  }
  this_thread::sleep_for(2000ms);
  cout << endl;

  for(int i = 0; i < 10; i++){
    thread t( [ i ]{ Single & s = get_s2( 100 + i ) ; cout << i << " " << s.id << endl;; } );
    t.detach();
  }
  this_thread::sleep_for(2000ms);

  // no mutex lock
  cout << "mutex" << endl;  
  thread t1( add_to_l, 5, 10, std::ref( l1 ), false );
  thread t2( add_to_l, 5, 20, std::ref( l1 ), false );

  t1.join();
  t2.join();

  print( l1 ); // e.g. 20 21 22 10 24 12 13 14

  // mutex lock
  t1 = thread( add_to_l, 5, 10, std::ref( l2 ), true );
  t2 = thread( add_to_l, 5, 20, std::ref( l2 ), true );

  t1.join();
  t2.join();

  print( l2 ); // e.g. 20 21 22 10 24 12 13 14

  // DEADLOCK - witing two threads to eachother
  Dlock a, b, c, d;
  a.data = 1; b.data = 2; c.data = 1; d.data = 2;
  a.name = "A"; b.name = "B";
  c.name = "C"; d.name = "D";
  std::cout << "DL" << endl;
  master_swap( a, b, "A" );
  std::cout << "no-DL" << endl;
  master_swap( c, d, "B" );
  std::cout << "unic" << endl;
  master_swap( c, d, "E" );

  // race condition
  // 1. is to wrap your data structure with a protection mechanism to ensure that only the
  // thread performing a modification can see the intermediate states where the invariants
  // 2. lock-free programming: modify the design of your data structure and its invariants so
  // that modifications are done as a series of indivisible changes, each of which preserves
  // the invariants.
  // 3. software transactional memory - DB write modifications, then commit

  // following 1.
  // mutex (mutual exclusion) - before accessing a shared data structure, 
  // you lock the mutex associated with that data, and when you’ve finished accessing
  // the data structure, you unlock the mutex.

  // mutex offers exclusive, non-recursive ownership semantics:
  // - A calling thread owns a mutex from the time that it successfully calls either lock or try_lock until it calls unlock.
  // - When a thread owns a mutex, all other threads will block (for calls to lock) or receive a false return value (for try_lock) if they attempt to claim ownership of the mutex.
  // - A calling thread must not own the mutex prior to calling lock or try_lock. 
  
  // deadlock
  // 1. AVOID NESTED LOCKS (locks one after each other)
  // 2. AVOID CALLING USER-SUPPLIED CODE WHILE HOLDING A LOCK
  // 3. ACQUIRE LOCKS IN A FIXED ORDER
  // 4. USE A LOCK HIERARCHY

  return 0;
}
