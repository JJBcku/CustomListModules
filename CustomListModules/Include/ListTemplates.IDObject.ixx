module;

export module ListTemplates.IDObject;

import std;

export typedef std::uint64_t IDType;

export template<class T>
class IDSubobject
{
private:
	IDType _subID;

public:
	IDSubobject(IDType subID = std::numeric_limits<IDType>::max()) : _subID(subID) {};
	IDSubobject(const IDSubobject& rhs) = default;
	IDSubobject(IDSubobject&& rhs) = default;

	IDSubobject<T>& operator=(const IDSubobject<T>& rhs) = default;
	IDSubobject<T>& operator=(IDSubobject<T>&& rhs) = default;

	bool operator==(const IDSubobject<T>& rhs) const noexcept = default;
	std::strong_ordering operator<=>(const IDSubobject<T>& rhs) const noexcept = default;

	bool operator==(const IDType& rhs) const noexcept { return _subID == rhs; };
	std::strong_ordering operator<=>(const IDType& rhs) const noexcept { return _subID <=> rhs; };
};

export template<class T>
class IDObject
{
private:
	IDSubobject<T> _objectID;
	IDSubobject<T> _vectorID;

public:
	IDObject(IDType objectID = std::numeric_limits<IDType>::max(), IDType vectorID = std::numeric_limits<IDType>::max()) : _objectID(objectID), _vectorID(vectorID) {}
	IDObject(IDType objectID, IDSubobject<T> vectorID) : _objectID(objectID), _vectorID(vectorID) {}
	IDObject(IDSubobject<T> objectID, size_t vectorID = std::numeric_limits<IDType>::max()) : _objectID(objectID), _vectorID(vectorID) {}
	IDObject(IDSubobject<T> objectID, IDSubobject<T> vectorID) : _objectID(objectID), _vectorID(vectorID) {}

	IDObject(const IDObject& other) = default;
	IDObject(IDObject&& other) noexcept = default;

	IDObject<T>& operator=(const IDObject<T>& other) = default;
	IDObject<T>& operator=(IDObject<T>&&) noexcept = default;

	bool operator==(const IDObject<T>&) const noexcept = delete;
	std::strong_ordering operator<=>(const IDObject<T>&) const noexcept = delete;

	const IDSubobject<T>& GetObjectID() const { return _objectID; }
	const IDSubobject<T>& GetVectorID() const { return _vectorID; }
};