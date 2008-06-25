#ifndef RDESTL_RB_TREE_H
#define RDESTL_RB_TREE_H

#include "rdestl/rdestl.h"

namespace rde
{
namespace internal
{
	struct rb_tree_base
	{
		typedef size_t	size_type;
		enum color_e
		{
			red,
			black
		};
	};
} // internal

template<typename T>
class rb_tree : private internal::rb_tree_base
{
public:
	typedef T	key_type;

	struct node
	{
		color_e	color;
		T		key;
		node*	left;
		node*	right;
		node*	parent;
	};

	rb_tree()
	:	m_size(0)
	{
		m_sentinel.color	= black;
		m_sentinel.left		= &m_sentinel;
		m_sentinel.right	= &m_sentinel;
		m_sentinel.parent	= &m_sentinel;
		m_root				= &m_sentinel;
	}

	void insert(const key_type& key)
	{
		node* iter(m_root);
		node* parent(&m_sentinel);
		while (iter != &m_sentinel)
		{
			parent = iter;
			if (iter->key < key)
				iter = iter->right;
			else if (key < iter->key)
				iter = iter->left;
			else	// key == iter->key
				return;
		}

		node* new_node = alloc_node();
		new_node->color = red;
		new_node->key	= key;
		new_node->left	= &m_sentinel;
		new_node->right	= &m_sentinel;
		new_node->parent = parent;
		if (parent != &m_sentinel)
		{
			if (key > parent->key)
				parent->right = new_node;
			else
				parent->left = new_node;
		}
		else	// empty tree
			m_root = new_node;

		rebalance(new_node);
		validate();
		++m_size;
	}

	node* find_node(const key_type& key) 
	{
		node* iter(m_root);
		while (iter != &m_sentinel)
		{
			if (iter->key < key)
				iter = iter->right;
			else if (key < iter->key)
				iter = iter->left;
			else // key == iter->key
				return iter;
		}
		return 0;	// not found
	}

	void erase(const key_type& key)
	{
		node* toErase = find_node(key);
		if (toErase != 0)
			erase(toErase);
	}
	void erase(node* n)
	{
		RDE_ASSERT(m_size > 0);
		node* toErase;
		if (n->left == &m_sentinel || n->right == &m_sentinel)
			toErase = n;
		else
		{
			toErase = n->right;
			while (toErase->left != &m_sentinel)
				toErase = toErase->left;
		}

		node* x = toErase->left;
		if (x == &m_sentinel)
			x = toErase->right;
		x->parent = toErase->parent;
		if (toErase->parent != &m_sentinel)
		{
			if (toErase == toErase->parent->left)
				toErase->parent->left = x;
			else
				toErase->parent->right = x;
		}
		else
		{
			m_root = x;
		}

		// Branching is probably worse than key copy anyway.
		// $$$ unless key is very expensive?
		//if (toErase != n)
		n->key = toErase->key;

		if (toErase->color == black)
			rebalance_after_erase(x);
		validate();
		--m_size;
	}

	bool empty() const	{ return m_size == 0; }
	size_type size() const
	{
		return m_size;
	}

	typedef void (*TravFunc)(node* n, int left, int depth);

	void traverse_node(node* n, TravFunc func, int depth)
	{
		int left(-1);
		if (n->parent != &m_sentinel)
		{
			left = n->parent->left == n;
		}
		func(n, left, depth);
		if (n->left != &m_sentinel)
			traverse_node(n->left, func, depth + 1);
		if (n->right != &m_sentinel)
			traverse_node(n->right, func, depth + 1);
	}

	void traverse(TravFunc func)
	{
		int depth(0);
		traverse_node(m_root, func, depth);
	}

private:
	size_type num_nodes(const node* n) const
	{
		return n == &m_sentinel ? 0 : 1 + num_nodes(n->left) + num_nodes(n->right);
	}
	void rebalance(node* new_node)
	{
		RDE_ASSERT(new_node->color == red);
		node* iter(new_node);
		while (iter->parent->color == red)
		{
			node* grandparent(iter->parent->parent);
			if (iter->parent == grandparent->left)
			{
				node* uncle = grandparent->right;
				// Both parent and uncle are red.
				// Repaint both, make grandparent red.
				if (uncle->color == red)
				{
					iter->parent->color = black;
					uncle->color = black;
					grandparent->color = red;
					iter = grandparent;
				}
				else 
				{
					if (iter == iter->parent->right)
					{
						iter = iter->parent;
						rotate_left(iter);
					}
					grandparent = iter->parent->parent;
					iter->parent->color = black;
					grandparent->color = red;
					rotate_right(grandparent);
				}
			}
			else
			{
				node* uncle = grandparent->left;
				if (uncle->color == red)
				{
					grandparent->color = red;
					iter->parent->color = black;
					uncle->color = black;
					iter = grandparent;
				}
				else
				{
					if (iter == iter->parent->left)
					{
						iter = iter->parent;
						rotate_right(iter);
					}
					grandparent = iter->parent->parent;
					iter->parent->color = black;
					grandparent->color = red;
					rotate_left(grandparent);
				}
			}
		}
		m_root->color = black;
	}

	void rebalance_after_erase(node* n)
	{
		node* iter(n);
		while (iter != m_root && iter->color == black)
		{
			if (iter == iter->parent->left)
			{
				node* sibling = iter->parent->right;
				if (sibling->color == red)
				{
					sibling->color = black;
					iter->parent->color = red;
					rotate_left(iter->parent);
					sibling = iter->parent->right;
				}
				if (sibling->left->color == black &&
					sibling->right->color == black)
				{
					sibling->color = red;
					iter = iter->parent;
				}
				else
				{
					if (sibling->right->color == black)
					{
						sibling->left->color = black;
						sibling->color = red;
						rotate_right(sibling);
						sibling = iter->parent->right;
					}
					sibling->color = iter->parent->color;
					iter->parent->color = black;
					sibling->right->color = black;
					rotate_left(iter->parent);
					iter = m_root;
				}
			}
			else	// iter == right child
			{
				node* sibling = iter->parent->left;
				if (sibling->color == red)
				{
					sibling->color = black;
					iter->parent->color = red;
					rotate_right(iter->parent);
					sibling = iter->parent->left;
				}
				if (sibling->left->color == black &&
					sibling->right->color == black)
				{
					sibling->color = red;
					iter = iter->parent;
				}
				else
				{
					if (sibling->left->color == black)
					{
						sibling->right->color = black;
						sibling->color = red;
						rotate_left(sibling);
						sibling = iter->parent->left;
					}
					sibling->color = iter->parent->color;
					iter->parent->color = black;
					sibling->left->color = black;
					rotate_right(iter->parent);
					iter = m_root;
				}
			}
		}
		m_root->color = black;
	}

	void validate() const
	{
		RDE_ASSERT(m_root->color == black);
		validate_node(m_root);

	}
	void validate_node(node* n) const
	{
		// - we're child of our parent.
		RDE_ASSERT(n->parent == &m_sentinel ||
			n->parent->left == n || n->parent->right == n);
		// - both children of red node are black
		if (n->color == red)
		{
			RDE_ASSERT(n->left->color == black);
			RDE_ASSERT(n->right->color == black);
		}
		if (n->left != &m_sentinel)
			validate_node(n->left);
		if (n->right != &m_sentinel)
			validate_node(n->right);
	}

	// n's right child replaces n and the right child's left child
	// becomes n's right child.
	void rotate_left(node* n)
	{
		// Right child's left child becomes n's right child.
		node* rightChild = n->right;
		n->right = rightChild->left;
		if (n->right != &m_sentinel)
			n->right->parent = n;

		// n's right child replaces n
		rightChild->parent = n->parent;
		if (n->parent == &m_sentinel)
		{
			m_root = rightChild;
		}
		else
		{
			if (n == n->parent->left)
				n->parent->left = rightChild;
			else
				n->parent->right = rightChild;
		}
		rightChild->left = n;
		n->parent = rightChild;
	}
	void rotate_right(node* n)
	{
		node* leftChild(n->left);
		n->left = leftChild->right;
		if (n->left != &m_sentinel)
			n->left->parent = n;

		leftChild->parent = n->parent;
		if (n->parent == &m_sentinel)
		{
			m_root = leftChild;
		}
		else
		{
			// Substitute us in the parent list with left child.
			if (n == n->parent->left)
				n->parent->left = leftChild;
			else
				n->parent->right = leftChild;
		}
		leftChild->right = n;
		n->parent = leftChild;
	}
	node* alloc_node()
	{
		return new node();
	}

	node		m_sentinel;
	node*		m_root;
	size_type	m_size;
};

} // rde

#endif // #ifndef RDESTL_RB_TREE_H
