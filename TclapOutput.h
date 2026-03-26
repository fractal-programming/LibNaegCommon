/*
  This file is part of the DSP-Crowd project
  https://www.dsp-crowd.com

  Author(s):
      - Johannes Natter, office@dsp-crowd.com

  File created on 08.01.2020

  Copyright (C) 2020 Authors and www.dsp-crowd.com

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

#ifndef TCLAP_OUTPUT_H
#define TCLAP_OUTPUT_H

#include <string>
#include <tclap/CmdLine.h>

// http://tclap.sourceforge.net/
class TclapOutput : public TCLAP::StdOutput
{

public:

	TclapOutput()
		: package("<package>")
		, versionApp("<version>")
		, nameApp("<appname>")
		, copyright("<copyright>")
	{
	}

	std::string package;
	std::string versionApp;
	std::string nameApp;
	std::string copyright;

	virtual void usage(TCLAP::CmdLineInterface &c)
	{
		std::string tmp;
		bool reqPrinted = false;

		std::cout << std::endl << package << std::endl;
		std::cout << "Version: " << versionApp << std::endl;

		std::cout << std::endl;
		std::cout << "Usage: " << nameApp << " [OPTION]" << std::endl;
		std::cout << std::endl;

		std::cout << "Required" << std::endl;
		std::cout << std::endl;

		std::list<TCLAP::Arg *> args = c.getArgList();

		// Required with alias
		for (TCLAP::ArgListIterator it = args.begin(); it != args.end(); ++it)
		{
			TCLAP::Arg *pArg = *it;

			if (!pArg->isRequired())
				continue;

			tmp = pArg->longID();
			if (tmp.find(",") == std::string::npos)
				continue;

			tmp.erase(2, tmp.find(",") - 2);
			std::cout << std::setw(2) << " ";
			std::cout << std::setw(35) << std::left << tmp;
			tmp = pArg->getDescription();
			tmp.erase(0, 12);
			std::cout << tmp << std::endl;
			reqPrinted = true;
		}

		// Required without alias
		for (TCLAP::ArgListIterator it = args.begin(); it != args.end(); ++it)
		{
			TCLAP::Arg *pArg = *it;

			if (!pArg->isRequired())
				continue;

			tmp = pArg->longID();
			if (tmp.find(",") != std::string::npos)
				continue;

			std::cout << std::setw(7) << " ";
			std::cout << std::setw(30) << std::left << tmp;
			tmp = pArg->getDescription();
			tmp.erase(0, 12);
			std::cout << tmp << std::endl;
			reqPrinted = true;
		}

		if (!reqPrinted)
			std::cout << "None" << std::endl;

		std::cout << std::endl;
		std::cout << "Optional" << std::endl;
		std::cout << std::endl;

		// Optional with alias
		for (TCLAP::ArgListIterator it = args.begin(); it != args.end(); ++it)
		{
			TCLAP::Arg *pArg = *it;

			if (pArg->isRequired())
				continue;

			tmp = pArg->longID();
			if (tmp.find(",") == std::string::npos)
				continue;

			tmp.erase(2, tmp.find(",") - 2);
			std::cout << std::setw(2) << " ";
			std::cout << std::setw(35) << std::left << tmp;
			std::cout << pArg->getDescription() << std::endl;
		}

		// Optional without alias
		for (TCLAP::ArgListIterator it = args.begin(); it != args.end(); ++it)
		{
			TCLAP::Arg *pArg = *it;

			if (pArg->isRequired())
				continue;

			tmp = pArg->longID();
			if (tmp.find(",") != std::string::npos)
				continue;

			std::cout << std::setw(7) << " ";
			std::cout << std::setw(30) << std::left << tmp;
			std::cout << (*it)->getDescription() << std::endl;
		}
		std::cout << std::endl;

		std::cout << copyright << std::endl;
		std::cout << std::endl;

		printAppCommands();
	}

	virtual void version(TCLAP::CmdLineInterface &c)
	{
		(void)c;
		std::cout << versionApp << std::endl;
	}

private:

	virtual void printAppCommands()
	{
	}

};

#endif

