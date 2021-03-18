#pragma once

template <typename T>
class HashMap
{
public:
	HashMap(const size_t _size);
	HashMap(const HashMap& rhs);
	HashMap& operator=(const HashMap& rhs);	
	~HashMap();

	//Използвам оператора за индексиране и за четене и за писане
	T& operator[](const std::string& key);

	bool hasKey(const std::string& key) const;
	const size_t getSize() const;

private:
	struct Node
	{
		Node(const std::string _key, const T _value, Node* _next = nullptr);

		std::string key;
		T value;
		Node *next;
	};

	Node **table;
	size_t size;

	unsigned hashFunc(const std::string& key) const;
	//справяне с колизиите
	Node *locate(unsigned index, const std::string& key) const;
	
	void copy(const HashMap& rhs);
	void clear();
};

template<typename T>
inline HashMap<T>::Node::Node(const std::string _key, const T _value, Node* _next)
{
	key = _key;
	value = _value;
	next = _next;
}

template<typename T>
inline HashMap<T>::HashMap(const size_t _size):size(_size)
{
	table = new Node*[size];
	for (int i = 0; i < size; ++i)
	{
		table[i] = nullptr;
	}
}

template<typename T>
inline HashMap<T>::HashMap(const HashMap& rhs)
{
	copy(rhs);
}

template<typename T>
inline HashMap<T>& HashMap<T>::operator=(const HashMap& rhs)
{
	if (this != &rhs)
	{
		clear();
		copy(rhs);
	}
	return *this;
}

template<typename T>
inline HashMap<T>::~HashMap()
{
	clear();
}

template<typename T>
inline T & HashMap<T>::operator[](const std::string& key)
{
	size_t index = hashFunc(key) % size;

	typename HashMap<T>::Node *current = locate(index, key);

	if (current == nullptr)
	{
		current = table[index] = new Node{ key, T(),table[index] };
	}

	return current->value;

}

template<typename T>
inline bool HashMap<T>::hasKey(const std::string& key) const
{
	return locate(hashFunc(key) % size, key) != nullptr;
}

template<typename T>
inline const size_t HashMap<T>::getSize() const
{
	return size;
}

template<typename T>
inline unsigned HashMap<T>::hashFunc(const std::string& key) const
{
	unsigned hash = 6451;
	size_t len = key.size();
	for (size_t i = 0; i < len; i++)
	{
		hash = hash * 33 + key[i];
	}
	return hash;
}

template<typename T>
inline typename HashMap<T>::Node * HashMap<T>::locate(unsigned index, const std::string& key) const
{
	typename HashMap<T>::Node *current = table[index];
	while (current != nullptr && current->key != key)
	{
		current = current->next;
	}
	return current;
}

template<typename T>
inline void HashMap<T>::copy(const HashMap& rhs)
{
	this->size = rhs.getSize();
	this->table = new typename HashMap<T>::Node *[this->size];

	typename HashMap<T>::Node *otherCurrent = nullptr;
	typename HashMap<T>::Node *copyFirst = nullptr;
	typename HashMap<T>::Node *copyCurrent = nullptr;

	for (unsigned i = 0; i < size; ++i)
	{
		otherCurrent = rhs.table[i];
		if (otherCurrent == nullptr)
		{
			table[i] = nullptr;
			continue;
		}
		copyFirst = new typename HashMap<T>::Node(otherCurrent->key, otherCurrent->value);
		copyCurrent = copyFirst;
		while (otherCurrent->next != nullptr)
		{
			copyCurrent->next = new typename HashMap<T>::Node(otherCurrent->next->key, otherCurrent->next->value);
			copyCurrent = copyCurrent->next;
			otherCurrent = otherCurrent->next;
		}
		this->table[i] = copyFirst;
	}
}

template<typename T>
inline void HashMap<T>::clear()
{
	typename HashMap<T>::Node *line = nullptr;
	typename HashMap<T>::Node *prev = nullptr;
	for (unsigned i = 0; i < size; ++i)
	{
		line = table[i];

		while (line != nullptr)
		{
			prev = line;
			line = line->next;
			delete prev;
		}
		table[i] = nullptr;
	}

	delete[] table;
	size = 0;
	table = nullptr;
}