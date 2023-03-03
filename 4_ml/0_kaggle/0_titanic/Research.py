

import re

class Connect():
  def __init__(self, id, cars):
    # print("id", id)
    self.id = int(id)
    self.cars = int(cars)
    self.rs = []

  def Print(self):
    print( self.id, self.cars, self.rs)

  def Res(self):
    self.working_rs = [ rs for rs in self.rs if int(rs[2]) > 0 ]

def parce(input_file):
  file1 = open(input_file, 'r')
  Lines = file1.readlines()
  m0 = re.compile(r'([\d]+) acars=([\d]+) mcars=([\d]+) wtime=([\d]+) tcars=([\d]+).*') #  731 acars=0 mcars=0 wtime=4 tcars=0
  m1 = re.compile(r'([\d]+) wait: ([\d]+) drive: ([\d]+) fin: ([\d]+) ([\d]+).*') #0 wait: 146 drive: 342 fin: 0 0

  cars = []
  cons = []

  state = None;
  for line in Lines :
    # print(line)
    if line == "Car reports : \n" : state = "car"
    elif line == "Connect reports : \n" : state = "connects"
    elif state == "car" :
      g = m1.search(line)
      if g :
        # print(g.groups())
        cars += [ g.groups() ]
        if int(g.groups()[1]) > 3500 : print(line[:-2])
    elif state == "connects":
      if len(line.split()) == 2 :
        # print( line.split() )
        if line.split()[0] == '(int)' : continue
        cons += [ Connect(line.split()[0], line.split()[1]) ]
      else:
        g = m0.search(line)
        if g :
          #print( g.groups() )
          cons[-1].rs += [ g.groups() ]
  return cars, cons


cars_v0, cons_v0 = parce("log_full_v0.txt")
cars_v1, cons_v1 = parce("log_full_v1.txt")

from matplotlib import pyplot
import numpy as np
def get_statistics( cars, cons ):
  N_cars = len(cars)
  N_finished = len([ car for car in cars if car[3] ])
  print( "cars fin/tot = ", N_finished, "/", N_cars )

  N_wait     = [ int(car[1]) for car in cars ]
  N_drive    = [ int(car[2]) for car in cars ]
  N_corners  = [ int(car[4]) - int(car[2]) - int(car[1]) for car in cars ]
  N_finished = [ int(car[4]) for car in cars ]

  N_wait_mean = float(sum( N_wait )) / N_cars
  N_drive_mean = float(sum( N_drive )) / N_cars
  N_corners_mean = float(sum( N_corners )) / N_cars
  N_finished_mean = float(sum( N_finished )) / N_cars

  N_wait_std = np.std(np.array(N_wait))
  N_drive_std = np.std(np.array(N_drive))
  N_corners_std = np.std(np.array(N_corners))
  N_finished_std = np.std(np.array(N_finished))

  print( "N_wait = ", N_wait_mean, "+-", N_wait_std )
  print( "N_drive = ", N_drive_mean, "+-", N_drive_std )
  print( "N_corners = ", N_corners_mean, "+-", N_corners_std )
  print( "N_finished = ", N_finished_mean, "+-", N_finished_std )

  const_off = [ con for con in cons if con.cars == 0 ]
  const_on = [ con for con in cons if con.cars > 0 ]
  print( "Cons on/tot = ", len(const_on), "/", len(cons) )

  for con in const_on : con.Res()

  N_rs         = sum([ len(con.rs) for con in const_on ])
  N_rs_working = sum([ len(con.working_rs) for con in const_on ])

  print( "RS working/tot = ", N_rs_working, "/", N_rs)
  return N_wait, N_drive, N_corners, N_finished


data_v0 = get_statistics( cars_v0, cons_v0 )
data_v1 = get_statistics( cars_v1, cons_v1 )

def plot(name, A, B):
    bins = np.linspace(min( min(A), min(B) ), max( max(A), max(B) ), 100)
    pyplot.hist(A, bins, alpha=0.5, label='name', density=False)
    pyplot.hist(B, bins, alpha=0.5, label='name', density=False)
    pyplot.legend(title=name, loc='upper right')
    pyplot.show()

for v1, v2, name in zip(data_v0, data_v1, ["N_wait", "N_drive", "N_corners", "N_finished"]) : plot(name, v1, v2)

def get_result_naive( cons ):
  answer = []
  for con in cons :
    routes = [  ]
    if con.cars == 0 : continue
    for rs in con.working_rs :
      routes += [ [rs[0], 1] ]
    answer += [ [ con.id, routes ] ]
  return answer

def get_result_naive_2( cons ):
  answer = []
  for con in cons :
    routes = [  ]
    if con.cars == 0 : continue
    for rs in con.working_rs :
      routes += [ [rs[0], rs[2]] ]
    answer += [ [ con.id, routes ] ]
  return answer

import street_names
def dump_results( answer ):
  f = open("results_v2.txt", 'w')
  answer_lines  = str(len( answer )) + "\n"

  for a in answer :
    answer_lines += str(a[0]) + "\n"
    answer_lines += str(len( a[1] )) + "\n"
    for r in a[1] :
      answer_lines += street_names.street_names[ int(r[0]) ] + " " + str( r[1] ) + "\n"

  f.write( answer_lines );

dump_results( get_result_naive_2( cons_v1 ) )


"""
cars fin/tot =  1000 / 1000
N_wait =  1249.954 +- 243.867418660222
N_drive =  3145.229 +- 147.64739943188977
N_corners =  119.0 +- 0.0
N_finished =  4514.183 +- 291.26812306017973
Cons on/tot =  7999 / 8000
RS working/tot =  54186 / 63964
"""





