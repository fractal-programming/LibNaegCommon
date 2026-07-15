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

#ifdef _WIN32
#include <windows.h>
#endif
#include <cstdio>

#include "LibTerminal.h"

using namespace std;

void cursorTermHide()
{
#ifdef _WIN32
	HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_CURSOR_INFO info;

	info.dwSize = 100;
	info.bVisible = FALSE;

	SetConsoleCursorInfo(consoleHandle, &info);
#else
	fprintf(stdout, "\033[?25l");
	fflush(stdout);
#endif
}

void cursorTermShow()
{
#ifdef _WIN32
	HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_CURSOR_INFO info;

	info.dwSize = 100;
	info.bVisible = TRUE;

	SetConsoleCursorInfo(consoleHandle, &info);
#else
	fprintf(stdout, "\033[?25h");
	fflush(stdout);
#endif
}

