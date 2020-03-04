### 1 基础知识
1. std::thread
   - 1.1 基础知识【thread_create.cpp】
     - 1.1.1 创建std::thread，一般会绑定一个底层的线程。若该thread还绑定好函数对象，则即刻将该函数运行于thread的底层线程。
线程相关的很多默认是move语义，因为在常识中线程复制是很奇怪的行为。
     - 1.1.2 joinable()：是否可以阻塞至该thread绑定的底层线程运行完毕（倘若该thread没有绑定底层线程等情况，则不可以join）
     - 1.1.3 join()：本线程阻塞直至该thread的底层线程运行完毕。
     - 1.1.4 detach()：该thread绑定的底层线程分离出来，任该底层线程继续运行（thread失去对该底层线程的控制）。
   - 1.2 线程创建方法
     - 1.2.1 函数指针
     - 1.2.2 函数对象
     - 1.2.3 Lambda函数
   - 1.3 基础相关
     - std::chrono::milliseconds(10) 定义一个10毫秒的间隔
     - this_thread::sleep_for（）线程休眠
     - this_thread::sleep_for(std::chrono::milliseconds(10))让本线程休眠10毫秒
### 2 互斥变量
   为了避免多线程对共享变量的一段操作会发生冲突，引入了互斥体和锁。
   - 2.1 std::mutex【mutex_test.cpp】
     - 互斥体，一般搭配锁使用，也可自己锁住自己（lock(),unlock()）。
     - 若互斥体被第二个锁请求锁住，则第二个锁所在线程被阻塞直至第一个锁解锁
   - 2.2 std::lock_guard【lock_gurad_test.cpp】
     - 简单锁，构造时请求上锁，释放时解锁，性能耗费较低。适用区域的多线程互斥操作。
   - 2.3 std::unique_lock【unique_lock_test.cpp】
     - 更多功能也更灵活的锁，随时可解锁或重新锁上（减少锁的粒度），性能耗费比前者高一点点。适用灵活的区域的多线程互斥操作，有的像mutex和lock_guard的结合体
   - 2.4 死锁产生条件【mutex_daed_lock_test.cpp】
      - 互斥条件：某资源只能被一个进程使用，其他进程请求该资源时，只能等待，知道资源使用完毕后释放资源。
　　  - 请求和保持条件：程序已经保持了至少一个资源，但是又提出了新要求，而这个资源被其他进程占用，自己占用资源却保持不放。
　　  - 不剥夺条件：任何一个资源在没被该进程释放之前，任何其他进程都无法对他剥夺占用 
　　  - 循环等待条件：当发生死锁时，所等待的进程必定会形成一个环路（类似于死循环），造成永久阻塞。
　　  - 破坏死锁一个条件即可避免死锁
### 3 原子变量
    原子变量的意思就是单个最小的、不可分割的变量（例如一个int），原子操作则指单个极小的操作（例如一个自增操作）
C++的原子类封装了这种数据对象，使多线程对原子变量的访问不会造成竞争。（可以利用原子类可实现无锁设计)，一般用于计数或者统计
- 3.1 std::atomic_flag
  - 它是一个原子的布尔类型，可支持两种原子操作。(实际上mutex可用atomic_flag实现)
  - test_and_set()： 如果atomic_flag对象被设置，则返回true; - 如果atomic_flag对象未被设置，则设置之，返回false。
clear()：清除atomic_flag对象。
- 3.2 std::atomic<T> 
  - 对int, char, bool等基本数据类型进行原子性封装（其实是特化模板）。
  - store()：修改被封装的值。
  - load()： 读取被封装的值。
### 4 条件变量
  条件变量一般是用来实现多个线程的等待队列，即主线程通知（notify）有活干了，则等待队列中的其它线程就会被唤醒，开始干活。
   - 4.1 std::condition_variable【thread_printAB.cpp】
        - wait(std::unique_lock<std::mutex>& lock, Predicate pred = [](){return true;})：pred()为true时直接返回，pred()为false时，lock必须满足已被当前线程锁定的前提。执行原子地释放锁定，阻塞当前线程，并将其添加到等待*this的线程列表中。
        - notify_one()/notify_all()：激活某个或者所有等待的线程，被激活的线程重新获得锁。
### 5 任务列队
任务队列对线程间通信进行了抽象，限定了线程间只能通过传递任务，而相关的数据及操作则被任务保存。任务队列这个名词可能在其他场景定义过其他意义，这里讨论的任务队列定义为：能够把封装了数据和操作的任务在多线程间传递的线程安全的先入先出的队列
![image](15E63E96BBC941229725845377CE0B3F)
- 5.1 生产消费者模型【product_comsume_test.cpp】
  - 5.1.1 模型要点
    - 生产者生产数据到缓冲区中，消费者从缓冲区中取数据。
    - 如果缓冲区已经满了，则生产者线程阻塞；
    - 如果缓冲区为空，那么消费者线程阻塞
  
```
void task_queue_t::produce(const task_t& task_) {
        lock_guard_t lock(m_mutex);
        if (m_tasklist->empty()){//! 条件满足唤醒等待线程
            m_cond.signal();
        }
        m_tasklist->push_back(task_);
    }
int   task_queue_t::comsume(task_t& task_){
        lock_guard_t lock(m_mutex);
        while (m_tasklist->empty())//! 当没有作业时，就等待直到条件满足被唤醒{
            if (false == m_flag){
                return -1;
            }
            m_cond.wait();
        }
        task_ = m_tasklist->front();
        m_tasklist->pop_front();
        return 0;
}
```
- 5.2 任务列队使用技巧
  - 5.2.1 IO与逻辑分离
    - 比如网络游戏服务器程序中，网络模块收到消息包，投递给逻辑层后立即返回，继续接受下一个消息包。逻辑线程在一个没有io操作的环境下运行，以保障实时性。
    
    ```
    void handle_xx_msg(long uid, const xx_msg_t& msg){
       logic_task_queue->post(boost::bind(&servie_t::proces, uid, msg));
    }
    ```
  - 5.2.2 并行流水线
    - 上面的只是完成了io 和 cpu运算的并行，而cpu中逻辑操作是串行的。在某些场合，cpu逻辑运算部分也可实现并行，如游戏中用户A种菜和B种菜两种操作是完全可以并行的，因 为两个操作没有共享数据。最简单的方式是A、B相关的操作被分配到不同的任务队列中
    
    ```
    void handle_xx_msg(long uid, const xx_msg_t& msg) {logic_task_queue_array[uid %     sizeof(logic_task_queue_array)]->post( boost::bind(&servie_t::proces, uid, msg));}
    ```

  - 5.2.3 线程池和异步回调
    - 比如逻辑Service模块需要数据库模块异步载入用户数据，并做后续处理计算。而数据库模块拥有一个固定连接数的连接池，当执行SQL的任务到来时，选择一个空闲的连接，执行SQL，并把SQL 通过回调函数传递给逻辑层。其步骤如下：

       - 预先分配好线程池，每个线程创建一个连接到数据库的连接
       - 为数据库模块创建一个任务队列，所有线程都是这个任务队列的消费者
       - 逻辑层想数据库模块投递sql执行任务，同时传递一个回调函数来接受sql执行结果
      
    ```
    void db_t:load(long uid_, boost::function<void (user_data_t&) func_)
    {
       //! sql execute, construct user_data_t user
       func_(user)
     }
    void process_user_data_loaded(user_data_t&)
    {
       //! todo something
    }
     db_task_queue->post(boost::bind(&db_t:load, uid, func));
    ```
### 6 线程池
 - 6.1 为什么需要线程池
    - 目前的大多数网络服务器，包括Web服务器、Email服务器以及数据库服务器等都具有一个共同点，就是单位时间内必须处理数目巨大的连接请求，但处理时间却相对较短。   
   - 传统多线程方案中我们采用的服务器模型则是一旦接受到请求之后，即创建一个新的线程，由该线程执行任务。任务执行完毕后，线程退出，这就是是“即时创建，即时销毁”的策略。尽管与创建进程相比，创建线程的时间已经大大的缩短，但是如果提交给线程的任务是执行时间较短，而且执行次数极其频繁，那么服务器将处于不停的创建线程，销毁线程的状态。 
 - 6.2 线程池适用的场合
   -  6.2.1 单位时间内处理任务频繁而且任务处理时间短
   -  6.2.2 对实时性要求较高。如果接受到任务后在创建线程，可能满足不了实时要求，因此必须采用线程池进行预创建。
   -  6.2.3 必须经常面对高突发性事件，比如Web服务器，如果有足球转播，则服务器将产生巨大的冲击。此时如果采取传统方法，则必须不停的大量产生线程，销毁线程。此时采用动态线程池可以避免这种情况的发生。
 - 6.3 构建线程池框架【thread_pool_test.cpp】
   - 6.3.1 线程池管理器：用于创建并管理线程池
   - 6.3.2 工作线程：线程池中实际执行的线程
   - 6.3.3 任务接口：尽管线程池大多数情况下是用来支持网络服务器，但是我们将线程执行的任务抽象出来，形成任务接口，从而是的线程池与具体的任务无关。
   - 6.3.4 任务队列：线程池的概念具体到实现则可能是队列，链表之类的数据结构，其中保存执行线程。




