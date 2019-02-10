/*
 * Utilities.cpp
 *
 *  Created on: Feb 9, 2019
 *      Author: dbetz
 */

#include <fstream>
#include <string>

#include "Process/Pipe.hpp"
#include "Process/Process.hpp"
#include "Process/Connection.hpp"

void Chain1(Process &p, std::istream & is, std::ostream & os){
	Pipe a, b, pErr;

	launch({
			{a,EndPointAction::CLOSE,EndPointAction::NONBLOCKING_WRITE},
			{p,a,EndPointAction::STDIN,EndPointAction::CLOSE},
			{b,EndPointAction::NONBLOCKING_READ,EndPointAction::CLOSE},
			{p,b,EndPointAction::CLOSE,EndPointAction::STDOUT},

			{pErr,EndPointAction::NONBLOCKING_READ,EndPointAction::CLOSE},
			{p,pErr,EndPointAction::CLOSE,EndPointAction::STDERR},
		   }
	      );

	std::string msg;
	char buf[1024];

	while(is){
		is.read(buf,sizeof(buf));
		msg+=std::string(buf,is.gcount());
		msg=a.write(msg);
		os << b.read();
	}
	a.closeWrite();

	while(p){
		os << b.read();
	}
	os << b.read();
}

void Chain1e(Process &p, std::istream & is, std::ostream & os){
	Pipe a, b, pErr;

	launch({
			{a,EndPointAction::CLOSE,EndPointAction::NONBLOCKING_WRITE},
			{p,a,EndPointAction::STDIN,EndPointAction::CLOSE},
			{b,EndPointAction::NONBLOCKING_READ,EndPointAction::CLOSE},
			{p,b,EndPointAction::CLOSE,EndPointAction::STDOUT},

			{pErr,EndPointAction::NONBLOCKING_READ,EndPointAction::CLOSE},
			{p,pErr,EndPointAction::CLOSE,EndPointAction::STDERR},
		   }
	      );

	std::string msg;
	char buf[1024];

	while(is){
		is.read(buf,sizeof(buf));
		msg+=std::string(buf,is.gcount());
		msg=a.write(msg);
		os << b.read();
		os << pErr.read();
	}
	a.closeWrite();

	while(p){
		os << b.read();
		os << pErr.read();
	}
	os << b.read();
	os << pErr.read();
}

void Chain2(Process &p, Process &q, std::istream & is, std::ostream & os){
	Pipe a, b, c, pErr, qErr;

	launch({
			{a,EndPointAction::CLOSE,EndPointAction::NONBLOCKING_WRITE},
			{p,a,EndPointAction::STDIN,EndPointAction::CLOSE},
			{b,EndPointAction::CLOSE,EndPointAction::CLOSE},
			{p,b,EndPointAction::CLOSE,EndPointAction::STDOUT},
			{q,b,EndPointAction::STDIN,EndPointAction::CLOSE},
			{c,EndPointAction::NONBLOCKING_READ,EndPointAction::CLOSE},
			{q,c,EndPointAction::CLOSE,EndPointAction::STDOUT},

			{pErr,EndPointAction::NONBLOCKING_READ,EndPointAction::CLOSE},
			{p,pErr,EndPointAction::CLOSE,EndPointAction::STDERR},
			{qErr,EndPointAction::NONBLOCKING_READ,EndPointAction::CLOSE},
			{q,qErr,EndPointAction::CLOSE,EndPointAction::STDERR},
		   }
	      );

	std::string msg;
	char buf[1024];

	while(is){
		is.read(buf,sizeof(buf));
		msg+=std::string(buf,is.gcount());
		msg=a.write(msg);
		os << c.read();
	}
	a.closeWrite();

	while(q){
		os << c.read();
	}
	os << c.read();
}

void Chain3(Process &p, Process &q, Process &r, std::istream & is, std::ostream & os){
	Pipe a, b, c, d, pErr, qErr, rErr;

	launch({
			{a,EndPointAction::CLOSE,EndPointAction::NONBLOCKING_WRITE},
			{p,a,EndPointAction::STDIN,EndPointAction::CLOSE},

			{b,EndPointAction::CLOSE,EndPointAction::CLOSE},
			{p,b,EndPointAction::CLOSE,EndPointAction::STDOUT},
			{q,b,EndPointAction::STDIN,EndPointAction::CLOSE},

			{c,EndPointAction::CLOSE,EndPointAction::CLOSE},
			{q,c,EndPointAction::CLOSE,EndPointAction::STDOUT},
			{r,c,EndPointAction::STDIN,EndPointAction::CLOSE},

			{d,EndPointAction::NONBLOCKING_READ,EndPointAction::CLOSE},
			{r,d,EndPointAction::CLOSE,EndPointAction::STDOUT},

			{pErr,EndPointAction::NONBLOCKING_READ,EndPointAction::CLOSE},
			{p,pErr,EndPointAction::CLOSE,EndPointAction::STDERR},
			{qErr,EndPointAction::NONBLOCKING_READ,EndPointAction::CLOSE},
			{q,qErr,EndPointAction::CLOSE,EndPointAction::STDERR},
			{rErr,EndPointAction::NONBLOCKING_READ,EndPointAction::CLOSE},
			{r,rErr,EndPointAction::CLOSE,EndPointAction::STDERR},
		   }
	      );

	std::string msg;
	char buf[1024];

	while(is){
		is.read(buf,sizeof(buf));
		msg+=std::string(buf,is.gcount());
		msg=a.write(msg);
		os << d.read();
	}
	a.closeWrite();

	while(r){
		os << d.read();
	}
	os << d.read();
}
