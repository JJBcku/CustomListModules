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
		_vectorID = std::numeric_limits<IDType>::lowest();

		if (initialCapacity != 0)
		{
			_list.reserve(initialCapacity);
			_deletedList.reserve(initialCapacity);
		}
	}

	~UnsortedList()
	{

	}

	UnsortedList(const UnsortedList<T>&) noexcept = delete;
	UnsortedList(UnsortedList<T>&&) noexcept = default;

	UnsortedList<T>& operator=(const UnsortedList<T>&) noexcept = delete;
	UnsortedList<T>& operator=(UnsortedList<T>&&) noexcept = default;

	IDObject<T> AddUniqueObject(const T& value, size_t addOnReserve)
	{
		auto found = std::find(_list.cbegin(), _list.cend(), value);

		if (found != _list.cend())
		{
			return found->GetObjectID();
		}
		else
		{
			if (!_deletedList.empty())
			{
				size_t pos = _deletedList.back();
				_list[pos].ReplaceValue(IDObject<T>(GetNextId(), _vectorID), value);
				_deletedList.pop_back();
				return _list[pos].GetObjectID();
			}
			else
			{
				CheckCapacity(addOnReserve);

				_list.emplace_back(IDObject<T>(GetNextId(), _vectorID), value);
				return _list.back().GetObjectID();
			}
		}
	}

	IDObject<T> AddUniqueObject(T&& value, size_t addOnReserve)
	{
		auto found = std::find(_list.cbegin(), _list.cend(), value);

		if (found != _list.cend())
		{
			return found->GetObjectID();
		}
		else
		{
			if (!_deletedList.empty())
			{
				size_t pos = _deletedList.back();
				_list[pos].ReplaceValue(IDObject<T>(GetNextId(), _vectorID), std::move(value));
				_deletedList.pop_back();
				return _list[pos].GetObjectID();
			}
			else
			{
				CheckCapacity(addOnReserve);

				_list.emplace_back(IDObject<T>(GetNextId(), _vectorID), std::move(value));
				return _list.back().GetObjectID();
			}
		}
	}

	IDObject<T> AddObject(const T& value, size_t addOnReserve)
	{
		if (!_deletedList.empty())
		{
			size_t pos = _deletedList.back();
			_list[pos].ReplaceValue(IDObject<T>(GetNextId(), _vectorID), value);
			_deletedList.pop_back();
			return _list[pos].GetObjectID();
		}
		else
		{
			CheckCapacity(addOnReserve);

			_list.emplace_back(IDObject<T>(GetNextId(), _vectorID), value);
			return _list.back().GetObjectID();
		}
	}

	IDObject<T> AddObject(T&& value, size_t addOnReserve)
	{
		if (!_deletedList.empty())
		{
			size_t pos = _deletedList.back();
			_list[pos].ReplaceValue(IDObject<T>(GetNextId(), _vectorID), std::move(value));
			_deletedList.pop_back();
			return _list[pos].GetObjectID();
		}
		else
		{
			CheckCapacity(addOnReserve);

			_list.emplace_back(IDObject<T>(GetNextId(), _vectorID), std::move(value));
			return _list.back().GetObjectID();
		}
	}

	bool RemoveObject(IDObject<T> objectID, bool throwOnIDNotFound)
	{
		auto it = std::find(_list.begin(), _list.end(), objectID);

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

	std::optional<T>& GetObjectOptional(IDObject<T> objectID)
	{
		auto it = std::find(_list.begin(), _list.end(), objectID);

		if (it == _list.end())
			throw std::runtime_error("UnsortedList GetObject Error: Program tried to get non-existent object!");

		return it->GetObjectOptional();
	}

	const std::optional<T>& GetConstObjectOptional(IDObject<T> objectID) const
	{
		auto it = std::find(_list.begin(), _list.end(), objectID);

		if (it == _list.end())
			throw std::runtime_error("UnsortedList GetConstObject Error: Program tried to get non-existent object!");

		return it->GetConstObjectOptional();
	}

	std::optional<T> GetObjectOptionalCopy(IDObject<T> objectID) const
	{
		auto it = std::find(_list.begin(), _list.end(), objectID);

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
				if (_list[i] == IDList[j])
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
		auto it = std::find(_list.begin(), _list.end(), objectID);

		if (it == _list.end())
			throw std::runtime_error("UnsortedList GetObject Error: Program tried to get non-existent object!");

		return it->GetObject();
	}

	const T& GetConstObject(IDObject<T> objectID) const
	{
		auto it = std::find(_list.cbegin(), _list.cend(), objectID);

		if (it == _list.cend())
			throw std::runtime_error("UnsortedList GetObject Error: Program tried to get non-existent object!");

		return it->GetConstObject();
	}

	T GetObjectCopy(IDObject<T> objectID) const
	{
		auto it = std::find(_list.begin(), _list.end(), objectID);

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
				if (_list[i] == IDList[j])
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

		if (_vectorID == std::numeric_limits<IDType>::max())
			throw std::runtime_error("UnsortedList Reset Error: vector ID overflow!");

		_vectorID++;

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