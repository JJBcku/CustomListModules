export module ListTemplates.CommonVectorObject;

import std;
export import ListTemplates.IDObject;

export template<class T>
class CommonVectorObject
{
public:
	CommonVectorObject(const IDObject<T>& objectID, const T& object) : _objectID(objectID), _object(object)
	{
	}

	CommonVectorObject(const IDObject<T>& objectID, T&& object) noexcept : _objectID(objectID), _object(std::move(object))
	{
	}

	CommonVectorObject(const CommonVectorObject<T>& other) noexcept : _objectID(other._objectID), _object(other._object)
	{
	}

	CommonVectorObject(CommonVectorObject<T>&& other) noexcept : _objectID(std::move(other._objectID)), _object(std::move(other._object))
	{
	}

	CommonVectorObject<T>& operator=(const CommonVectorObject<T>& other)
	{
		_objectID = other._objectID;
		_object = other._object;
	}

	CommonVectorObject<T>& operator=(CommonVectorObject<T>&& other)
	{
		_objectID = std::move(other._objectID);
		_object = std::move(other._object);
	}

	void ReplaceValue(const IDObject<T>& objectID, const T& object)
	{
		if (_object.has_value())
			throw std::runtime_error("ListObjectTemplate Error: Program tried to replace existing value with const object!");

		_object = object;
		_objectID = objectID;
	}

	void ReplaceValue(const IDObject<T>& objectID, T&& object)
	{
		if (_object.has_value())
			throw std::runtime_error("ListObjectTemplate Error: Program tried to replace existing value with moved object!");

		_object = std::move(object);
		_objectID = objectID;
	}

	IDObject<T> GetObjectID() const { return _objectID; }

	std::optional<T>& GetObjectOptional() { return _object; }
	const std::optional<T>& GetConstObjectOptional() const { return _object; }
	std::optional<T> GetObjectOptionalCopy() const { return _object; }

	T& GetObject() { return _object.value(); }
	const T& GetConstObject() const { return _object.value(); }
	T GetObjectCopy() const { return _object.value(); }

	bool HasValue() const { return _object.has_value(); }

	void DeleteObject() { _object.reset(); }

	bool operator==(bool has_value) const { return _object.has_value() == has_value; }
	bool operator==(const IDObject<T>& ID) const { return ID == _objectID; }

	std::strong_ordering operator<=>(const CommonVectorObject<T>&) const noexcept = default;
	bool operator==(const CommonVectorObject<T>&) const noexcept = default;

	bool operator==(const T& other) const noexcept { return _object == other; };
	bool operator==(const std::optional<T>& other) const noexcept { return _object == other; };

private:
	IDObject<T> _objectID;
	std::optional<T> _object;
	char _padding[16 - (sizeof(_object) % 8)];
};
