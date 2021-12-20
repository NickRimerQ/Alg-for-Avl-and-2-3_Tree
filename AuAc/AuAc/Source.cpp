#include <iostream>
#include <stdio.h>
#include <cstdlib>
#include <vector>
#include <cmath>
#include <time.h> 

using namespace std;


struct point
{
    double x, y;
    int number;
    char side;
};

struct segment
{
    point l, r;
};

inline int area(point a, point b, point c)
{
	return (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x);
}

inline bool intersect_1(double a, double b, double c, double d)
{
	if (a > b)  swap(a, b);
	if (c > d)  swap(c, d);
	return max(a, c) <= min(b, d);
}

bool intersect(point a, point b, point c, point d)
{
	return intersect_1 (a.x, b.x, c.x, d.x) && intersect_1 (a.y, b.y, c.y, d.y) && area(a,b,c) * area(a,b,d) <= 0 && area(c,d,a) * area(c,d,b) <= 0;
	/*if (intersect_1(a.x, b.x, c.x, d.x) && intersect_1(a.y, b.y, c.y, d.y) && area(a, b, c) * area(a, b, d) <= 0 && area(c, d, a) * area(c, d, b) <= 0)
	{
		cout << "True" << endl;
	}
	else
	{
		cout << "ne true" << endl;
	}
	return bool();*/
}

bool segm_intersect(segment a, segment b)
{
	return intersect(a.l, a.r, b.l, b.r);
}


struct node // структура дл€ представлени€ узлов дерева
{
	segment key;
	unsigned char height;
	node* left;
	node* right;
	node(segment k) { key = k; left = right = 0; height = 1; }
};

unsigned char height(node* p)
{
	return p ? p->height : 0;
}

int bfactor(node* p)
{
	return height(p->right) - height(p->left);
}

void fixheight(node* p)
{
	unsigned char hl = height(p->left);
	unsigned char hr = height(p->right);
	p->height = (hl > hr ? hl : hr) + 1;
}

node* rotateright(node* p) // правый поворот вокруг p
{
	node* q = p->left;
	p->left = q->right;
	q->right = p;
	fixheight(p);
	fixheight(q);
	return q;
}

node* rotateleft(node* q) // левый поворот вокруг q
{
	node* p = q->right;
	q->right = p->left;
	p->left = q;
	fixheight(q);
	fixheight(p);
	return p;
}

node* balance(node* p) // балансировка узла p
{
	fixheight(p);
	if (bfactor(p) == 2)
	{
		if (bfactor(p->right) < 0)
			p->right = rotateright(p->right);
		return rotateleft(p);
	}
	if (bfactor(p) == -2)
	{
		if (bfactor(p->left) > 0)
			p->left = rotateleft(p->left);
		return rotateright(p);
	}
	return p; // балансировка не нужна
}

node* insert(node* p, segment k) // вставка ключа k в дерево с корнем p
{
	if (!p) return new node(k);
	if (k.l.y < p->key.l.y)
		p->left = insert(p->left, k);
	else
		p->right = insert(p->right, k);
	return balance(p);
}

node* findmin(node* p) // поиск узла с минимальным ключом в дереве p 
{
	return p->left ? findmin(p->left) : p;
}

node* removemin(node* p) // удаление узла с минимальным ключом из дерева p
{
	if (p->left == 0)
		return p->right;
	p->left = removemin(p->left);
	return balance(p);
}

node* remove(node* p, segment k) // удаление ключа k из дерева p
{
	if (!p) return 0;
	if (k.l.y < p->key.l.y)
		p->left = remove(p->left, k);
	else if (k.l.y > p->key.l.y)
		p->right = remove(p->right, k);
	else //  k == p->key 
	{
		node* q = p->left;
		node* r = p->right;
		delete p;
		if (!r) return q;
		node* min = findmin(r);
		min->right = removemin(r);
		min->left = q;
		return balance(min);
	}
	return balance(p);
}

node* search(node* p, segment k) { // ѕоиск ключа k в дереве с корнем p.
	if (!p) return nullptr;

	if (p->key.l.y == k.l.y) return p;
	else if (k.l.y < p->key.l.y) return search(p->left, k);
	else if (k.l.y > p->key.l.y) return search(p->right, k);
}


node* parent(node* p, segment k)
{
	if (!p) return nullptr;

	if (p->key.l.y == k.l.y) return nullptr;

	if (p->left && p->left->key.l.y == k.l.y) return p;
	else if (p->right && p->right->key.l.y == k.l.y) return p;
	else if (k.l.y < p->key.l.y) return parent(p->left, k);
	else if (k.l.y > p->key.l.y) return parent(p->right, k);
}

bool is_leaf(node* p) // явл€етс€ ли узел листом;
{
	return (p->left == nullptr) && (p->right == nullptr);
	/*if ((p->left == nullptr) && (p->right == nullptr))
	{
		cout << "True" << endl;
	}
	else 
	{
		cout << "ne true" << endl;
	}*/
}


segment under(node* p, node* m, segment k) //корень, текущий эл-нт дерева, сегмент текущего эл-та дерева
{//поиск ближайшего минимального
	node* extra = m;
	if (is_leaf(extra)) 
	{
		while (parent(p, k)) 
		{
			if (extra->key.l.y > parent(p, k)->key.l.y) 
			{
				return parent(p, k)->key;
			}
			k = parent(p, k)->key;
		}
		segment error;
		error.l.x = k.l.y + 1;
		error.l.y = k.l.y + 1;
		error.r.x = k.l.y + 1;
		error.r.y = k.l.y + 1;
		error.l.number = -1;
		error.r.number = -1;
		return error;
	}
	else
	{
		if (extra->left != nullptr && extra->right != nullptr)
		{
			extra = extra->left;
			while (extra->right)
			{
				extra = extra->right;
			}
			return extra->key;
		}
		else if (extra->left == nullptr) 
		{
			while (parent(p, k)) 
			{
				if (extra->key.l.y > parent(p, k)->key.l.y) 
				{
					//return min?
					return parent(p, k)->key;
				}
				k = parent(p, k)->key;
			}
			segment error;
			error.l.x = k.l.y + 1;
			error.l.y = k.l.y + 1;
			error.r.x = k.l.y + 1;
			error.r.y = k.l.y + 1;
			error.l.number = -1;
			error.r.number = -1;
			return error;
		}
		else if (extra->right == nullptr) 
		{
			extra = extra->left;
			while (extra->right)
			{
				extra = extra->right;
			}
			return extra->key;
		}
	}
}

segment above(node* p, node* m, segment k) //корень, текущий эл-нт дерева, сегмент текущего эл-та дерева
{//поиск ближайшего максимального
	node* extra = m;
	if (is_leaf(extra)) 
	{
		while (parent(p, k))
		{
			if (extra->key.l.y < parent(p, k)->key.l.y)
			{
				return parent(p, k)->key;
			}
			k = parent(p, k)->key;
		}
		segment error;
		error.l.x = k.l.y + 1;
		error.l.y = k.l.y + 1;
		error.r.x = k.l.y + 1;
		error.r.y = k.l.y + 1;
		error.l.number = -1;
		error.r.number = -1;
		return error;
	}
	else 
	{
		if (extra->left != nullptr && extra->right != nullptr) 
		{
			extra = extra->right;
			while (extra->left)
			{
				extra = extra->left;
			}
			return extra->key;
		}
		else if (extra->left == nullptr) 
		{
			extra = extra->right;
			while (extra->left)
			{
				extra = extra->left;
			}
			return extra->key;
		}
		else if (extra->right == nullptr) 
		{
			while (parent(p, k))
			{
				if (extra->key.l.y < parent(p, k)->key.l.y)
				{
					return parent(p, k)->key;
				}
				k = parent(p, k)->key;
			}
			segment error;
			error.l.x = k.l.y + 1;
			error.l.y = k.l.y + 1;
			error.r.x = k.l.y + 1;
			error.r.y = k.l.y + 1;
			error.l.number = -1;
			error.r.number = -1;
			return error;
		}

	}
}



struct node2_3 
{
private:
	int size;
	segment key[3];
	node2_3* first;
	node2_3* second;
	node2_3* third;
	node2_3* fourth;
	node2_3* parent;

	bool find(segment k) 
	{
		for (int i = 0; i < size; ++i)
			if (key[i].l.y == k.l.y) return true;
		return false;
	}

	void swap(segment& x, segment& y) {
		segment r = x;
		x = y;
		y = r;
	}

	void sort2(segment& x, segment& y) 
	{
		if (x.l.y > y.l.y) swap(x, y);
	}

	void sort3(segment& x, segment& y, segment& z)
	{
		if (x.l.y > y.l.y) swap(x, y);
		if (x.l.y > z.l.y) swap(x, z);
		if (y.l.y > z.l.y) swap(y, z);
	}

	void sort()
	{
		if (size == 1) return;
		if (size == 2) sort2(key[0], key[1]);
		if (size == 3) sort3(key[0], key[1], key[2]);
	}

	void insert_to_node(segment k)
	{
		key[size] = k;
		size++;
		sort();
	}

	void remove_from_node(segment k)
	{
		if (size >= 1 && key[0].l.y == k.l.y) 
		{
			key[0] = key[1];
			key[1] = key[2];
			size--;
		}
		else if (size == 2 && key[1].l.y == k.l.y) 
		{
			key[1] = key[2];
			size--;
		}
	}

	void become_node2(segment k, node2_3* first_, node2_3* second_) 
	{
		key[0] = k;
		first = first_;
		second = second_;
		third = nullptr;
		fourth = nullptr;
		parent = nullptr;
		size = 1;
	}

	bool is_leaf()
	{
		return (first == nullptr) && (second == nullptr) && (third == nullptr);
	}

public:
	node2_3(segment k) : size(1), key{ k, segment(), segment()}, first(nullptr), second(nullptr),
		third(nullptr), fourth(nullptr), parent(nullptr) {}

	node2_3(segment k, node2_3* first_, node2_3* second_, node2_3* third_, node2_3* fourth_, node2_3* parent_) :
		size(1), key{ k, segment(), segment()}, first(first_), second(second_),
		third(third_), fourth(fourth_), parent(parent_) {}

	node2_3* split(node2_3* item)
	{
		if (item->size < 3) return item;

		node2_3* x = new node2_3(item->key[0], item->first, item->second, nullptr, nullptr, item->parent);
		node2_3* y = new node2_3(item->key[2], item->third, item->fourth, nullptr, nullptr, item->parent);
		if (x->first)  x->first->parent = x;
		if (x->second) x->second->parent = x;
		if (y->first)  y->first->parent = y;
		if (y->second) y->second->parent = y;

		if (item->parent) {
			item->parent->insert_to_node(item->key[1]);

			if (item->parent->first == item) item->parent->first = nullptr;
			else if (item->parent->second == item) item->parent->second = nullptr;
			else if (item->parent->third == item) item->parent->third = nullptr;

			if (item->parent->first == nullptr) {
				item->parent->fourth = item->parent->third;
				item->parent->third = item->parent->second;
				item->parent->second = y;
				item->parent->first = x;
			}
			else if (item->parent->second == nullptr) {
				item->parent->fourth = item->parent->third;
				item->parent->third = y;
				item->parent->second = x;
			}
			else {
				item->parent->fourth = y;
				item->parent->third = x;
			}

			node2_3* tmp = item->parent;
			delete item;
			return tmp;
		}
		else {
			x->parent = item;
			y->parent = item;
			item->become_node2(item->key[1], x, y);
			return item;
		}
	}
	node2_3* insert(node2_3* p, segment k)
	{ 
		if (!p) return new node2_3(k);

		if (p->is_leaf()) p->insert_to_node(k);
		else if (k.l.y <= p->key[0].l.y) insert(p->first, k);
		else if ((p->size == 1) || ((p->size == 2) && k.l.y <= p->key[1].l.y)) insert(p->second, k);
		else insert(p->third, k);

		return split(p);
	}
	node2_3* search(node2_3* p, segment k)
	{
		if (!p) return nullptr;

		if (p->find(k)) return p;
		else if (k.l.y < p->key[0].l.y) return search(p->first, k);
		else if ((p->size == 2) && (k.l.y < p->key[1].l.y) || (p->size == 1)) return search(p->second, k);
		else if (p->size == 2) return search(p->third, k);
	}
	node2_3* search_min(node2_3* p)
	{
		if (!p) return p;
		if (!(p->first)) return p;
		else return search_min(p->first);
	}
	node2_3* merge(node2_3* leaf)
	{
		node2_3* parent = leaf->parent;

		if (parent->first == leaf) {
			parent->second->insert_to_node(parent->key[0]);
			parent->second->third = parent->second->second;
			parent->second->second = parent->second->first;

			if (leaf->first != nullptr) parent->second->first = leaf->first;
			else if (leaf->second != nullptr) parent->second->first = leaf->second;

			if (parent->second->first != nullptr) parent->second->first->parent = parent->second;

			parent->remove_from_node(parent->key[0]);
			delete parent->first;
			parent->first = nullptr;
		}
		else if (parent->second == leaf) {
			parent->first->insert_to_node(parent->key[0]);

			if (leaf->first != nullptr) parent->first->third = leaf->first;
			else if (leaf->second != nullptr) parent->first->third = leaf->second;

			if (parent->first->third != nullptr) parent->first->third->parent = parent->first;

			parent->remove_from_node(parent->key[0]);
			delete parent->second;
			parent->second = nullptr;
		}

		if (parent->parent == nullptr) {
			node2_3* tmp = nullptr;
			if (parent->first != nullptr) tmp = parent->first;
			else tmp = parent->second;
			tmp->parent = nullptr;
			delete parent;
			return tmp;
		}
		return parent;
	}
	node2_3* redistribute(node2_3* leaf)
	{
		node2_3* parent = leaf->parent;
		node2_3* first = parent->first;
		node2_3* second = parent->second;
		node2_3* third = parent->third;

		if ((parent->size == 2) && (first->size < 2) && (second->size < 2) && (third->size < 2)) {
			if (first == leaf) {
				parent->first = parent->second;
				parent->second = parent->third;
				parent->third = nullptr;
				parent->first->insert_to_node(parent->key[0]);
				parent->first->third = parent->first->second;
				parent->first->second = parent->first->first;

				if (leaf->first != nullptr) parent->first->first = leaf->first;
				else if (leaf->second != nullptr) parent->first->first = leaf->second;

				if (parent->first->first != nullptr) parent->first->first->parent = parent->first;

				parent->remove_from_node(parent->key[0]);
				delete first;
			}
			else if (second == leaf) {
				first->insert_to_node(parent->key[0]);
				parent->remove_from_node(parent->key[0]);
				if (leaf->first != nullptr) first->third = leaf->first;
				else if (leaf->second != nullptr) first->third = leaf->second;

				if (first->third != nullptr) first->third->parent = first;

				parent->second = parent->third;
				parent->third = nullptr;

				delete second;
			}
			else if (third == leaf) {
				second->insert_to_node(parent->key[1]);
				parent->third = nullptr;
				parent->remove_from_node(parent->key[1]);
				if (leaf->first != nullptr) second->third = leaf->first;
				else if (leaf->second != nullptr) second->third = leaf->second;

				if (second->third != nullptr)  second->third->parent = second;

				delete third;
			}
		}
		else if ((parent->size == 2) && ((first->size == 2) || (second->size == 2) || (third->size == 2))) {
			if (third == leaf) {
				if (leaf->first != nullptr) {
					leaf->second = leaf->first;
					leaf->first = nullptr;
				}

				leaf->insert_to_node(parent->key[1]);
				if (second->size == 2) {
					parent->key[1] = second->key[1];
					second->remove_from_node(second->key[1]);
					leaf->first = second->third;
					second->third = nullptr;
					if (leaf->first != nullptr) leaf->first->parent = leaf;
				}
				else if (first->size == 2) {
					parent->key[1] = second->key[0];
					leaf->first = second->second;
					second->second = second->first;
					if (leaf->first != nullptr) leaf->first->parent = leaf;

					second->key[0] = parent->key[0];
					parent->key[0] = first->key[1];
					first->remove_from_node(first->key[1]);
					second->first = first->third;
					if (second->first != nullptr) second->first->parent = second;
					first->third = nullptr;
				}
			}
			else if (second == leaf) {
				if (third->size == 2) {
					if (leaf->first == nullptr) {
						leaf->first = leaf->second;
						leaf->second = nullptr;
					}
					second->insert_to_node(parent->key[1]);
					parent->key[1] = third->key[0];
					third->remove_from_node(third->key[0]);
					second->second = third->first;
					if (second->second != nullptr) second->second->parent = second;
					third->first = third->second;
					third->second = third->third;
					third->third = nullptr;
				}
				else if (first->size == 2) {
					if (leaf->second == nullptr) {
						leaf->second = leaf->first;
						leaf->first = nullptr;
					}
					second->insert_to_node(parent->key[0]);
					parent->key[0] = first->key[1];
					first->remove_from_node(first->key[1]);
					second->first = first->third;
					if (second->first != nullptr) second->first->parent = second;
					first->third = nullptr;
				}
			}
			else if (first == leaf) {
				if (leaf->first == nullptr) {
					leaf->first = leaf->second;
					leaf->second = nullptr;
				}
				first->insert_to_node(parent->key[0]);
				if (second->size == 2) {
					parent->key[0] = second->key[0];
					second->remove_from_node(second->key[0]);
					first->second = second->first;
					if (first->second != nullptr) first->second->parent = first;
					second->first = second->second;
					second->second = second->third;
					second->third = nullptr;
				}
				else if (third->size == 2) {
					parent->key[0] = second->key[0];
					second->key[0] = parent->key[1];
					parent->key[1] = third->key[0];
					third->remove_from_node(third->key[0]);
					first->second = second->first;
					if (first->second != nullptr) first->second->parent = first;
					second->first = second->second;
					second->second = third->first;
					if (second->second != nullptr) second->second->parent = second;
					third->first = third->second;
					third->second = third->third;
					third->third = nullptr;
				}
			}
		}
		else if (parent->size == 1) {
			leaf->insert_to_node(parent->key[0]);

			if (first == leaf && second->size == 2) {
				parent->key[0] = second->key[0];
				second->remove_from_node(second->key[0]);

				if (leaf->first == nullptr) leaf->first = leaf->second;

				leaf->second = second->first;
				second->first = second->second;
				second->second = second->third;
				second->third = nullptr;
				if (leaf->second != nullptr) leaf->second->parent = leaf;
			}
			else if (second == leaf && first->size == 2) {
				parent->key[0] = first->key[1];
				first->remove_from_node(first->key[1]);

				if (leaf->second == nullptr) leaf->second = leaf->first;

				leaf->first = first->third;
				first->third = nullptr;
				if (leaf->first != nullptr) leaf->first->parent = leaf;
			}
		}
		return parent;
	}
	node2_3* fix(node2_3* leaf)
	{
		if (leaf->size == 0 && leaf->parent == nullptr)
		{
			delete leaf;
			return nullptr;
		}
		if (leaf->size != 0) 
		{
			if (leaf->parent) return fix(leaf->parent);
			else return leaf;
		}

		node2_3* parent = leaf->parent;
		if (parent->first->size == 2 || parent->second->size == 2 || parent->size == 2) leaf = redistribute(leaf);
		else if (parent->size == 2 && parent->third->size == 2) leaf = redistribute(leaf);
		else leaf = merge(leaf);

		return fix(leaf);
	}
	node2_3* remove(node2_3* p, segment k)
	{
		node2_3* item = search(p, k);

		if (!item) return p;

		node2_3* min = nullptr;
		if (item->key[0].l.y == k.l.y) min = search_min(item->second);
		else min = search_min(item->third);

		if (min) 
		{
			segment& z = (k.l.y == item->key[0].l.y ? item->key[0] : item->key[1]);
			item->swap(z, min->key[0]);
			item = min;
		}

		item->remove_from_node(k);
		return fix(item);
	}


	segment under(node2_3* m, segment k)
	{
		node2_3* extra = m;
		if (extra->is_leaf()) 
		{
			if (extra->size != 2) 
			{
				while (1)
				{
					if (extra->parent)
					{
						extra = extra->parent;
						for (int i = extra->size - 1; i != -1; i--)
						{
							if (extra->key[i].l.y < k.l.y)
							{
								return extra->key[i];
							}
						}
					}
					else
					{
						segment error;
						error.l.x = k.l.y + 1;
						error.l.y = k.l.y + 1;
						error.r.x = k.l.y + 1;
						error.r.y = k.l.y + 1;
						error.l.number = -1;
						error.r.number = -1;
						return error;
					}
				}
			}
			else 
			{
				if (extra->key[1].l.y == k.l.y)
				{
					return extra->key[0];
				}
				else
				{
					while (1)
					{
						if (extra->parent)
						{
							extra = extra->parent;
							for (int i = extra->size - 1; i != -1; i--)
							{
								if (extra->key[i].l.y < k.l.y)
								{
									return extra->key[i];
								}
							}
						}
						else
						{
							segment error;
							error.l.x = k.l.y + 1;
							error.l.y = k.l.y + 1;
							error.r.x = k.l.y + 1;
							error.r.y = k.l.y + 1;
							error.l.number = -1;
							error.r.number = -1;
							return error;
						}
					}
				}
			}
		}
		else 
		{
			if (extra->size != 2) 
			{
				extra = extra->first;
				while (extra->second || extra->third)
				{
					if (extra->third)
					{
						extra = extra->third;
					}
					else
					{
						extra = extra->second;
					}
				}
				return extra->key[extra->size - 1];
			}
			else 
			{
				if (extra->key[0].l.y == k.l.y)
				{
					extra = extra->first;
					while (extra->second || extra->third)
					{
						if (extra->third)
						{
							extra = extra->third;
						}
						else
						{
							extra = extra->second;
						}
					}
					return extra->key[extra->size - 1];
				}
				else if (extra->key[1].l.y == k.l.y)
				{
					extra = extra->second;
					while (extra->second || extra->third)
					{
						if (extra->third)
						{
							extra = extra->third;
						}
						else
						{
							extra = extra->second;
						}
					}
					return extra->key[extra->size - 1];
				}
			}
		}
	}

	segment above(node2_3* m, segment k) 
	{
		node2_3* extra = m;
		if (extra->is_leaf()) 
		{
			if (extra->size != 2) 
			{
				while (1)
				{
					if (extra->parent)
					{
						extra = extra->parent;
						for (int i = extra->size - 1; i != -1; i--)
						{
							if (extra->key[i].l.y > k.l.y)
							{
								return extra->key[i];
							}
						}
					}
					else
					{
						segment error;
						error.l.x = k.l.y + 1;
						error.l.y = k.l.y + 1;
						error.r.x = k.l.y + 1;
						error.r.y = k.l.y + 1;
						error.l.number = -1;
						error.r.number = -1;
						return error;
					}
				}
			}
			else 
			{
				if (extra->key[0].l.y == k.l.y)
				{
					return extra->key[1];
				}
				else
				{
					while (1)
					{
						if (extra->parent)
						{
							extra = extra->parent;
							for (int i = extra->size - 1; i != -1; i--)
							{
								if (extra->key[i].l.y > k.l.y)
								{
									return extra->key[i];
								}
							}
						}
						else
						{
							segment error;
							error.l.x = k.l.y + 1;
							error.l.y = k.l.y + 1;
							error.r.x = k.l.y + 1;
							error.r.y = k.l.y + 1;
							error.l.number = -1;
							error.r.number = -1;
							return error;
						}
					}
				}
			}
		}
		else 
		{
			if (extra->size != 2) 
			{
				extra = extra->second;
				while (extra->first)
				{
					extra = extra->first;
				}
				return extra->key[0];
			}
			else 
			{
				if (extra->key[1].l.y == k.l.y)
				{
					extra = extra->third;

					while (extra->second || extra->third)
					{
						if (extra->third)
						{
							extra = extra->third;
						}
						else
						{
							extra = extra->second;
						}
					}
					return extra->key[extra->size - 1];
				}
				else
				{
					extra = extra->second;

					while (extra->second || extra->third)
					{
						if (extra->third)
						{
							extra = extra->third;
						}
						else
						{
							extra = extra->second;
						}
					}
					return extra->key[extra->size - 1];
				}
			}
		}
	}
};



int main()
{
	int n;
	cout << "Enter the number of your segments:" << endl;
	cin >> n;

	cout << "Enter a method for specifying line segments:" << endl;
	int q;
	cin >> q;


	time_t start, end;


	vector<point> a;
	vector <segment> b;

	switch (q)
	{
	case 1:
	{
		int k = -1;
		cout << "Enter k?" << endl;
		cin >> k;
		if (k >= n)
		{
			k = n - 2;
		}
		

		for (int i = 0; i < 2 * n; i++)
		{
			point p;
			p.x = (2 * (double)rand() - RAND_MAX) / RAND_MAX;
			p.y = (2 * (double)rand() - RAND_MAX) / RAND_MAX;
			p.number = i / 2;
			if (i % 2 == 0)
			{
				p.side = 'l';


			}
			else
			{
				/*if (a[a.size() - 1].x > p.x)
				{
					point hp = a[a.size() - 1];
					a[a.size() - 1] = p;
					a[a.size() - 1].side = 'l';
					a.push_back(hp);
					a[a.size() - 1].side = 'r';
					continue;
				}*/
				p.side = 'r';
				//a[a.size() - 1] 
			}
			a.push_back(p);
			if (i % 2 != 0 && k != -1) 
			{
				bool tk = false;
				segment hp;
				hp.l = a[a.size() - 2];
				hp.r = a[a.size() - 1];

				if (i / 2 == k + 1)
				{
					segment hpk;
					hpk.l = a[a.size() - 3];
					hpk.r = a[a.size() - 4];
					if (!segm_intersect(hp, hpk))
					{
						i = i - 2;
						a.erase(a.end() - 1);
						a.erase(a.end() - 1);
						continue;
					}
				}

				for (int j = 0; j < a.size() - 1; j++)
				{
					if (j % 2 != 0)
					{
						segment new_hp;
						new_hp.l = a[j - 1];
						new_hp.r = a[j];
						if (i / 2 == k + 1 && new_hp.l.number == k)
						{
							break;
						}
						if (segm_intersect(hp, new_hp))
						{
							tk = true;
							i = i - 2;
							a.erase(a.end() - 1);
							a.erase(a.end() - 1);
							break;
						}
					}
				}
				if (tk)
				{
					continue;
				}

			}
		}

		for (int i = 0; i < a.size(); i++)
		{
			if (i % 2 == 1)
			{
				if (a[i].x < a[i - 1].x)
				{
					point t = a[i];
					a[i] = a[i - 1];
					a[i - 1] = t;
					a[i].side = 'r';
					a[i - 1].side = 'l';
				}
			}
		}


		for (int i = 0; i < 2 * n; i++)
		{

			segment extra;

			if (i % 2 == 0)
			{
				extra.l = a[i];
				b.push_back(extra);

			}
			else
			{
				b[b.size() - 1].r = a[i];
			}

		}
		break;
	}

	case 2:
	{
		int k = -1;
		cout << "Enter k?" << endl;
		cin >> k;
		if (k >= n)
		{
			k = n - 2;
		}

		double l;
		cout << "Enter l" << endl;
		cin >> l;

		double r = l / 2;

		for (int i = 0; i < n; i++)
		{
			point u;
			u.x = (2 * (double)rand() - RAND_MAX) / RAND_MAX;
			u.y = (2 * (double)rand() - RAND_MAX) / RAND_MAX;
			double new_x1 = u.x - r;
			double new_x2 = u.x + r;
			double rand_x = (double)rand() / RAND_MAX * (new_x2 - new_x1) + new_x1;
			double y_help = sqrt(r * r - (rand_x - u.x) * (rand_x - u.x)) + u.y;
			//rand_y = ((int)rand() % 2) > 0 ? rand_y : -1 * rand_y;
			double extra_y = u.y - (y_help - u.y);
			double rand_y = ((int)rand() % 2) > 0 ? y_help : extra_y;

			double mirror_rand_x;
			if (u.x > rand_x)
			{
				//mirror_rand_x = u.x + rand_x;
				mirror_rand_x = u.x + (u.x - rand_x);
			}
			else
			{
				//mirror_rand_x = u.x - rand_x;
				mirror_rand_x = u.x - (rand_x - u.x);
			}

			double mirror_rand_y;
			if (u.y > rand_y)
			{
				mirror_rand_y = u.y + (u.y - rand_y);
			}
			else
			{
				mirror_rand_y = u.y - (rand_y - u.y);
			}

			point m1, m2;
			m1.x = rand_x;
			m1.y = rand_y;
			m2.x = mirror_rand_x;
			m2.y = mirror_rand_y;

			if (m1.x > m2.x)
			{
				m1.side = 'r';
				m2.side = 'l';
			}
			else
			{
				m1.side = 'l';
				m2.side = 'r';
			}
			m1.number = i;
			m2.number = i;

			segment extr;

			if (m1.side == 'l')
			{
				a.push_back(m1);
				a.push_back(m2);
				extr.l = m1;
				extr.r = m2;
			}
			else
			{
				a.push_back(m2);
				a.push_back(m1);
				extr.l = m2;
				extr.r = m1;
			}
			b.push_back(extr);

			if (k != -1)
			{
				bool tk = false;
				segment hp;
				hp.l = a[a.size() - 2];
				hp.r = a[a.size() - 1];

				if (i == k + 1)
				{
					segment hpk;
					hpk.l = a[a.size() - 3];
					hpk.r = a[a.size() - 4];
					if (!segm_intersect(hp, hpk))
					{
						i = i - 1;
						a.erase(a.end() - 1);
						a.erase(a.end() - 1);
						b.erase(b.end() - 1);
						continue;
					}
				}

				for (int j = 0; j < a.size() - 1; j++)
				{
					if (j % 2 != 0)
					{
						segment new_hp;
						new_hp.l = a[j - 1];
						new_hp.r = a[j];
						if (i == k + 1 && new_hp.l.number == k)
						{
							break;
						}
						if (segm_intersect(hp, new_hp))
						{
							tk = true;
							i = i - 1;
							a.erase(a.end() - 1);
							a.erase(a.end() - 1);
							b.erase(b.end() - 1);
							break;
						}
					}
				}
				if (tk)
				{
					continue;
				}

			}
		}
		break;
	}

	case 3:
	{
		for (int i = 0; i < 2 * n; i++)
		{
			point p;
			cout << "Enter x" << i / 2 << ":" << endl;
			cin >> p.x;
			cout << "Enter y:" << i / 2 << ":" << endl;
			cin >> p.y;
			p.number = i / 2;
			if (i % 2 == 0)
			{
				p.side = 'l';


			}
			else
			{
				/*if (a[a.size() - 1].x > p.x)
				{
					point hp = a[a.size() - 1];
					a[a.size() - 1] = p;
					a[a.size() - 1].side = 'l';
					a.push_back(hp);
					a[a.size() - 1].side = 'r';
					continue;
				}*/
				p.side = 'r';
				//a[a.size() - 1] 
			}
			a.push_back(p);
		}

		for (int i = 0; i < a.size(); i++)
		{
			if (i % 2 == 1)
			{
				if (a[i].x < a[i - 1].x)
				{
					point t = a[i];
					a[i] = a[i - 1];
					a[i - 1] = t;
					a[i].side = 'r';
					a[i - 1].side = 'l';
				}
			}
		}


		for (int i = 0; i < 2 * n; i++)
		{

			segment extra;

			if (i % 2 == 0)
			{
				extra.l = a[i];
				b.push_back(extra);

			}
			else
			{
				b[b.size() - 1].r = a[i];
			}

		}
		break;
	}
	}



	//сортировка
	time(&start);
	for (int i = 0; i < a.size(); i++)
	{
		for (int j = i + 1; j < a.size(); j++)
		{
			if (a[i].x > a[j].x || (a[i].x == a[j].x && a[i].y > a[j].y))
			{
				point helper = a[i];
				a[i] = a[j];
				a[j] = helper;
			}
		}
	}
	time(&end);


	double sort = difftime(end, start);




	time(&start);
	node* g = nullptr;

	for (int i = 0; i < 2 * n; i++)
	{
		point p = a[i];
		segment s = b[a[i].number];
		if (p.side == 'l')
		{
			g = insert(g, s);
			segment s1 = above(g, search(g, s), s);
			segment s2 = under(g, search(g, s), s);
			if ((s1.l.number != -1 && intersect(s.l, s.r, s1.l, s1.r)) || (s2.l.number != -1 && intersect(s.l, s.r, s2.l, s2.r)))
			{
				cout << "True for avl (l point)" << endl;
				if (s1.l.number != -1 && intersect(s.l, s.r, s1.l, s1.r))
				{
					cout << "s1" << endl;
					cout << s.l.number << " " << s1.l.number << endl;
				}
				if (s2.l.number != -1 && intersect(s.l, s.r, s2.l, s2.r))
				{
					cout << "s2" << endl;
					cout << s.l.number << " " << s2.l.number << endl;
				}
				break;
			}
		}
		else
		{
			segment s1 = above(g, search(g, s), s);
			segment s2 = under(g, search(g, s), s);
			if (s1.l.number != -1 && s2.l.number != -1 && intersect(s2.l, s2.r, s1.l, s1.r))
			{
				cout << "True for avl (r point)" << endl;
				cout << "s1 and s2" << endl;
				cout << s2.l.number << " " << s1.l.number << endl;
				break;
			}
			g = remove(g, s);
		}
	}
	time(&end);

	double avl = difftime(end, start);
	


	time(&start);
	node2_3* h = nullptr;

	for (int i = 0; i < 2 * n; i++) 
	{
		point p = a[i];
		segment s = b[a[i].number];
		if (p.side == 'l') 
		{
			h = h->insert(h, s);
			segment s1 = h->above(h->search(h, s), s);
			segment s2 = h->under(h->search(h, s), s);
			if ((s1.l.number != -1 && intersect(s.l, s.r, s1.l, s1.r)) || (s2.l.number != -1 && intersect(s.l, s.r, s2.l, s2.r)))
			{
				cout << "True for d2_3 (l point)" << endl;
				if (s1.l.number != -1 && intersect(s.l, s.r, s1.l, s1.r))
				{
					cout << "s1" << endl;
					cout << s.l.number << " " << s1.l.number << endl;
				}
				if (s2.l.number != -1 && intersect(s.l, s.r, s2.l, s2.r))
				{
					cout << "s2" << endl;
					cout << s.l.number << " " << s2.l.number << endl;
				}
				break;
			}
		}
		else 
		{
			segment s1 = h->above(h->search(h, s), s);
			segment s2 = h->under(h->search(h, s), s);
			if (s1.l.number != -1 && s2.l.number != -1 && intersect(s2.l, s2.r, s1.l, s1.r))
			{
				cout << "True for d2_3 (r point)" << endl;
				cout << "s1 and s2" << endl;
				cout << s2.l.number << " " << s1.l.number << endl;
				break;
			}
			h = h->remove(h, s);
		}
	}
	time(&end);
	cout << end - start << endl;


	double d2_3 = difftime(end, start);


	cout << "Avl" << endl;
	cout << avl + sort << endl;
	cout << "d2_3" << endl;
	cout << d2_3 + sort << endl;


	return 0;
}