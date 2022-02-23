#pragma once
#include <initializer_list>

template<class T> class Set {
public:
    enum class Color {
        black,
        red,
    };

    struct Node {
        Node() = default;
        Node(T value): value_(value) {};
        Node(const Node& node) {
            this->color_ = node.color_;
            this->value_ = node.value_;
        }

        Color color_ = Color::red;
        T value_{};
        Node* left_son_ = nullptr;
        Node* right_son_ = nullptr;
        Node* parent_ = nullptr;
    };

    class iterator {
    public:
        iterator() = default;
        explicit iterator(const Set* object): object_(object) {};
        iterator(const Set* object, const Node* to): to_(to), object_(object) {};

        const T& operator*() {
            return to_->value_;
        }

        const T* operator->() {
            return &to_->value_;
        }

        iterator& operator++() {
            if (to_->right_son_) {
                to_ = to_->right_son_;
                while (to_->left_son_) {
                    to_ = to_->left_son_;
                }
            }
            else {
                while (to_->parent_ && to_ == to_->parent_->right_son_) {
                    to_ = to_->parent_;
                }
                to_ = to_->parent_;
            }

            return *this;
        }

        iterator operator++(int) {
            iterator old = *this;
            operator++();
            return old;
        }

        iterator& operator --() {
            if (to_ == nullptr) {
                to_ = object_->root_;
                while (to_->right_son_) {
                    to_ = to_->right_son_;
                }
            }
            else {
                if (to_->left_son_) {
                    to_ = to_->left_son_;
                    while (to_->right_son_) {
                        to_ = to_->right_son_;
                    }
                }
                else {
                    while (to_->parent_ && to_ == to_->parent_->left_son_) {
                        to_ = to_->parent_;
                    }
                    to_ = to_->parent_;
                }
            }

            return *this;
        }

        iterator operator--(int) {
            iterator old = *this;
            operator--();
            return old;
        }

        bool operator == (const iterator& it) {
            return it.object_ == this->object_ && it.to_ == this->to_;
        }

        bool operator != (const iterator& it) {
            return it.object_ != this->object_ || it.to_ != this->to_;
        }

    private:
        const Node* to_ = nullptr;
        const Set* object_ = nullptr;
    };

    iterator begin() const {
        Node* v = root_;

        if (v) {
            while (v->left_son_) {
                v = v->left_son_;
            }
        }

        return iterator(this, v);
    }

    iterator end() const {
        return iterator(this, nullptr);
    }

    ~Set() {
        Node* v = root_;

        while (v != nullptr) {
            if (v->left_son_) {
                v = v->left_son_;
            }
            else if (v->right_son_) {
                v = v->right_son_;
            }
            else {
                if (v->parent_ == nullptr) {
                    delete v;
                    break;
                }
                v = v->parent_;
                if (v->left_son_) {
                    delete v->left_son_;
                    v->left_son_ = nullptr;
                }
                else {
                    delete v->right_son_;
                    v->right_son_ = nullptr;
                }
            }
        }

        size_ = 0;
        root_ = nullptr;
    }


    Set() {};

    template<typename Iterator>
    Set(Iterator first, Iterator last) : Set() {
        for (Iterator it = first; it != last; ++it) {
            insert(*it);
        }
    }

    Set(std::initializer_list<T> elems) : Set() {
        for (T x : elems) {
            insert(x);
        }
    }

    Set(const Set& set) : Set() {
        if (!set.root_) {
            return;
        }
        this->root_ = new Node(*set.root_);
        this->size_ = set.size_;

        Node *this_v = root_;
        Node *set_v = set.root_;

        while (this_v) {
            if (set_v->left_son_ && !this_v->left_son_) {
                this_v->left_son_ = new Node(*set_v->left_son_);
                this_v->left_son_->parent_ = this_v;

                this_v = this_v->left_son_;
                set_v = set_v->left_son_;
            }
            else if (set_v->right_son_ && !this_v->right_son_) {
                this_v->right_son_ = new Node(*set_v->right_son_);
                this_v->right_son_->parent_ = this_v;

                this_v = this_v->right_son_;
                set_v = set_v->right_son_;
            }
            else {
                set_v = set_v->parent_;
                this_v = this_v->parent_;
            }
        }
    }

    Set& operator=(const Set& set) {
        if (this == &set) {
            return *this;
        }

        Node* v = root_;

        while (v != nullptr) {
            if (v->left_son_) {
                v = v->left_son_;
            }
            else if (v->right_son_) {
                v = v->right_son_;
            }
            else {
                if (v->parent_ == nullptr) {
                    delete v;
                    break;
                }
                v = v->parent_;
                if (v->left_son_) {
                    delete v->left_son_;
                    v->left_son_ = nullptr;
                }
                else {
                    delete v->right_son_;
                    v->right_son_ = nullptr;
                }
            }
        }

        size_ = set.size_;
        root_ = nullptr;

        if (!set.root_) {
            return *this;
        }
        root_ = new Node(*set.root_);

        Node *this_v = root_;
        Node *set_v = set.root_;

        while (this_v) {
            if (set_v->left_son_ && !this_v->left_son_) {
                this_v->left_son_ = new Node(*set_v->left_son_);
                this_v->left_son_->parent_ = this_v;

                this_v = this_v->left_son_;
                set_v = set_v->left_son_;
            }
            else if (set_v->right_son_ && !this_v->right_son_) {
                this_v->right_son_ = new Node(*set_v->right_son_);
                this_v->right_son_->parent_ = this_v;

                this_v = this_v->right_son_;
                set_v = set_v->right_son_;
            }
            else {
                set_v = set_v->parent_;
                this_v = this_v->parent_;
            }
        }

        return *this;
    }

    std::size_t size() const {
        return size_;
    }

    bool empty() const {
        return size_ == 0;
    }

    void insert(const T& elem) {
        if (!root_) {
            root_ = new Node(elem);
            balanceInsert(root_);
            ++size_;
        }
        else {
            Node* v = root_;
            while ((elem < v->value_ && v->left_son_) || (v->value_ < elem && v->right_son_)) {
                if (elem < v->value_) {
                    v = v->left_son_;
                }
                else {
                    v = v->right_son_;
                }
            }
            if (elem < v->value_) {
                v->left_son_ = new Node(elem);
                v->left_son_->parent_ = v;
                balanceInsert(v->left_son_);
                ++size_;
            }
            else if (v->value_ < elem) {
                v->right_son_ = new Node(elem);
                v->right_son_->parent_ = v;
                balanceInsert(v->right_son_);
                ++size_;
            }
        }
    }

    void erase(const T& elem) {
        Node* v = root_;

        while (v && (elem < v->value_ || v->value_ < elem)) {
            if (elem < v->value_) {
                v = v->left_son_;
            }
            else {
                v = v->right_son_;
            }
        }

        if (!v) {
            return;
        }
        else {
            --size_;

            if (v->left_son_ && v->right_son_) {
                Node* next_value = v->right_son_;

                while (next_value->left_son_) {
                    next_value = next_value->left_son_;
                }

                v->value_ = next_value->value_;
                v = next_value;
            }

            Node* new_son = (v->left_son_ ? v->left_son_ : v->right_son_);

            if (new_son) {
                new_son->parent_ = v->parent_;
            }
            if (v->parent_) {
                if (v == v->parent_->left_son_) {
                    v->parent_->left_son_ = new_son;
                }
                else {
                    v->parent_->right_son_ = new_son;
                }
            }
            else {
                root_ = new_son;
            }

            if (v->color_ == Color::black) {
                balanceErase(v->parent_, new_son);
            }

            delete v;
        }
    }

    iterator find(const T& elem) const {
        iterator it = lower_bound(elem);

        if (it == this->end() || elem < *it) {
            return iterator(this, nullptr);
        }
        else {
            return it;
        }
    }

    iterator lower_bound(const T& elem) const {
        Node *v = root_;
        Node *p = nullptr;

        while (v) {
            if (!(v->value_ < elem)) {
                p = v;
            }
            if (v->value_ < elem) {
                v = v->right_son_;
            }
            else {
                v = v->left_son_;
            }
        }

        return iterator(this, p);
    }

private:
    Node* root_ = nullptr;
    std::size_t size_ = 0;

    Node* getBrother(Node* v, Node* p) {
        if (!p) {
            return nullptr;
        }
        else if (v == p->left_son_) {
            return p->right_son_;
        }
        else {
            return p->left_son_;
        }
    }

    Node* getGrand(Node* v) {
        if (!v || !v->parent_ || !v->parent_->parent_) {
            return nullptr;
        }
        else {
            return v->parent_->parent_;
        }
    }

    void leftRotate(Node* v) {
        if (!v || !v->right_son_) {
            return;
        }

        Node* right_son = v->right_son_;

        v->right_son_ = right_son->left_son_;
        if (v->right_son_) {
            v->right_son_->parent_ = v;
        }

        right_son->parent_ = v->parent_;
        if (v->parent_) {
            if (v == v->parent_->left_son_) {
                v->parent_->left_son_ = right_son;
            }
            else {
                v->parent_->right_son_ = right_son;
            }
        }

        right_son->left_son_ = v;
        v->parent_ = right_son;

        if (!right_son->parent_) {
            root_ = right_son;
        }
    }

    void rightRotate(Node* v) {
        if (!v || !v->left_son_) {
            return;
        }

        Node* left_son = v->left_son_;

        v->left_son_ = left_son->right_son_;
        if (v->left_son_) {
            v->left_son_->parent_ = v;
        }

        left_son->parent_ = v->parent_;
        if (v->parent_) {
            if (v == v->parent_->left_son_) {
                v->parent_->left_son_ = left_son;
            }
            else {
                v->parent_->right_son_ = left_son;
            }
        }

        left_son->right_son_ = v;
        v->parent_ = left_son;

        if (!left_son->parent_) {
            root_ = left_son;
        }
    }

    void balanceInsert(Node* v) {
        while (getGrand(v) && v->parent_->color_ == Color::red) {
            Node* uncle = getBrother(v->parent_, getGrand(v));

            if (uncle && uncle->color_ == Color::red) {
                v = getGrand(v);

                v->left_son_->color_ = Color::black;
                v->right_son_->color_ = Color::black;

                v->color_ = Color::red;
            }
            else {
                if (v->parent_->value_ < getGrand(v)->value_) {
                    if (v->parent_->value_ < v->value_) {
                        v = v->parent_;
                        leftRotate(v);
                    }
                    v->parent_->color_ = Color::black;
                    getGrand(v)->color_ = Color::red;
                    rightRotate(getGrand(v));
                }
                else {
                    if (v->value_ < v->parent_->value_) {
                        v = v->parent_;
                        rightRotate(v);
                    }
                    v->parent_->color_ = Color::black;
                    getGrand(v)->color_ = Color::red;
                    leftRotate(getGrand(v));
                }
            }
        }

        root_->color_ = Color::black;
    }

    void balanceErase(Node* p, Node* v) {
        if (!root_) {
            return;
        }

        while (!v || (v->color_ == Color::black && v != root_)) {
            Node* brother = getBrother(v, p);

            if (v == p->left_son_) {
                if (brother->color_ == Color::red) {
                    brother->color_ = Color::black;
                    p->color_ = Color::red;

                    leftRotate(p);

                    brother = getBrother(v, p);
                }
                if ((!brother->left_son_ || brother->left_son_->color_ == Color::black)
                    && (!brother->right_son_ || brother->right_son_->color_ == Color::black)) {
                    brother->color_ = Color::red;

                    v = p;
                    p = p->parent_;
                }
                else {
                    if (!brother->right_son_ || brother->right_son_->color_ == Color::black) {
                        if (brother->left_son_) {
                            brother->left_son_->color_ = Color::black;
                        }
                        brother->color_ = Color::red;

                        rightRotate(brother);

                        brother = getBrother(v, p);
                    }

                    brother->color_ = p->color_;
                    p->color_ = Color::black;
                    if (brother->right_son_) {
                        brother->right_son_->color_ = Color::black;
                    }

                    leftRotate(p);

                    v = root_;
                }
            }
            else {
                if (brother->color_ == Color::red) {
                    brother->color_ = Color::black;
                    p->color_ = Color::red;

                    rightRotate(p);

                    brother = getBrother(v, p);
                }

                if ((!brother->left_son_ || brother->left_son_->color_ == Color::black)
                    && (!brother->right_son_ || brother->right_son_->color_ == Color::black)) {
                    brother->color_ = Color::red;

                    v = p;
                    p = p->parent_;
                }
                else {
                    if (!brother->left_son_ || brother->left_son_->color_ == Color::black) {
                        if (brother->right_son_) {
                            brother->right_son_->color_ = Color::black;
                        }
                        brother->color_ = Color::red;

                        leftRotate(brother);

                        brother = getBrother(v, p);
                    }

                    brother->color_ = p->color_;
                    p->color_ = Color::black;
                    if (brother->left_son_) {
                        brother->left_son_->color_ = Color::black;
                    }

                    rightRotate(p);

                    v = root_;
                }
            }
        }

        v->color_ = Color::black;
        root_->color_ = Color::black;
    }
};
