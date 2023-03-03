
from matplotlib import pyplot

if True:
  import pandas
  import numpy as np

  def f_ticket( x ) : 
    a = x.split()
    if len(a) == 1 : return "def";
    return a[0]

  def plot(predictions, truth):
    x = []
    y = []
    for xv, yv in zip(predictions, truth):
      if yv[0] : y+=[xv[0]]
      else     : x+=[xv[0]]

    print(x)
    print(y)
    bins = np.linspace(0, 1, 20)
    pyplot.hist(x, bins, alpha=0.5, label='d', density=False)
    pyplot.hist(y, bins, alpha=0.5, label='s', density=False)
    pyplot.legend(title="", loc='upper right')
    pyplot.show()

  def get_datasets( name = "train.csv" ):
    all_data = pandas.read_csv(name, dtype=str)
    id_data = pandas.DataFrame(data=all_data, dtype=np.int64, columns=['PassengerId'])
    y_data = pandas.DataFrame(data=all_data, dtype=np.float64, columns=['Survived'])

    all_data['Pclass'] = all_data['Pclass'].map( lambda x : int(x) )
    all_data['Sex']    = all_data['Sex'].map( lambda x : 0 if x == 'male' else 1 )
    all_data['Age']    = all_data['Age'].fillna(-1).map( lambda x : float(x) )
    all_data['SibSp']     = all_data['SibSp'].fillna(-1).map( lambda x : int(x) )
    all_data['Parch']     = all_data['Parch'].fillna(-1).map( lambda x : int(x) )
    all_data['Ticket']    = all_data['Ticket'].map( f_ticket )
    all_data['Cabin']     = all_data['Cabin'].fillna("Z").map( lambda x : ord(x[0]) )
    all_data['Embarked']  = all_data['Embarked'].fillna("Z").map( lambda x : ord(x[0]) )

    a = all_data.to_dict('list')['Ticket']
    ticket_dic = { aa : 0 for aa in a } 
    ticket_dic = list(ticket_dic.keys())
    ticket_dic = { val : i for i, val in enumerate(ticket_dic) }
    def f_ticket_2( x ) : return ticket_dic[x];
    all_data['Ticket']    = all_data['Ticket'].map( f_ticket_2 )

    if False :
      for column in all_data:
        print(all_data[column])
        x = []
        y = []
        for xv, yv in zip(all_data[column], y_data['Survived']):
          print(xv, yv)
          if yv : y+=[xv]
          else  : x+=[xv]

        try:
          bins = np.linspace(int(min(x))-1,int( max(x))+1, 20)
              
          pyplot.hist(x, bins, alpha=0.5, label='d', density=True)
          pyplot.hist(y, bins, alpha=0.5, label='s', density=True)
          pyplot.legend(title=column, loc='upper right')
          pyplot.show()
        except : continue

    # x_data_raw = pandas.DataFrame(data=all_data, dtype=np.int64, columns=['Pclass', 'Sex', 'Age', 'SibSp', 'Parch', 'Ticket', 'Cabin', 'Embarked'])
    x_data_raw = pandas.DataFrame(data=all_data, dtype=np.int64, columns=['Pclass', 'Sex', 'Age', 'SibSp', 'Parch', 'Ticket', 'Cabin', 'Embarked'])
    x_data=(x_data_raw-x_data_raw.min())/(x_data_raw.max()-x_data_raw.min())

    # print( x_data )
    # print( y_data )
    # print( id_data )

    return x_data, y_data, id_data

  import tensorflow as tf
  from tensorflow import keras

  x_data, y_data, id_data = get_datasets( name = "train.csv" )
  dataset = tf.data.Dataset.from_tensor_slices((x_data.values, y_data.values)).repeat(10).shuffle(1000).batch(1)
  model = keras.Sequential([
    keras.layers.Dense(units=36*2, activation='relu'),
    keras.layers.Dense(units=26*2, activation='relu'),
    keras.layers.Dense(units=16*2, activation='relu'),
    keras.layers.Dense(units=1, activation='sigmoid'),
  ])
  model.compile(optimizer='adam', loss=tf.keras.losses.BinaryCrossentropy(from_logits=False), metrics=['accuracy'])

  history = model.fit(dataset, epochs=20, steps_per_epoch=500)

  predictions = model.predict( tf.data.Dataset.from_tensor_slices((x_data.values, y_data.values)).batch(1) )

  rate = 0
  for pred, id, trye in zip( predictions, id_data.values, y_data.values ):
    # print(id[0], int(pred[0] > 0.1), trye[0])
    if int(pred[0] > 0.1) == trye[0] : rate += 1
  print( float(rate) / len(y_data.values) )

  plot(predictions, y_data.values)


  x_data_test, y_data_test, id_data_test = get_datasets( name = "test.csv" )
  dataset_test = tf.data.Dataset.from_tensor_slices((x_data_test.values, y_data_test.values)).batch(1)
  predictions = model.predict( dataset_test )

  # print( predictions )

  import csv
  csvfile = open('results.csv', 'w', newline='')
  fieldnames = ['PassengerId','Survived']
  writer = csv.DictWriter(csvfile, fieldnames=fieldnames)
  writer.writeheader()
  for pred, id in zip( predictions, id_data_test.values ):
    writer.writerow({'PassengerId': id[0], 'Survived': int(pred[0] > 0.1)})

  









