# 加速循环链表

## 题目描述

Vitalrubbish 想设计一套存储系统来存储最近堆积如山的数据。

所有的数据都是以键值对 (key-value pair) 的形式呈现。其中，键 (key) 是`string`类型的，值 (value) 是**任意但给定类型**的，一套存储系统中值的类型只有一种。

所有的数据都是以键值对的形式呈现，其中：
- 键`key`是`string`类型；
- 值`value`是模板类型`T`（在单个存储系统中所有值的类型相同）；

看到这里，聪明的你可能想到了STL中一个纯天然存储键值对的结构`map`。然而，由于一台机器的内存是有限的，其能够存储的键值对数量也是有限的。对于更大规模的数据，你必须要用更多的机器来进行存储。

于是，Vitalrubbish 设计了一套由 $n$ 台机器构成的分布式存储系统。这个系统的设计如下：
- 系统的主体结构由多个节点组成的循环链表；
- 每个节点都对应一台可以存储数据的机器，每个节点负责存储特定范围内的数据；
- 键值对通过哈希函数将键`key`键映射到整数`code`，根据这个整数决定数据存储在哪个节点；

下图是一个 $n = 8$ 的例子：

<img src="https://acm.sjtu.edu.cn/OnlineJudge/oj-images/a19ea847-5157-457c-8c54-797adddd229d" style="width: 61.8%">

为了验证这个存储系统的可行性，他打算先用 C++ 实现一个`SpeedCircularLinkedList`模板类来模拟这个存储系统，模拟方式如下：

我们用结构体`Node`来模拟系统中的单个节点，其有以下几个成员：
- `int bound`：节点的**上界**，用于确定键值对在分布式系统中的存储位置。系统根据键的哈希值`code`与该值的比较结果决定数据应当存储到哪个节点。
- `Node* next`：存储链表中当前节点的**下一个节点**；
- `Node** fast_search_list`：快速查找表，用于实现**加速查询**（加速查询的具体设计见下文【加速查询思路】部分）；
- `map<string, T> kv_map`：存储当前节点维护的键值对；

注：你可以为`Node`节点类合理增设一些成员变量（如前驱节点等），**但是不能修改已有的成员变量**。

我们用类`SpeedCircularLinkedList`来模拟整个存储系统，其拥有以下类成员：
- `Node* head`：存储链表的“头节点”，即图上的`Node0`
	- 虽然说这是一个循环链表，但是为了实现方便，我们还是象征性定义一个头节点作为链表遍历的起点。但与链表不同的是，我们的头节点中需要存放数据；
- `int list_size`：存储链表的大小（即节点个数）；
- `int fast_search_list_size`：存储快速查找表`fast_search_list`的长度；

注：你不能为`SpeedCircularLinkedList`增设任何的成员变量，也不能更改已有变量的`public/private`属性。违者的提交会被助教手动 void。

在本题中，你们需要实现这个加速循环链表的模板类（以下简称“链表类”）`SpeedCircularLinkedList`，里面有如下函数：

### 节点类相关函数
你需要实现该部分的**所有**函数。
- `Node(int bound, int fast_search_list_size)`：节点类的构造函数，负责初始化一个节点，其中 `bound` 代表节点的上界，`fast_search_list_size` 代表快速查找表的长度。

- `~Node()`：析构函数，用于释放构造函数中分配的内存空间。

### 辅助函数
你**无需实现**下面这些辅助函数，只需要理解它们的功能即可。
- `int log2(int x)`：用于计算整数 $x$ 的对数（向下取整）。
- `int GetHashCode(std::string str)`：转换函数，可以将一个字符串映射到 $[0, b_{prime} - 1]$ 范围的整数。

### 链表类相关函数
你需要实现该部分的**所有**函数。
- `SpeedCircularLinkedList(std::vector<int> node_bounds)`：链表类的构造函数，其中 `node_bounds` 代表链表中各个节点的上界。
	- 题目保证 `node_bounds` 中的元素全是正整数，并且随着下标的增加**严格递增**，且最大的元素等于$b_{prime}$；
	- 你需要保证链表的大小等于 `node_bounds` 的长度，头节点 `head` 存储的是上界最小的节点；
	- 从`head`节点出发，通过调用`next`指针遍历链表，则各节点的上界`bound`按照遍历顺序严格递增；
- `~SpeedCircularLinkedList()`： 链表类的析构函数，用于释放构造函数中分配的内存空间。
- `int size()`：返回链表的大小，即链表中的节点个数 `list_size`；
- `void put(std::string str, T value)`：放入键值对。
	- 在函数的开头，会调用辅助函数 `GetHashCode(str)` 来获取`str`对应的`code`值；
	- 如果`code`值小于等于`head`节点的上界`bound`，则将键值对存入头节点`head`的`kv_map`中；
	- 否则，通过遍历链表，找到一个节点使得`code`**小于等于**该节点的上界`bound`且**严格大于**其前驱节点的上界`bound`；
	- 题目保证`code`值**严格小于** $b_{prime}$，即所有的键值对都能够被放入存储系统中；
- `T get(std::string str)`：查找键值对。
	- 在函数的开头，会调用辅助函数 `GetHashCode(str)` 来获取`str`对应的`code`值；
	- 通过遍历链表，找到应当存储此键值对的节点，并返回对应的值（如果没有存储该键值对则返回`T`的默认构造函数产生的值）；
- `void print()`：按照`bound`从小到大的顺序输出链表，单行输出格式如下:
```
[Node] bound = {bound}, kv_map_size = {size}
示例：
[Node] bound = 104, kv_map_size = 1
```
行与行之间以及最后一行的结尾都需要输出换行符。

### 加速查询思路

**友情提醒**：本题除了性能测试以外的所有数据点都不需要实现和 `fast_search_list` 有关的任何内容。

在朴素的`put/get`操作中，时间复杂度都是 $O(n)$ 的；加速查询要求大家利用`fast_search_list`实现 $O(log^2n)$ 复杂度的查询`put/get`；

你需要在 `void BuildFastSearchList()` 函数中完成你的加速查找表的构建。当然，只要能通过我们的性能测试，你也可以设计自己的查询思路。

下面给出一种可行的加速查询思路：
- 所有节点 `fast_search_list` 的长度都是$\lfloor log_2n \rfloor$；
- 对于一个节点 `cur`，其 `fast_search_list` 中的第 $k$ 项 (0-based) 存储了其 $2^{k}$ 级后继。例如，在上图中 $0$ 号节点的`fast_search_list`依次存储了 $1, 2, 4$ 号节点， $5$ 号节点的 `fast_search_list`依次存储了 $6, 7, 2$ 号节点。
- 从`head`出发开始查询；查询的时候，遍历当前节点`cur`的`fast_search_list`，找到`fast_search_list`中没有越过目标节点中下标最大的节点，并且将当前节点赋值为该节点，完成一次迭代；
- 如果查询到目标节点，查询完毕；

### 代码模板
```c++
#ifndef SPEEDCIRCULARLIST_H
#define SPEEDCIRCULARLIST_H
#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <algorithm>

constexpr int s_prime = 31;

inline int log2(int x) {
	int ans = 0;
	while (x != 1) {
		x /= 2;
		++ans;
	}
	return ans;
}

template<typename T, int b_prime>
class SpeedCircularLinkedList {
public:
    struct Node {
    	// 你可以为 Node 结构体添加更多的成员变量，但是不能修改已有的成员变量
    	std::map<std::string, T> kv_map;
        int bound = 0;
        Node* next = nullptr;
        Node** fast_search_list = nullptr;
		
    	Node(int bound, int fast_search_list_size) {} // TODO : Constructor

    	~Node() {} // TODO : Destructor
    };

private:
	// 你不能添加更多的成员变量
	Node* head = nullptr;
	int fast_search_list_size = 0;
	int list_size = 0;

	/* 这是将字符串映射到整数的哈希函数，用于决定键值对的存储位置。
	 * 你无需关注这个函数的实现细节，只需要在 put 和 get 函数调用它即可。
	 */
	static int GetHashCode(std::string str) {
		long long ans = 0;
		for (auto& ch : str) {
			ans = (ans * s_prime + ch) % b_prime;
		}
		return static_cast<int>((ans + b_prime) % b_prime);
	}


	/* TODO : Build your fast search list.
	 * 题面描述中给出了一种快速查找表的构建方式，你也可以按照自己的方式构造快速查找表。
	 * 我们只会对快速查找的正确性和时间性能进行测试，不会测试你的具体实现。
	 * 如果你暂时没有实现快速查找功能，请留空这个函数。
	 */
	void BuildFastSearchList() {}

public:

	/* TODO : Constructor
	 * 在这里实现你的加速循环链表类的构造函数。
	 * node_bounds 存储了加速循环链表各节点的存储上界。
	 * 题目保证 node_bounds 严格升序，并且其最后一项等于 b_prime。
	 * 构造函数的最后一行应当是 BuildFastSearchList()，如果没有实现可以将其注释掉。
	 */
    explicit SpeedCircularLinkedList(std::vector<int> node_bounds) {

    	BuildFastSearchList();
    }

	/* TODO : Destructor
	 * 在这里实现你的析构函数
	 */
    ~SpeedCircularLinkedList() {}

	/* TODO : Put key-value pair
	 * 函数功能：插入一个键值对，其中 str 代表键，value 代表值
	 * 在函数的开始，需要通过调用 GetHashCode() 来获取 str 映射到的整数 code
	 * 根据 code 和各个节点 bound 的关系找到存储该键值对的节点
	 */
    void put(std::string str, T value) {
	    int code = GetHashCode(str);
    	// 请不要修改上面这行代码
    }

	/* TODO : Get key-value pair
	 * 函数功能：查询 str 对应的 value
	 * 在函数的开始，需要通过调用 GetHashCode() 来获取 str 映射到的整数 code
	 */
	T get(std::string str) {
    	int code = GetHashCode(str);
    	// 请不要修改上面这行代码

    	return T(); // 返回T类型默认构造函数产生的值
    }

    /* TODO : Print SpeedCircularList
     * 遍历链表，按照 bound 从小到大输出值
     * 单行输出格式：[Node] Bound = {bound}, kv_map_size = {size}
     * 单行输出示例：[Node] Bound = 104, kv_map_size = 1
     * 行与行之间以及最后一行的结尾都需要输出换行符
     */
    void print() {}

    /* TODO Size Method
     * 获取链表大小
     */
    int size() const {}
};
#endif //SPEEDCIRCULARLIST_H
```

## 输入格式

无

## 输出格式

见题面或者下发文件

## 数据范围

| 测试点编号  | 测试内容  |  前序测试点依赖  | 节点数n | put/get 总次数 m | 内存检查 | 分值 |
| :------------: | :------------: | :------------: | :------------: | :-----------: | :----------: | :------------: |
| $1$  | 节点构造函数  | - | - | - | 无 | $15$
| $2$  | 链表构造函数，print()  | $1$ | $\le 20$  | - | 无  | $16$
| $3$ | 析构函数 | $1, 2$  | $\le 20$  | - | 有 | $4$
| $4$ | size()  | $1, 2$  | $\le 20$  | - | 无 | $5$
| $5$ | put()  | $1, 2$  |  $\le 50$ | $\le 100$  | 无 | $12$
| $6$ | put()  | $1, 2, 5$  | $\le 50$ | $\le 100$ | 有   | $3$
| $7$ | get()  | $1, 2, 5$  | $\le 50$  | $\le 100$  | 无 | $12$
| $8$ | get()  | $1, 2, 5, 7$  | $\le 50$ | $\le 100$  | 有  | $3$
| $9$  | 综合测试 | $1, 2, 4, 5, 7$ | $\le 1000$ | $\le 4000$  | 无 | $15$
| $10$ | 性能测试 | $1, 2, 4, 5, 7$ | $\le 5\times10^4$  | $\le 2 \times 10^5$  | 无 | $15$

