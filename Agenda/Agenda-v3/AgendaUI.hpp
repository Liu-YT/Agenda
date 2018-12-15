#ifndef AgendaUI_H
#define AgendaUI_H

#define NONE "\033[m"
#define RED "\033[0;32;31m"
#define LIGHT_RED "\033[1;31m"
#define GREEN "\033[0;32;32m"
#define LIGHT_GREEN "\033[1;32m"
#define BLUE "\033[0;32;34m"
#define LIGHT_BLUE "\033[1;34m"
#define DARY_GRAY "\033[1;30m"
#define CYAN "\033[0;36m"
#define LIGHT_CYAN "\033[1;36m"
#define PURPLE "\033[0;35m"
#define LIGHT_PURPLE "\033[1;35m"
#define BROWN "\033[0;33m"
#define YELLOW "\033[1;33m"
#define LIGHT_GRAY "\033[0;37m"
#define WHITE "\033[1;37m"

#include "MD5.h"
#include <assert.h>
#include <cstdio>
#include "CatchError.hpp"
#include "AgendaService.hpp"
#include <iostream>
#include <list>
#include <string>
#include <vector>
#include <iomanip>
#include <unistd.h>
#include <termios.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <signal.h>

using std::setw;
using std::string;
using std::endl;
using std::cin;
using std::cout;
using std::vector;
using std::list;


class AgendaUI
{
public:
	static char *line_read;
	static bool isNumber(const string& str){
		for(int i = 0;i < str.size();i++)
			if(str[i] < '0' || str[i] > '9')	return false;
		return true;
	}
	static int getch(void) {		
        int c=0;
        struct termios org_opts, new_opts;
        int res=0;
        //-----  store old settings -----------
        res=tcgetattr(STDIN_FILENO, &org_opts);
        assert(res==0);
        //---- set new terminal parms --------
        memcpy(&new_opts, &org_opts, sizeof(new_opts));
        new_opts.c_lflag &= ~(ICANON | ECHO | ECHOE | ECHOK | ECHONL | ECHOPRT | ECHOKE | ICRNL);
        tcsetattr(STDIN_FILENO, TCSANOW, &new_opts);
        c=getchar();
        //------  restore old settings ---------
        res=tcsetattr(STDIN_FILENO, TCSANOW, &org_opts);assert(res==0);
        return c;
	}
	~AgendaUI(){
		if(line_read)	delete [](line_read);
		line_read = NULL;
		AgendaQuit_UI();
	}
	AgendaUI(){ 
		service.startAgenda();
		user_name = user_password = "";
	}
	void AgendaShow(){
		string step = "Start";
		while(step != "q"){
			cout << endl;
			cout << LIGHT_RED << "------------------------------------Agenda--------------------------------------" << endl;
			cout << "Action :" << endl;
			cout << "l     - log in Agenda by user name and password" << endl;
			cout << "r     - register an Agenda account" << endl;
			cout << "q     - quit Agenda" << endl;
			cout << "--------------------------------------------------------------------------------" <<  endl << endl;
			cout << "Agenda :~$ " << NONE;
			cin >> step;
			if(step.size() > 1) cout << "Unvalideated Input!Please input again" << endl;
			else
				switch(step[0]){
					case 'l':
							userLoginIn_UI();
							break;
					case 'r':
							userRegister_UI();
							break;
					case 'q':
							AgendaQuit_UI();
				        	break;
					 default:
					 		cout << "Unvalideated Input!Please input again" << endl;
				}
		}
	}
private:
	AgendaService service;
	string user_name,user_password; 
	void AgendaQuit_UI(){
		service.quitAgenda();
	}
	void userLoginIn_UI(){
		string name,password,tmp;
		//get username
		cout <<  endl << "[log in] [username] : "; 
		std::getline(cin,name);
		std::getline(cin,name);
again:  //get password
		char p[100] = {};
		int i =0;
		cout  << "[log in] [password] : ";
		while((p[i] = getch()) != '\n'){
			if(p[i] == 127) i--,cout << '\b' << ' ' << '\b';
			else cout  << '*',i++;
		}
		p[i] = '\0';
		password = getMD5(string(p));
		cout <<  endl << "[log in] " ;
		try{
			service.userLogIn(name,password);
			user_name = name;
			user_password = password;
			cout << LIGHT_GREEN << "succeed!" << NONE << endl << endl;
			userMenu();
		}
		catch(Error tmp){
			cout << LIGHT_RED << "[ERROR REASON] : " << tmp.get() << NONE << endl << endl;
			if(tmp.get() == "The password is't correct!"){
				cout << LIGHT_RED << "Would you want to try again? " << endl;
				cout << "[Y / N]  " << NONE;
				char choice;
				cin >> choice;
				if(choice == 'Y' || choice == 'y'){
					getchar();
					goto again;
				}
			}
		}
	}
	char * rl_gets (){
		/* If the buffer has already been allocated, return the memory to the free pool. */
		if (line_read){
		  delete []line_read;
		  line_read = (char *)NULL;
		}
		string tmp = "Agenda@" + user_name + " :~# ";
		/* Get a line from the user. */
		cout << LIGHT_GREEN << endl;
		line_read = readline (tmp.c_str());
		cout << NONE;
		/* If the line has any text in it, save it on the history. */
		if (line_read && *line_read)
			add_history (line_read);

		return (line_read);
	}

	void userMenu(){
		cout << endl;
step1:	
		cout << LIGHT_RED << "------------------------------------Agenda--------------------------------------" << NONE << endl;
		cout << "Action :" << endl;
		cout << "o     - log out Agenda" << endl;
		cout << "dc    - delete Agenda account" << endl;
		cout << "lu    - list all Agenda user" << endl;
		cout << "cm    - create a meeting" << endl;
		cout << "amp   - add meeting participator" << endl;
		cout << "rmp   - remove meeting participator" << endl;
		cout << "rqm   - request to quit meeting" << endl;
		cout << "la    - list all meetings" << endl;
		cout << "las   - list all sponsor meetings" << endl;
		cout << "lap   - list all participator meetings" << endl;
		cout << "qm    - query meeting by title" << endl;
		cout << "qt    - query meeting by time interval" << endl;
		cout << "dm    - delete meeting by title" << endl;
		cout << "da    - delete all meetings" << endl;
		cout << "help  - list action" << endl;
		cout <<  LIGHT_RED << "--------------------------------------------------------------------------------" << NONE << endl << endl;
step2:
		cout << endl;
		string choice = string(rl_gets());
		if(choice == "o")	return;
		else if(choice == "dc"){
			string password;
	step3:		
			char c;
			cout << YELLOW << "Do you really want to do this?" << endl;
			cout << "[Y / N] : " << NONE;
			cin >> c;
			if(c == 'n' || c == 'N')  goto step2;
			cout << LIGHT_RED << "[delete agenda account] [please enter your password again]" << endl;
			cout << "[delete agenda account] : " ;
			cin >> password;
			if(user_password  == getMD5(password)){ 
				service.deleteUser(user_name,user_password);
				cout <<  endl << "[delete agenda account] " << LIGHT_GREEN << "succeed!" << NONE << endl;
				return;
			}
			else{
				cout << endl << "[delete agenda account] " << LIGHT_RED << "failed!" << NONE << endl;
				cout << YELLOW << "The password is wrong.Please try again!" << NONE << endl;
				goto step3;
			}
		}
		else if(choice == "lu"){
			cout << YELLOW << "[list all users]" << NONE << endl << endl;
			cout << LIGHT_BLUE << "name                    email                   phone" << NONE<< endl; 
			list<User> userList = service.listAllUsers();
			for(auto it : userList)
				cout << std::left << setw(24) << it.getName() <<  std::left << setw(24) << it.getEmail() << std::left << setw(24) << it.getPhone() << endl;
			goto step2;
		}
		else if(choice == "cm"){
			try{
				string numOfParticipator;
				string nameOfParticipator;
				std::vector<string> participators;
				string title , startDate , endDate;
				cout << endl << YELLOW << "[create meeting] [the number of the participators]" << NONE << endl;
				cout << "[create meeting] ";
				cin >> numOfParticipator;
				if(!isNumber(numOfParticipator)){
					cout << "[Error] Not a number!" << endl;
					goto step2;
				}
				int n = stoi(numOfParticipator);
				for(int i = 1;i <= n;i++){
					cout << YELLOW << "[create meeting] [please enter the participator " << i << " ] " << NONE << endl;
					cout << "[create meeting] ";
					cin >> nameOfParticipator;
					participators.push_back(nameOfParticipator);
				}
				cout << YELLOW << "[create meeting] [title] "  << NONE<< endl;
				cout << "[create meeting] ";
				cin >> title;
				cout << YELLOW << "[create meeting] [start time(xxxx-xx-xx/xx:xx)]" << NONE << endl;
				line_read = readline("[create meeting] ");
				startDate = string(line_read);
				add_history(line_read);
				free(line_read);
				line_read = NULL;
				cout << YELLOW << "[create meeting] [end time(xxxx-xx-xx/xx:xx)] " << NONE << endl;
				line_read = readline("[create meeting] ");
				endDate = string(line_read);
				add_history(line_read);
				free(line_read);
				line_read = NULL;
				if(service.createMeeting(user_name,title,startDate, endDate,participators))
					cout << "[create meeting] " << LIGHT_GREEN << "succeed!" << NONE << endl;
			}
			catch(Error tmp){
				cout << "[create meeting] " << LIGHT_RED << "error!"  << NONE << endl;
				cout << LIGHT_RED << "[ERROR REASON] : " << tmp.get() << NONE << endl;
			}
			goto step2;
		}
		else if(choice == "amp"){
			try{
				string title,nameOfParticipator;
				cout << endl << YELLOW << "[add participator] [meeting title] " << NONE << endl;
				cout << "[add participator] ";
				cin >> title;
				cout << YELLOW << "[add participator] [participator username]" << NONE << endl;
				cout << "[add participator] ";
				std::getline(cin,nameOfParticipator);
				std::getline(cin,nameOfParticipator);
				cin.clear();
				service.addMeetingParticipator(user_name,title,nameOfParticipator);
				cout << "[add participator] " << LIGHT_GREEN << "succeed!" << NONE << endl;
			}
			catch(Error tmp){
				cout << "[add participator] " << LIGHT_RED << "error!" << NONE << endl;
 				cout <<  LIGHT_RED << "[ERROR REASON] : " << tmp.get() << NONE << endl;
			}
			goto step2;
		}
		else if(choice == "rmp"){
			try{
				string title,nameOfParticipator;
				cout << endl << "[remove participator] [meeting title]" << endl;
				cout << "[remove participator] ";
				cin >> title;
				cout << "[remove participator] [participator username]" << endl;
				cout << "[remove participator] ";
				std::getline(cin,nameOfParticipator);
				std::getline(cin,nameOfParticipator);
				cin.clear();
				service.removeMeetingParticipator(user_name,title,nameOfParticipator);
				cout << "[remove participator]" << LIGHT_GREEN << " succeed!" << endl;
			}
			catch(Error tmp){
				cout << "[remove participator] " << LIGHT_RED << "error!" << endl;
				cout << "[ERROR REASON] : " << LIGHT_RED << tmp.get() << NONE << endl;
			}
			goto step2;
		}
		else if(choice == "rqm"){
			try{
				string title;
				cout << endl << YELLOW << "[quit meeting] [meeting title]" << NONE << endl;
				cout << "[quit meeting] ";
				cin >> title;
				if(service.quitMeeting(user_name,title))
					cout << "[quit meeting] " << LIGHT_GREEN << "succeed!" << endl;
			}
			catch(Error tmp){
				cout <<  "[quit meeting] " << LIGHT_RED << "error!" << endl;
				cout << "[ERROR REASON] : " << LIGHT_RED << tmp.get() << NONE << endl;
			}
			goto step2;
		}
		else if(choice == "la"){
			cout << endl << YELLOW << "list all meetings" << NONE << endl;
			cout << LIGHT_BLUE << "title       sponsor     start time         end time          participators" << NONE << endl;
			list<Meeting> meetingList = service.listAllMeetings(user_name);
			for(auto i : meetingList){
				cout << std::left << setw(12) << i.getTitle() << std::left << setw(12) << i.getSponsor() << std::left << setw(18) <<  Date::dateToString(i.getStartDate()) << std::left << setw(18) <<  Date::dateToString(i.getEndDate());
				std::vector<string> participators = i.getParticipator();
				for(auto it = participators.begin();it != participators.end();it++)
					if(it != --participators.end())
						cout  << (*it) << "," ;
					else cout << (*it) << endl;
			}
			cout << endl;
			goto step2;
		}
		else if(choice == "las"){
			cout << endl << YELLOW << "list all sponsor meetings" << NONE << endl;
			cout << LIGHT_BLUE << "title       sponsor     start time         end time          participators" << NONE << endl;
			list<Meeting> meetingList = service.listAllSponsorMeetings(user_name);
			for(auto i : meetingList){
				cout << std::left <<  setw(12) << i.getTitle() << std::left <<  setw(12) << i.getSponsor() << std::left << setw(18) <<  Date::dateToString(i.getStartDate()) << std::left << setw(18) <<  Date::dateToString(i.getEndDate());
				std::vector<string> participators = i.getParticipator();
				for(auto it = participators.begin();it != participators.end();it++)
					if(it != --participators.end())
						cout  << (*it) << "," ;
					else cout << (*it) << endl;
			}
			cout << endl;
			goto step2;
		}
		else if(choice == "lap"){
			cout << endl << YELLOW << "list all participator meetings" << NONE << endl;
			cout << LIGHT_BLUE << "title       sponsor     start time        end time          participators" << NONE << endl;
			list<Meeting> meetingList = service.listAllParticipateMeetings(user_name);
			for(auto i : meetingList){
				cout << std::left << setw(12) << i.getTitle() << std::left << setw(12) << i.getSponsor() << std::left << std::left << setw(18) <<  Date::dateToString(i.getStartDate()) << std::left << setw(18) <<  Date::dateToString(i.getEndDate());
				std::vector<string> participators = i.getParticipator();
				for(auto it = participators.begin();it != participators.end();it++)
					if(it != --participators.end())
						cout  << (*it) << "," ;
					else cout << (*it) << endl;
			}
			cout << endl;
			goto step2;
		}
		else if(choice == "qm"){
			try{
				string title;
				cout << endl << YELLOW << "[query meeting] [title] " << NONE << endl;
				cout << "[query meeting] ";
				cin >> title;
				cout << LIGHT_BLUE << "title       sponsor     start time        end time          participators" << NONE << endl;
				list<Meeting> meetingList = service.meetingQuery(user_name,title);
				if(meetingList.empty()) goto step2;
				cout << std::left << setw(12) << meetingList.front().getTitle() << std::left << setw(12) << meetingList.front().getSponsor() << std::left << setw(18) << Date::dateToString(meetingList.front().getStartDate()) << std::left << setw(18) << Date::dateToString(meetingList.front().getEndDate());
				std::vector<string> participators = meetingList.front().getParticipator();
					for(auto it = participators.begin();it != participators.end();it++)
						if(it != --participators.end())
							cout  << (*it) << "," ;
						else cout << (*it) << endl;
			}
			catch(Error tmp){
				cout <<  "[quit meeting] " << LIGHT_RED << "error!" << endl;
				cout << "[ERROR REASON] : " << LIGHT_RED << tmp.get() << NONE << endl;
			}
			goto step2;
		}
		else if(choice == "qt"){
			try{
				string startDate,endDate;
				cout << YELLOW << "[query meetings] [start time(xxxx-xx-xx/xx:xx)] " << NONE << endl;
				line_read = readline("[query meeting] : ");
				startDate = string(line_read);
				add_history(line_read);
				delete []line_read;
				line_read = NULL;
				cout << YELLOW << "[query meetings] [end time(xxxx-xx-xx/xx:xx)] " << NONE << endl;
				line_read = readline("[query meeting] : ");
				endDate = string(line_read);
				add_history(line_read);
				delete []line_read;
				line_read = NULL;
				cout << "[query meetings] " << endl << endl;
				cout << LIGHT_BLUE << "title       sponsor     start time        end time          participators" << NONE << endl;
				list<Meeting> meetingList = service.meetingQuery(user_name,startDate,endDate);
				for(auto i : meetingList){
					cout << std::left << setw(12) << i.getTitle() << std::left << setw(12) << i.getSponsor() << std::left << setw(18) <<  Date::dateToString(i.getStartDate()) << std::left << setw(18) <<  Date::dateToString(i.getEndDate());
					std::vector<string> participators = i.getParticipator();
					for(auto it = participators.begin();it != participators.end();it++)
						if(it != --participators.end())
							cout  << (*it) << "," ;
						else cout << (*it) << endl;
				}
			}
			catch(Error tmp){
				cout <<  "[quit meeting] " << LIGHT_RED << "error!" << endl;
				cout << "[ERROR REASON] : " << LIGHT_RED << tmp.get() << NONE << endl;
			}
			goto step2;
		}
		else if(choice == "dm"){
			try{
				string title;
				cout <<  endl << YELLOW << "[delete meeting] [title]" << NONE << endl;
				cout << "[delete meeting] " ;
				cin >> title;
				service.deleteMeeting(user_name,title);
				cout << "[delete meeting] " << LIGHT_GREEN << "succeed!" << NONE << endl;
			}
			catch(Error tmp){
				cout << "[delete meeting] " << LIGHT_RED << "delete meeting fail!" << endl;
				cout << "[ERROR REASON] : " << LIGHT_RED << tmp.get() << NONE << endl;
			}
			goto step2;
		}
		else if(choice == "da"){
			if(service.deleteAllMeetings(user_name))
				cout << endl << "[delete all meeting]" << LIGHT_GREEN << " succeed!" << NONE << endl;
			goto step2;
		}
		else if(choice == "o")	return;
		else if(choice == "help") goto step1;
		else{
			cout << YELLOW << "Sorry!We do not have such action!"  << NONE << endl;
			goto step2;
		}
	}

	void userRegister_UI(){
		string name,password,email,phone;
		cout  << "[register] [username] : ";
		getchar();
		std::getline(cin,name);
		cout  << "[register] [password] : ";
		cin >> password;
		password = getMD5(string(password));
		cout  << "[register] [email] : ";
		cin >> email;
		cout  << "[register] [phone] : ";
		cin >> phone;
		cout  << "[register] "  << endl;
		try{
			service.userRegister(name,password,email,phone);
			cout  << "[register] " << LIGHT_GREEN  << "succeed!"  << NONE << endl;
		}
		catch(Error tmp){
			cout << "[register] " << LIGHT_RED << "failed! " << endl << "[ERROR REASON] : " << tmp.get() << NONE << endl;
		}
	}
};

char * AgendaUI::line_read = (char*)NULL;


#endif
