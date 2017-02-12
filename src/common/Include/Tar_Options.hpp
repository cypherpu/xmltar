/*

Copyright 2010 by David A. Betz

This file is part of xmltar.

xmltar is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

xmltar is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with xmltar.  If not, see <http://www.gnu.org/licenses/>.

*/

#ifndef Tar_Options_hpp_
#define Tar_Options_hpp_

#include <sstream>
#include <boost/filesystem.hpp>

namespace Parse_Opts {

enum N_Args { ARGS_0, ARGS_1 };

class Action {
public:
	virtual void Do_Action(std::string)=0;
};

/*
 * assign a single argument
 */
template <typename T> class Action_Assign_Args : public Action {
	T& destination;
public:
	Action_Assign_Args(T& dest)
		: destination(dest) { }
	~Action_Assign_Args(void) { }
	void Do_Action(std::string s){
		std::istringstream iss(s);
		iss >> destination;
		if (!iss.eof()) throw "unused characters in option";
	}
};

/*
 * we need a template specialization for std::string args, to handle
 * the case in which the argument might contain a filename
 */
template <> class Action_Assign_Args<std::string> : public Action {
	std::string& destination;
public:
	Action_Assign_Args(std::string& dest)
		: destination(dest) { }
	~Action_Assign_Args(void) { }
	void Do_Action(std::string s){
		destination=s;
	}
};

/*
 * we need a template specialization for std::string args, to handle
 * the case in which the argument might contain a filename
 */
template <> class Action_Assign_Args<boost::filesystem::path> : public Action {
	boost::filesystem::path& destination;
public:
	Action_Assign_Args(boost::filesystem::path& dest)
		: destination(dest) { }
	~Action_Assign_Args(void) { }
	void Do_Action(std::string s){
		destination=boost::filesystem::path(s);
	}
};

/*
 * assign a pre-selected value
 */
template <typename T> class Action_Assign_Value : public Action {
	T& destination;
	T value;
public:
	Action_Assign_Value(T& dest, T val)
		: destination(dest), value(val) { }
	~Action_Assign_Value(void){ }

	void Do_Action(std::string s){
		destination=value;
	}
};

/*
 * increment a value
 */
template <typename T> class Action_Increment_Value : public Action {
	T& destination;
public:
	Action_Increment_Value(T& dest)
		: destination(dest) { }
	~Action_Increment_Value(void) { }

	void Do_Action(std::string s){
		destination++;
	}
};

/*
 * append multiple args
 */
template <typename T> class Action_Append_Args : public Action {
	std::vector<T>& destination;
public:
	Action_Append_Args(std::vector<T>& dest)
		: destination(dest) { }
	~Action_Append_Args(void) { }
	void Do_Action(std::string s){
		T tmp;
		std::istringstream iss(s);
		iss >> tmp;
		if (!iss.eof()) throw "unused characters in option";
		destination.push_back(tmp);
	}
};

template <> class Action_Append_Args<std::string> : public Action {
	std::vector<std::string>& destination;
public:
	Action_Append_Args(std::vector<std::string>& dest)
		: destination(dest) { }
	~Action_Append_Args(void) { }
	void Do_Action(std::string s){
		destination.push_back(s);
	}
};

template <> class Action_Append_Args<boost::filesystem::path> : public Action {
	std::vector<boost::filesystem::path>& destination;
public:
	Action_Append_Args(std::vector< boost::filesystem::path>& dest)
		: destination(dest) { }
	~Action_Append_Args(void) { }
	void Do_Action(std::string s){
		destination.push_back(boost::filesystem::path(s));
	}
};

class Option {
public:
	int number_of_times_assigned;
	N_Args number_of_arguments;
	std::string short_form;
	std::string long_form;
	std::string description;

	std::vector<Action *> actions;

	Option(N_Args nargs, std::string sf, std::string lf, std::string desc)
		: number_of_arguments(nargs), number_of_times_assigned(0), short_form(sf), long_form(lf), description(desc) {
	}
	bool Matches(std::string){
	}

	void Do_Actions(std::string s){
		for(std::vector<Action *>::iterator i=actions.begin(); i!=actions.end(); ++i)
			(*i)->Do_Action(s);
	}
};

class Option_Parser {
public:

	std::vector<Option *> all_options;
	std::map<std::string, Option *> short_forms;
	std::map<std::string, Option *> long_forms;

public:
	template <typename T> Action_Assign_Args<T> *Assign_Args(T& dest){
		Action_Assign_Args<T> *tmp=new Action_Assign_Args<T>(dest);

		return tmp;
	}

	template <typename T> Action_Assign_Value<T> *Assign_Value(T& dest, T val){
		Action_Assign_Value<T> *tmp = new Action_Assign_Value<T>(dest,val);

		return tmp;
	}

	template <typename T> Action_Increment_Value<T> *Increment_Value(T& dest){
		Action_Increment_Value<T> *tmp=new Action_Increment_Value<T>(dest);

		return tmp;
	}

	template <typename T> Action_Append_Args<T> *Append_Args(std::vector<T>& dest){
		Action_Append_Args<T> *tmp=new Action_Append_Args<T>(dest);

		return tmp;
	}

	void Add_Option(N_Args nargs, const char *sf, const char *lf, const char *desc, Action* a1){
		std::string sfs(sf), lfs(lf), descs(desc);

		if (sfs!="")
			if (sfs.size()>2)
				throw "Add_Option: short form option has too many characters";
			else if (sfs.size()<2)
				throw "Add_Option: short form option has too few characters";
			else if (sfs[0]!='-')
				throw "Add_Option: short form option must start with a '-'";

		Option *p=new Option(nargs, sfs, lfs, descs);
		p->actions.push_back(a1);
		all_options.push_back(p);
		if (sfs!="") short_forms.insert(std::pair<std::string,Option *>(sfs,p));;
		if (lfs!="") long_forms.insert(std::pair<std::string,Option *>(lfs,p));
	}

	void Add_Option(N_Args nargs, const char *sf, const char *lf, const char *desc, Action* a1, Action* a2){
		std::string sfs(sf), lfs(lf), descs(desc);

		if (sfs!="")
			if (sfs.size()>2)
				throw "Add_Option: short form option has too many characters";
			else if (sfs.size()<2)
				throw "Add_Option: short form option has too few characters";
			else if (sfs[0]!='-')
				throw "Add_Option: short form option must start with a '-'";

		Option *p=new Option(nargs, sfs, lfs, descs);
		p->actions.push_back(a1);
		p->actions.push_back(a2);
		all_options.push_back(p);
		if (sfs!="") short_forms.insert(std::pair<std::string,Option *>(sfs,p));;
		if (lfs!="") long_forms.insert(std::pair<std::string,Option *>(lfs,p));
	}

	std::vector<std::string> Parse(int argc, char *argv[]){
		std::vector<std::string> left_over_args;

		int next_arg=1;
		if (argc<=1)
			throw "called without options";

		// look for old-style options first.  These are single letter options, not preceded by a "-", which are clustered
		// together in one option group immediately following the tar command. if any of these options require arguments,
		// these arguments follow the option cluster in the same order as the options appear

		if (argv[1][0]!='-'){
			next_arg=2;
			for(int i=0; argv[1][i]!='\0'; ++i){
				std::string opt=std::string(1,'-')+argv[1][i];

				if (short_forms.find(opt)==short_forms.end())
					throw "Add_Option: old-style option not recognized";
				else {
					short_forms[opt]->number_of_times_assigned++;

					if (short_forms[opt]->number_of_arguments==ARGS_0)
						short_forms[opt]->Do_Actions(opt);
					else if (short_forms[opt]->number_of_arguments==ARGS_1)
						short_forms[opt]->Do_Actions(argv[next_arg]);
					else throw "multiple arguments not implemented";

					if (short_forms[opt]->number_of_arguments==ARGS_1)
						next_arg++;
				}
			}
		}

		//	look for short options or long options
		//	short options with mandatory arguments have their arguments either immediately following the option letter with
		//  no intervening space, or have their argument following the option letter with intervening space
		//	long options with mandatory arguments have their arguments either immediately following an equals sign which immediately
		//	follows the option, or are separated by white space

		for( ; next_arg<argc && argv[next_arg][0]=='-'; ++next_arg){
			if (argv[next_arg][1]=='-'){							// long option: argument space-separated or '=' separated
				std::string tmp=std::string(argv[next_arg]);
				std::string opt, arg;
				std::string::size_type i;

				if ((i=tmp.find_first_of('='))!=std::string::npos){
					opt=tmp.substr(0,i);
					arg=tmp.substr(i+1);
				}
				else {
					opt=tmp;

					if (long_forms.find(opt)==long_forms.end()){
						for(std::map<std::string,Option *>::iterator j=long_forms.begin(); j!=long_forms.end(); ++j)
							std::cerr << "longform[" << j->first << "]=" << j->second << std::endl;
						throw "unknown options 1 \""+opt+"\"";
					}

					if (long_forms.find(opt)->second->number_of_arguments==ARGS_0)
						arg=opt;
					else if (long_forms.find(opt)->second->number_of_arguments==ARGS_1)
						if (++next_arg>=argc)
							throw "long option requires argument";
						else {
							opt=tmp;
							arg=argv[next_arg];
						}
					else throw "number of arguments>1 not supported";
				}

				if (long_forms.find(opt)==long_forms.end())
					throw "unknown options 2 \""+opt+"\"";

				long_forms[opt]->number_of_times_assigned++;
				long_forms[opt]->Do_Actions(arg);
			}
			else if (argv[next_arg][1]!='\0'){						// short option
				std::string opt(argv[next_arg],2);
				std::string arg;

				if (short_forms.find(opt)->second->number_of_arguments==0)
					if (argv[next_arg][2]!='\0')
						throw "unrecognized option, switch without arguments, or extra characters in short option";
					else ;
				else if (short_forms.find(opt)->second->number_of_arguments==1)
					if (argv[next_arg][2]=='\0')
						if (next_arg+1<argc)
							arg=std::string(argv[++next_arg]);
						else throw "option needs argument";
					else
						arg=std::string(argv[next_arg++]+2);
				else throw "no support except for 0 or 1 arguments";

				if (short_forms.find(opt)==short_forms.end())
					throw "unknown options";

				short_forms[opt]->number_of_times_assigned++;
				short_forms[opt]->Do_Actions(arg);
			}
			else throw "option cannot be only -";
		}

		while(next_arg<argc)
			left_over_args.push_back(argv[next_arg++]);

		return left_over_args;
	}

	std::vector<std::string> left_over_args;
};
}

#endif /*Tar_Options_hpp_*/
