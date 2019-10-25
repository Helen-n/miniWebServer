> **1. C++是一个语言联邦，分为四部分**：
> -  C
> -  Object-Oriented C++
> -  Template C++
> -  STL 
---
> **2. 尽量以const,enum,inline替换#define**
> - 对于单纯变量，最好以const对象或enums替换#define
> - 对于形似函数的宏，最好改用inline函数来替换#define
---
> **3. 尽可能使用const**
> - const 在星号左边，表示被指物是常量
> - const 在星号右边，表示指针自身是常量
>    - void f1(const Widget* pw);
>    - void f2(Widget const* pw); 同上面的意思是一样
> - const 成员函数
>   - 目的是为了确认该成员函数可作用于const对象身上
---
> **6. 若不想使用编译器自动生成的函数**
> - 为了驳回编译器自动提供的机能，可将相应的成员函数声明为private并且不予实现。使用像Uncopyable这样的base class也是一种做法。
---
> **7. 构造,析构,赋值计算**
> - 多态的base classes 应该声明一个virtual析构函数。如果class带有任何virtual函数，他就应该拥有一个virtual析构函数
> - class的设计目的如果不是作为base class使用，或不是为了具备多态性，就不该声明virtual函数
---
> **8. 别让异常逃出析构函数**
> - 析构函数绝对不要吐出异常。如果一个被析构函数调用的函数可能抛出异常，析构函数应该捕捉任何异常，然后吞下它们或结束程序
> - 如果客户需要对某个操作函数运行阶段抛出异常做出反应，那么class应该提供一个普通函数执行该操作
---
> **9. 绝不在构造和析构过程中调用virtual函数**
> - base class 构造函数的执行更早于derived class 构造函数，当base class构造函数执行时derived class的成员变量尚未初始化。如果此期间调用的virtual函数下降至derived class阶层，要知道derived class的函数几乎必然取用local成员变量，而那些成员变量尚未初始化。
> - 在构造和析构期间不要调用virtual函数，因为这类调用从不下降至derived class
---
> **10. 令operator返回一个reference to** ***this**
---
> **12. 在operator=中处理“自我赋值”**
> 
> ```
> Widget& WdiGet::operator=(const Widget& rhs)
> {
>    if(this == &rhs) return *this;
>    delete pb;
>    pb = new Bitmap(*rhs.pb);
>    return *this;
> }

> - 确保当对象自我赋值时operator=有良好行为。其中技术包括比较“来源对象”和“目标对象”的地址，精心周到的语句顺序，以及copy-and-swap
> - 确定任何函数如果操作一个以上的对象，而其中多个对象是同一个对象时，其行为仍然正确
---
> **12. 构造/析构/赋值计算**
> - Copying函数应该确保复制“对象内的所有成员变量”及“所有Base class 成分”
> - 不要尝试以某个copying函数实现另一个copying函数。应该共同放入第三个函数中，并由两个copying函数共同调用
---
> **13. 以对象管理资源**
> - 获得资源后立刻放入管理对象
> - 管理对象运用析构函数确保资源被释放
> - auto_ptr的替代方案是“引用计数型指针”。所谓RCSP也是个智能指针，持续追踪共用多少对象指向某笔资源，并在无人指向它时自动删除该资源
> - auto_ptr和tr1::shared_ptr两者都在其析构函数内做delete而不是delete[]动作。
> - 为防止资源泄露，使用RAII对象，他们在构造函数中获得资源并在资源中释放资源
> - 两个常被使用的RAII class分别是tr1::shared_ptr和auto_ptr。前者通常是最佳选择，因为其copy行为比较直观。若选择auto_ptr，复制动作会使它指向null。
---
> **14. 在资源管理类中小心copying行为**
> - 禁止复制，许多时间允许RAII对象被复制并不合理
> - 对底层资源祭出“引用计数法”。有时候我们希望保有资源，直到它最后一个使用者被销毁。
> - 复制底部资源， 可以针对一份资源拥有其任意数量的附件。
> - 转移底部资源的拥有权
---
> **15. 在资源管理类中提供对原始资源的访问**
> - APIs往往要求访问原始资源，所以每一个RAII class应该提供一个“取得其所管理之资源”的办法
> - 对原始资源的访问可能经由显式转换或者隐式转换。一般而言显式转换比较安全，但隐式转换对客户比较方便
---
> **16. 成对使用new和delete时要采取相同形式**
> - 如果你在new表达式中使用[],必须在相应的delete表达式中也使用[]。如果你在new表达式中不使用[]，一定也不要在相应的delete表达式中使用[]。
---
> **17. 以独立语句将newed对象置入智能指针**
---
> **18. 让接口容易被正确使用，不易被误用**
> - 好的接口很容易被正确使用，不容易被误用。你应该在你的所有接口中努力达成这些性质
> - "促使正确使用"的办法包括接口的一致性，以及与内置类型的行为兼容
> - "阻止误用"的办法包括建立新类型，限制类型上的操作，束缚对象值，以及消除客户的资源管理责任
> - tr1::shared_ptr支持定制型删除器。这可防范DLL问题，可被用来自动解除互斥器
> - "cross-DLL-problem",发生在动态连接程序库（DLL）中被new创建，却在另外一个DLL内被delete销毁。
---
> **19. 设计class犹如设计type**   
> 设计规范：
> - 1. 新type的对象应该如何被创建和销毁？
> - 2. 对象的初始化和对象的赋值该有什么样的差别？
> - 3. 新type的对象如果被passed by value（以值传递），意味着什么？
> - 4. 什么是新type的“合法值”
> - 5. 你的新type需要配合某个继承图系（inheritance graph）
> - 6. 你的新type需要什么样的转换？
> - 7. 什么样的操作符和函数对此新type而言是合理的？
> - 8. 什么样的标准函数应该驳回？
>    - 那些必须声明为private
> - 9. 谁该取用新type的成员？
> - 10. 什么是新type的“未声明接口” 
> - 11. 你的新type有多么一般化？ 
> - 12. 你真的需要一个新type吗？
---
> **20. 宁以pass-by-reference-to-const替换pass-by-value**
> - by value方式，函数参数都是实际以实参的副本为初值，为调用端所获得的亦是函数返回值的一个复件。这些复件系由对象的copy构造函数产出，这可能使得pass-by-value成为昂贵的操作。
> - by reference方式传递参数也可以避免slicing（对象切割）问题。当一个derived class对象以by value方式传递并被视为一个base class对象， base class的copy构造函数会被调用，而“造成此对象的行为像个derived class对象”的那些特化性质全被切割掉了，仅仅留下一个base class对象。
> 
> ```
> class Window
> {
>    public:
>    std::string name() const;     // 返回窗口名称
>    vortual viod display() const; // 显示窗口和其内容
> };
> 
> class WindowWithScrollBars: public Window
> {
>     public:
>     virtual void display() const;
> }
> 
> // 写一个函数来打印窗口
> void printNameAndDisplay(Windows w)
> {
>     std::cout << w.name();
>     w.display();
> }
> 
> // 会造成切割的现象
> WindowWithScrollBars wwsb;
> printNameAndDisplay(wwsb);
> 
> // 解决切割问题的办法，就是以 by > reference-to-const的方式传递w:
> 现在是传进来是什么窗口类型，w就表现什么类型
> void printNameAndDisplay(const Windows& w)
> {
>    std::cout << w.name();
>    w.display();
> }
> ```
> - 对于内置类型和STL的迭代器和函数对象还是比较高效的，因为他们设计为passed by value
> - 总结1：尽量以pass-by-reference-to-const替换pass-by-value。前者通常比较高效，并可以避免切割问题
> - 总结2：以上规则并不适合内置类型，STL的迭代器和函数对象
---
> **21. 必须返回对象时，别妄想返回其reference**
> - 绝不要返回pointer或reference指向一个local stack对象，或返回reference指向一个heap-allocated对象，或返回pointer或reference指向一个local static对象而有可能同时需要多个这样的对象
---
> **22. 将成员变量声明为private**
> - 切记将成员变量声明为private。这可赋予客户访问数据的一致性，可细微划分访问控制，允诺约束条件获得保证，并提供class作者以充分的实现弹性
> - protected并不比public更具封装性
---
> **23. 宁以non-menmber, non-friend替换member函数**
> - 如果某些东西被封装，他就不再可见。越多东西被封装，约少人可以看见它。而越少人看见它，我们就有越大的弹性去变化它，因为我们的改变仅仅直接影响看到改变的那些人事物。
> - 宁可拿non-member non-friend函数替换member函数。这样做可以增加封装性，包裹弹性和机能扩充性。
---
> **24. 若所有参数皆需要类型转换，请为此采用non-member函数**
---
> **25. 考虑写出一个不抛出异常的swap函数**
> - 当std::swap对你的类型效率不高时，提供一个swap成员函数，并确定这个函数不抛出异常
> - 如果你提供一个member swap，也该提供一个non-member swap来调用前者。对于classes，也请特化std::swap
> - 调用swap是应针对std::swap使用using声明式，然后调用swap并且不带任何“命名空间资格修饰”
> - 为“用户定义类型”进行std templates 全特化是好的，但千万不要尝试在std内加入某些对std而言全新的东西
---
### 五： 实现
- 太快定义变量可能导致效率上的拖延
- 过度使用转型可能导致代码变慢又难维护，又招来微妙难解的错误
- 返回对象“内部数据之号码牌”可能会破坏封装并留给客户虚号码牌
- 未考虑异常带来的冲击则可能导致资源泄露和数据败坏
- 过度热心地inlining可能引起代码膨胀
- 过度耦合则可能导致让人不满意的冗长建制时间
---
> **27. 尽可能延后变量定义式的出现时间**
> - 尽可能延后变量定义式的出现。这样做可以增加程序的清晰度并改善程序效率
---
> **28. 尽量少做转型动作**
> - const_cast: 用来将对象的常量性移除。
> - dynamic_cast: 主要用来执行“安全向下转型”，也就是用来决定某对象是否归属继承体系中的某个类型。【耗费重大运行成本】
> - reinterpret_cast: 意图执行低级转型，实际动作取决于编译器，表示不可以移植。
> - static_cast: 用来强迫隐式转换，例如将non-const转为const对象，或者将int转double，例如将void* 转为typed指针
> - 如果可以，尽量避免转型，特别是在注重效率的代码中避免dynamic_cast。如果有个设计需要转型动作，试着发展无需转型的替代设计。
> - 如果转型是必要的，试着将它隐藏在某个函数后面。客户随后可以调用该函数，而不需要将转型放进他们自己的代码内。
> - 宁可使用c++-style转型，不要用旧式转型，前者很容易辨别出来，而且也比较有着分门别类的执掌。
---
> **28. 避免返回handle指向对象内部成分**
> - （包括references，指针，迭代器）指向对象内部。遵守这个条款可增加封装性，帮助const成员函数的行为像个const,并将发生“虚吊号码牌”的可能性降到最低。
---
> **29. 为“异常安全”而努力是值得的**   
> - 当异常被抛出时，带有异常安全性的函数会：
>   - 不泄露任何资源
>   - 不允许数据败坏   
> - 异常安全函数提供以下三个保证之一：
>   - 基本承诺：如果异常被抛出，程序内的任何事物仍然保持在有效状态下
>   - 强烈保证：如果异常被抛出，程序状态不改变
>   - 不抛出承诺：承诺绝不抛出异常，因为它们总是能够完成它们原先承诺的功能
> - “强烈保证”往往能够以copy-and-swap实现出来，但“强烈保证”并非对所有函数都可实现或具备实现意义
> - 函数保证的“异常安全保证”通常最高只等于其所调用之各个函数的“异常安全保证”中的最弱者
---
> **30. 透彻的了解inlining的里里外外**
> - 隐喻的方式是将函数定义雨class定义式内
> - 将大多数lilining限制在小型，被频繁调用的函数身上。这可使日后的调试过程和二进制升级更容易，也可使潜在的代码膨胀问题最小化，使程序的速度提升机会最大化
> - 不要只因为function templates 出现在头文件，就将它们声明为inline
---
> **31. 将文件间的编译依存关系降至最低**

> ```
> // 未能将接口和实现分离
> #include <string>
> #include "date.h"
> #include "sddress.h"
> 
> class Person
> {
>     public:
>     Person(const std::string& name, const Date& birthday, const Address& addr);
>     std::string name() const;
>     std::string birthDate() const;
>     std::string address() const;
>     
>     private:
>     std::string theName;
>     Date theBirthDate;
>     Address theAddress;
> }
> 
> // 将接口从实现中分离
> 
> #include <string>
> #include <memory>
> 
> class PersonImpl;
> class Date;
> class Address;
>
> class Person
> {
>    public:
>    Person(const std::string& name, const Date& birthday, const Address& addr);
>    std::string name() const;
>    std::string birthDate() const;
>    std::string address() const;
>    
>    private:
>    std::tr1::shared_ptr<PersonImpl> pImpl;
> };
> ```
> // 分离的关键在于以“声明的依赖性”替换“定义的依存性”，那正是编译内存依赖性最小化的本质：现实中让头文件尽可能自我满足，万一做不到，则让它与其他文件内的声明式相依，源于这个简单的设计策略：
> - 如果使用object references 或 object pointers可以完成任务，就不要使用objects.
> - 如果能够，尽量以class声明式替换class定义式
> - 为声明式和定义式提供不同的头文件

### 六： 继承与面对对象设计   
 - 继承可以单一继承和多重继承   
 - 每一个继承可以public，protected，private   
 - 也可以是virtual和non-virtual   
 - virtual意味着“接口必须继承 ”  
 - non-virtual意味着“接口和实现度必须继承”
---
> **32. 确定你的public继承塑模出is-a的关系**   
> - “public继承”意味着is-a。适用于base class身上的每一件事情一定也适用于dervied classes对象也都是一个base class对象
---
> **33. 避免遮掩继承而来的名称**
> - derived classes内的名称会遮掩base classes内的名称。在public继承下从来没有人希望如此
> - 为了让被遮掩的名称再见天日，可使用using声明式或转交函数
---
> **34. 区分接口继承和实现继承**
> 
> ```
> class Shape{
> public:
>    virtual void draw() const = 0;
>    virtual void erroe(const std::string& msg);
>    int objectID() const;
> };
> class Rectangle: public Shape{...};
> class Ellipse: public Shape{...};
> ```
> - 成员函数的接口总是会被继承，public继承意味着is-a，所以对base class为真的任何事情一定也对其derived classes为真。因此如果某个函数可实施于某个class身上，一定也可实施于其derived class身上。
> - 声明一个pure virtual 函数的目的是为了让derived classes 只继承函数接口
>    - shape::draw函数是在合理不过得事情，因为所有shape对象都应该是可绘出。但shape::draw无法为此函数提供合理的缺省实现，毕竟椭圆形绘法迥异于矩形绘法。
> - 声明简朴的impure virtual函数的目的，是让derived classes继承该函数的接口和缺省实现
>   - 其接口表示，每个class都必须支持一个“当遇上错误时可调用”的函数，但每个class可自由处理错误。如果某个class不想针对错误做出任何特殊行为，它可以退回到Shape class提供的缺省错误处理行为。
> - 声明non-virtual函数的目的是为了令derived classes继承函数的接口及一份强制性实现
---
> **35. 考虑virtual函数以外的其他选择**
> - 使用non-virtual interface手法，那是template method设计模式的一种特殊形式。它以public non-virtual成员函数包裹访问性的virtual函数
> - 将virtual函数替换为“函数指针成员变量”，这是Strategy设计模式的一种分解表现形式
> - 以tr1::function成员变量替换virtual函数，因而允许使用任何可调物，搭配一个兼容于需求的签名式
> - 将继承体系内的virtual函数替换为另一个函数体系的virtual函数。这是Strategy设计模式的传统实现手法
---
> **36. 绝不重新定义继承而来的non-virtual函数**
> **37. 绝不重新定义继承而来的缺省参考值**
> - 绝不要重新定义一个继承而来的缺省参数值，因为缺省参数值都是静态绑定，而virtual函数--你唯一应该覆写的东西--却是动态绑定
---
> **38. 通过符合塑模出has-a或“根据某物实现出”**
> - 复合是类型之间的一种关系，当某种关系的对象内含它种类型的对象。
> - 在应用域，复合意味has-a(有一个)。在实现域（implementation domain），复合意味is-implemented-in-terms-of
---
> **39. 明智而审慎的使用private继承**
> - private继承意味着is-implemented-in-terms-of(根据某物实现出)。它通常比复合的级别低。但是当derived class需要访问protected base class的成员，或需要重新定义继承而来的virtual函数时，这么设计是合理的。
> - 和复合不同，private继承可以造成empty base最优化。这对致力于“对象尺寸最小化”的程序库开发而言，可能很重要。
---
> **40. 明智而审慎的使用多重继承**

> ```
> // 钻石型多重继承,造成重复问题
> class File {...};
> class InputFile: public File{...};
> class OutputFile: public File{...};
> class IOFile:public InputFile, public OutputFile{...};
> 
> // 解决办法：采用“virtual 继承”
> class File{...};
> class InputFile: virtual public File{...};
> class OutputFile: virtual public File{...};
> class IOFile: public InputFile, public OutputFile{...};
> ```
>
> - 有一个简单的规则，任何时候当你使用public继承，请改用virtual public继承。但是virtual继承的兄弟往往体积变大，访问成员变量速度变慢：你得为virtual继承付出代价
> 
> ```
> class IPerson
> {
> public:
>    virtual ~IPerson();
>    virtual std::string name() const = 0;
>    virtual std::string birthDate() const = 0;
> };
> 
> class DatabaseID{...};
> 
> class PersonInfo
> {
> public:
>   explict PersonInfo(Database pid);
>   virtual ~PersonInfo();
>   virtual const char* theName() const;
>   virtual const char* theBirthDate() const;
>   virtual const char* valueDelimOpen() const;
>   virtual const char* valueDelimClose() const;
>   ...
> };
> 
> class CPerson: public IPerson, private PersonInfo
> {
> public:
>   explicit CPerson(DatabaseID pid): PersonInfo(pid){}
>   virtual std::string name() const
>   { return PersonInfo::theName();}
>   virtual std::string birthDate() const
>   { return PersonInfo::theBirthDate();}
> private:
>   const char* valueDelimOpen() const {return "";}
>   const char* valueDelimClose() const {return "";}
> };
> ```
> - 多重继承比单一继承复杂。它可能导致新的歧义性，以及对virtual继承的需要。
> - virtual继承会增加大小，速度，初始化（及赋值）复杂度等成本。virtual base class 不带任何数据，将是最具实用价值的情况
> - 多重继承的确有正当途径。其中一个情节涉及“public继承某个Interface class”和“private继承某个协助实现的class”的两相组合。
---
> **41. 了解new-handle的行为**
> - 当operator new抛出异常以反映一个未满足的内存需求之前，它会先调用一个客户指定的错误处理函数，一个所谓的new_handler
> - set_new_handler的参数是个指针，指向operator new无法分配足够内存时该被调用的函数。其返回值也是一个指针，指向set_new_handler被调用前正在执行的那个new_handler函数
> - 设计良好的new-handler函数必须做到以下事情：
>    - 让更多内存可被使用
>    - 安装另一个new-handler
>    - 卸除new-handler
>    - 不返回，通常调用abort或exit
> - set_new_handler允许客户指定一个函数，在内存分配无法获得满足时被调用
> - Nothrow new是一个颇为局限的工具，因为它只适用于内存分配；后继的构造函数调用还是可能抛出异常
---
> **50. 了解new和delete的合理替换时机**
> - 为什么有人想替换编译器的operator new和operator delete，以下是三个最常见的理由：
>   - ==用来检测运用上的错误==：如果“new所得内存”delete失败会导致内存泄露。
>   - ==为了强化效能==：编译器所带的operator new和operator delete主要用于一般目的，它们不但可以可被长时间执行的程序接受，也可被执行时间少于一秒的程序接受。所以制定operator new 和 operator delete性能胜过缺省版本。
>   - ==为了收集使用上的统计数据==：收集内存分布，寿命分布等信息。
>   - ==为了增加分配和归还的速度==
>   - ==为了降低缺省内存管理器带来的空间额外开销==
>   - ==为了弥补缺省分配器中的非最佳齐位==
>   - ==为了将相关对象成簇集中==
>   - ==为了获得非传统的行为==
---
> **51. 编写new和delete时需固守常规**
> - operator new应该内含一个无穷循环，并在其中尝试分配内存，如果它无法满足内存需求，就应该调用new-handler。它也应该有能力处理0bytes申请。class专属版本还应该处理“比正确大小更大的申请”。
> - operator delete应该在收到null指针时不做任何事。class专属版本则还应该处理“比正确大小更大的申请”。
---
> **52. 写了placement new也要写placement delete**
> - 当你写一个 placement operator new， 请确定也写出了对应的placement operator delete。如果没有这样做，你的程序可能会发生隐藏而时断时续的内存泄露。
> - 当你声明placement new和placement delete，请确定不要无意识地遮掩了它们的正常版本。
---
> **53. 不要轻易忽视编译器的警告**
> - 严肃对待编译器发出的警告信息。努力在你的编译器的最高警告级别下争取“无任何警告”的荣誉。
> - 不要过度依赖编译器的警报能力，因为不同的编译器对待事情的态度并不相同。一旦移植到另一个编译器上，你原本依赖的警告信息有可突然消失。
---
> **54. 让自己熟悉包括TR1在内的标准程序库**
> - 智能指针 tr1::shared_ptr和tr1::weak_ptr。
> - tr1::function，此物得以表示任何callable entity，只要其签名符合目标。
> - tr1::bind，它能够做STL绑定器bind1st和bind2nd所做的每一件事，而又多。
> - Hash tables， 用来实现sets，multisets，multi-maps。
> - 正则表达式
> - Tuples，tr1::tuple可持有任意个数的对象
> - tr1::array，本质上是个“STL化“的数组。不过大小固定。
> - tr1::mem_fn，这个语句构造上于成员函数指针一致的东西。
> - tr1::reference_wrapper，一个“让references的行为更像对象”的设施。
> - 随机数
> - 特殊数学函数
> - C99兼容扩充
> - Type traits，一组traits classes用以提供类型的编译期信息。
> - tr1::result_of，这个是template，用来推导函数调用的返回类型
> - ==TR1自身只是一份规范。为了获得TR1提供的好处，你需要一份实物。一个好的实物来源是Boost。==
---
> **55. 让自己熟悉Boost**
> - Boost是一个社群，也是一个网站。致力于免费，源码开放，同僚复审的C++程序开发。Boost在C++标准化过程中扮演深具影响力的角色。
> - Boost提供许多TR1组件实现品，以及其他程序库。



















