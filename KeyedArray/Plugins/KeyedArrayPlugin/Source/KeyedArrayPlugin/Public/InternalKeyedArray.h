#pragma once

#include "CoreTypes.h"

/**
 * This class is responsible for most of the logic required for Keyed Arrays.
 * Ideally, this class shouldn't exist but Unreal mean and doesn't allow generic USTRUCTs nor inheritance from non-USTRUCTs.
 */
template<typename KeyType, typename ValueType, typename PairType>
class TInternalKeyedArray
{
	typedef TArray<PairType> ArrayType;
	typedef TMap<KeyType, int32> MapType;
	typedef TTuple<KeyType, int32> KeyValuePairAsTuple;

protected:
	ArrayType* Array;
	MapType* Map;

	TArray<KeyType> OldKeys;

	
public:
	virtual ~TInternalKeyedArray() = default;

	TInternalKeyedArray()
	{
		Array = nullptr;
		Map = nullptr;
	}
	
	TInternalKeyedArray(ArrayType* NewArray, MapType* NewMap)
	{
		Array = NewArray;
		Map = NewMap;
	}


protected:
	FORCEINLINE int32 UpdateValue(const KeyType Key, const ValueType& Item)
	{
		// Doesn't do any checks so yeah.
		int32 Index = GetIndex(Key);
		(*Array)[Index] = PairType(Key, Item);
		return Index;
	}
	
	FORCEINLINE void AddToMap(const KeyType& Key, int32 Index)
	{
		Map->Add(Key, Index);
	}

	FORCEINLINE void RemoveFromMap(const KeyType& Key)
	{
		const int32 Index = (*Map)[Key];
		Map->Remove(Key);
		DecrementMap(Index + 1);
	}

	/** This operation is O(n) so don't use if not necessary. */
	FORCEINLINE void RemoveFromMap(int32 Index)
	{
		const KeyType* Key = Map->FindKey(Index);
		if (Key != nullptr)
			Map->Remove(*Key);
	}

	FORCEINLINE bool ContainsKey(const KeyType& Key) const
	{
		return Map->Contains(Key);
	}

public:
	virtual void IncrementMap(int32 StartingIndex)
	{
		// Don't bother looping if the index doesn't exist.
		if (!this->Array->IsValidIndex(StartingIndex))
			return;

		// Resort map.
		// For every key with a index above the inserted index, increment their index by 1.
		for (auto KeyValue : *this->Map)
			if (KeyValue.Value >= StartingIndex)
				(*this->Map)[KeyValue.Key] = KeyValue.Value + 1;
	}

	virtual void DecrementMap(int32 StartingIndex)
	{
		// Gotta -1 since the array is now one index smaller than it was.
		if (StartingIndex <= 0 || StartingIndex > this->Array->Num())
			return;
		
		// Resort map.
		// For every key with a index above the inserted index, decrement their index by 1.
		for (auto KeyValue : *this->Map)
			if (KeyValue.Value >= StartingIndex)
				(*this->Map)[KeyValue.Key] = KeyValue.Value - 1;
	}

	TArray<KeyType>& GetOldKeys()
	{
		return OldKeys;
	}

	const TArray<KeyType>& GetOldKeys() const
	{
		return OldKeys;
	}

	void OnKeyAdded(int32 Index, const KeyType Key)
	{
		AddToMap(Key, Index);
	}

	void OnKeyRemoved(int32 Index, const KeyType Key)
	{
		RemoveFromMap(Key, Index);
	}

	
public:
	FORCEINLINE int32 GetIndex(const KeyType& Key) const
	{
		if (Map->Contains(Key))
			return (*Map)[Key];

		return -1;
	}

	FORCEINLINE const KeyType* GetKey(int32 Index) const
	{
		return Map->FindKey(Index);
	}

	
	FORCEINLINE int32 Add(const KeyType Key, ValueType&& Item)
	{
		if (ContainsKey(Key))
			return UpdateValue(Key, Item);
		
		const int32 Index = Array->Add(PairType(Key, Item));
		AddToMap(Key, Index);
		return Index;
	}

	FORCEINLINE int32 Add(const KeyType Key, const ValueType& Item)
	{
		if (ContainsKey(Key))
			return UpdateValue(Key, Item);
		
		const int32 Index = Array->Add(PairType(Key, Item));
		AddToMap(Key, Index);
		return Index;
	}

	FORCEINLINE int32 Emplace(const KeyType Key, ValueType Item)
	{
		if (ContainsKey(Key))
			return UpdateValue(Key, Item);
		
		const int32 Index = Array->Emplace(PairType(Key, Item));
		AddToMap(Key, Index);
		return Index;
	}

	FORCEINLINE int32 EmplaceAt(const KeyType Key, ValueType Item, int32 Index)
	{
		// Todo Removal
		
		Array->EmplaceAt(Index, PairType(Key, Item));
		AddToMap(Key, Index);

		IncrementMap(Index + 1);
		
		return Index;
	}
	

	FORCEINLINE int32 Insert(const KeyType Key, const ValueType& Item, int32 Index)
	{
		if (ContainsKey(Key))
			return UpdateValue(Key, Item);
		
		Index = Array->Insert(PairType(Key, Item), Index);
		AddToMap(Key, Index);

		IncrementMap(Index + 1);
		
		return Index;
	}

	FORCEINLINE bool Remove(const KeyType Key)
	{
		int32 Index = GetIndex(Key);
		if (Array->IsValidIndex(Index))
		{
			Array->RemoveAt(Index);
			RemoveFromMap(Key);
			return true;
		}

		return false;
	}

	FORCEINLINE bool RemoveAt(int32 Index)
	{
		if (Array->IsValidIndex(Index))
		{
			Array->RemoveAt(Index);
			RemoveFromMap(Index);
			return true;
		}

		return false;
	}

	FORCEINLINE PairType& operator[](KeyType Key)
	{
		return (*Array)[(*Map)[Key]];
	}

	FORCEINLINE const PairType& operator[](KeyType Key) const
	{
		return (*Array)[(*Map)[Key]];
	}

	FORCEINLINE PairType& operator[](int32 Index)
	{
		return (*Array)[Index];
	}

	FORCEINLINE const PairType& operator[](int32 Index) const
	{
		return (*Array)[Index];
	}

	FORCEINLINE PairType* GetPairAsPointer(KeyType Key)
	{
		if (ContainsKey(Key))
			return &(*Array)[(*Map)[Key]];

		return nullptr;
	}

	FORCEINLINE PairType* GetPairAsPointer(int32 Index)
	{
		if (Array->IsValidIndex(Index))
			return &(*Array)[Index];

		return nullptr;
	}

	FORCEINLINE const PairType* GetPairAsPointer(KeyType Key) const
	{
		if (ContainsKey(Key))
			return &(*Array)[(*Map)[Key]];

		return nullptr;
	}

	FORCEINLINE const PairType* GetPairAsPointer(int32 Index) const
	{
		if (Array->IsValidIndex(Index))
			return &(*Array)[Index];

		return nullptr;
	}

	FORCEINLINE bool Contains(KeyType Key) const
	{
		return ContainsKey(Key);
	}

	
	FORCEINLINE PairType& Last(int32 IndexFromTheEnd = 0)
	{
		return Array->Last(IndexFromTheEnd);
	}

	FORCEINLINE const PairType& Last(int32 IndexFromTheEnd = 0) const
	{
		return Array->Last(IndexFromTheEnd);
	}
	

	FORCEINLINE void Empty(int32 AllocatedElements)
	{
		Array->Empty(AllocatedElements);
		Map->Empty(AllocatedElements);
	}
};
