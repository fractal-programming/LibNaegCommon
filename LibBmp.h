/*
  This file is part of the DSP-Crowd project
  https://www.dsp-crowd.com

  Author(s):
      - Johannes Natter, office@dsp-crowd.com

  File created on 14.03.2026

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

#ifndef LIB_BMP_H
#define LIB_BMP_H

#include <stdio.h>
#include <mutex>

const size_t cNumBytesPerPixel = 3 * sizeof(char);

class FileBmp
{
public:
	FileBmp();
	virtual ~FileBmp() { this->close(); }

	bool writeOpen(const char *pFilename, uint32_t width, uint32_t height);
	bool lineWrite(const char *pData, size_t len);

	void close();

private:
	void imageComplete(size_t szLine);
	bool lineWriteUnlocked(const char *pData, size_t len);

	std::mutex mMtxFile;
	uint32_t mWidth;
	uint32_t mHeight;
	uint32_t mIdxWritten;
	FILE *mpFile;
};

#endif

