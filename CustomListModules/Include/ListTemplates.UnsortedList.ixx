module;

export module ListTemplates.UnsortedList;

import std;
export import ListTemplates.IDObject;
import ListTemplates.CommonVectorObject;

export template<class T>
class UnsortedList
{
public:
	UnsortedList(size_t initialCapacity)
	{
		_nextID = std::numeric_limits<IDType>::lowest();
		_vectorID = GetNextVectorID();

		if (initialCapacity != 0)
		{
			_list.reserve(initialCapacity);
			_deletedList.reserve(initialCapacity);
		}

		for (size_t i = 0; i < sizeof(_padding); i++)
			_padding[i] = 0;
	}

	~UnsortedList()
	{

	}

	UnsortedList(const UnsortedList<T>&) noexcept = default;
	UnsortedList(UnsortedList<T>&&) noexcept = default;

	UnsortedList<T>& operator=(const UnsortedList<T>&) noexcept = default;
	UnsortedList<T>& operator=(UnsortedList<T>&&) noexcept = default;

	IDObject<T> AddUniqueObject(const T& value, size_t addOnReserve)
	{
		auto found = std::find(_list.cbegin(), _list.cend(), value);

		if (found != _list.cend())
		{
			return IDObject<T>(found->GetObjectID(), _vectorID);
		}
		else
		{
			if (!_deletedList.empty())
			{
				size_t pos = _deletedList.back();
				_list[pos].ReplaceValue(IDSubobject<T>(GetNextId()), value);
				_deletedList.pop_back();
				return IDObject<T>(_list[pos].GetObjectID(), _vectorID);
			}
			else
			{
				CheckCapacity(addOnReserve);

				_list.emplace_back(IDSubobject<T>(GetNextId()), value);
				return IDObject<T>(_list.back().GetObjectID(), _vectorID);
			}
		}
	}

	IDObject<T> AddUniqueObject(T&& value, size_t addOnReserve)
	{
		auto found = std::find(_list.cbegin(), _list.cend(), value);

		if (found != _list.cend())
		{
			return IDObject<T>(found->GetObjectID(), _vectorID);
		}
		else
		{
			if (!_deletedList.empty())
			{
				size_t pos = _deletedList.back();
				_list[pos].ReplaceValue(IDSubobject<T>(GetNextId()), std::move(value));
				_deletedList.pop_back();
				return IDObject<T>(_list[pos].GetObjectID(), _vectorID);
			}
			else
			{
				CheckCapacity(addOnReserve);

				_list.emplace_back(IDSubobject<T>(GetNextId()), std::move(value));
				return IDObject<T>(_list.back().GetObjectID(), _vectorID);
			}
		}
	}

	IDObject<T> AddObject(const T& value, size_t addOnReserve)
	{
		if (!_deletedList.empty())
		{
			size_t pos = _deletedList.back();
			_list[pos].ReplaceValue(IDSubobject<T>(GetNextId()), value);
			_deletedList.pop_back();
			return IDObject<T>(_list[pos].GetObjectID(), _vectorID);
		}
		else
		{
			CheckCapacity(addOnReserve);

			_list.emplace_back(IDSubobject<T>(GetNextId()), value);
			return IDObject<T>(_list.back().GetObjectID(), _vectorID);
		}
	}

	IDObject<T> AddObject(T&& value, size_t addOnReserve)
	{
		if (!_deletedList.empty())
		{
			size_t pos = _deletedList.back();
			_list[pos].ReplaceValue(IDSubobject<T>(GetNextId()), std::move(value));
			_deletedList.pop_back();
			return IDObject<T>(_list[pos].GetObjectID(), _vectorID);
		}
		else
		{
			CheckCapacity(addOnReserve);

			_list.emplace_back(IDSubobject<T>(GetNextId()), std::move(value));
			return IDObject<T>(_list.back().GetObjectID(), _vectorID);
		}
	}

	bool RemoveObject(IDObject<T> objectID, bool throwOnIDNotFound)
	{
		if (objectID.GetVectorID() != _vectorID)
			throw std::runtime_error("UnsortedList RemoveObject Error: Program tried to user an ID from another instance of this list!");

		auto it = std::find(_list.begin(), _list.end(), objectID.GetObjectID());

		if (it == _list.cend())
		{
			if (throwOnIDNotFound)
				throw std::runtime_error("UnsortedList: Program tried to delete a non-existent entry in a list!");
			else
				return false;
		}
		else
		{
			if (it->HasValue())
			{
				it->DeleteObject();
				_deletedList.push_back(static_cast<size_t>(std::distance(_list.begin(), it)));
			}
			return true;
		}
	}

	size_t GetSize() const { return _list.size(); }
	size_t GetUsedSize() const
	{
		size_t ret = _list.size() - _deletedList.size();

		if (ret > _list.size())
			throw std::runtime_error("UnsortedList GetUsedSize Error: returned value underflowed!");

		return ret;
	}

	size_t GetCapacity() const { return _list.capacity(); }
	size_t GetUnusedCapacity() const
	{
		size_t ret = _list.capacity() - _list.size();

		if (ret > _list.capacity())
			throw std::runtime_error("UnsortedList GetUnusedCapacity Error: returned value underflowed!");

		return ret;
	}

	size_t GetUnusedAndDeletedCapacity() const
	{
		size_t ret = GetUnusedCapacity() + _deletedList.size();

		if (ret < _deletedList.size())
			throw std::runtime_error("UnsortedList GetUnusedAndDeletedCapacity Error: returned value overflowed!");

		return ret;
	}

	void ShrinkToFit(size_t changeToCapacity, bool addToCapacity) noexcept(std::is_nothrow_move_constructible_v<T> || std::is_nothrow_copy_constructible_v<T>)
	{
		if (_deletedList.empty())
			return;

		_list.shrink_to_fit();

		std::vector<CommonVectorObject<T>> tempList;

		if (addToCapacity)
		{
			size_t fullres = GetUsedSize() + changeToCapacity;

			if (fullres < changeToCapacity)
				throw std::runtime_error("UnsortedList ShrinkToFit Error: reservation amount overflowed!");

			tempList.reserve(fullres);
		}
		else
		{
			size_t usedSize = GetUsedSize();
			if (changeToCapacity > usedSize)
			{
				tempList.reserve(changeToCapacity);
			}
			else
			{
				tempList.reserve(usedSize);
			}
		}

		for (auto& object : _list)
		{
			if (object.HasValue())
			{
				if constexpr (std::is_nothrow_move_constructible_v<T>)
				{
					tempList.push_back(std::move(object));
				}
				else
				{
					if constexpr (std::is_nothrow_copy_constructible_v<T>)
					{
						tempList.push_back(object);
					}
					else
					{
						if constexpr (std::is_move_constructible_v<T>)
						{
							tempList.push_back(std::move(object));
						}
						else
						{
							tempList.push_back(object);
						}
					}
				}
			}
		}

		_list = std::move(tempList);
		_deletedList.clear();
		_deletedList.shrink_to_fit();
		_deletedList.reserve(_list.capacity());
	}

	bool CheckForID(IDObject<T> objectID) const
	{
		if (objectID.GetVectorID() != _vectorID)
			throw std::runtime_error("UnsortedList CheckForID Error: Program tried to user an ID from another instance of this list!");

		auto it = std::find(_list.cbegin(), _list.cend(), objectID.GetObjectID());

		return it != _list.cend();
	}

	std::optional<T>& GetObjectOptional(IDObject<T> objectID)
	{
		if (objectID.GetVectorID() != _vectorID)
			throw std::runtime_error("UnsortedList GetObjectOptional Error: Program tried to user an ID from another instance of this list!");

		auto it = std::find(_list.begin(), _list.end(), objectID.GetObjectID());

		if (it == _list.end())
			throw std::runtime_error("UnsortedList GetObject Error: Program tried to get non-existent object!");

		return it->GetObjectOptional();
	}

	const std::optional<T>& GetConstObjectOptional(IDObject<T> objectID) const
	{
		if (objectID.GetVectorID() != _vectorID)
			throw std::runtime_error("UnsortedList GetConstObjectOptional Error: Program tried to user an ID from another instance of this list!");

		auto it = std::find(_list.begin(), _list.end(), objectID.GetObjectID());

		if (it == _list.end())
			throw std::runtime_error("UnsortedList GetConstObject Error: Program tried to get non-existent object!");

		return it->GetConstObjectOptional();
	}

	std::optional<T> GetObjectOptionalCopy(IDObject<T> objectID) const
	{
		if (objectID.GetVectorID() != _vectorID)
			throw std::runtime_error("UnsortedList GetObjectOptionalCopy Error: Program tried to user an ID from another instance of this list!");

		auto it = std::find(_list.begin(), _list.end(), objectID.GetObjectID());

		if (it == _list.end())
			throw std::runtime_error("UnsortedList GetObjectCopy Error: Program tried to get non-existent object!");

		return it->GetObjectOptionalCopy();
	}

	std::vector<std::optional<T>> GetObjectOptionalList(std::vector<IDObject<T>> IDList) const
	{
		std::vector<std::optional<T>> ret;
		ret.reserve(IDList.size());

		for (size_t i = 0; i < _list.size(); ++i)
		{
			for (size_t j = 0; j < IDList.size(); ++j)
			{
				if (IDList[j].GetVectorID() != _vectorID)
					throw std::runtime_error("UnsortedList GetObjectOptionalList Error: Program tried to user an ID from another instance of this list!");

				if (_list[i] == IDList[j].GetObjectID())
				{
					ret.push_back(_list[i].GetObjectOptionalCopy());

					if (ret.size() >= IDList.size())
						break;
				}
			}

			if (ret.size() >= IDList.size())
				break;
		}

		return ret;
	}

	T& GetObject(IDObject<T> objectID)
	{
		if (objectID.GetVectorID() != _vectorID)
			throw std::runtime_error("UnsortedList GetObject Error: Program tried to user an ID from another instance of this list!");

		auto it = std::find(_list.begin(), _list.end(), objectID.GetObjectID());

		if (it == _list.end())
			throw std::runtime_error("UnsortedList GetObject Error: Program tried to get non-existent object!");

		return it->GetObject();
	}

	const T& GetConstObject(IDObject<T> objectID) const
	{
		if (objectID.GetVectorID() != _vectorID)
			throw std::runtime_error("UnsortedList GetConstObject Error: Program tried to user an ID from another instance of this list!");

		auto it = std::find(_list.cbegin(), _list.cend(), objectID.GetObjectID());

		if (it == _list.cend())
			throw std::runtime_error("UnsortedList GetObject Error: Program tried to get non-existent object!");

		return it->GetConstObject();
	}

	T GetObjectCopy(IDObject<T> objectID) const
	{
		if (objectID.GetVectorID() != _vectorID)
			throw std::runtime_error("UnsortedList GetObjectCopy Error: Program tried to user an ID from another instance of this list!");

		auto it = std::find(_list.begin(), _list.end(), objectID.GetObjectID());

		if (it == _list.end())
			throw std::runtime_error("UnsortedList GetObject Error: Program tried to get non-existent object!");

		return it->GetObjectCopy();
	}

	std::vector<T> GetObjectList(std::vector<IDObject<T>> IDList) const
	{
		std::vector<T> ret;
		ret.reserve(IDList.size());

		for (size_t i = 0; i < _list.size(); ++i)
		{
			for (size_t j = 0; j < IDList.size(); ++j)
			{
				if (IDList[j].GetVectorID() != _vectorID)
					throw std::runtime_error("UnsortedList GetObjectList Error: Program tried to user an ID from another instance of this list!");

				if (_list[i] == IDList[j].GetObjectID())
				{
					ret.push_back(_list[i].GetObjectCopy());

					if (ret.size() >= IDList.size())
						break;
				}
			}

			if (ret.size() >= IDList.size())
				break;
		}

		return ret;
	}

	void Reset(size_t capacityAfterReset)
	{
		_list.clear();
		_deletedList.clear();

		_list.shrink_to_fit();
		_deletedList.shrink_to_fit();

		_vectorID = GetNextVectorID();

		if (capacityAfterReset != 0)
		{
			_list.reserve(capacityAfterReset);
			_deletedList.reserve(capacityAfterReset);
		}
	}

protected:
	IDType _nextID;
	IDType _vectorID;
	std::vector<CommonVectorObject<T>> _list;
	std::vector<size_t> _deletedList;
	char _padding[16 - (((sizeof(_deletedList) << 1) + (sizeof(_nextID) << 1)) % 8)];

	static IDType _nextVectorID;

	static IDType GetNextVectorID()
	{
		if (_nextVectorID == std::numeric_limits<IDType>::max())
			throw std::runtime_error("UnsortedList::GetNextVectorID Error: Vector ID overflow!");

		return _nextVectorID++;
	}

	IDType GetNextId()
	{
		IDType ret = _nextID;
		if (_nextID == std::numeric_limits<IDType>::max())
			throw std::runtime_error("UnsortedList Error: Id system overflowed!");

		_nextID++;
		return ret;
	}

	void ReserveAdditional(size_t addToCapacity)
	{
		auto size = _list.capacity();

		_list.reserve(size + addToCapacity);
		_deletedList.reserve(size + addToCapacity);
	}

	void CheckCapacity(size_t addOnReserve)
	{
		if (_list.capacity() == _list.size())
		{
			if (addOnReserve == 0)
			{
				size_t capacity = _list.capacity();
				if (capacity != 0)
					ReserveAdditional(_list.capacity());
				else
					ReserveAdditional(1);
			}
			else
			{
				ReserveAdditional(addOnReserve);
			}
		}
	}
};

template<class T>
IDType UnsortedList<T>::_nextVectorID = std::numeric_limits<IDType>::lowest();
