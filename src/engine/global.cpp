﻿#include "stdafx.h"
#include "Global.h"

namespace local {
	Global::Global() {

	}
	Global::~Global() {

	}

	std::string Global::Encode6Bit(const std::string& strSrc)
	{
		std::string result;
		auto nSrcLen = strSrc.length();
		auto pszSrc = (unsigned char*)strSrc.c_str();
		int				nDestPos = 0;
		int				nRestCount = 0;
		unsigned char	chMade = 0, chRest = 0;
		for (decltype(nSrcLen) i = 0; i < nSrcLen; i++)
		{
			chMade = ((chRest | (pszSrc[i] >> (2 + nRestCount))) & 0x3f);
			chRest = (((pszSrc[i] << (8 - (2 + nRestCount))) >> 2) & 0x3f);
			nRestCount += 2;
			if (nRestCount < 6) {
				result.push_back(chMade + 0x3c);
			}
			else {
				result.push_back(chMade + 0x3c);
				result.push_back(chRest + 0x3c);

				nRestCount = 0;
				chRest = 0;
			}
		}
		if (nRestCount > 0) {
			result.push_back(chRest + 0x3c);
		}
		if (!result.empty()) {
			result.insert(0, "#", strlen("#"));
			result.push_back('!');
		}
		return result;
	}

	std::string Global::Decode6Bit(const std::string& in)
	{
		std::string strSrc = in;
		std::string result;
		if (strSrc.empty()) {
			return result;
		}
		auto endPos = strSrc.rfind(0x21);
		if (endPos == std::string::npos)
		{
			return result;
		}
		else
		{
			strSrc.erase(endPos, strSrc.size() - endPos);
			strSrc.push_back(0x21);
		}
		if (0x23 != strSrc.front() || 0x21 != strSrc.back()) {
			return result;
		}
		strSrc.erase(strSrc.begin());
		strSrc.pop_back();
		const unsigned char Decode6BitMask[5] = { 0xfc, 0xf8, 0xf0, 0xe0, 0xc0 };
		int				nDestPos = 0, nBitPos = 2;
		int				nMadeBit = 0;
		unsigned char	ch, chCode, tmp;
		for (auto it = strSrc.begin(); it != strSrc.end(); ++it) {
			if ((*it - 0x3c) >= 0) {
				ch = *it - 0x3c;
			}
			else {
				nDestPos = 0;
				break;
			}
			if ((nMadeBit + 6) >= 8) {
				chCode = (tmp | ((ch & 0x3f) >> (6 - nBitPos)));
				result.push_back(chCode);
				nMadeBit = 0;
				if (nBitPos < 6)
					nBitPos += 2;
				else {
					nBitPos = 2;
					continue;
				}
			}
			tmp = ((ch << nBitPos) & Decode6BitMask[nBitPos - 2]);
			nMadeBit += (8 - nBitPos);
		}
		return result;
	}
}///namespace local