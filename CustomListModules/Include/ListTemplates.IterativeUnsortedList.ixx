export module ListTemplates.IterativeUnsortedList;

export import ListTemplates.UnsortedList;
import ListTemplates.CommonVectorObject;
import ListTemplates.IDObject;
import std;

export template<class T>
class IterativeUnsortedList : public UnsortedList<T>
{
public:
	IterativeUnsortedList(size_t reserve) : UnsortedList<T>(reserve) {}

	IterativeUnsortedList(const IterativeUnsortedList<T>&) noexcept = default;
	IterativeUnsortedList(IterativeUnsortedList<T>&&) noexcept = default;

	IterativeUnsortedList& operator=(const IterativeUnsortedList<T>&) noexcept = default;
	IterativeUnsortedList& operator=(IterativeUnsortedList<T>&&) noexcept = default;

	std::optional<T>& GetObjectOptional(size_t index)
	{
		if (index >= UnsortedList<T>::_list.size())
			throw std::runtime_error("IterativeUnsortedList::GetObjectOptional Error: Program tried to read data from outside of the list!");

		return UnsortedList<T>::_list[index].GetObjectOptional();
	}

	std::optional<T>& GetObjectOptional(IDObject<T> ID)
	{
		return UnsortedList<T>::GetObjectOptional(ID);
	}

	const std::optional<T>& GetConstObjectOptional(size_t index) const
	{
		if (index >= UnsortedList<T>::_list.size())
			throw std::runtime_error("IterativeUnsortedList::GetObjectOptional Error: Program tried to read data from outside of the list!");

		return UnsortedList<T>::_list[index].GetConstObjectOptional();
	}

	const std::optional<T>& GetConstObjectOptional(IDObject<T> ID) const
	{
		return UnsortedList<T>::GetConstObjectOptional(ID);
	}

	std::optional<T> GetObjectOptionalCopy(size_t index) const
	{
		if (index >= UnsortedList<T>::_list.size())
			throw std::runtime_error("IterativeUnsortedList::GetObjectOptional Error: Program tried to read data from outside of the list!");

		return UnsortedList<T>::_list[index].GetObjectOptionalCopy();
	}

	std::optional<T> GetObjectOptionalCopy(IDObject<T> ID) const
	{
		return UnsortedList<T>::GetObjectOptionalCopy(ID);
	}

	T& GetObject(size_t index)
	{
		if (index >= UnsortedList<T>::_list.size())
			throw std::runtime_error("IterativeUnsortedList::GetObject Error: Program tried to read data from outside of the list!");

		return UnsortedList<T>::_list[index].GetObject();
	}

	T& GetObject(IDObject<T> ID)
	{
		return UnsortedList<T>::GetObject(ID);
	}

	const T& GetConstObject(size_t index) const
	{
		if (index >= UnsortedList<T>::_list.size())
			throw std::runtime_error("IterativeUnsortedList::GetConstObject Error: Program tried to read data from outside of the list!");

		return UnsortedList<T>::_list[index].GetConstObject();
	}

	const T& GetConstObject(IDObject<T> ID) const
	{
		return UnsortedList<T>::GetConstObject(ID);
	}

	T GetObjectCopy(size_t index) const
	{
		if (index >= UnsortedList<T>::_list.size())
			throw std::runtime_error("IterativeUnsortedList::GetObjectCopy Error: Program tried to read data from outside of the list!");

		return UnsortedList<T>::_list[index].GetObjectCopy();
	}

	T GetObjectCopy(IDObject<T> ID) const
	{
		return UnsortedList<T>::GetObjectCopy(ID);
	}
};