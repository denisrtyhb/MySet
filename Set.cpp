#include <set>
#include <cstddef>
#include <bits/stdc++.h>
using namespace std;
#define slice slefwef
template<typename T>
std::vector<T> slice(std::vector<T> arr, int l, int len) {
    std::vector<T> res;
    for (int i = 0; i < len; ++i)
        res.push_back(arr[l + i]);
    return res;
}
int cnt = 0;

template<class T>
class Set {
public:
    Set() {}

    template<typename Iterator>
    Set(Iterator first, Iterator last): data_(first, last) {
        for (auto it = first; it != last; ++it)
            insert(*it);
    }

    Set(std::initializer_list<T> elems) {
        for (auto& t : elems)
            insert(t);
    }

    Set(const Set& other) {
        for (auto& i : other.data_) insert(i);
    }

    Set& operator=(const Set& other) {
        if (&other == this) return *this;
        delete root;
        root = NULL;
        data_.clear();
        for (auto& i : other) insert(i);
        return *this;
    }

    ~Set() {
        delete root;
        root = NULL;
    }

    size_t size() const {
        return data_.size();
    }

    bool empty() const {
        return data_.empty();
    }

    void insert(const T& elem) {
        if (find(elem) != end()) return;
        if (root == NULL) {
            //assert(data_.empty());
            data_.clear();
            data_.push_back(elem);
            root = new node();
            root->min_val = root->max_val = data_.begin();
            return;
        }
        if (root->children.size() == 0) {
            node* newroot = new node({root});
            root->parent = newroot;
            root = newroot;
        }
        node* now = root;
        while (now->children.size() != 0) {
            bool flag = false;
            for (auto ch : now->children) {
                if (*(ch->max_val) < elem) continue;
                now = ch;
                flag = true;
                break;
            }
            if (!flag) now = now->children.back();
        }
        now = now->parent;
        int i = 0;
        while (i < now->children.size() && *(now->children[i]->min_val) < elem) {
            ++i;
        }
        iterator data_it = now->min_val;
        for (int _ = 0; _ < i; ++_) {
            ++data_it;
        }
        data_.insert(data_it, elem);
        --data_it;
        auto new_leaf = new node();
        new_leaf ->min_val = data_it;
        new_leaf ->max_val = data_it;
        new_leaf->parent = now;
        now->children.insert(now->children.begin() + i, new_leaf);
        now->relax();
        while (now->children.size() == 4) {

            if (now->parent == NULL) {
                assert(now == root);
                node* root1 = new node({now->children[0], now->children[1]}, root);
                node* root2 = new node({now->children[2], now->children[3]}, root);
                root->children = {root1, root2};
                root->relax();
                return;
            }
            now = now->parent;
            now->reshuffle();
        }

        while (now != NULL) {
            now->relax();
            now = now->parent;
        }
    }

    void erase(const T& elem) {
        if (root == NULL) return;
        if (root->children.size() == 0) {
            if (*(root->min_val) < elem || elem < *(root->min_val)) return;
            delete root;
            data_.clear();
            root = NULL;
            return;
        }
        node* now = root;
        while (now->children.size() != 0) {
            bool flag = false;
            for (auto ch : now->children) {
                if (elem < *(ch->min_val) || *(ch->max_val) < elem) continue;
                now = ch;
                flag = true;
                break;
            }
            if (!flag) return;
        }
        auto data_delete = now->min_val;
        now = now->parent;
        for (int i = 0; i < now->children.size(); ++i) {
            if (elem < *(now->children[i]->min_val) || *(now->children[i]->min_val) < elem) continue;
            delete now->children[i];
            now->children.erase(now->children.begin() + i);
            break;
        }
        data_.erase(data_delete);
        now->relax();
        while (now->children.size() == 1) {
            if (now->parent == NULL) {
                assert(root == now);
                root = root->children[0];
                root->parent = NULL;
                now->children.clear();
                delete now;
                return;
            }
            now = now->parent;
            now->reshuffle();
        }
        while (now != NULL) {
            now->relax();
            now = now->parent;
        }
    }

    typedef typename std::list<T>::const_iterator iterator;

    iterator begin() const {
        return data_.cbegin();
    }

    iterator end() const {
        return data_.cend();
    }

    iterator find(const T& elem) const {
        if (root == NULL) return end();
        node* now = root;
        if (elem < *(root->min_val) || *(root->max_val) < elem) {
            return end();
        }
        while (now->children.size() != 0) {
            bool flag = false;
            for (auto ch : now->children) {
                if (elem < *(ch->min_val) || *(ch->max_val) < elem) continue;
                now = ch;
                flag = true;
                break;
            }
            if (!flag) return end();
        }
        if (*(now->min_val) < elem) return end();
        if (elem < *(now->min_val)) return end();
        return now->min_val;
    }

    iterator lower_bound(const T& elem) const {
        node* now = root;
        if (root == NULL) return end();
        if (*(root->max_val) < elem) {
            return end();
        }
        while (now->children.size() != 0) {
            bool flag = false;
            for (auto ch : now->children) {
                if (*(ch->max_val) < elem) continue;
                now = ch;
                flag = true;
                break;
            }
            if (!flag) std::cout << "???" << std::endl;
        }
        return now->min_val;
    }
public:
    struct node {
        node* parent = NULL;
        std::vector<node*> children;
        iterator min_val;
        iterator max_val;
        node() {
            ++cnt;
        }
        node(std::vector<node*> ch, node* par = NULL) {
            children = ch;
            min_val = ch[0]->min_val;
            max_val = ch.back()->max_val;
            for (auto i : ch) i->parent = this;
            parent = par;
            ++cnt;
        }
        void relax() {
            if (children.empty()) return;
            min_val = children[0]->min_val;
            max_val = children.back()->max_val;
        }
        static void move_left(node* x, node* y) {
            x->children.push_back(y->children[0]);
            x->max_val = x->children.back()->max_val;
            x->children.back()->parent = x;
            y->children.erase(y->children.begin());
            y->min_val = y->children[0]->min_val;
        }
        static void move_right(node* x, node* y) {
            y->children.insert(y->children.begin(), x->children.back());
            y->min_val = y->children[0]->min_val;
            y->children[0]->parent = y;
            x->children.pop_back();
            x->max_val = x->children.back()->max_val;
        }
        static void move_left_simple(node* x, node* y) {
            x->children.push_back(y->children[0]);
            x->children.back()->parent = x;
            y->children.erase(y->children.begin());
        }
        static void move_right_simple(node* x, node* y) {
            y->children.insert(y->children.begin(), x->children.back());
            y->children[0]->parent = y;
            x->children.pop_back();
        }
        void reshuffle() {
            if (children.size() == 0 || children[0]->children.size() == 0) return;
            int sons = children.size(), grandsons = 0;
            for (auto& son : children) grandsons += son->children.size();
            if (sons == 2) {
                if (grandsons == 3) {
                    if (children[0]->children.size() == 1) {
                        move_right_simple(children[0], children[1]);
                        delete children[0];
                        children.erase(children.begin());
                    } else {
                        move_left_simple(children[0], children[1]);
                        delete children[1];
                        children.pop_back();
                    }
                    children[0]->relax();
                } else if (grandsons == 7) {
                    if (children[0]->children.size() == 4) move_right(children[0], children[1]);
                    children.push_back(new node({children[1]->children[2], children[1]->children[3]}, this));
                    children[1]->children.pop_back();
                    children[1]->children.pop_back();
                    children[1]->relax();
                } else {
                    if (children[0]->children.size() < children[1]->children.size()) move_left(children[0], children[1]);
                    else move_right(children[0], children[1]);
                }
                relax();
                return;
            } else if (sons == 3) {
                if (grandsons == 10) {
                    for (int i = 0; i < sons; ++i) {
                        if (children[i]->children.size() == 4) {
                            node* new_child = new node({children[i]->children[0], children[i]->children[1]}, this);
                            children[i]->children.erase(children[i]->children.begin());
                            children[i]->children.erase(children[i]->children.begin());
                            children[i]->relax();
                            children.insert(children.begin() + i, new_child);
                            break;
                        }
                    }
                    relax();
                    return;
                }
            }
            for (int i = 0; i < children.size(); ++i) {
                if (children[i]->children.size() == 1) {
                    if (i > 0 && children[i - 1]->children.size() == 3) move_right(children[i - 1], children[i]);
                    if (i + 1 < sons && children[i + 1]->children.size() == 3) move_left(children[i], children[i + 1]);
                    else continue;
                } else if (children[i]->children.size() == 4) {
                    if (i > 0 && children[i - 1]->children.size() == 2) move_left(children[i - 1], children[i]);
                    else if (i + 1 < sons && children[i + 1]->children.size() == 2) move_right(children[i], children[i + 1]);
                    else continue;
                } else continue;
                return;
            }
            std::vector<node*> all;
            for (auto& son : children) {
                for (auto grandson : son->children) {
                    all.push_back(grandson);
                }
                son->children.clear();
            }
            int remaining;
            if (all.size() == 3) remaining = 1;
            else if (all.size() <= 5) remaining = 2;
            else if (all.size() <= 9) remaining = 3;
            else remaining = 4;
            int now_pos = 0;
            int now_son = 0;
            while (remaining) {
                int cnt = now_pos + remaining * 2 < all.size() ? 3 : 2;
                assert(now_pos + cnt <= all.size());
                if (now_son < children.size()) {
                    children[now_son]->children = slice(all, now_pos, cnt);
                    for (auto grandson : children[now_son]->children) grandson->parent = children[now_son];
                    children[now_son]->relax();
                    ++now_son;
                } else {
                    children.push_back(new node(slice(all, now_pos, cnt), this));
                    now_son = 179;
                }
                now_pos += cnt;
                remaining--;
            }
            while (now_son < children.size()) {
                delete children.back();
                children.pop_back();
            }
            relax();
        }
        void print() {
            std::cout << children.size() << " sons, " << *(min_val) << " " << *(max_val) << std::endl;
            for (auto i : children) i->print();
        }
        bool vibe_check() {
            for (auto i : children) {
                if (i->parent != this) return true;
                if (i->vibe_check()) return true;
                int j = i->children.size();
                if (j != 0 && j != 2 && j != 3) return true;
            }
            return false;
        }
        ~node() {
            --cnt;
            for (auto i : children) delete i;
        }
    };
    node* root = NULL;
    std::list<T> data_;

}; 
