#include "NameFloatKeyedArray.h"

#include "Net/Core/PushModel/PushModel.h"

UNameFloatKAComponent::UNameFloatKAComponent()
{
	SetIsReplicatedByDefault(true);
}

void UNameFloatKAComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	FDoRepLifetimeParams SharedParams;
	SharedParams.bIsPushBased = true;

	DOREPLIFETIME_WITH_PARAMS_FAST(UNameFloatKAComponent, KeyedArray, SharedParams);
}

void UNameFloatKAComponent::OnRep_KeyedArray()
{
	if (KeyedArray.Clean())
		OnKeyedArrayChanged.Broadcast(KeyedArray);
}

float UNameFloatKAComponent::Get(const FName Key)
{
	return KeyedArray.GetSafe(Key);
}

bool UNameFloatKAComponent::Contains(const FName Key)
{
	return KeyedArray.Contains(Key);
}

int32 UNameFloatKAComponent::Num()
{
	return KeyedArray.Num();
}

const TArray<FNameFloatPair>& UNameFloatKAComponent::GetData()
{
	return KeyedArray.GetData();
}

const TMap<FName, int>& UNameFloatKAComponent::GetMap()
{
	return KeyedArray.GetTranslator();
}

int32 UNameFloatKAComponent::Add(const FName Key, float Item)
{
	if (!GetOwner()->HasAuthority())
		return -1;

	const int32 Index = KeyedArray.Add(Key, Item);
	if (Index >= 0)
	{
		MARK_PROPERTY_DIRTY_FROM_NAME( UNameFloatKAComponent, KeyedArray, this );
		OnKeyedArrayChanged.Broadcast(KeyedArray);
	}

	return Index;
}

int32 UNameFloatKAComponent::Emplace(const FName Key, float Item)
{
	if (!GetOwner()->HasAuthority())
		return -1;

	const int32 Index = KeyedArray.Emplace(Key, Item);
	if (Index >= 0)
	{
		MARK_PROPERTY_DIRTY_FROM_NAME( UNameFloatKAComponent, KeyedArray, this );
		OnKeyedArrayChanged.Broadcast(KeyedArray);
	}

	return Index;
}

bool UNameFloatKAComponent::Remove(const FName Key)
{
	if (!GetOwner()->HasAuthority())
		return false;

	const bool bRemoved = KeyedArray.Remove(Key);
	if (bRemoved)
	{
		MARK_PROPERTY_DIRTY_FROM_NAME( UNameFloatKAComponent, KeyedArray, this );
		OnKeyedArrayChanged.Broadcast(KeyedArray);
	}

	return bRemoved;
}

bool UNameFloatKAComponent::RemoveAt(int32 Index)
{
	if (!GetOwner()->HasAuthority())
		return false;
	
	const bool bRemoved = KeyedArray.RemoveAt(Index);
	if (bRemoved)
	{
		MARK_PROPERTY_DIRTY_FROM_NAME( UNameFloatKAComponent, KeyedArray, this );
		OnKeyedArrayChanged.Broadcast(KeyedArray);
	}

	return bRemoved;
}

FName UNameFloatKAComponent::GetKey(int32 Index)
{
	return KeyedArray.GetKey(Index);
}

float UNameFloatKAComponent::Last(int32 IndexFromTheEnd)
{
	return KeyedArray.Last(IndexFromTheEnd);
}

FNameFloatPair UNameFloatKAComponent::LastPair(int32 IndexFromTheEnd)
{
	return KeyedArray.LastPair(IndexFromTheEnd);
}

void UNameFloatKAComponent::Empty(int32 AllocatedElements)
{
	if (!GetOwner()->HasAuthority())
		return;

	if (KeyedArray.Num() > 0)
	{
		KeyedArray.Empty(AllocatedElements);
		MARK_PROPERTY_DIRTY_FROM_NAME( UNameFloatKAComponent, KeyedArray, this );
		OnKeyedArrayChanged.Broadcast(KeyedArray);
	}
}
