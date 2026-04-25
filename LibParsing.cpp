/*
  This file is part of the DSP-Crowd project
  https://www.dsp-crowd.com

  Author(s):
      - Johannes Natter, office@dsp-crowd.com

  File created on 25.04.2026

  Copyright (C) 2026, Johannes Natter

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

#include "LibParsing.h"
#include "Processing.h"

using namespace std;

static void caseNumber(int digit, int &num)
{
	if (num < 0)
	{
		num = digit;
		return;
	}

	num *= 10;
	num += digit;
}

static bool caseDelimiter(bool isRange, Range &r, int &num, Ranges &ranges)
{
	r.b = num;
	if (!isRange) r.a = num;
	num = -1;

	if (r.a < 0 && r.b < 0)
		return true;

	if (r.b >= 0 && r.a > r.b)
	{
		errLog(-1, "start (%d) of range higher than end (%d)",
							r.a, r.b);
		return false;
	}

	if (ranges.size())
	{
		Ranges::iterator iLeft = ranges.end();
		--iLeft;

		if (iLeft->b < 0 || r.a < 0)
		{
			errLog(-1, "central range boundary cannot be optional");
			return false;
		}

		if (iLeft->b >= r.a)
		{
			errLog(-1, "range overlap");
			return false;
		}
	}

	ranges.push_back(r);

	r.a = -1;
	r.b = -1;

	return true;
}

bool rangesParse(const string &pattern, Ranges &ranges, size_t *pIdxErr)
{
	if (!pattern.size())
	{
		errLog(-1, "parameter empty");
		return false;
	}

	if (pIdxErr) *pIdxErr = 0;

	bool ok, isRange = false;
	size_t i, idxStart;
	Range r;
	char ch;
	int num;

	r.a = -1;
	r.b = -1;
	num = -1;

	idxStart = i = 0;
	for (; i < pattern.size(); ++i)
	{
		ch = pattern[i];

		ok = true;

		if (ch == '-' && !isRange)
		{
			r.a = num;
			num = -1;
			isRange = true;
		}
		else
		if (ch == ',')
		{
			ok = caseDelimiter(isRange, r, num, ranges);
			idxStart = i + 1;
			isRange = false;
		}
		else
		if (ch >= '0' && ch <= '9')
			caseNumber(ch - '0', num);
		else
		{
			if (pIdxErr) *pIdxErr = idxStart;

			errLog(-1, "invalid character: '%c' 0x%02x", ch, ch);
			return false;
		}

		if (!ok)
		{
			if (pIdxErr) *pIdxErr = idxStart;

			errLog(-1, "could not process character: '%c' 0x%02x", ch, ch);
			return false;
		}
	}

	// Finalize

	ok = caseDelimiter(isRange, r, num, ranges);
	if (!ok)
	{
		if (pIdxErr) *pIdxErr = idxStart;

		errLog(-1, "could not finalize pattern");
		return false;
	}

	if (!ranges.size())
	{
		errLog(-1, "no range provided");
		return false;
	}

	return true;
}

