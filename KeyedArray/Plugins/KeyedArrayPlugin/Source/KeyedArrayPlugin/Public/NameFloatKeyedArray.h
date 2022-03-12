#pragma once

#include "CoreTypes.h"
#include "InternalKeyedArray.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Net/UnrealNetwork.h"
#include "NameFloatKeyedArray.generated.h"


USTRUCT(BlueprintType)
struct FNameFloatPair
{
	GENERATED_BODY()
		
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName Key;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Value;

	FNameFloatPair()
	{
		Key = FName();
		Value = 0;
	}

	FNameFloatPair(FName NewKey, float NewValue)
	{
		Key = NewKey;
		Value = NewValue;
	}
};


USTRUCT(BlueprintType)
struct FNameFloatKeyedArray
{
	GENERATED_BODY()

public:
	typedef FName KeyType;
	typedef float ValueType;
	typedef FNameFloatPair PairType;

protected:
	TInternalKeyedArray<KeyType, ValueType, PairType> Internal;
	
	UPROPERTY(EditAnywhere)
	TArray<FNameFloatPair> BackingPairs;
	
	TMap<KeyType, int32> Translator;
	
public:
	FNameFloatKeyedArray()
	{
		Internal = TInternalKeyedArray<KeyType, ValueType, PairType>(&BackingPairs, &Translator);
	}

public:
	/** Call this whenever the array is modified on clients (i.e. OnRep_KeyedArray).
	 *  It ensures the Map responsible for allowing key-based access is always up-to-date.
	 */
	bool Clean()
	{
		// I am not sure how expensive it is to empty a map and rebuild but alternate solutions require quite a bit
		// of looping and I feel like that looping may end up being significantly more expensive.

		// Under normal circumstances, it will usually be the values that change, not the keys.
		
		// Don't bother rebuilding if they keys haven't changed.
		if (Translator.Num() == BackingPairs.Num())
		{
			for (int32 i = 0; i < BackingPairs.Num(); i++)
			{
				KeyType Key = BackingPairs[i].Key;
				if (Translator.Contains(Key))
				{
					if (Translator[Key] != i)
					{
						// Index for key has changed = dirty.
						Rebuild();
						return true;
					}
				}
				else
				{
					// Key cannot be found = new = dirty.
					Rebuild();
					return true;
				}
			}

			return false;
		}

		Rebuild();
		return true;
	}

	/**
	 * Forcefully refreshes the map based entirely on the array data. This is expensive as it's O(n).
	 */
	void Rebuild()
	{
		Translator.Empty(BackingPairs.Num());

		for (int32 i = 0; i < BackingPairs.Num(); i++)
		{
			Translator.Add(BackingPairs[i].Key, i);
		}
	}

	
public:
	FORCEINLINE int32 Add(const KeyType Key, ValueType&& Item)
	{
		return Internal.Add(Key, Item);
	}

	FORCEINLINE int32 Add(const KeyType Key, const ValueType& Item)
	{
		return Internal.Add(Key, Item);
	}
	
	FORCEINLINE int32 Emplace(const KeyType Key, ValueType Item)
	{
		return Internal.Emplace(Key, Item);
	}

	FORCEINLINE int32 EmplaceAt(const KeyType Key, ValueType Item, int32 Index)
	{
		return Internal.EmplaceAt(Key, Item, Index);
	}
	

	FORCEINLINE int32 Insert(const KeyType Key, const ValueType& Item, int32 Index)
	{
		return Internal.Insert(Key, Item, Index);
	}

	FORCEINLINE bool Remove(const KeyType Key)
	{
		return Internal.Remove(Key);
	}

	FORCEINLINE int32 RemoveFirst(const ValueType& Item)
	{
		for (int32 i = 0; i < Num(); i++)
		{
			if (BackingPairs[i].Value == Item)
			{
				RemoveAt(i);
				return i;
			}
		}

		return -1;
	}

	FORCEINLINE int32 RemoveAll(const ValueType& Item)
	{
		int32 Removals = 0;
		for (int32 i = Num(); i > 0; i--)
		{
			if (BackingPairs[i].Value == Item)
			{
				RemoveAt(i);
				Removals++;
			}
		}

		return Removals;
	}

	FORCEINLINE bool RemoveAt(int32 Index)
	{
		return Internal.RemoveAt(Index);
	}
	

	FORCEINLINE PairType& GetPair(int32 Index)
	{
		return Internal[Index];
	}

	FORCEINLINE const PairType& GetPair(int32 Index) const
	{
		return Internal[Index];
	}

	FORCEINLINE PairType& GetPair(KeyType Key)
	{
		return Internal[Key];
	}

	FORCEINLINE const PairType& GetPair(KeyType Key) const
	{
		return Internal[Key];
	}

	/**
	 * Returns a copy so should only be used for small data types.
	 */
	FORCEINLINE ValueType GetSafe(KeyType Key) const
	{
		const ValueType* Value = GetAsPointer(Key);
		if (Value)
			return *Value;

		return ValueType();
	}

	FORCEINLINE ValueType& operator[](KeyType Key)
	{
		return GetPair(Key).Value;
	}

	FORCEINLINE const ValueType& operator[](KeyType Key) const
	{
		return GetPair(Key).Value;
	}

	FORCEINLINE ValueType& operator[](int32 Index)
	{
		return GetPair(Index).Value;
	}

	FORCEINLINE const ValueType& operator[](int32 Index) const
	{
		return GetPair(Index).Value;
	}

	FORCEINLINE ValueType* GetAsPointer(KeyType Key)
	{
		PairType* Pair = Internal.GetPairAsPointer(Key);
		if (Pair)
			return &Pair->Value;
		
		return nullptr;
	}

	FORCEINLINE ValueType* GetAsPointer(int32 Index)
	{
		PairType* Pair = Internal.GetPairAsPointer(Index);
        if (Pair)
        	return &Pair->Value;
        
        return nullptr;
	}

	FORCEINLINE const ValueType* GetAsPointer(KeyType Key) const
	{
		const PairType* Pair = Internal.GetPairAsPointer(Key);
		if (Pair)
			return &Pair->Value;
		
		return nullptr;
	}

	FORCEINLINE const ValueType* GetAsPointer(int32 Index) const
	{
		const PairType* Pair = Internal.GetPairAsPointer(Index);
		if (Pair)
			return &Pair->Value;
        
		return nullptr;
	}

	FORCEINLINE int32 Num() const
	{
		return BackingPairs.Num();
	}

	FORCEINLINE bool Contains(KeyType Key) const
	{
		return Translator.Contains(Key);
	}

	FORCEINLINE bool Contains(const ValueType& Item) const
	{
		return GetFirstIndex(Item) > -1;
	}

	FORCEINLINE int32 GetFirstIndex(const ValueType& Item) const
	{
		for (int32 i = 0; i < BackingPairs.Num(); i++)
			if (BackingPairs[i].Value == Item)
				return i;

		return -1;
	}

	FORCEINLINE KeyType GetFirstKey(const ValueType& Item) const
	{
		const KeyType* FirstKey = FindFirstKey(Item);
		if (FirstKey)
			return *FirstKey;

		return KeyType();
	}

	FORCEINLINE const KeyType* FindFirstKey(const ValueType& Item) const
	{
		for (int32 i = 0; i < BackingPairs.Num(); i++)
			if (BackingPairs[i].Value == Item)
				return &BackingPairs[i].Key;

		return nullptr;
	}

	FORCEINLINE const KeyType* GetKey(int32 Index) const
	{
		return Internal.GetKey(Index);
	}

	FORCEINLINE KeyType GetKey(int32 Index)
	{
		return *Internal.GetKey(Index);
	}

	FORCEINLINE bool IsValidIndex(int32 Index) const
	{
		return BackingPairs.IsValidIndex(Index);
	}
	
	FORCEINLINE ValueType& Last(int32 IndexFromTheEnd = 0)
	{
		return LastPair(IndexFromTheEnd).Value;
	}

	FORCEINLINE const ValueType& Last(int32 IndexFromTheEnd = 0) const
	{
		return LastPair(IndexFromTheEnd).Value;
	}

	FORCEINLINE PairType& LastPair(int32 IndexFromTheEnd = 0)
	{
		return Internal.Last(IndexFromTheEnd);
	}

	FORCEINLINE const PairType& LastPair(int32 IndexFromTheEnd = 0) const
	{
		return Internal.Last(IndexFromTheEnd);
	}

	FORCEINLINE void Empty(int32 AllocatedElements = 0)
	{
		Internal.Empty(AllocatedElements);
	}

	FORCEINLINE const TArray<PairType>& GetData() const
	{
		return BackingPairs;
	}

	FORCEINLINE const TMap<KeyType, int32>& GetTranslator() const
	{
		return Translator;
	}

	FORCEINLINE const TInternalKeyedArray<KeyType, ValueType, PairType>& GetInternal() const
	{
		return Internal;
	}
};

/**
 *  The Blueprint Function Library required for the Keyed Array to be accessed through Blueprints.
 */
UCLASS()
class UNameFloatKALibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static float Get(const FNameFloatKeyedArray& Class, const FName Key)
	{
		return Class.GetSafe(Key);
	}

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static bool Contains(const FNameFloatKeyedArray& Class, const FName Key)
	{
		return Class.Contains(Key);
	}

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static int32 Num(const FNameFloatKeyedArray& Class)
	{
		return Class.Num();
	}

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static TArray<FNameFloatPair> GetData(const FNameFloatKeyedArray& Class)
	{
		return Class.GetData();
	}

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static TMap<FName, int> GetMap(const FNameFloatKeyedArray& Class)
	{
		return Class.GetTranslator();
	}

	UFUNCTION(BlueprintCallable)
	static int32 Add(const FNameFloatKeyedArray& Class, const FName Key, float Item)
	{
		return const_cast<FNameFloatKeyedArray&>(Class).Add(Key, Item);
	}

	UFUNCTION(BlueprintCallable)
	static int32 Emplace(const FNameFloatKeyedArray& Class, const FName Key, float Item)
	{
		return const_cast<FNameFloatKeyedArray&>(Class).Emplace(Key, Item);
	}

	UFUNCTION(BlueprintCallable)
	static bool Remove(const FNameFloatKeyedArray& Class, const FName Key)
	{
		return const_cast<FNameFloatKeyedArray&>(Class).Remove(Key);
	}

	UFUNCTION(BlueprintCallable)
	static bool RemoveAt(const FNameFloatKeyedArray& Class, int32 Index)
	{
		return const_cast<FNameFloatKeyedArray&>(Class).RemoveAt(Index);
	}

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static FName GetKey(const FNameFloatKeyedArray& Class, int32 Index)
	{
		return const_cast<FNameFloatKeyedArray&>(Class).GetKey(Index);
	}

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static float Last(const FNameFloatKeyedArray& Class, int32 IndexFromTheEnd = 0)
	{
		return Class.Last(IndexFromTheEnd);
	}

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static FNameFloatPair LastPair(const FNameFloatKeyedArray& Class, int32 IndexFromTheEnd = 0)
	{
		return Class.LastPair(IndexFromTheEnd);
	}

	UFUNCTION(BlueprintCallable)
	static void Empty(const FNameFloatKeyedArray& Class, int32 AllocatedElements = 0)
	{
		const_cast<FNameFloatKeyedArray&>(Class).Empty(AllocatedElements);
	}
};


/**
 *  This is a simple Actor Component that includes all the recommended replication code required to make Keyed Arrays
 *  work across the network.
 *  
 *  You do not have to use this component. You can copy how this is implemented to implement replicated Keyed Arrays
 *  as you wish.
 */
UCLASS( ClassGroup=(KeyedArray), meta=(BlueprintSpawnableComponent) )
class UNameFloatKAComponent : public UActorComponent
{
	GENERATED_BODY()

	UNameFloatKAComponent();
	
public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	UPROPERTY(EditAnywhere, ReplicatedUsing=OnRep_KeyedArray, meta = (AllowPrivateAccess = true))
	FNameFloatKeyedArray KeyedArray;

	UFUNCTION()
	void OnRep_KeyedArray();

public:
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FNameFloatKeyedArrayChangedSignature,
		const FNameFloatKeyedArray&, NewKeyedArray);
	
	UPROPERTY(BlueprintCallable, BlueprintAssignable)
	FNameFloatKeyedArrayChangedSignature OnKeyedArrayChanged;

/**
 *	Since we are using the push-based model for replication, we need to mark the Keyed Array as dirty whenever a
 *	modification has been made.
 *	To prevent accidental unwarranted modification, the Keyed Array will be publicly inaccessible but this isn't
 *	necessary if you know what you're doing and want access to all the methods.
 *	None of these methods are necessary if you are not using the push-based model.
 */
public:
	UFUNCTION(BlueprintCallable, BlueprintPure)
	float Get(const FName Key);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool Contains(const FName Key);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	int32 Num();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	const TArray<FNameFloatPair>& GetData();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	const TMap<FName, int>& GetMap();

	UFUNCTION(BlueprintCallable)
	int32 Add(const FName Key, float Item);

	UFUNCTION(BlueprintCallable)
	int32 Emplace(const FName Key, float Item);

	UFUNCTION(BlueprintCallable)
	bool Remove(const FName Key);

	UFUNCTION(BlueprintCallable)
	bool RemoveAt(int32 Index);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FName GetKey(int32 Index);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	float Last(int32 IndexFromTheEnd = 0);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FNameFloatPair LastPair(int32 IndexFromTheEnd = 0);

	UFUNCTION(BlueprintCallable)
	void Empty(int32 AllocatedElements = 0);
};
