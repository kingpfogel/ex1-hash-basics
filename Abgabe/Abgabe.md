# Experimentelle Algorithmik
## Übung 1 



- Niklas Neesemann *(Matrikelnr. 609789)*
- Leonhard Thyssen *(Matrikelnr. 573374)*

*Github Repo: [here](https://github.com/kingpfogel/ex1-hash-basics)*

-

# 1.)
### 1.a) 
See the simex archive: [data.tar.gz](https://github.com/kingpfogel/ex1-hash-basics/blob/master/data.tar.gz)

### 1.b) 
![](/Users/leonhardthyssen/Desktop/EA/UE/1/Abgabe\ Repo/ex1-hash-basics/plots/pngs/1b\)\ -\ time_insert.png)

![](/Users/leonhardthyssen/Desktop/EA/UE/1/Abgabe\ Repo/ex1-hash-basics/plots/pngs/1b\)\ -\ time_lookup.png)

-

# 2.)

### 2.a) 

See the Quadratic Probing Implementation [here](https://github.com/kingpfogel/ex1-hash-basics/blob/d3ef687f096cccb0e83ff7736d477a729d666a51/develop/hashing/main.cpp) (starts at line 161)


### 2.b)  

See the raw `perf stat` cache misses results [here](https://github.com/kingpfogel/ex1-hash-basics/blob/master/perf.csv)

The number of cache misses grows in the following order (independent of the fill-factor [0.25, 0.50, 0.75, 0.90]): Chaining, linear, quadratic. This lies in the nature of the algorithms and how the cache works. The cache can grab a certain amount of bytes into its memory and when we access a cell/node it can grab the bytes around it, which makes another access much faster. Chaining stores elements with the same hash in a (linked) list, which might cause the elements to lie next to each other in the memory. So the next element that will be traversed could already be in the cache memory. Linear and especially quadratic probing, in contrast, hashes to (a completely) different index, hence to a different location in memory, which is why the next accessed cell is not in the cache. 

### 2.c) 
![](/Users/leonhardthyssen/Desktop/EA/UE/1/Abgabe\ Repo/ex1-hash-basics/plots/pngs/2c\)\ -\ time_insert.png)
![](/Users/leonhardthyssen/Desktop/EA/UE/1/Abgabe\ Repo/ex1-hash-basics/plots/pngs/2c\)\ -\ time_lookup.png)

Compared to linear probing and chaining it performs poorly.

-

# 3.)
### 3.a) 
We have chosen a threshold of log2 (M) for the maximal length of the eviction chain. [This](https://github.com/kingpfogel/ex1-hash-basics/blob/d3ef687f096cccb0e83ff7736d477a729d666a51/develop/hashing/main.cpp) link leads to the file with the implementation (starts at line 225). 

### 3.b) 
![](/Users/leonhardthyssen/Desktop/EA/UE/1/Abgabe\ Repo/ex1-hash-basics/plots/pngs/3b\)\ -\ time_insert.png)
![](/Users/leonhardthyssen/Desktop/EA/UE/1/Abgabe\ Repo/ex1-hash-basics/plots/pngs/3b\)\ -\ time_lookup.png)
