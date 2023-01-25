# **Leela-Zero**
* Reference: [github.com/leela-zero/leela-zero](https://github.com/leela-zero/leela-zero)

* <Leela.cpp>: the main function

# Rules of Go
## <FastBoard.h/cpp>

* structure
  * parameter
    |parameter|meaning|
    |---|---|
    |```enum vertex_t```| the value in the board {BLACK, WHITE, EMPTY, INVAL}, INVAL means outside of the board |
    |```array<4> m_dirs``` | the directions of adjacent positions {up, down, left, right} |
    |```int NBR_SHIFT```, ```int NBR_MASK```| binary bit record neighbor, ```EEEEWWWWBBBB``` |
    |```int PASS```| the action of PASS this move |
    |||

  * member variable
    |member variable|meaning|
    |---|---|
    |```array m_state``` | board including {empty, black, white} |
    |```array m_next``` | the next position in board of the stone belonging to the same stone block |
    |```array m_parent``` | the head position of the stone block to which the stone belongs |
    |```array m_libs``` | the qi of the stone block |
    |||

* state & update state & board
  * reset / initial board
    |function|meaning|
    |---|---|
    |```reset_board()``` | judge wether suicide |
    |||

  * add / remove neighbour

  * count the number of neighbours
    |function|meaning|
    |---|---|
    |```count_neighbours()``` | count the number of neighbours |
    |```count_pliberties()``` | count the number of empty neighbours |
    |||

* action & judge whether the action is legal
  * judge wether suicide
    |function|meaning|
    |---|---|
    |```is_suicide()``` | judge wether suicide |
    |||

* reward & Judge the victory & valuate the termination status  
  ```A player’s score is the number of points of her color, plus the number of empty points that reach only her color.``` 
  |function|meaning|
  |---|---|
  |```calc_reach_color()``` | calculate the reach colors |
  |```area_score()``` | valuate the termination status |
  |||

## <FullBoard.h/cpp>

# Neural Networks
## <CPUPipe.h/cpp>
* Winograd convolutional algorithm
  $$\boldsymbol Y = \boldsymbol g * \boldsymbol d = \boldsymbol A^T ((\boldsymbol G \boldsymbol g \boldsymbol G^T) \odot (\boldsymbol B^T \boldsymbol d \boldsymbol B)) \boldsymbol A$$

  |function|meaning|
  |---|---|
  |```winograd_convolve3()``` | winograd convolutional algorithm |
  |```winograd_transform_in()``` | $\boldsymbol G \boldsymbol g \boldsymbol G^T$, $\boldsymbol B^T \boldsymbol d \boldsymbol B$ |
  |```winograd_sgemm()``` | General Matrix Mutiplication algorithm |
  |```winograd_transform_out()``` | $\boldsymbol A^T () \boldsymbol A$ |
  |||

* General Matrix Mutiplication algorithm

## <Img2Col.h>
* Img2Col convolutional algorithm  
  $$\boldsymbol Y = \boldsymbol g * \boldsymbol d = \text{Img2Col}(g) * \text{Img2Col}(d)$$

  note: Leela-Zero only uses Img2Col in $1 \times 1$ convolutional kernal.

  |function|files|meaning|
  |---|---|---|
  |```im2col()``` |<Img2Col.h>| transfor img to cols|
  |```convolve()``` | <CPUPipe.h/cpp>| Img2Col convolutional algorithm |
  |||

# Monte Carlo Tree Search
## <UCTSearch.h/cpp>

# Other Trick
## <Zobrist.h/cpp>
* Zobrist hashing algorithm

## <ThreadPool.h>
* Thread Pool
  * ```class ThreadPool```
    |member variable|meaning|
    |---|---|
    |```vector m_threads``` | thread array |
    |```queue m_tasks``` | task queue |
    |```mutex m_mutex``` | mutex |
    |```condition_variable m_condvar``` | condition variable |
    |```bool m_exit``` | record whether the thread pool exits |
    |||

    |function|meaning|
    |---|---|
    |```initialize()``` | initialize all threads by null function |
    |```add_thread()``` | add a new thread to thread array |
    |```add_task()``` | add a new task to task queue and notify threads to work by condition variable |
    |||

  * ```class ThreadGroup```
    |member variable|meaning|
    |---|---|
    |```m_pool``` | Thread Pool |
    |```vector<future> m_taskresults``` | record the results of all threads |
    |||

    |function|meaning|
    |---|---|
    |```add_task()``` | add a new task to task queue and notify threads to work by condition variable |
    |```wait_all()``` | wait all threads return the results |
    |||

## <OpenCL.h.cpp>
* OpenCL