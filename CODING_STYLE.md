Coding standards should adhere to the OSG coding style

# Member variables

```cpp
int _variableName // note the camel case
```

# Signatures

```cpp
// check const correctness - do const wherever possible
int myFunction(const std::string& val) const
{
	return 5;
}
```

# Brackets
```cpp
// 
int myFunction(const std::string& val) const
{
	if (val.empty())
	{
		return -1;
	}
	
	return 1
}
```
	
