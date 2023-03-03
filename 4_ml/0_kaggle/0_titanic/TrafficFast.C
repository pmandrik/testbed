

class Connect {
  public:
  bool used, is_tuned;
  int id, period, optic_iters;
  vector< int > light_indexes;
  map< int, int > route_n_indexes;

  Connect(){
    wasted_time = 0;
    n_cars      = 0;
    used        = false;
    is_tuned = false;
    btime = 0;
    
    optic_iters = 0;
  }

  map< int, int > route_mask;
  void AddRoute(int a){
    if( route_mask.find( a ) != route_mask.end() ) return;
    route_mask[a] = 1;

    light_indexes.push_back( -1 );
    period = light_indexes.size();
  }

  int AddPriority(int id, int size, int optic){
    auto it =  prev_priority.find( optic );
    if( it  != prev_priority.end() ){
      if( it->second.second > size ){
        return AddPriority(id, size, (optic+1)%period);
      } else
        AddPriority( it->second.first, it->second.second, (optic+1)%period );
    } 

    prev_priority[ optic ] = make_pair( id, size );

    // auto ita = prev_prev_optimum.find( id );
    // if( ita != prev_prev_optimum.end() ) optic = (ita->second * optic_iters + optic) / (optic_iters + 1);
    prev_optimum[ id ] = optic;
    return optic;
  }

  map<int, int> prev_prev_optimum;
  void Reset(bool tune){
    busy_map = map<int, bool>();

    wasted_time = 0;
    n_cars      = 0;
    used        = false;
    btime = 0;

    route_n_indexes = map<int, int>();
    light_indexes = vector< int >( light_indexes.size(), -1 );

    if( not is_tuned ){
      // prev_prev_optimum = prev_optimum;
      prev_optimum  = map<int, int>();
      prev_priority = map<int, pair<int,int> >();
    }
    if( tune ){
      optic_iters++;
      is_tuned = true;
      // prev_prev_optimum = prev_optimum;
      prev_optimum  = map<int, int>();
      prev_priority = map<int, pair<int,int> >();
      for(auto it = love_time.begin(); it != love_time.end(); ++it){
        vector<int> & vals = it->second;
        int optic = vals[0];  
        sort( vals.begin(), vals.end() );

        // cout << endl << it->first << " " << optic << "<- ";
        // for(int i = 0; i < vals.size(); i++){
        //   cout << vals[i] << " ";
        // }

        optic = vals[0];  
        int delta = 999999;
        for(int i = 0; i < vals.size(); i++){
          int ndelta = 0;

          for(int j = 0; j < vals.size(); j++){
            if( vals[j] <= vals[i] ) ndelta += vals[i] - vals[j];
            else ndelta += period - vals[j] + vals[i];
          }

          if( ndelta >= delta ) continue;
          optic = vals[i];
          delta = ndelta;
        }
        optic = AddPriority(it->first, vals.size(), optic);
        // cout << " ->" << optic << " in " << period;
      }
    }

    love_time = map<int, vector<int> >();
  }

  map<int, int> prev_optimum;
  map<int, pair<int,int> > prev_priority;

  map<int, vector<int> > love_time;
  void AddLight( int time, int id ){
    if( prev_optimum.size() != 0){
      // cout << time << " " << prev_optimum.find( id )->second << endl;
      time = prev_optimum.find( id )->second;
    }

    for( int i = time; i < period; i++){
      if( light_indexes[i] != -1 ) continue;
      light_indexes[i] = id;
      route_n_indexes[id] = i;
      return;
    }
    for( int i = 0; i < period; i++){
      if( light_indexes[i] != -1 ) continue;
      light_indexes[i] = id;
      route_n_indexes[id] = i;
      return;
    }
    return;
  }

  int GetTime( int stime, int r_id, int c_id ){
    // if( id == 1000 ) cout << stime << " " << stime % period << " " <<  r_id << " " << c_id << endl;

    used = true;
    auto it = route_n_indexes.find( r_id );

    if( it == route_n_indexes.end() ) {
      AddLight( stime % period, r_id );
      it = route_n_indexes.find( r_id );
    }
      
    love_time[ r_id ].push_back( stime % period );
    int ltime = it->second;

    int target_time = 0;
    int shift = 0;
    while(true){
      if( stime % period <= ltime ) 
        target_time = ( stime / period + shift ) * period + ltime;
      if( stime % period > ltime )
        target_time = ( stime / period + shift + 1 ) * period + ltime;

      if( busy_map.find( target_time ) == busy_map.end() ){
        busy_map[ target_time ] = true;
        break;
      } else btime++;
      
      shift++;
    }

    wasted_time += target_time - stime;
    n_cars++;

    return target_time;
  }

  map<int, bool> busy_map;
  int btime;
  int wasted_time, n_cars;
};

class Route {
  public:
  string name;
  int id, L;
  Connect * A, * B;
};

class Car{
  public:
  vector<Route*> routes;
  int id ;
  int route_index ;
  bool finished;
  int timer_wait, timer_drive, end_time;

  void Reset(){
    route_index = 0;
    timer_wait = 0;
    timer_drive = 0;
    end_time = 0;
    finished = false;
  }

  Car(){
    route_index = 0;
    timer_wait = 0;
    timer_drive = 0;
    end_time = 0;
    finished = false;
  }
};

int era = 0;


struct Events{
  int time, car_id, route_id, sema_id;
  Car * car;

  void Print(){
    cout << time << " " << car_id << " " << route_id << " " << sema_id << " " << endl;
  }
};

struct cmp {
    bool operator() (Events A, Events B) const {
        if( A.time == B.time ) return A.car_id < B.car_id;
        return A.time < B.time;
    }
};

set<Events, cmp> events;

bool tick(){

  while(true){
    if( not events.size() ) return false;
    Events ev = *events.begin();
    events.erase(events.begin());
//cout << 1 << endl;
    Car * car = ev.car;
    car->route_index++;
    Route   * route = car->routes[car->route_index];

    // if(car->id == 171) ev.Print(); // FIXME

//cout << 2 << endl;
    if( car->route_index == car->routes.size()-1 ){ 
      car->finished = true; 
      car->end_time     = ev.time + route->L; 
      car->timer_drive += route->L;
      continue; 
    }
//cout << 3 << endl;
    Connect * con = route->B;
    int start_time = ev.time + route->L;
    int cross_time = con->GetTime( start_time, route->id, car->id );
//cout << 4 << endl;
    car->timer_drive += route->L;
    car->timer_wait  += cross_time - start_time;
//cout << 5 << endl;
    Events next_ev       = Events();
    next_ev.time         = cross_time; 
    next_ev.car_id       = car->id;
    next_ev.route_id     = route->id;
    next_ev.sema_id      = con->id;
    next_ev.car          = car;
    events.insert( next_ev );
//cout << 6 << endl;
    break;
  } 

  return true;
}

#include <fstream>
int TrafficFast(){
  int N_Cars_MAX = 99999999;
  
  std::ifstream infile("hashcode.in");

  int N_D, N_I, N_S, N_V, N_F;
  infile >> N_D >> N_I >> N_S >> N_V >> N_F;
  cout << N_D <<" "<< N_I<<" " << N_S <<" "<< N_V<<" " << N_F << endl;

  N_Cars_MAX = min( N_Cars_MAX, N_V );

  map<string, Route*> routes;
  map<int, string> routes_id;
  vector<Connect> connects = vector<Connect>( N_I );
  for(int i = 0; i < N_S; i++){
    Route * r = new Route();
    int A_id, B_id, L;
    string name;
    infile >> A_id >> B_id >> name >> L;
    r->A = & (connects[A_id]);
    r->B = & (connects[B_id]);
    r->L = L;
    r->id = i;
    r->name = name;
    routes[name] = r;
    routes_id[r->id]  = r->name;
    connects[B_id].id = B_id;
  }

  vector<Car> cars = vector<Car>( N_Cars_MAX );
  for(int i = 0; i < N_Cars_MAX; i++){
    int P;
    infile >> P;
    Car & car = cars[i];
    while(P>0){
      string rname;
      infile >> rname;
      Route* r = routes.find( rname )->second;
      car.routes.push_back( r );
      if(P>1) r->B->AddRoute( r->id );
      P--;
    }
    car.id = i;
  }
  
  // ADD EVENTS ZERO ===========================================
  for(int i = 0; i < N_Cars_MAX; i++){
    Car * car = & cars.at( i );

    Route   * route = car->routes[0];
    Connect * con = route->B;
    int cross_time = con->GetTime( 0, route->id, car->id );
    car->timer_wait += cross_time;

    Events next_ev       = Events();
    next_ev.time         = cross_time; 
    next_ev.car_id       = car->id;
    next_ev.route_id     = route->id;
    next_ev.sema_id      = con->id;
    next_ev.car          = car;
    events.insert( next_ev );

      if( i == 1000 ) cout << "THIS 0 !!!" << endl;

    if(car->id == 0) next_ev.Print();
  }

  // WORKING LOOP ===========================================
  while(true){
    bool work = tick();
    if(not work) break;
  }

  int score = 0, max_score = 0;
  cout << "Car reports : " << endl;
  for(Car & car : cars) {
    if( car.id >= N_Cars_MAX ) break;
    // cout << " " << car.id << " wait: " << car.timer_wait << " drive: " << car.timer_drive << " fin: " << car.finished << " " << car.end_time << endl;
    if(car.finished){
      score += N_F;
      score += (N_D - car.end_time);

      max_score += N_F;
      max_score += (N_D - car.timer_drive);
    }
  }
  cout << "SCORE: " << score << " " << max_score << endl;

  // TUNE LOOP ===========================================
  if(true){
    max_score = score;
    int N_tunes = 1;
    int tune_index = 0;
    int prev_score = 0;
    int m_iters = 6;
    int c_iters = 0;

    N_tunes = 5000;
    while(N_tunes>0){

      int old_waste = connects[ tune_index ].wasted_time;
      cout << "tune " << tune_index << " (" << connects[ tune_index ].wasted_time << " " << connects[ tune_index ].wasted_time/connects[ tune_index ].n_cars  << ") ";
      for( int i = 0; i < N_I; i++ ){
        connects[i].Reset( i == tune_index );
        //if( i == 1000 ){
        //  cout << connects[i].id << endl; 
        //}
      }

      for(int i = 0; i < N_Cars_MAX; i++){
        Car * car = & cars.at( i );
        car->Reset();
        Route   * route = car->routes[0];
        Connect * con  = route->B;

        int cross_time = con->GetTime( 0, route->id, car->id );
        car->timer_wait += cross_time;
        Events next_ev       = Events();
        next_ev.time         = cross_time; 
        next_ev.car_id       = car->id;
        next_ev.route_id     = route->id;
        next_ev.sema_id      = con->id;
        next_ev.car          = car;
        events.insert( next_ev );
      }

      while(true){
        bool work = tick();
        if(not work) break;
      }

      int score = 0, n_finished = 0;
      for(Car & car : cars) {
        if(car.finished){
          score += N_F;
          score += (N_D - car.end_time);
          n_finished++;
        }
      }

      c_iters++;
      // if( abs(prev_score - score) < 100 or c_iters > m_iters ) { tune_index--; prev_score = 0; c_iters = 0; }

      if( score <= max_score ){
        // connects[ tune_index ].is_tuned = false;
        cout << "SCORE: " << score << " " << max_score << " " << connects[ tune_index ].wasted_time << " (" << connects[ tune_index ].wasted_time/connects[ tune_index ].n_cars << ") " << endl;
      } else {
        cout << "SCORE: " << score << " " << max_score << " " << connects[ tune_index ].wasted_time << " (" << connects[ tune_index ].wasted_time/connects[ tune_index ].n_cars << ") x " << endl;
        max_score = max(max_score, score);
      }

      tune_index = rand() % 6000;
      while( connects[ tune_index ].wasted_time < 100 or connects[ tune_index ].wasted_time/connects[ tune_index ].n_cars < 2 ){
        tune_index = rand() % 6000;
      }
      tune_index = 0;

      prev_score = score;
      // cout << "SCORE: " << score << " " << n_finished << " " << max_score << endl;
      N_tunes--;
    }

    cout << " ======= LAST RUN ======= " << endl ;
    if(true){
      for( int i = 0; i < N_I; i++ ){ connects[i].Reset( false ); }
      for(int i = 0; i < N_Cars_MAX; i++){
        Car * car = & cars.at( i );
        car->Reset();
        Route   * route = car->routes[0];
        Connect * con  = route->B;

        int cross_time = con->GetTime( 0, route->id, car->id );
        car->timer_wait += cross_time;
        Events next_ev       = Events();
        next_ev.time         = cross_time; 
        next_ev.car_id       = car->id;
        next_ev.route_id     = route->id;
        next_ev.sema_id      = con->id;
        next_ev.car          = car;
        events.insert( next_ev );
      }
      while(true){
        bool work = tick();
        if(not work) break;
      }
    }
  }

  cout << " ======= ANALYTICS ======= " << endl ;
  for(Connect & con : connects) {
    if( not con.used ) continue;
    cout << con.id << " rs=" << con.light_indexes.size() << " wt=" << con.wasted_time << " nc=" << con.n_cars  << " bt=" << con.btime << endl;
  }

  if(false){
    cout << " ======= OUTPUT ======= " << endl ;
    int used_connects = 0;
    for(Connect & con : connects) {
      if( not con.used ) continue;
      used_connects++;
    }

    cout << used_connects << endl;
    for(Connect & con : connects) {
      if( not con.used ) continue;
      cout << con.id << endl;
      cout << con.light_indexes.size() << endl;
      for(int i = 0; i < con.light_indexes.size(); i++){
        cout << routes_id[ con.light_indexes[i] ] << " " << 1 << endl;
      }
    }
  }

  return 0;
}







