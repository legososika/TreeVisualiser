#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <algorithm>
#include <cmath>
#include <iostream>
#include <queue>
#include <vector>

struct Node {
  Node* left = nullptr;
  Node* right = nullptr;
  Node* prev = nullptr;
  int key = 0;
  int priority = 0;
  int color = 0;
  int height = 1;
  int size = 1;
};

void Swap(Node*& a, Node*& b) {}

int Size(Node* treap) { return treap == nullptr ? 0 : treap->size; }

int Recalc(Node* treap) { return Size(treap->left) + Size(treap->right) + 1; }

Node* Create(int key, int color = 0) {
  Node* p = new Node;
  p->key = key;
  p->color = color;
  p->priority = std::rand() % 1000000000;
  return p;
}

bool CheckColor(Node* p) {
  if (p == nullptr) return true;
  if (p->color == 0) return true;
  if (p->color == 1) return false;
}

struct Avl {
  Node* root = nullptr;

  Node* find(int key, Node* search) {
    if (search == nullptr) {
      return nullptr;
    }
    if (search->key == key) {
      return search;
    }
    if (key > search->key) {
      return find(key, search->right);
    }
    if (key < search->key) {
      return find(key, search->left);
    }
  }

  void fixHeight(Node* p) {
    if (p == nullptr) {
      return;
    }
    int left = 0, right = 0;
    if (p->left != nullptr) {
      left = p->left->height;
      p->left->prev = p;
    }
    if (p->right != nullptr) {
      right = p->right->height;
      p->right->prev = p;
    }
    p->height = std::max(left, right) + 1;
  }

  int dif(Node* p) {
    if (p == nullptr) {
      return 0;
    }
    int left = 0, right = 0;
    if (p->left != nullptr) {
      left = p->left->height;
    }
    if (p->right != nullptr) {
      right = p->right->height;
    }
    return right - left;
  }

  void rotateSmall(Node*& p, bool direction) {
    if (p == nullptr) {
      return;
    }
    Node* q;
    if (!direction) {
      q = p->right;
      p->right = q->left;
      q->left = p;
    } else {
      q = p->left;
      p->left = q->right;
      q->right = p;
    }
    fixHeight(p);
    fixHeight(q);
    p = q;
  }

  void rotateBig(Node*& p, bool direction) {
    if (p == nullptr) {
      return;
    }
    Node *q, *r;
    if (!direction) {
      q = p->right;
      r = q->left;
      q->left = r->right;
      p->right = r->left;
      r->left = p;
      r->right = q;
    } else {
      q = p->left;
      r = q->right;
      q->right = r->left;
      p->left = r->right;
      r->right = p;
      r->left = q;
    }
    fixHeight(p);
    fixHeight(q);
    fixHeight(r);
    p = r;
  }

  void balance(Node*& p) {
    if (p == nullptr) {
      return;
    }
    fixHeight(p);
    if (dif(p) == 2) {
      if (dif(p->right) < 0) {
          rotateBig(p, false);
          return;
      }
      rotateSmall(p, false);
      
    }
    if (dif(p) == -2) {
      if (dif(p->left) > 0) {
          rotateBig(p, true);
          return;
      }
      rotateSmall(p, true);
    }
    return;
  }

  bool insert(int key, Node*& p) {
      if (find(key, root) != nullptr) {
          return false;
      }
      if (p == nullptr) {
          p = Create(key);
          if (root == nullptr) {
              root = p;
          }
          balance(p);
          return true;
      }
      if (key < p->key) {
          insert(key, p->left);
      }
      else {
          insert(key, p->right);
      }
      balance(p);
      return true;
  }

  Node* findMin(Node* search) {
    if (search == nullptr) {
        return nullptr;
    }
    if (search->left == nullptr) {
      return search;
    }
    return findMin(search->left);
  }

  void removeMin(Node*& search) {
    if (search == nullptr) {
      return;
    }
    if (search->left == nullptr) {
      Node* toDelete = search;
      search = search->right;
      delete toDelete;
      return;
    }
    removeMin(search->left);
    balance(search);
  }

  bool remove(int key, Node*& p) {
    if (p == nullptr || find(key, root) == nullptr) {
        return false;
    }
    if (key < p->key) {
        remove(key, p->left);
    }
    else if (key > p->key) {
        remove(key, p->right);
    }
    else {
        Node* left = p->left;
        if (p->right == nullptr) {
            if (left == nullptr) {
                Node* toDelete = p;
                p = nullptr;
                delete toDelete;
                return true;
            }
            else {
                Node* toDelete = p;
                p = p->left;
                delete toDelete;
                return true;
            }
        }
        Node* min = findMin(p->right);
        p->key = min->key;
        removeMin(p->right);
        balance(p);
        return true;
    }
    balance(p);
    return true;
  }
};

struct Treap {
    Node* root = nullptr;
    Node* cur = nullptr;
    Node* splitLeft = nullptr, * splitRight = nullptr;

    Node* find(int key, Node* search) {
        if (search == nullptr) {
            return nullptr;
        }
        if (search->key == key) {
            return search;
        }
        if (key > search->key) {
            return find(key, search->right);
        }
        if (key < search->key) {
            return find(key, search->left);
        }
    }

    void split(int key, Node* treap, Node*& left, Node*& right) {
        Node* newNode = nullptr;
        if (treap == nullptr) {
            left = nullptr;
            right = nullptr;
            return;
        }
        if (treap->key < key) {
            if (treap->right == nullptr) {
                right = nullptr;
                left = treap;
            }
            else {
                split(key, treap->right, newNode, right);
                left = treap;
                left->right = newNode;
            }
        }
        else {
            if (treap->left == nullptr) {
                left = nullptr;
                right = treap;
            }
            else {
                split(key, treap->left, left, newNode);
                right = treap;
                right->left = newNode;
            }
        }
    }

    Node* merge(Node* left, Node* right) {
        if (left == nullptr) {
            return right;
        }
        if (right == nullptr) {
            return left;
        }
        if (right->priority > left->priority) {
            Node* newNode = merge(left->right, right);
            left->right = newNode;
            return left;
        }
        else {
            Node* newNode = merge(left, right->left);
            right->left = newNode;
            return right;
        }
    }

    Node* insert(int key) {
        if (find(key, root) != nullptr) {
            return nullptr;
        }
        Node* node = Create(key);
        if (root == nullptr) {
            root = node;
            return root;
        }
        Node* left, * right;
        split(key, root, left, right);
        left = merge(left, node);
        root = merge(left, right);
        return root;
    }

    void remove(int key) {
        if (find(key, root) == nullptr) {
            return;
        }
        Node* left, * right2, * right, * toDelete;
        split(key, root, left, right);
        split(key + 1, right, toDelete, right2);
        if (right2 == nullptr) {
            if (left == nullptr) {
                delete toDelete;
                root = nullptr;
                return;
            }
            else {
                root = left;
                return;
            }
        }
        delete toDelete;
        root = merge(left, right2);
    }
};

struct Splay {
    Node* root = nullptr;

    Node* find(int key, Node* search) {
        if (search == nullptr) {
            return nullptr;
        }
        if (search->key == key) {
            return splay(search);
        }
        if (key > search->key && search->right != nullptr) {
            return find(key, search->right);
        }
        if (key < search->key && search->left != nullptr) {
            return find(key, search->left);
        }
        return splay(search);
    }

    void rotate(Node* par, Node* chld) {
        Node* gpa = par->prev;
        if (gpa != nullptr) {
            if (gpa->left == par) {
                gpa->left = chld;
            }
            else {
                gpa->right = chld;
            }
        }
        if (par->left == chld) {
            par->left = chld->right;
            if (chld->right != nullptr) {
                chld->right->prev = par;
            }
            chld->right = par;
            par->prev = chld;
        }
        else {
            par->right = chld->left;
            if (chld->left != nullptr) {
                chld->left->prev = par;
            }
            chld->left = par;
            par->prev = chld;
        }
        chld->prev = gpa;
    }

    Node* splay(Node* p) {
        if (p->prev == nullptr) {
            return p;
        }
        Node* par = p->prev;
        Node* gpa = p->prev->prev;
        if (gpa == nullptr) {
            rotate(par, p);
            return p;
        }
        else {
            if (gpa->left == par && par->left == p) {
                rotate(gpa, par);
                rotate(par, p);
            }
            else {
                rotate(par, p);
                rotate(gpa, p);
            }
            return splay(p);
        }
    }

    void split(Node*& p, Node*& left, Node*& right, int key) {
        if (p == nullptr) {
            left = nullptr;
            right = nullptr;
            return;
        }
        p = find(key, p);
        if (p->key == key) {
            if (p->left != nullptr) {
                p->left->prev = nullptr;
            }
            if (p->right != nullptr) {
                p->right->prev = nullptr;
            }
            right = p->right;
            left = p->left;
            return;
        }
        if (p->key < key) {
            right = p->right;
            p->right = nullptr;
            if (right != nullptr) {
                right->prev = nullptr;
            }
            left = p;
            p = nullptr;
            return;
        }
        if (p->key > key) {
            left = p->left;
            p->left = nullptr;
            if (left != nullptr) {
                left->prev = nullptr;
            }
            right = p;
            p = nullptr;
            return;
        }
    }

    void insert(int key) {
        Node* left, * right;
        split(root, left, right, key);
        if (root == nullptr) {
            root = Create(key);
        }
        root->left = left;
        root->right = right;
        if (left != nullptr) {
            left->prev = root;
        }
        if (right != nullptr) {
            right->prev = root;
        }
    }

    Node* merge(Node* left, Node* right) {
        if (left == nullptr) return right;
        if (right == nullptr) return left;
        right = find(left->key, right);
        right->left = left;
        left->prev = right;
        return right;
    }

    bool remove(int key) {
        root = find(key, root);
        if (root == nullptr) {
            return false;
        }
        if (root->key != key) {
            return false;
        }
        Node* left = root->left, * right = root->right;
        if (left != nullptr) {
            left->prev = nullptr;
        }
        if (right != nullptr) {
            right->prev = nullptr;
        }
        root = merge(left, right);
        return true;
    }
};

struct RedBlack {
    Node* root = nullptr;

    void insert(int key) {
        if (find(key, root) != nullptr) {
            return;
        }
        Node* p = Create(key, 1);
        if (root == nullptr) {
            root = p;
            p->color = 0;
            return;
        }
        place(p, root);
        balance(p);
    }

    Node* find(int key, Node* search) {
        while (search != nullptr) {
            if (search->key == key) {
                return search;
            }
            if (key > search->key) {
                search = search->right;
            }
            else {
                search = search->left;
            }
        }
        return nullptr;
    }

    void place(Node* q, Node* parent) {
        if (q->key > parent->key) {
            if (parent->right == nullptr) {
                parent->right = q;
                q->prev = parent;
                return;
            }
            place(q, parent->right);
        }
        else {
            if (parent->left == nullptr) {
                parent->left = q;
                q->prev = parent;
                return;
            }
            place(q, parent->left);
        }
    }

    void balance(Node* p) {
        while (p != root) {
            if (p->prev == nullptr) {
                p->color = 0;
                return;
            }
            if (p->prev->color == 0) return;
            Node* par = p->prev, * gpa = p->prev->prev;
            Node* bro = (par == gpa->left ? gpa->right : gpa->left);
            if (!CheckColor(bro)) {
                par->color = 0;
                bro->color = 0;
                gpa->color = 1;
                p = gpa;
                continue;
            }
            if (CheckColor(bro)) {
                if (p == par->right && par == gpa->left) {
                    p->prev = gpa;
                    par->prev = p;
                    if (p->left != nullptr) {
                        p->left->prev = par;
                    }
                    par->right = p->left;
                    p->left = par;
                    gpa->left = p;
                    std::swap(par, p);
                }
                else if (p == par->left && par == gpa->right) {
                    p->prev = gpa;
                    par->prev = p;
                    if (p->right != nullptr) {
                        p->right->prev = par;
                    }
                    par->left = p->right;
                    p->right = par;
                    gpa->right = p;
                    std::swap(par, p);
                }
                if (gpa->left == par) {
                    if (gpa->prev == nullptr) {
                        root = par;
                    }
                    else {
                        if (gpa->prev->left == gpa) {
                            gpa->prev->left = par;
                        }
                        else {
                            gpa->prev->right = par;
                        }
                    }
                    par->prev = gpa->prev;
                    gpa->prev = par;
                    gpa->left = par->right;
                    if (par->right != nullptr) {
                        par->right->prev = gpa;
                    }
                    par->right = gpa;
                }
                else {
                    if (gpa->prev == nullptr) {
                        root = par;
                    }
                    else {
                        if (gpa->prev->left == gpa) {
                            gpa->prev->left = par;
                        }
                        else {
                            gpa->prev->right = par;
                        }
                    }
                    par->prev = gpa->prev;
                    gpa->prev = par;
                    gpa->right = par->left;
                    if (par->left != nullptr) {
                        par->left->prev = gpa;
                    }
                    par->left = gpa;
                }
                par->color = 0;
                gpa->color = 1;
                return;
            }
        }
        if (p == root) {
            p->color = 0;
        }
    }

    bool erase(int key) {
        Node* q = find(key, root);
        if (q == nullptr) {
            return false;
        }
        Node* min = findMin(q->right);
        if (min == nullptr) {
            min = findMax(q->left);
            if (min == nullptr) {
                min = q;
            }
        }
        if (!CheckColor(min)) {
            q->key = min->key;
            if (min->prev->left == min) {
                min->prev->left = nullptr;
            }
            else {
                min->prev->right = nullptr;
            }
            delete min;
            return true;
        }
        else {
            q->key = min->key;
            if (min->right != nullptr) {
                min->key = min->right->key;
                delete min->right;
                min->right = nullptr;
                return true;
            }
            else if (min->left != nullptr) {
                min->key = min->left->key;
                delete min->left;
                min->left = nullptr;
                return true;
            }
            if (min->prev == nullptr) {
                delete min;
                root = nullptr;
                return true;
            }
            rbalance(min, min->prev,
                (min == min->prev->left ? min->prev->right : min->prev->left));
            if (min->prev->left == min) {
                min->prev->left = nullptr;
            }
            else {
                min->prev->right = nullptr;
            }
            delete min;
        }
        // if (min == nullptr) min = q;
        /*
        q->key = min->key;
        if (!CheckColor(min)) {
          if (min->prev->left == min) {
            min->prev->left = nullptr;
          } else {
            min->prev->right = nullptr;
          }
          delete min;
          return true;
        }
        if (!CheckColor(min->right)) {
          if (min->right != nullptr) {
            min->right->prev = q->prev;
            if (min->prev->left == min) {
              min->prev->left = min->right;
            } else {
              min->prev->right = min->right;
            }
            min->left->color = 0;
            delete min;
            return true;
          }
        }

        Node* toDelete = min;
        if (min->prev == nullptr) {
          root = nullptr;
          delete min;
          return true;
        }
        if (min->prev->left == min) {
          min->prev->left = min->right;
        } else {
          min->prev->right = min->right;
        }
        min = min->right;
        delete toDelete;
        rbalance(min);
        return true;
        */
    }

    void rbalance(Node* p, Node* par, Node* bro) {
        
        while (p != root) {
            if (bro == nullptr) {
                par = p->prev;
                bro = (par == p->prev->left ? p->prev->right : p->prev->left);
            }
            Node* d1, * d2;
            if (p == par->left) {
                d1 = bro->left;
                d2 = bro->right;
            }
            else {
                d1 = bro->right;
                d2 = bro->left;
            }
            if (CheckColor(par) && CheckColor(bro) && CheckColor(d1) &&
                CheckColor(d2)) {
                bro->color = 1;
                if (par->prev == nullptr) {
                    return;
                }
                p = par;
                par = par->prev;
                bro = (p == par->right ? par->left : par->right);
                continue;
            }
            if (!CheckColor(par) && CheckColor(bro) && CheckColor(d1) &&
                CheckColor(d2)) {
                par->color = 0;
                bro->color = 1;
                return;
            }
            if (CheckColor(bro) && !CheckColor(d2)) {
                if (par->prev != nullptr) {
                    if (par->prev->right == par) {
                        par->prev->right = bro;
                    }
                    else {
                        par->prev->left = bro;
                    }
                }
                else {
                    root = bro;
                }
                bro->prev = par->prev;
                d2->color = 0;
                if (p == par->left) {
                    par->right = d1;
                    bro->left = par;
                }
                else {
                    par->left = d1;
                    bro->right = par;
                }
                if (d1 != nullptr) {
                    d1->prev = par;
                }   
                par->prev = bro;
                bro->color = par->color;
                par->color = 0;
                return;
            }
            if (CheckColor(bro) && !CheckColor(d1) && CheckColor(d2)) {
                if (p == par->left) {
                    par->right = d1;
                    bro->left = d1->right;
                    d1->right = bro;
                    par->right = d1;
                }
                else {
                    par->left = d1;
                    bro->right = d1->left;
                    d1->left = bro;
                    par->left = d1;
                }
                d1->prev = par;
                bro->prev = d1;
                d1->color = 0;
                bro->color = 1;
                bro = d1;
                continue;
            }
            if (!CheckColor(bro)) {
                if (par->prev != nullptr) {
                    if (par->prev->right == par) {
                        par->prev->right = bro;
                    }
                    else {
                        par->prev->left = bro;
                    }
                }
                else {
                    root = bro;
                }
                bro->prev = par->prev;
                par->prev = bro;
                if (p == par->left) {
                    par->right = d1;
                    bro->left = par;
                }
                else {
                    par->left = d1;
                    bro->right = par;
                }
                if (d1 != nullptr) {
                    d1->prev = par;
                }
                par->color = 1;
                bro->color = 0;
                bro = d1;
                continue;
            }
            return;
        }
    }

    Node* findMin(Node* p) {
        if (p == nullptr) {
            return nullptr;
        }
        if (p->left == nullptr) {
            return p;
        }
        return findMin(p->left);
    }

    Node* findMax(Node* p) {
        if (p == nullptr) {
            return nullptr;
        }
        if (p->right == nullptr) {
            return p;
        }
        return findMin(p->right);
    }
};

void InOrder(Node* p, int layer = 0) {
  if (p == nullptr) return;
  InOrder(p->left, layer + 1);
  std::cout << p->key << " ";
  InOrder(p->right, layer + 1);
}

void TreeOrder(Node* p) {
  int layer = 0;
  std::queue<std::pair<Node*, int>> queue;
  std::pair<Node*, int> blank;
  if (p != nullptr) {
    blank = {p, layer};
    queue.push(blank);
  }
  while (!queue.empty()) {
    if (queue.front().second > layer) {
      std::cout << "\n";
      ++layer;
    }
    Node* q = queue.front().first;
    std::cout << q->key << " ";
    queue.pop();
    if (q->left != nullptr) {
      blank.second = layer + 1;
      blank.first = q->left;
      queue.push(blank);
    }
    if (q->right != nullptr) {
      blank.second = layer + 1;
      blank.first = q->right;
      queue.push(blank);
    }
  }
}

sf::Font* font;

sf::Color textColor = sf::Color(232, 197, 71),
          backgroundColor = sf::Color(34, 40, 49),
          elementColor = sf::Color(254, 233, 225),
          fillColor = sf::Color(57, 62, 70),
          reactColor = sf::Color(217, 3, 104),
          redFillColor = sf::Color(157, 62, 70),
          touchColor = sf::Color::White;

struct RenderNode {
    sf::RectangleShape rect;
    sf::Text text;
    sf::Text priority;
    int height = 0;
    Node* node = nullptr;
    int key = 0;
    sf::Vector2f scale;
    RenderNode* left = nullptr, * right = nullptr;
    int level;
    int col;

    void create(Node* node, int key, int height, bool treap, float scale = 1) {
        this->node = node;
        this->key = key;
        this->text.setString(std::to_string(key));
        if (treap) {
            priority.setString(std::to_string(node->priority));
        }
        //this->height = height;
        this->level = height;

        sf::Vector2f sc = { scale, scale };
        this->scale = sc;

        rect.setSize(sf::Vector2f(150, 60));

        // text.setOrigin(text.getGlobalBounds().width / 2,
        // text.getGlobalBounds().height / 2);
        rect.setOrigin(75, 30);
        text.setOrigin(75, 20);
        priority.setOrigin(75, -20);

        text.setFont(*font);
        priority.setFont(*font);
        rect.setOutlineThickness(1);

        if (node->color) {
            rect.setFillColor(redFillColor);
        }
        else {
            rect.setFillColor(fillColor);
        }
        text.setFillColor(textColor);
        priority.setFillColor(reactColor);
        rect.setOutlineColor(elementColor);

        text.setScale(this->scale);
        priority.setScale(this->scale);
        rect.setScale(this->scale);
    }

    void grow(sf::Vector2f scale) {
        this->scale = scale;
        text.setScale(scale);
        rect.setScale(scale);
    }

    void setPos(sf::Vector2f pos) {
        rect.setPosition(pos);
        text.setPosition(pos);
        priority.setPosition(pos);
    }

    void draw(sf::RenderWindow& window) {
        window.draw(rect);
        window.draw(text);
        window.draw(priority);
    }

    bool contains(float x, float y) {
        return rect.getGlobalBounds().contains(x, y);
    }
};

struct RenderTree {
    std::vector<sf::Vertex> connections;
    RenderNode* root = nullptr;
    int height;
    /*
    void create(Node* tree, sf::Vector2f pos, float scale) {
      int height = 0;
      RenderNode* rend = createNodes(tree, height, scale);
      root = rend;
      positionNodes(rend, pos, height);
      this->height = height;
    }

    void positionNodes(RenderNode* rend, sf::Vector2f pos, int height) {
      if (rend == nullptr) return;
      rend->setPos(pos);
      positionNodes(rend->left,
                    {pos.x - 80 * (int)pow(2, height - 2) * rend->scale.x,
                     pos.y + 200 * rend->scale.x},
                    height - 1);
      positionNodes(rend->right,
                    {pos.x + 80 * (int)pow(2, height - 2) * rend->scale.x,
                     pos.y + 200 * rend->scale.x},
                    height - 1);
      return;
    }

    RenderNode* createNodes(Node* tree, int& height, float scale) {
      if (tree == nullptr) {
        return nullptr;
      }

      int h1 = height, h2 = height;
      RenderNode* rend = new RenderNode;
      rend->left = createNodes(tree->left, h1, scale);
      rend->right = createNodes(tree->right, h2, scale);
      height = std::max(h1, h2) + 1;
      rend->create(tree, tree->key, height, scale);
      return rend;
    }

    void scale(RenderNode* rend, float scaleNum) {
      if (rend == nullptr) return;
      sf::Vector2f s = {scaleNum, scaleNum};
      rend->grow(s);
      scale(rend->left, scaleNum);
      scale(rend->right, scaleNum);
    }
    */
    void create(Node* tree, bool treap = false) {
        int col = 0;
        root = buildShadowTree(tree, 0, col, treap);
        positionNodes(root);
    }

    RenderNode* buildShadowTree(Node* tree, int level, int& col, bool treap) {
        if (tree == nullptr) return nullptr;
        RenderNode* newNode = new RenderNode;
        newNode->create(tree, tree->key, level, treap);

        RenderNode* newLeft = buildShadowTree(tree->left, level + 1, col, treap);
        newNode->left = newLeft;

        newNode->col = col;
        ++col;

        RenderNode* newRight = buildShadowTree(tree->right, level + 1, col, treap);
        newNode->right = newRight;

        return newNode;
    }

    void positionNodes(RenderNode* rend) {
        if (rend == nullptr) return;
        sf::Vector2f pos = { float(rend->col * 200), float(rend->level * 80) };
        rend->setPos(pos);
        positionNodes(rend->left);
        positionNodes(rend->right);
    }

    void draw(sf::RenderWindow& window) { drawNodes(window, root); }

    void drawNodes(sf::RenderWindow& window, RenderNode* rend) {
        if (rend == nullptr) return;
        //sf::Vector2f pos = { rend->rect.getPosition().x, rend->rect.getPosition().y };
        //std::cout << rend->node->key << " " << pos.x << ' ' << pos.y << "\n";
        if (rend->left != nullptr) {
            sf::Vector2f pos1, pos2;
            pos1 = { rend->rect.getPosition().x, rend->rect.getPosition().y };
            pos2 = { rend->left->rect.getPosition().x,
                    rend->left->rect.getPosition().y };
            sf::Vertex line[] = { sf::Vertex(pos1), sf::Vertex(pos2) };
            window.draw(line, 2, sf::Lines);
            
        }
        if (rend->right != nullptr) {
            sf::Vector2f pos1, pos2;
            pos1 = { rend->rect.getPosition().x, rend->rect.getPosition().y };
            pos2 = { rend->right->rect.getPosition().x,
                    rend->right->rect.getPosition().y };
            sf::Vertex line[] = { sf::Vertex(pos1), sf::Vertex(pos2) };
            window.draw(line, 2, sf::Lines);
        }
        rend->draw(window);
        drawNodes(window, rend->left);
        drawNodes(window, rend->right);
    }

    void clear() {
        connections.clear();
        clearNodes(root);
        root = nullptr;
    }

    void clearNodes(RenderNode* rend) {
        if (rend == nullptr) {
            return;
        }
        clearNodes(rend->left);
        clearNodes(rend->right);
        delete rend;
    }

    /* bool contains(float x, float y, int type, RenderNode* node = nullptr, bool
     first = false) { if (node == nullptr && !first) { node = root; first = true;
         }
         if (node == nullptr) return;
         if (node->rect.getGlobalBounds().contains(x, y)) {
             erase()
         }
     }*/
};

void InOrder2(RenderNode* p, int layer = 0) {
    if (p == nullptr) return;
    InOrder2(p->left, layer + 1);
    std::cout << p->key << " ";
    InOrder2(p->right, layer + 1);
}

void UpdateText(sf::Text& text, std::string s) { text.setString(s); }

RenderNode* MousePressed(float x, float y, RenderNode* node) {
    if (node == nullptr) return nullptr;
    //std::cout << node->rect.getGlobalBounds().left << "\n";
    if (node->contains(x, y)) {
        return node;
    }
    RenderNode* left = MousePressed(x, y, node->left);
    RenderNode* right = MousePressed(x, y, node->right);
    if (left != nullptr) return left;
    return right;
}

int Convetr(std::string s) {
    bool minus = false;
    int i = 0, ans = 0;
    if (s[i] == '-') {
        minus = true;
        ++i;
    }
    while (i < s.size()) {
        ans *= 10;
        ans += s[i] - '0';
        ++i;
    }
    if (minus) {
        ans = ans * -1;
    }
    return ans;
}

int main() {
    sf::RenderWindow window(sf::VideoMode(1000, 1000), "FUNKY GROOVY");

    // RedBlack splay;
    RenderTree renderTree;
    sf::Vertex line[] = { sf::Vertex(sf::Vector2f(10.f, 10.f)),
                         sf::Vertex(sf::Vector2f(150.f, 150.f)) };

    std::string addText, delText, addNtext;
    sf::RectangleShape addNodeBox, removeNodeBox, addNBox, addNodeButton, removeNodeButton, treetype1, treetype2, treetype3,
        treetype4;
    sf::Text addDisplayText, delDisplayText, addNDisplayText, addButtonText, delButtonText, treetext1, treetext2, treetext3, treetext4;

    addDisplayText.setPosition(402, 944);
    delDisplayText.setPosition(702, 944);
    addNDisplayText.setPosition(402, 892);
    addButtonText.setPosition(562, 944);
    delButtonText.setPosition(862, 944);

    treetext1.setPosition(52, 944);
    treetext2.setPosition(132, 944);
    treetext3.setPosition(212, 944);
    treetext4.setPosition(292, 944);

    addNDisplayText.setOutlineThickness(2);
    addDisplayText.setOutlineThickness(2);
    addButtonText.setFillColor(textColor);
    delButtonText.setFillColor(textColor);
    addDisplayText.setFillColor(textColor);
    addNDisplayText.setFillColor(textColor);
    delDisplayText.setOutlineThickness(2);
    addButtonText.setOutlineThickness(2);
    delButtonText.setOutlineThickness(2);
    delDisplayText.setFillColor(textColor);

    treetext1.setOutlineThickness(2);
    treetext3.setFillColor(textColor);
    treetext4.setFillColor(textColor);
    treetext1.setFillColor(textColor);
    treetext2.setOutlineThickness(2);
    treetext3.setOutlineThickness(2);
    treetext4.setOutlineThickness(2);
    treetext2.setFillColor(textColor);

    addNodeBox.setSize(sf::Vector2f(150, 30));
    removeNodeBox.setSize(sf::Vector2f(150, 30));
    addNBox.setSize(sf::Vector2f(150, 30));

    addNodeBox.setPosition(400, 950);
    removeNodeBox.setPosition(700, 950);
    addNBox.setPosition(400, 898);

    addNodeBox.setOutlineThickness(2);
    addNodeBox.setOutlineColor(elementColor);
    addNodeBox.setFillColor(fillColor);
    removeNodeBox.setOutlineThickness(2);
    removeNodeBox.setOutlineColor(elementColor);
    removeNodeBox.setFillColor(fillColor);
    addNBox.setOutlineThickness(2);
    addNBox.setOutlineColor(elementColor);
    addNBox.setFillColor(fillColor);

    addNodeButton.setSize(sf::Vector2f(50, 30));
    removeNodeButton.setSize(sf::Vector2f(50, 30));

    addNodeButton.setPosition(560, 950);
    removeNodeButton.setPosition(860, 950);

    addNodeButton.setOutlineThickness(2);
    addNodeButton.setOutlineColor(elementColor);
    addNodeButton.setFillColor(fillColor);
    removeNodeButton.setOutlineThickness(2);
    removeNodeButton.setOutlineColor(elementColor);
    removeNodeButton.setFillColor(fillColor);

    treetype1.setSize(sf::Vector2f(50, 30));
    treetype2.setSize(sf::Vector2f(50, 30));
    treetype3.setSize(sf::Vector2f(50, 30));
    treetype4.setSize(sf::Vector2f(50, 30));

    treetype1.setPosition(50, 950);
    treetype2.setPosition(130, 950);
    treetype3.setPosition(210, 950);
    treetype4.setPosition(290, 950);

    treetype1.setOutlineThickness(2);
    treetype1.setOutlineColor(reactColor);
    treetype1.setFillColor(fillColor);
    treetype2.setOutlineThickness(2);
    treetype2.setOutlineColor(elementColor);
    treetype2.setFillColor(fillColor);
    treetype3.setOutlineThickness(2);
    treetype3.setOutlineColor(elementColor);
    treetype3.setFillColor(fillColor);
    treetype4.setOutlineThickness(2);
    treetype4.setOutlineColor(elementColor);
    treetype4.setFillColor(fillColor);

    font = new sf::Font;
    if (!font->loadFromFile("UbuntuMono-Regular.ttf")) {
        std::cout << "err\n";
    }

    addNDisplayText.setFont(*font);
    addDisplayText.setFont(*font);
    delDisplayText.setFont(*font);
    addButtonText.setFont(*font);
    delButtonText.setFont(*font);

    treetext1.setFont(*font);
    treetext2.setFont(*font);
    treetext3.setFont(*font);
    treetext4.setFont(*font);

    addButtonText.setString("ADD");
    delButtonText.setString("DEL");

    treetext1.setString("AVL");
    treetext2.setString("R/B");
    treetext3.setString("TRP");
    treetext4.setString("SPL");

    sf::Vector2f treePos = { 500, 50 };
    float scale = 1;
    int selected = 0;
    bool minus1 = false, minus2 = false;

    int treeType = 0;
    Avl avl;
    RedBlack redBlack;
    Treap treap;
    Splay splay;

    sf::View view(sf::Vector2f(500, 500), sf::Vector2f(1000, 1000));

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Enter) {
                    if (selected == 1 || selected == 3) {
                        if (selected == 3) {
                            int n = Convetr(addNtext);
                            addNtext.clear();
                            UpdateText(addNDisplayText, addNtext);
                            for (int i = 0; i < n; ++i) {
                                int ans = std::rand() % 1000000000;
                                avl.insert(ans, avl.root);
                                redBlack.insert(ans);
                                treap.insert(ans);
                                splay.insert(ans);
                            }
                        }
                        else if (selected == 1) {
                            int ans = Convetr(addText);
                            addText.clear();
                            UpdateText(addDisplayText, addText);
                            avl.insert(ans, avl.root);
                            redBlack.insert(ans);
                            treap.insert(ans);
                            splay.insert(ans);
                        }
                        if (treeType == 0) {
                            renderTree.create(avl.root);
                        }
                        if (treeType == 1) {
                            renderTree.create(redBlack.root);
                        }
                        if (treeType == 2) {
                            renderTree.create(treap.root, true);
                        }
                        if (treeType == 3) {
                            renderTree.create(splay.root);
                        }
                    }
                    if (selected == 2) {
                        int ans = Convetr(delText);
                        delText.clear();
                        UpdateText(delDisplayText, delText);
                        avl.remove(ans, avl.root);
                        redBlack.erase(ans);
                        treap.remove(ans);
                        splay.remove(ans);
                        if (treeType == 0) {
                            renderTree.create(avl.root);
                        }
                        if (treeType == 1) {
                            renderTree.create(redBlack.root);
                        }
                        if (treeType == 2) {
                            renderTree.create(treap.root, true);
                        }
                        if (treeType == 3) {
                            renderTree.create(splay.root);
                        }
                    }
                    if (renderTree.root != nullptr) {
                        view.setCenter(renderTree.root->col * 200, 400);
                    }
                }
                if (event.key.code == sf::Keyboard::P) {
                    /*if (scale <= 0.2) {
                        
                    }
                    else {
                        scale += 0.1;
                    }*/
                    //scale += 0.1;
                    view.zoom(0.9);
                    //renderTree.scale(renderTree.root, scale);
                    //renderTree.positionNodes(renderTree.root, treePos, renderTree.height);
                }
                if (event.key.code == sf::Keyboard::O) {
                    /*if (scale <= 0.2 && scale > 0.02) {
                        scale -= 0.02;
                        //renderTree.scale(renderTree.root, scale);
                        //renderTree.positionNodes(renderTree.root, treePos, renderTree.height);
                    }
                    else if (scale > 0.2) {
                        
                        //renderTree.scale(renderTree.root, scale);
                        //renderTree.positionNodes(renderTree.root, treePos, renderTree.height);
                    }*/
                    //scale -= 0.1;
                    view.zoom(1.1);
                }
                if (event.key.code == sf::Keyboard::Up) {
                    /*
                    if (scale <= 0.2) {
                        treePos.y += 5 * scale * 100;
                    }
                    else {
                        treePos.y += 5 * scale * 10;
                    }
                    
                    renderTree.positionNodes(renderTree.root, treePos, renderTree.height);
                    */
                    view.move(0, -10 * view.getSize().x / 500);
                    
                }
                if (event.key.code == sf::Keyboard::Down) {
                    /*
                    if (scale <= 0.2) {
                        treePos.y -= 5 * scale * 100;
                    }
                    else {
                        treePos.y -= 5 * scale * 10;
                    }
                    renderTree.positionNodes(renderTree.root, treePos, renderTree.height);
                    */
                    view.move(0, 10 * view.getSize().x / 500);
                }
                if (event.key.code == sf::Keyboard::Left) {
                    /*
                    if (scale <= 0.2) {
                        treePos.x += 5 * scale * 100;
                    }
                    else {
                        treePos.x += 5 * scale * 10;
                    }
                    renderTree.positionNodes(renderTree.root, treePos, renderTree.height);
                    */
                    view.move(-10 * view.getSize().x / 500, 0);
                }
                if (event.key.code == sf::Keyboard::Right) {
                    /*
                    if (scale <= 0.2) {
                        treePos.x -= 5 * scale * 100;
                    }
                    else {
                        treePos.x -= 5 * scale * 10;
                    }
                    renderTree.positionNodes(renderTree.root, treePos, renderTree.height);
                    */
                    view.move(10 * view.getSize().x / 500, 0);
                }
                if (event.key.code == sf::Keyboard::BackSpace) {
                    if (selected == 1) {
                        if (addText.size() > 0) {
                            addText.pop_back();
                            UpdateText(addDisplayText, addText);
                        }
                    }
                    if (selected == 2) {
                        if (delText.size() > 0) {
                            delText.pop_back();
                            UpdateText(delDisplayText, delText);
                        }
                    }
                    if (selected == 3) {
                        if (addNtext.size() > 0) {
                            addNtext.pop_back();
                            UpdateText(addNDisplayText, addNtext);
                        }
                    }
                }
            }
            if (event.type == sf::Event::TextEntered) {
                if (selected == 1) {
                    if (addText.size() == 0) {
                        if (event.text.unicode == '-') {
                            minus1 = true;
                            addText += event.text.unicode;
                        }
                        else {
                            minus1 = false;
                        }
                    }
                    if (event.text.unicode >= '0' && event.text.unicode <= '9' &&
                        (addText.size() < 9 || addText.size() < 10 && minus1)) {
                        addText += event.text.unicode;
                    }
                    UpdateText(addDisplayText, addText);
                }
                if (selected == 2) {
                    if (event.text.unicode == '-') {
                        minus2 = true;
                        delText += event.text.unicode;
                    }
                    else {
                        minus2 = false;
                    }
                    if (event.text.unicode >= '0' && event.text.unicode <= '9' &&
                        (delText.size() < 9 || delText.size() < 10 && minus2)) {
                        delText += event.text.unicode;
                    }
                    UpdateText(delDisplayText, delText);
                }
                if (selected == 3) {
                    if (event.text.unicode >= '0' && event.text.unicode <= '9' &&
                        (addNtext.size() < 9)) {
                        addNtext += event.text.unicode;
                    }
                    UpdateText(addNDisplayText, addNtext);
                }
            }
            if (event.type == sf::Event::MouseButtonPressed) {
                if (addNodeButton.getGlobalBounds().contains(event.mouseButton.x,
                    event.mouseButton.y)) {
                    if (selected == 3) {
                        int n = Convetr(addNtext);
                        addNtext.clear();
                        UpdateText(addNDisplayText, addNtext);
                        for (int i = 0; i < n; ++i) {
                            int ans = std::rand() % 1000000000;
                            avl.insert(ans, avl.root);
                            redBlack.insert(ans);
                            treap.insert(ans);
                            splay.insert(ans);
                        }
                    }
                    else if (selected == 1) {
                        int ans = Convetr(addText);
                        addText.clear();
                        UpdateText(addDisplayText, addText);
                        avl.insert(ans, avl.root);
                        redBlack.insert(ans);
                        treap.insert(ans);
                        splay.insert(ans);
                    }
                    if (treeType == 0) {
                        renderTree.create(avl.root);
                    }
                    if (treeType == 1) {
                        renderTree.create(redBlack.root);
                    }
                    if (treeType == 2) {
                        renderTree.create(treap.root, true);
                    }
                    if (treeType == 3) {
                        renderTree.create(splay.root);
                    }
                    if (renderTree.root != nullptr) {
                        view.setCenter(renderTree.root->col * 200, 400);
                    }
                }
                if (removeNodeButton.getGlobalBounds().contains(event.mouseButton.x,
                    event.mouseButton.y)) {
                    int ans = Convetr(delText);
                    delText.clear();
                    UpdateText(delDisplayText, delText);
                    avl.remove(ans, avl.root);
                    redBlack.erase(ans);
                    treap.remove(ans);
                    splay.remove(ans);
                    if (treeType == 0) {
                        renderTree.create(avl.root);
                    }
                    if (treeType == 1) {
                        renderTree.create(redBlack.root);
                    }
                    if (treeType == 2) {
                        renderTree.create(treap.root, true);
                    }
                    if (treeType == 3) {
                        renderTree.create(splay.root);
                    }
                }
                if (addNodeBox.getGlobalBounds().contains(event.mouseButton.x,
                    event.mouseButton.y)) {
                    selected = 1;
                    addNodeBox.setOutlineColor(reactColor);
                    removeNodeBox.setOutlineColor(elementColor);
                    addNBox.setOutlineColor(elementColor);
                }
                else if (removeNodeBox.getGlobalBounds().contains(event.mouseButton.x,
                    event.mouseButton.y)) {
                    selected = 2;
                    removeNodeBox.setOutlineColor(reactColor);
                    addNodeBox.setOutlineColor(elementColor);
                    addNBox.setOutlineColor(elementColor);
                }
                else if (addNBox.getGlobalBounds().contains(event.mouseButton.x,
                    event.mouseButton.y)) {
                    selected = 3;
                    addNBox.setOutlineColor(reactColor);
                    addNodeBox.setOutlineColor(elementColor);
                    removeNodeBox.setOutlineColor(elementColor);
                }
                else {
                    selected = 0;
                    removeNodeBox.setOutlineColor(elementColor);
                    addNodeBox.setOutlineColor(elementColor);
                    addNBox.setOutlineColor(elementColor);
                }
                
                if (treetype1.getGlobalBounds().contains(event.mouseButton.x,
                    event.mouseButton.y)) {
                    treeType = 0;
                    treetype1.setOutlineColor(reactColor);
                    treetype2.setOutlineColor(elementColor);
                    treetype3.setOutlineColor(elementColor);
                    treetype4.setOutlineColor(elementColor);
                    renderTree.create(avl.root);
                    if (renderTree.root != nullptr) {
                        view.setCenter(renderTree.root->col * 200, 400);
                    }
                }
                if (treetype2.getGlobalBounds().contains(event.mouseButton.x,
                    event.mouseButton.y)) {
                    treeType = 1;
                    treetype2.setOutlineColor(reactColor);
                    treetype1.setOutlineColor(elementColor);
                    treetype3.setOutlineColor(elementColor);
                    treetype4.setOutlineColor(elementColor);
                    renderTree.create(redBlack.root);
                    if (renderTree.root != nullptr) {
                        view.setCenter(renderTree.root->col * 200, 400);
                    }
                }
                if (treetype3.getGlobalBounds().contains(event.mouseButton.x,
                    event.mouseButton.y)) {
                    treeType = 2;
                    treetype3.setOutlineColor(reactColor);
                    treetype2.setOutlineColor(elementColor);
                    treetype1.setOutlineColor(elementColor);
                    treetype4.setOutlineColor(elementColor);
                    renderTree.create(treap.root, true);
                    if (renderTree.root != nullptr) {
                        view.setCenter(renderTree.root->col * 200, 400);
                    }
                }
                if (treetype4.getGlobalBounds().contains(event.mouseButton.x,
                    event.mouseButton.y)) {
                    treeType = 3;
                    treetype4.setOutlineColor(reactColor);
                    treetype2.setOutlineColor(elementColor);
                    treetype3.setOutlineColor(elementColor);
                    treetype1.setOutlineColor(elementColor);
                    renderTree.create(splay.root);
                    if (renderTree.root != nullptr) {
                        view.setCenter(renderTree.root->col * 200, 400);
                    }
                }
                sf::Vector2i pixelPos = sf::Mouse::getPosition(window);
                sf::Vector2f worldPos = window.mapPixelToCoords(pixelPos, view);
                RenderNode* mbNode = MousePressed(worldPos.x, worldPos.y, renderTree.root);
                if (mbNode != nullptr) {
                    int ans = mbNode->key;
                    avl.remove(ans, avl.root);
                    redBlack.erase(ans);
                    treap.remove(ans);
                    splay.remove(ans);
                    if (treeType == 0) {
                        renderTree.create(avl.root);
                    }
                    if (treeType == 1) {
                        renderTree.create(redBlack.root);
                    }
                    if (treeType == 2) {
                        renderTree.create(treap.root, true);
                    }
                    if (treeType == 3) {
                        renderTree.create(splay.root);
                    }
                }
                else {

                }
            }
        }

        window.clear(backgroundColor);

        window.setView(view);

        renderTree.draw(window);

        window.setView(window.getDefaultView());

        window.draw(addNodeBox);
        window.draw(addNBox);
        window.draw(removeNodeButton);
        window.draw(addNodeButton);
        window.draw(removeNodeBox);

        window.draw(treetype1);
        window.draw(treetype2);
        window.draw(treetype3);
        window.draw(treetype4);

        window.draw(addDisplayText);
        window.draw(addNDisplayText);
        window.draw(addButtonText);
        window.draw(delButtonText);
        window.draw(delDisplayText);

        window.draw(treetext1);
        window.draw(treetext3);
        window.draw(treetext4);
        window.draw(treetext2);



        window.display();
    }
}

/*
Avl avl;
    while (true) {
        int a;
        std::cin >> a;
        if (a == 0) {
            InOrder(avl.root);
            std::cout << "\n";
            TreeOrder(avl.root);
        }
        if (a == 1) {
            int key;
            std::cin >> key;
            std::cout << avl.insert(key, avl.root) << "\n";
        }
        if (a == 2) {
            int key;
            std::cin >> key;
            std::cout << avl.remove(key, avl.root) << "\n";
        }
        if (a >= 3) {
            for (int i = 0; i < a; ++i) {
                int k;
                int key;
                std::cin >> key;
                std::cout << avl.insert(key, avl.root) << "\n";
            }
        }
    }
*/

/*
5
3 10 15 30 29


437
3485
238
483
285
22
355
348
345
23
25699
8346867
934988
483276
2931
59328528
283582385
38723
237582358
2
32735285
69
1585
3382
384
23415
2643
523
247724
999999999
556
*/