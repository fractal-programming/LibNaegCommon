/*
  This file is part of the DSP-Crowd project
  https://www.dsp-crowd.com

  Author(s):
      - Johannes Natter, office@dsp-crowd.com

  File created on 15.07.2026

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

#include <errno.h>
#include <string.h>
#ifndef _WIN32
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#endif
#if defined(__FreeBSD__)
#include <netinet/in.h>
#endif

#include "LibTcp.h"
#include "Processing.h"

#define dKeepAliveCntDefault			10

#if defined(__APPLE__)
#define dTcpKeepIdle TCP_KEEPALIVE
#else
#define dTcpKeepIdle TCP_KEEPIDLE
#endif

bool tcpNoDelaySet(SOCKET fd, bool enabled)
{
	int opt = enabled ? 1 : 0;
	int res;

	res = ::setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, (const char *)&opt, sizeof(opt));
	if (res)
	{
#if defined(_WIN32)
		int numErr = WSAGetLastError();
#else
		int numErr = errno;
#endif
		errLog(-1, "setsockopt(TCP_NODELAY) failed: %s (%d)", strerror(numErr), numErr);
		return false;
	}

	return true;
}

bool tcpKeepAliveSet(SOCKET fd, uint32_t intervalS)
{
#if defined(_WIN32)
	struct tcp_keepalive alive;
	DWORD bytesReturned;
	int res;

	alive.onoff = 1;
	alive.keepalivetime = intervalS * 1000;
	alive.keepaliveinterval = intervalS * 1000;

	res = ::WSAIoctl(fd, SIO_KEEPALIVE_VALS, &alive, sizeof(alive),
					NULL, 0, &bytesReturned, NULL, NULL);
	if (res)
	{
		int numErr = WSAGetLastError();
		errLog(-1, "WSAIoctl(SIO_KEEPALIVE_VALS) failed: %s (%d)", strerror(numErr), numErr);
		return false;
	}
#elif defined(__linux__) || defined(__APPLE__) || defined(__FreeBSD__)
	int opt;
	int res;

	opt = 1;
	res = ::setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, (const char *)&opt, sizeof(opt));
	if (res)
	{
		errLog(-1, "setsockopt(SO_KEEPALIVE) failed: %s (%d)", strerror(errno), errno);
		return false;
	}

	opt = (int)intervalS;
	res = ::setsockopt(fd, IPPROTO_TCP, dTcpKeepIdle, (const char *)&opt, sizeof(opt));
	if (res)
	{
		errLog(-1, "setsockopt(TCP_KEEPIDLE) failed: %s (%d)", strerror(errno), errno);
		return false;
	}

	opt = (int)intervalS;
	res = ::setsockopt(fd, IPPROTO_TCP, TCP_KEEPINTVL, (const char *)&opt, sizeof(opt));
	if (res)
	{
		errLog(-1, "setsockopt(TCP_KEEPINTVL) failed: %s (%d)", strerror(errno), errno);
		return false;
	}

	opt = dKeepAliveCntDefault;
	res = ::setsockopt(fd, IPPROTO_TCP, TCP_KEEPCNT, (const char *)&opt, sizeof(opt));
	if (res)
	{
		errLog(-1, "setsockopt(TCP_KEEPCNT) failed: %s (%d)", strerror(errno), errno);
		return false;
	}
#else
	(void)fd;
	(void)intervalS;

	errLog(-1, "keep-alive not implemented for this platform");
	return false;
#endif
	return true;
}

