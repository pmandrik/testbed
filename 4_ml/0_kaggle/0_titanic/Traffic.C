
int era = 0;

/*
  1. average number of waiting cars per road
  2. maximum number of waiting cars per road
  3. waisting time per road
  4. total number of cars
*/

struct Report{
  int average_cars, max_cars, waist_time, total_cars;
  Report(){ total_cars = 0; }
  Report(int a, int b, int c){
    average_cars = a, max_cars = b, waist_time = c;
    total_cars = 0;
  }
};

class Car;
class Route;
struct CarEvent{
  int itime, otime, route_id;
  bool ok;
  Route * r;
};

class Connect {
  public:
  void Add( Car * car, int route_id ){
    auto it = street_n_cars.find( route_id );
    it->second.first.push_back( car );
    it->second.second.max_cars = max( it->second.second.max_cars, int(it->second.first.size()) );
    it->second.second.total_cars += 1;
    it->second.second.average_cars += it->second.first.size();
  };
  bool Free( Car * car, int route_id, int waist_time ){
    if( era_index > era ) return false;
    if( route_id != route_green ) return false;
    auto it = street_n_cars.find( route_id );
    if( it->second.first.front() != car ) return false;
    it->second.first.pop_front();
    it->second.second.waist_time += waist_time;

    era_index += 99999;
    report_n_cars++;
    return true;
  };

  void AddRoute( int route_id ){
    street_n_cars  [ route_id ] = make_pair( list<Car*>(), Report(0,0,0) );
    era_index = 0;
    siter = 0;
    liter = 0;
    route_green = -1;
  }

  void Print(){
    cout << id << " gl route " << route_green << " ";
    cout << street_n_cars.size() << ",_ ";
    // for(auto it = street_n_cars.begin(); it != street_n_cars.end(); ++it) cout << it->second.size() << " ";
  }

  // DATA COLLECTION START //
  void CollectDataTick(){
  }
  // DATA COLLECTION END //
  void Tick(){
    CollectDataTick();
    era_index = era;

    if( not light_indexes.size() ) return;
    siter--;
    if( siter <= 0){
      liter++;
      if(liter >= light_indexes.size()) liter = 0;
      // siter = light_indexes[ liter ].second;
      // route_green = light_indexes[ liter ].first;
      route_green = light_indexes[ liter ];
    }
  }

  void AddGL(int route, int time){
    // light_indexes.push_back( make_pair(route, time) );
    light_indexes.push_back( route );
    liter = 0;
    // siter = light_indexes[ liter ].second;
    route_green = light_indexes[ liter ];

    period = light_indexes.size();
    road_ling_index[ route ] = light_indexes.size()-1;
    frozen.push_back( false );
  }

  void AddDefault(){
    for(auto it = street_n_cars.begin(); it != street_n_cars.end(); ++it){
      AddGL( it->first, 1 );
    }
  }

  int id, era_index;
  unordered_map< int, pair< list<Car*>, Report> > street_n_cars;
  // vector< pair<int,int> > light_indexes;
  vector< int > light_indexes;
  vector<bool> frozen;
  int siter, liter, route_green;
  int report_n_cars, period;

  unordered_map< int, int > road_ling_index;
  vector<CarEvent> car_events;
};

class Route {
  public:
  string name;
  int id, L;
  Connect * A, * B;
};

struct DriveData{
  int time;
  int route_id;
  int corn_id;
  int type;
  DriveData(int t, int ty, int ia, int ib){ time = t; type = ty; route_id = ia; corn_id = ib; }
};

class Car{
  public:
  vector<Route*> routes;
  vector< DriveData > time_stamps;
  Connect * target_connect;
  int route_index ;
  int route_pos   ;
  int id ;
  bool on_route, finished;
  int ftime;

  int timer_wait, timer_drive, current_wait_time;

  Car(){
    timer_wait = 0;
    timer_drive = 0;
    current_wait_time = 0;
  }

  string lstate;
  void Print(){
    if(finished) return;
    else if (on_route) {
      if(lstate == "drive" and (route_pos == 0 or route_pos == routes[ route_index ]->L) )
        cout << era << " drive " << routes[ route_index ]->name << "(" << routes[ route_index ]->id << ")" << " " << route_pos << "/" << routes[ route_index ]->L << endl;
      lstate = "drive";
    }
    else {
      cout << era << " wait " << routes[ route_index ]->name << "(" << routes[ route_index ]->id << ")" << " in ";
      target_connect->Print();
      cout << "+" << target_connect->street_n_cars[ routes[ route_index ]->id ].first.size() << "cars";
      cout << endl;
      lstate = "wait";
    }
  }
  
  void Tick(){
    if(finished) return;

    Route * r = routes[ route_index ];
    if( on_route ){
      route_pos++;
      timer_drive++;
      if( route_pos >= r->L ){
        on_route = false;

        if( routes.size() <= route_index+1 ) {
          finished = true;
          ftime = era;
          return;
        }

        target_connect->Add( this, r->id );
        current_wait_time = 0;

        time_stamps.push_back( DriveData(era, 0, r->id, r->B->id) );
      }
    }
    if( not on_route ){
      if( ! target_connect->Free(this, r->id, current_wait_time) ) { timer_wait++; current_wait_time++; return; }
      time_stamps.push_back( DriveData(era, 1, r->id, r->B->id) );

      route_index++;
      route_pos = 0;
      on_route = true;
      target_connect = routes[ route_index ]->B;
    }
    return;
  }
};

int get_loss(int & period, int & tresh, int & itime, int & otime){
  return ( itime < tresh ? tresh - itime : period - itime + tresh ) 
       + ( otime > tresh ? otime - tresh : period - tresh + otime );
}

bool set_minimums(map< int, CarEvent> & events, int period, vector<int> & poses_back){
  vector<int> poses      = vector<int>( period, -1 );
  map<int, bool> vals_map;
  for(auto it = events.begin(); it != events.end(); ++it){
    CarEvent & event = it->second;
    event.ok = false;
    for( int i = event.itime; i < event.otime; i++ ){
      if( (i-event.itime)/period >= 1 ) break;
      if( poses[i%period] != -1 ) continue;
      poses[i%period] = event.route_id;
      event.ok = true;
      vals_map[event.route_id] = true;
      break;
    }
  }
  for(auto it = events.begin(); it != events.end(); ++it){
    CarEvent & event = it->second;
    if( event.ok ) continue;
    for( int i = event.otime; i < event.otime+period; i++ ){
      if( poses[i%period] != -1 ) continue;
      poses[i%period] = event.route_id;
      event.ok = true;
      vals_map[event.route_id] = true;
      break;
    }
    if( event.ok ) continue;
    for( int i = 0; i < period; i++ ){
      if( poses[i] != -1 ) continue;
      poses[i] = event.route_id;
      event.ok = true;
      vals_map[event.route_id] = true;
      break;
    }
    if( event.ok ) continue;
    return false;
  }

  for(int & val : poses_back){
    if( vals_map.find( val ) != vals_map.end() ) continue;
    for( int i = 0; i < period; i++ ){
      if( poses[i] != -1 ) continue;
      poses[i] = val;
      break;
    }
    return false;
  }

  poses_back = poses;
  return true;
}

void iterate_routes_data( vector<Connect> & connects ){
  // vector<CarEvent> con_n_events; //  
  for(int i = 0; i < connects.size(); ++i){
    Connect & con = connects[i];
    if( not con.car_events.size() ) continue;

    map< int, CarEvent> events;
    for( CarEvent & event : con.car_events ){
      if(event.otime  == -1) continue;
      auto it = events.find( event.route_id );
      if( it == events.end() ){
        events[ event.route_id ] = event;
        continue;
      }
      it->second.itime = max(it->second.itime, event.itime);
      it->second.otime = min(it->second.otime, event.otime);
    }

    // map< int, CarEvent> & events, int period, vector<int> & poses
    set_minimums( events, con.period, con.light_indexes );
  }
}

#include <fstream>
int Traffic(){
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
    routes_id[r->id] = r->name;
    connects[B_id].AddRoute( r->id );
    connects[B_id].id = B_id;
  }

  /*
  // DUMP DICTIONARY
  cout << "street_names = { ";
  for(auto it = routes.begin(); it != routes.end(); ++it){
    cout << it->second->id << ": \"" << it->first << "\"" << ", ";
  }
  cout << "}" << endl;
  return 0;
  */

  vector<Car> cars = vector<Car>( N_V );
  for(int i = 0; i < N_V; i++){
    int P;
    infile >> P;
    Car & car = cars[i];
    while(P>0){
      string rname;
      infile >> rname;
      car.routes.push_back( routes.find( rname )->second );
      P--;
    }

    car.target_connect = car.routes[0]->B;
    car.target_connect->Add( & cars[i], car.routes[0]->id );
    car.route_index = 0;
    car.route_pos   = car.routes[0]->L;
    car.id          = i;
    car.on_route = false;
    car.finished = false;
    car.current_wait_time = 0;
  }

  // study
  unsigned long nmax = 0;
  unsigned long average = 0;
  for( Connect & con : connects ){
    nmax = max( nmax, con.street_n_cars.size() );
    average += con.street_n_cars.size();
    // cout << con.street_n_cars.size() << " ";
  }
  cout << "Max N. streets per connection = " << nmax << endl;
  cout << "Avr N. streets per connection = " << average / connects.size() << endl;

  // SETUP SEMAFORES ========================================
  //connects[1412].AddGL(42365, 100); 
  if( true ){
    // std::ifstream rfile("results_v1.txt");
    std::ifstream rfile("results_tmp_2.txt");
    int N_corn, id_corn, N_rs, N_time;
    string rname;
    rfile >> N_corn;

    while(N_corn> 0){
      N_corn--;
      rfile >> id_corn;
      rfile >> N_rs;
      while(N_rs>0){
        N_rs--;
        rfile >> rname;
        rfile >> N_time;
        // cout << rname << " " << N_time << " " << N_rs << endl;
        if( routes.find(rname) == routes.end() ){ cout << rname << " " << N_time << " " << N_rs << endl; return 0; }
        connects[ id_corn ].AddGL( routes.find(rname)->second->id, N_time ); 
      }
    }

  } else {
    for(Connect & con : connects){
      con.AddDefault();
      con.CollectDataTick();
    }
  }

  // WORKING LOOP ========================================
  for(Connect & con : connects){ con.CollectDataTick(); }
  for(era = 0; era < N_D; era++){
    if( era%500 == 0 ) cout << era << "/" << N_D << endl;
    for(Car & car : cars) {
      if( car.id >= N_Cars_MAX ) break;
      car.Tick();
      if( car.id == 0 ) car.Print();
    }

    for(Connect & con : connects) {
      con.Tick();
    }
  }

  // TUNE SEMAFORES ========================================
/*
  if( true ){
    cout << " ======= BEGIN ======= TUNE " << endl ;
    for(int i = 0; i < 50; i++) iterate_routes_data( connects ) ;
    cout << " ======= OUTPUT ======= " << endl ;
    cout << connects.size() << endl;
    
    for(Connect & con : connects) {
      cout << con.id << endl;
      cout << con.light_indexes.size() << endl;
      for(int i = 0; i < con.light_indexes.size(); i++){
        cout << routes_id[ con.light_indexes[i] ] << " " << 1 << endl;
      }
    }
  }
*/

  // DUMP DATA ========================================
  int score = 0;
  cout << "Car reports : " << endl;
  for(Car & car : cars) {
    if( car.id >= N_Cars_MAX ) break;
    cout << " " << car.id << " wait: " << car.timer_wait << " drive: " << car.timer_drive << " fin: " << car.finished << " " << car.ftime << endl;
    if(car.finished){
      score += N_F;
      score += (N_D - car.ftime);
    }
  }
  cout << "SCORE: " << score << endl;
  return 0;

  cout << "Connect reports : " << endl;
  for(Connect & con : connects) {
    cout << con.id << " " << con.report_n_cars << endl;
    for(auto it = con.street_n_cars.begin(); it != con.street_n_cars.end(); ++it){
      if( it->second.second.max_cars == 0 ) continue;
      cout << "  " << it->first;
      cout << " acars=" << it->second.second.average_cars;
      cout << " mcars=" << it->second.second.max_cars;
      cout << " wtime=" << it->second.second.waist_time;
      cout << " tcars=" << it->second.second.total_cars << endl;
    }
  }


  return 0;
}










