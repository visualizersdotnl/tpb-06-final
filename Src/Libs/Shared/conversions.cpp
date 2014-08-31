#include <stdafx.h>
#include "conversions.h"


std::string Float2Str(double d)
{
	char c[1024];

	sprintf_s(c, 1024, "%f", d);
	return c;
}

std::string Vector2i2Str(const Vector2i& v)
{
	char c[1024];

	sprintf_s(c,1024, "%i,%i", v.x, v.y);
	return c;
}

Vector2i Str2Vector2i(const std::string& s)
{
	Vector2i v;

	std::string a,b;
	bool first = true;

	for (int i=0; i<(int)s.size(); ++i)
		if (first)
		{
			if (s[i] == ',')
				first = false;
			else
				if (s[i] != ' ')
					a += s[i];
		} else
		{
			if (s[i] != ' ')
				b += s[i];
		}

	v.x = atoi(a.c_str());
	v.y = atoi(b.c_str());

	return v;
}

// quite inefficient conversion routine ;)
Vector3 Str2Vector3(const std::string& s)
{
	Vector3 v(0,0,0);

	std::vector<std::string> tokens = Tokenize(s, ",");

	if (tokens.size() > 0)
	{
		v.x = (float)atof(tokens[0].c_str());

		if (tokens.size() > 1)
		{
			v.y = (float)atof(tokens[1].c_str());

			if (tokens.size() > 2)
				v.z = (float)atof(tokens[2].c_str());
		}
	}

	return v;
}

Quaternion Str2Quaternion(const std::string& s)
{
	Quaternion q;

	std::vector<std::string> tokens = Tokenize(s, ",");

	if (tokens.size() > 0)
	{
		q.x = (float)atof(tokens[0].c_str());

		if (tokens.size() > 1)
		{
			q.y = (float)atof(tokens[1].c_str());

			if (tokens.size() > 2)
			{
				q.z = (float)atof(tokens[2].c_str());

				if (tokens.size() > 3)
					q.w = (float)atof(tokens[3].c_str());
			}
		}
	}

	return q;
}


std::string LowerCase(const std::string& s)
{
	std::string ret = s;

	for (int i=(int)ret.size()-1; i>=0; --i)
		if ((ret[i] >= 'A') && (ret[i] <= 'Z'))
			ret[i] = (ret[i]-'A')+'a';

	return ret;
}

std::string Trim(const std::string& s)
{
	int firstIndex = (int)s.find_first_not_of(" \t");
	int lastIndex = (int)s.find_last_not_of(" \t");

	if (firstIndex >= 0)
	{
		return s.substr(firstIndex, (lastIndex - firstIndex)+1);
	}
	else
	{
		return s;
	}
}


std::string DWORD2HexStr(DWORD val)
{
	char c[1024];

	sprintf_s(c,1024, "%08X", val);
	return c;	
}

DWORD HexStr2DWORD(const std::string& s)
{
	DWORD v = 0;
	int mul = 1;
	std::string str = LowerCase(s);

	for (int i=(int)str.size()-1; i>=0; --i)
	{
		int val = 0;
		if ((str[i] >= '0') && (str[i] <= '9'))
			val = str[i]-'0';
		else
		if ((str[i] >= 'a') && (str[i] <= 'f'))
			val = 10 + (str[i]-'a');

		val *= mul;
		mul *= 16;

		v += val;
	}

	return v;
}


std::string BinaryToEncodedString(const std::string& s)
{
	std::string out;
	char c[128];

	for (int i=0; i<(int)s.length(); ++i)
	{
		unsigned char v = (unsigned char)s[i];
		sprintf_s(c, "%02X", v);
		out += c;
	}

	return out;
}

std::string EncodedToBinaryString(const std::string& s)
{
	std::string tmp, result;

	int count = (int)s.length() / 2;

	result.resize(count);

	for (int i=0; i<count; i++)
	{
		tmp = s[i*2];
		tmp += s[i*2+1];
		DWORD val = HexStr2DWORD(tmp);

		result[i] = (unsigned char) val;
	}

	return result;
}


std::vector<std::string> Tokenize(const std::string& line, const std::string& separators)
{
	std::vector<std::string> result;
	int finger = 0;

	while (finger < (int)line.size())
	{
		int pos = (int)line.find_first_of(separators, finger);

		if (pos != std::string::npos)
		{
			// found a separator, add line inbetween to our list
			if (pos != finger)
				result.push_back(line.substr(finger, pos-finger));

			finger = pos+1; // move finger

		} else			
		{
			// didn't find any separators anymore!
			result.push_back(line.substr(finger));
			break;
		}
	}

	return result;
}


float Str2Float(const std::string& s)
{
	return (float)atof(s.c_str());
}

int Str2Int(const std::string& s)
{
	return atoi(s.c_str());
}

std::string Int2Str(int v)
{
	char c[1024];

	sprintf_s(c, 1024, "%d", v);
	return c;	
}


std::wstring StrToWStr(const std::string& s)
{
	std::wstring temp(s.length(),L' ');
	std::copy(s.begin(), s.end(), temp.begin());
	return temp;
}


std::string Beat2Str(float beatIndex, int beatsPerMeasure)
{
	int measure = (int)floorf(beatIndex / beatsPerMeasure);
	float beat = beatIndex - measure * beatsPerMeasure;

	char c[256];
	sprintf_s(c,256,"%d:%.1f", measure, beat);

	return c;
}

std::string Beat2Str(int beatIndex, int beatsPerMeasure)
{
	int beat = beatIndex % beatsPerMeasure;
	int measure = (beatIndex - beat) / beatsPerMeasure;

	char c[256];
	sprintf_s(c,256,"%d:%d", measure, beat);

	return c;
}

int Str2Beat(const std::string& s, int beatsPerMeasure)
{
	int seperatorIndex = -1;

	for (int i=0; i<(int)s.length(); ++i)
	{
		if (s[i] == ':')
		{
			seperatorIndex = i;
			break;
		}
	}

	if (seperatorIndex >= 0)
	{
		int measure = 0;
		int beat = 0;

		if (seperatorIndex > 0)
		{
			measure = Str2Int(s.substr(0, seperatorIndex));
		}

		beat = Str2Int(s.substr(seperatorIndex+1));

		return beat + measure * beatsPerMeasure;
	}
	else
	{
		return Str2Int(s);
	}	
}

std::string ReplaceStr(const std::string& source, const std::string& strToSearchFor, const std::string& strToReplaceWith)
{
	std::string newstring = source;

	int startSearchIndex = 0;
	int index;
	while (true)
	{
		index = (int)newstring.find(strToSearchFor.c_str(), startSearchIndex);

		if (index == std::string::npos)
			break;

		startSearchIndex = index + (int)strToReplaceWith.length();
		newstring = 
			newstring.substr(0, index) + 
			strToReplaceWith + 
			newstring.substr(index + strToSearchFor.length());
	}	

	return newstring;
}
