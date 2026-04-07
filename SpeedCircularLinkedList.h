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
    	std::map<std::string, T> kv_map;
        int bound = 0;
        Node* next = nullptr;
        Node** fast_search_list = nullptr;
		
    	Node(int bound, int fast_search_list_size) {
            this->bound = bound;
            if (fast_search_list_size > 0) {
                this->fast_search_list = new Node*[fast_search_list_size];
                for (int i = 0; i < fast_search_list_size; ++i) {
                    this->fast_search_list[i] = nullptr;
                }
            } else {
                this->fast_search_list = nullptr;
            }
        }

    	~Node() {
            if (fast_search_list != nullptr) {
                delete[] fast_search_list;
            }
        }
    };

private:
	Node* head = nullptr;
	int fast_search_list_size = 0;
	int list_size = 0;

	static int GetHashCode(std::string str) {
		long long ans = 0;
		for (auto& ch : str) {
			ans = (ans * s_prime + ch) % b_prime;
		}
		return static_cast<int>((ans + b_prime) % b_prime);
	}

	void BuildFastSearchList() {
        if (fast_search_list_size == 0) return; 
        
        Node* curr = head;
        for (int i = 0; i < list_size; ++i) {
            curr->fast_search_list[0] = curr->next;
            curr = curr->next;
        }

        for (int k = 1; k < fast_search_list_size; ++k) {
            curr = head;
            for (int i = 0; i < list_size; ++i) {
                curr->fast_search_list[k] = curr->fast_search_list[k - 1]->fast_search_list[k - 1];
                curr = curr->next;
            }
        }
    }

public:

    explicit SpeedCircularLinkedList(std::vector<int> node_bounds) {
        list_size = node_bounds.size();
        if (list_size == 0) return;
        fast_search_list_size = log2(list_size);

        head = new Node(node_bounds[0], fast_search_list_size);
        Node* curr = head;
        for (int i = 1; i < list_size; ++i) {
            curr->next = new Node(node_bounds[i], fast_search_list_size);
            curr = curr->next;
        }
        curr->next = head;

    	BuildFastSearchList();
    }

    ~SpeedCircularLinkedList() {
        Node* curr = head;
        for (int i = 0; i < list_size; ++i) {
            Node* next = curr->next;
            delete curr;
            curr = next;
        }
    }

    void put(std::string str, T value) {
	    int code = GetHashCode(str);
        
        if (code <= head->bound) {
            head->kv_map[str] = value;
            return;
        }

        Node* curr = head;
        for (int k = fast_search_list_size - 1; k >= 0; --k) {
            while (curr->fast_search_list && 
                   curr->fast_search_list[k]->bound > curr->bound && 
                   curr->fast_search_list[k]->bound < code) {
                curr = curr->fast_search_list[k];
            }
        }
        while (curr->next->bound > curr->bound && curr->next->bound < code) {
            curr = curr->next;
        }
        curr->next->kv_map[str] = value;
    }

	T get(std::string str) {
    	int code = GetHashCode(str);
        if (list_size == 0) return T();

        if (code <= head->bound) {
            if (head->kv_map.count(str)) return head->kv_map[str];
            return T();
        }

        Node* curr = head;
        for (int k = fast_search_list_size - 1; k >= 0; --k) {
            while (curr->fast_search_list && 
                   curr->fast_search_list[k]->bound > curr->bound && 
                   curr->fast_search_list[k]->bound < code) {
                curr = curr->fast_search_list[k];
            }
        }
        while (curr->next->bound > curr->bound && curr->next->bound < code) {
            curr = curr->next;
        }
        
        if (curr->next->kv_map.count(str)) {
            return curr->next->kv_map[str];
        }
        return T();
    }

    void print() {
        Node* curr = head;
        for (int i = 0; i < list_size; ++i) {
            std::cout << "[Node] Bound = " << curr->bound << ", kv_map_size = " << curr->kv_map.size() << "\n";
            curr = curr->next;
        }
    }

    int size() const {
        return list_size;
    }
};
#endif //SPEEDCIRCULARLIST_H