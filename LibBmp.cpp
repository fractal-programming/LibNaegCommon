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

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "LibBmp.h"
#include "Processing.h"

using namespace std;

const size_t cSzHeaderBmp = 14;
const size_t cSzHeaderDib = 40;

FileBmp::FileBmp()
	: mMtxFile()
	, mWidth(0)
	, mHeight(0)
	, mIdxWritten(0)
	, mpFile(NULL)
{
}

bool FileBmp::writeOpen(const char *pFilename, uint32_t width, uint32_t height)
{
	if (!pFilename)
		return false;

	if (!width || !height)
		return false;

	Guard lock(mMtxFile);

	if (mpFile)
		return false;

#if defined(_WIN32)
	errno_t numErr = ::fopen_s(&mpFile, pFilename, "wb");
	if (numErr)
#else
	mpFile = fopen(pFilename, "wb");
	if (!mpFile)
#endif
		return false;

	// Object
	mWidth = width;
	mHeight = height;
	mIdxWritten = 0;

	// Headers
	uint8_t buf[cSzHeaderDib];
	size_t len;

	// Header BMP
	len = cSzHeaderBmp;

	(void)memset(buf, 0, len);

	buf[0] = 'B'; // Signature
	buf[1] = 'M';
	buf[10] = 54; // Pixel data offset

	fwrite(buf, sizeof(buf[0]), len, mpFile);

	// Header DIB
	len = sizeof(buf);

	(void)memset(buf, 0, len);

	buf[0] = (uint8_t)len; // Header size

	buf[12] = 1; // Planes
	buf[14] = 24; // Bits per pixel

	fwrite(buf, sizeof(buf[0]), len, mpFile);

	return true;
}

bool FileBmp::lineWrite(const char *pData, size_t len)
{
	Guard lock(mMtxFile);
	return lineWriteUnlocked(pData, len);
}

void FileBmp::close()
{
	Guard lock(mMtxFile);

	if (!mpFile)
		return;

	if (!mWidth || !mHeight)
	{
		fclose(mpFile);
		mpFile = NULL;
		return;
	}

	size_t szData = mWidth * cNumBytesPerPixel; // Size of data per line
	size_t maskLine = 3;
	size_t szLine = ((szData + maskLine) & ~maskLine);
	size_t szFile;

	/*
	 * Try to be as tolerant as possible to any situation.
	 * Do not overreact on errors!
	 * In this case we fill up the remaining
	 * lines if they weren't written by the user.
	 */
	imageComplete(szLine);

	szData = szLine * mIdxWritten; // Size of data for all (written) lines
	szFile = szData + cSzHeaderBmp + cSzHeaderDib;
#if 0
	dbgLog("Updating headers");
	dbgLog("Size file        %u", szFile);
	dbgLog("Width            %u", mWidth);
	dbgLog("Height written   %u", mIdxWritten);
	dbgLog("Size data        %u", szData);
#endif
	fseek(mpFile, 2, SEEK_SET);
	fwrite(&szFile, sizeof(uint32_t), 1, mpFile);

	fseek(mpFile, cSzHeaderBmp + 4, SEEK_SET);
	fwrite(&mWidth, sizeof(mWidth), 1, mpFile);

	fseek(mpFile, cSzHeaderBmp + 8, SEEK_SET);
	fwrite(&mIdxWritten, sizeof(mIdxWritten), 1, mpFile);

	fseek(mpFile, cSzHeaderBmp + 20, SEEK_SET);
	fwrite(&szData, sizeof(szData), 1, mpFile);

	fclose(mpFile);
	mpFile = NULL;
}

void FileBmp::imageComplete(size_t szLine)
{
	if (mIdxWritten >= mHeight)
		return;

	wrnLog("Image not finished. Filling up");
	wrnLog("Line index       %u", mIdxWritten);
	wrnLog("Size             %u", szLine);

	size_t numLinesRemaining;
	char *pData;
	bool ok;

	pData = new dNoThrow char[szLine];
	if (!pData)
	{
		wrnLog("could not allocate data buffer");
		return;
	}

	memset(pData, 12, szLine);

	/*
	 * Important:
	 * We are in an unusual situation.
	 * We could check mIdxWritten and mHeight,
	 * but we can control the remaining lines
	 * variable by ourselves.
	 */
	numLinesRemaining = mHeight - mIdxWritten;

	for (; numLinesRemaining; --numLinesRemaining)
	{
		ok = lineWriteUnlocked(pData, szLine);
		if (ok)
			continue;

		wrnLog("could not append line");
		break;
	}

	delete[] pData;
}

bool FileBmp::lineWriteUnlocked(const char *pData, size_t len)
{
	if (!mpFile || !pData || !len)
		return false;

	if (!mWidth || !mHeight)
		return false;

	if (mIdxWritten >= mHeight)
		return false;

	if (len & 3)
	{
		wrnLog("padding error: %u (%u) @ %p -> %p", mIdxWritten, len, pData, mpFile);
		return false;
	}
#if 0
	if (mIdxWritten < 5)
		wrnLog("Writing line: %u (%u) @ %p -> %p", mIdxWritten, len, pData, mpFile);
#endif
	fwrite(pData, sizeof(*pData), len, mpFile);
	++mIdxWritten;

	return true;
}

