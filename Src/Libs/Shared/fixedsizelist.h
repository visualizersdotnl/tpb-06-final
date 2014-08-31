#pragma once


template <class T>
class FixedSizeList
{
private:
	T* items;
	int nAllocated;
	int nResident;

public:
	// construct
	FixedSizeList(int nInitialAllocated)
		: nResident(0), nAllocated(nInitialAllocated), items(0)
	{
		items = new T[nInitialAllocated];
	}

	// copy constructor
	FixedSizeList(const FixedSizeList<T>& other)
	{
		nAllocated = other.nAllocated;
		nResident = other.nResident;

		items = new T[nAllocated];

		for (int i=0; i<other.nResident; ++i)
			items[i] = other.items[i];
	}


	// copy operator
	FixedSizeList<T>& operator =(const FixedSizeList<T>& other)
	{
		ASSERT_MSG(nAllocated >= other.nResident, "Cannot copy items from other list, not enough free space allocated.");

		for (int i=0; i<other.nResident; ++i)
			items[i] = other.items[i];
		
		nResident = other.nResident;

		return *this;
	}


	// destruct
	~FixedSizeList()
	{
		delete [] items;
	}

	// clear list
	void Clear()
	{
		nResident = 0;
	}

	// see if we're empty or not
	bool IsEmpty() const
	{
		return nResident == 0;
	}

	// retrieve resident size (number of resident items)
	int Size() const
	{
		return nResident;
	}

	// retrieve allocated size (number of allocated items)
	int SizeAllocated() const
	{
		return nAllocated;
	}

	// add an item
	void Add(const T& item)
	{
		ASSERT_MSG(nResident < nAllocated, "Cannot add one more item to fixedsizelist. The list is full!");

		items[nResident++] = item;
	}

	// add multiple items at once
	void AddMultiple(const T* newItems, int numItems)
	{
		ASSERT_MSG(nResident+numItems <= nAllocated, "Cannot add more items to fixedsizelist. The list is full!");

		while (numItems-- > 0)
			items[nResident++] = *(newItems++);
	}

	void AddMultipleNoninitialized(int numItems)
	{
		ASSERT_MSG(nResident+numItems <= nAllocated, "Cannot add more items to fixedsizelist. The list is full!");
		nResident += numItems;
	}

	// remove an item (if it exists),
	// by moving the last item in the list to this item's position
	void Remove(const T& item)
	{
		for (int i=0; i<nResident; ++i)
			if (items[i] == item)
			{
				if (nResident-1 > i)
					// move last item in list to this position
					items[i] = items[nResident-1];

				--nResident;

				return;
			}
	}

	// remove an item at a certain index
	// by moving the last item in the list to this position
	void RemoveAtIndex(int index)
	{
		if (nResident > 0)
		{
			if (nResident-1 > index)
				// move last item in list to this position
				items[index] = items[nResident-1];

			--nResident;
		}
	}

	// remove an item at a certain index
	// by maintaining the ordering
	void RemoveOrderedAtIndex(int index)
	{
		ASSERT_MSG((index >= 0) && (index < nResident), "Incorrect index for removal!");

		for (int i=index; i<nResident-1; ++i)
		{
			items[i] = items[i+1];
		}

		nResident--;
	}

	// remove all occurrences of item
	void RemoveAll(const T& item)
	{
		for (int i=nResident-1; i>=0; --i)
			if (items[i] == item)
			{
				// move last item in list to this position
				items[i] = items[nResident-1];
				--nResident;
			}
	}

	// remove multiple items at a certain index
	// by maintaining the ordering
	void RemoveOrderedAtIndex(int index, int count)
	{
		ASSERT_MSG((index >= 0) && (index < nResident), "Incorrect index for removal!");

		for (int i=index; i<nResident-count; ++i)
		{
			items[i] = items[i+count];
		}

		nResident-=count;
	}


	// insert an item, and preserve the order
	void Insert(int index, const T& item)
	{
		ASSERT_MSG((index >= 0) && (index <= nResident), "Incorrect index for insertion!");
		ASSERT_MSG(nResident < nAllocated, "Cannot add one more item to fixedsizelist. The list is full!");

		for (int i=nResident; i>index; i--)
			items[i] = items[i-1];

		items[index] = item;
		nResident++;
	}
	
	// retrieve one element
	T& operator[](int index)
	{
		ASSERT_MSG((index >= 0) && (index < nResident), "Index out of bounds!");

		return items[index];
	}

	// retrieve one const element
	const T& operator[](int index) const
	{
		ASSERT_MSG((index >= 0) && (index < nResident), "Index out of bounds!");

		return items[index];
	}


	
	T* GetItemsPtr() const
	{
		return items;
	}

	T* GetItemPtr(int index) const
	{
		ASSERT_MSG((index >= 0) && (index < nResident), "Index out of bounds!");

		return &items[index];
	}

	// Does this list contain the given item?
	bool Contains(const T& item) const
	{
		for (int i=0; i<nResident; ++i)
			if (items[i] == item)
				return true;

		return false;
	}

	// Return index of first occurance of this item in the list. Returns -1 when not found
	int Find(const T& item) const
	{
		for (int i=0; i<nResident; ++i)
			if (items[i] == item)
				return i;

		return -1;
	}

	// Is the given index a valid one?
	bool IsValidIndex(int index) const
	{
		return index >= 0 && index < nResident;
	}
};
