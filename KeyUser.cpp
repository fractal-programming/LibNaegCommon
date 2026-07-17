/*
  This file is part of the DSP-Crowd project
  https://www.dsp-crowd.com

  Author(s):
      - Johannes Natter, office@dsp-crowd.com

  File created on 24.10.2024

  Copyright (C) 2022, Johannes Natter

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#define _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING
#include "KeyUser.h"

using namespace std;

// Keychecks

bool keyIsAlphaNum(const KeyUser &key)
{
	if (!key.isPrint())
		return false;

	if (key.val() >= 'a' && key.val() <= 'z')
		return true;

	if (key.val() >= 'A' && key.val() <= 'Z')
		return true;

	if (keyIsNum(key))
		return true;

	return false;
}

bool keyIsNum(const KeyUser &key)
{
	if (!key.isPrint())
		return false;

	if (key.val() >= '0' && key.val() <= '9')
		return true;

	return false;
}

bool keyIsBoundary(const KeyUser &key)
{
	const char *chBoundaries = " .,;:-_(){}[]\"'/\\+*=%<>";
	const char *pCh = chBoundaries;

	for (; *pCh; ++pCh)
	{
		if (key == *pCh)
			return true;
	}

	return false;
}

bool keyIsCtrl(const KeyUser &key)
{
	if (!key.isCtrl())
		return false;

	return true;
}

bool keyIsBackspace(const KeyUser &key)
{
	if (!key.isCtrl())
		return false;

	return key.ctr() == keyBackspace || key.ctr() == keyBackspaceWin;
}

bool keyIsUserDisconnect(const KeyUser &key)
{
	if (!key.isCtrl())
		return false;

	return key.ctr() == keyCtrlC || key.ctr() == keyCtrlD;
}

bool keyIsCommon(const KeyUser &key)
{
	if (keyIsAlphaNum(key))
		return true;

	if (key.isCtrl() && key.ctr() == keyHelp)
		return true;

	if (!key.isPrint())
		return false;

	return key.val() == ' ' || key.val() == '-' || key.val() == '_';
}

bool keyIsAccept(const KeyUser &key)
{
	if (key.isCtrl() && key.ctr() == keyEnter)
		return true;

	if (key.isPrint() && key.val() == 'l')
		return true;

	return false;
}

bool keyIsAbort(const KeyUser &key)
{
	if (key.isCtrl() && key.ctr() == keyEsc)
		return true;

	if (key.isPrint() && key.val() == 'q')
		return true;

	return false;
}

bool keyIsHome(const KeyUser &key)
{
	if (key.isCtrl() && key.ctr() == keyHome)
		return true;

	if (key.isPrint() && key.val() == 'g')
		return true;

	return false;
}

bool keyIsEnd(const KeyUser &key)
{
	if (key.isCtrl() && key.ctr() == keyEnd)
		return true;

	if (key.isPrint() && key.val() == 'G')
		return true;

	return false;
}

bool keyIsUp(const KeyUser &key)
{
	if (key.isCtrl() && key.ctr() == keyUp)
		return true;

	if (key.isPrint() && key.val() == 'k')
		return true;

	return false;
}

bool keyIsDown(const KeyUser &key)
{
	if (key.isCtrl() && key.ctr() == keyDown)
		return true;

	if (key.isPrint() && key.val() == 'j')
		return true;

	return false;
}

bool keyIsUpJump(const KeyUser &key)
{
	if (key.isCtrl() && key.ctr() == keyPgUp)
		return true;

	if (key.isPrint() && key.val() == 'K')
		return true;

	return false;
}

bool keyIsDownJump(const KeyUser &key)
{
	if (key.isCtrl() && key.ctr() == keyPgDn)
		return true;

	if (key.isPrint() && key.val() == 'J')
		return true;

	return false;
}

// Unicode

void strToUtf(const string &str, u32string &ustr)
{
	try
	{
		wstring_convert<codecvt_utf8<char32_t>, char32_t> converter;
		ustr = converter.from_bytes(str);
	}
	catch (...)
	{
	}
}

void utfToStr(const u32string &ustr, string &str)
{
	try
	{
		wstring_convert<codecvt_utf8<char32_t>, char32_t> converter;
		str = converter.to_bytes(ustr);
	}
	catch (...)
	{
	}
}

void utfStrAdd(u32string &ustr, const string &str)
{
	u32string ustrTmp;
	strToUtf(str, ustrTmp);
	ustr += ustrTmp;
}

