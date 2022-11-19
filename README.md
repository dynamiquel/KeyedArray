# KeyedArray
 
## Summary
KeyedArray is a drop-in plugin for Unreal Engine 4 that allows you to replicate something similar to a TMap across the network.
In essence, it is a TArray that uses a TMap to allow indices to be referenced by a key.

Due to Unreal Engine not supporting template/generic types, it does require a decent amount of copy, paste and replace for every Key/Value combination you wish to use.
The project has two Key/Value combinations included:
- FName/UObject*
- FName/float

The project also includes an example on how to make the KeyedArray work with replication through the provided ActorComponents.

![image](https://user-images.githubusercontent.com/50085636/202844059-83e86d89-e0a9-47b5-91d9-0a6216f07f37.png)
