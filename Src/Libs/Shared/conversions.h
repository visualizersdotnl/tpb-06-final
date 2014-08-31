#pragma once

#include <string>
#include <vector>
#include "stdafx.h"


std::string Float2Str(double d);
std::string Vector2i2Str(const Vector2i& v);
Vector2i Str2Vector2i(const std::string& s);
Vector3 Str2Vector3(const std::string& s);
Quaternion Str2Quaternion(const std::string& s);

float Str2Float(const std::string& s);
int Str2Int(const std::string& s);

std::string Int2Str(int v);

std::string LowerCase(const std::string& s);
std::string Trim(const std::string& s);

std::string DWORD2HexStr(DWORD val);
DWORD HexStr2DWORD(const std::string& s);

std::string BinaryToEncodedString(const std::string& s);
std::string EncodedToBinaryString(const std::string& s);

std::vector<std::string> Tokenize(const std::string& line, const std::string& separators);

std::wstring StrToWStr(const std::string& s);

std::string Beat2Str(int beatIndex, int beatsPerMeasure);
int Str2Beat(const std::string& s, int beatsPerMeasure);
std::string Beat2Str(float beatIndex, int beatsPerMeasure);


template <typename T>
T* StlVector2Array(const std::vector<T>& v)
{
	T* storage = new T[(int)v.size()];

	for (int i=0; i<(int)v.size(); ++i)
		storage[i] = v[i];

	return storage;
}

template <typename T>
std::vector<T> Array2StlVector(const T* arr, int numItems)
{
	std::vector<T> v;

	v.resize(numItems);

	for (int i=0; i<numItems; ++i)
		v[i] =  arr[i];

	return v;
}

std::string ReplaceStr(const std::string& source, const std::string& strToSearchFor, const std::string& strToReplaceWith);

