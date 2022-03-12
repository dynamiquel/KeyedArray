#include "NameObjectKeyedArray.h"

#include "Net/Core/PushModel/PushModel.h"

UNameObjectKAComponent::UNameObjectKAComponent()
{
	SetIsReplicatedByDefault(true);
}

void UNameObjectKAComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	FDoRepLifetimeParams SharedParams;
	SharedParams.bIsPushBased = true;

	DOREPLIFETIME_WITH_PARAMS_FAST(UNameObjectKAComponent, KeyedArray, SharedParams);
}

void UNameObjectKAComponent::OnRep_KeyedArray()
{
	if (KeyedArray.Clean())
		OnKeyedArrayChanged.Broadcast(KeyedArray);
}

UObject* UNameObjectKAComponent::Get(const FName Key)
{
	return KeyedArray.GetSafe(Key);
}

bool UNameObjectKAComponent::Contains(const FName Key)
{
	return KeyedArray.Contains(Key);
}

int32 UNameObjectKAComponent::Num()
{
	return KeyedArray.Num();
}

const TArray<FNameObjectPair>& UNameObjectKAComponent::GetData()
{
	return KeyedArray.GetData();
}

const TMap<FName, int>& UNameObjectKAComponent::GetMap()
{
	return KeyedArray.GetTranslator();
}

int32 UNameObjectKAComponent::Add(const FName Key, UObject* Item)
{
	if (!GetOwner()->HasAuthority())
		return -1;

	const int32 Index = KeyedArray.Add(Key, Item);
	if (Index >= 0)
	{
		MARK_PROPERTY_DIRTY_FROM_NAME( UNameObjectKAComponent, KeyedArray, this );
		OnKeyedArrayChanged.Broadcast(KeyedArray);
	}

	return Index;
}

int32 UNameObjectKAComponent::Emplace(const FName Key, UObject* Item)
{
	if (!GetOwner()->HasAuthority())
		return -1;

	const int32 Index = KeyedArray.Emplace(Key, Item);
	if (Index >= 0)
	{
		MARK_PROPERTY_DIRTY_FROM_NAME( UNameObjectKAComponent, KeyedArray, this );
		OnKeyedArrayChanged.Broadcast(KeyedArray);
	}

	return Index;
}

bool UNameObjectKAComponent::Remove(const FName Key)
{
	if (!GetOwner()->HasAuthority())
		return false;

	const bool bRemoved = KeyedArray.Remove(Key);
	if (bRemoved)
	{
		MARK_PROPERTY_DIRTY_FROM_NAME( UNameObjectKAComponent, KeyedArray, this );
		OnKeyedArrayChanged.Broadcast(KeyedArray);
	}

	return bRemoved;
}

bool UNameObjectKAComponent::RemoveAt(int32 Index)
{
	if (!GetOwner()->HasAuthority())
		return false;
	
	const bool bRemoved = KeyedArray.RemoveAt(Index);
	if (bRemoved)
	{
		MARK_PROPERTY_DIRTY_FROM_NAME( UNameObjectKAComponent, KeyedArray, this );
		OnKeyedArrayChanged.Broadcast(KeyedArray);
	}

	return bRemoved;
}

FName UNameObjectKAComponent::GetKey(int32 Index)
{
	return KeyedArray.GetKey(Index);
}

UObject* UNameObjectKAComponent::Last(int32 IndexFromTheEnd)
{
	return KeyedArray.Last(IndexFromTheEnd);
}

FNameObjectPair UNameObjectKAComponent::LastPair(int32 IndexFromTheEnd)
{
	return KeyedArray.LastPair(IndexFromTheEnd);
}

void UNameObjectKAComponent::Empty(int32 AllocatedElements)
{
	if (!GetOwner()->HasAuthority())
		return;

	if (KeyedArray.Num() > 0)
	{
		KeyedArray.Empty(AllocatedElements);
		MARK_PROPERTY_DIRTY_FROM_NAME( UNameObjectKAComponent, KeyedArray, this );
		OnKeyedArrayChanged.Broadcast(KeyedArray);
	}
}
