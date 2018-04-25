#ifndef AgendaUI_H
#define AgendaUI_H

#include "AgendaService.hpp"
#include <iostream>
#include <list>
#include <string>
#include <vector>
#include <iomanip>
#include <unistd.h>

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
	AgendaUI(){ 
		service.startAgenda();
		user_name = user_password = "";
	}
	void AgendaShow(){
		string step = "Start";
		while(step != "q"){
			cout << "----------------------------Agenda-----------------------------" << endl;
			cout << "Action :" << endl;
			cout << "l     - log in Agenda by user name and password" << endl;
			cout << "r     - register an Agenda account" << endl;
			cout << "q     - quit Agenda" << endl;
			cout << "---------------------------------------------------------------" << endl << endl;
			cout << "Agenda :~$ ";
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
		cout << "BYE! " << endl;
	}
	void userLoginIn_UI(){
		string name,password;
		//cout << endl << "[log in] [username] [password]" << endl;
		cout <<  endl << "[log in] [username] : "; 
		cin >> name;
		char* p = getpass("[log in] [password] : ");
		password = string(p);
		cout << "[log in] ";
		if(service.userLogIn(name,password)){
			user_name = name;
			user_password = password;
			cout << "succeed!" << endl;
			userMenu();
		}
		else
			cout << "Failed!Try again" << endl;
	}

	void userMenu(){
Step:	cout << "------------------------------Agenda-----------------------" << endl;
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
		cout << "--------------------------------------------------------" << endl << endl;
step:
		cout << endl << "Agenda@" << user_name << " :~# ";
		string choice;
		cin  >> choice;
		if(choice == "o")
			return;
		else if(choice == "dc"){
			service.deleteUser(user_name,user_password);
			cout <<  endl << "[delete agenda account] succeed!" << endl;
			return;
		}
		else if(choice == "lu"){
			cout << "[list all users]" << endl << endl;
			cout << "name                    email                   phone" << endl; 
			list<User> userList = service.listAllUsers();
			for(auto it : userList)//24//24
				cout << std::left << setw(24) << it.getName() <<  std::left << setw(24) << it.getEmail() << std::left << setw(24) << it.getPhone() << endl;
			goto step;
		}
		else if(choice == "cm"){
			int numOfParticipator;
			string nameOfParticipator;
			std::vector<string> participators;
			string title , startDate , endDate;
			cout << endl << "[create meeting] [the number of the participators]" << endl;
			cout << "[create meeting] ";
			cin >> numOfParticipator;
			for(int i = 1;i <= numOfParticipator;i++){
				cout << "[create meeting] [please enter the participator " << i << " ] " << endl;
				cout << "[create meeting] ";
				cin >> nameOfParticipator;
				participators.push_back(nameOfParticipator);
			}
			cout << "[create meeting] [title] [start time(xxxx-xx-xx/xx:xx)] [end time(xxxx-xx-xx/xx:xx)]" << endl;
			cout << "[create meeting] ";
			cin >> title >> startDate >> endDate;
			if(service.createMeeting(user_name,title,startDate, endDate,participators))
				cout << "[create meeting] succeed!" << endl;
			else cout << "[create meeting] error!" << endl;
			goto step;
		}
		else if(choice == "amp"){
			string title,nameOfParticipator;
			cout << endl << "[add participator] [meeting title] [participator username]" << endl;
			cout << "[add participator] ";
			cin >> title >> nameOfParticipator;
			if(service.addMeetingParticipator(user_name,title,nameOfParticipator))
				cout << "[add participator] succeed!" << endl;
			else 
				cout << "[add participator] error!" << endl;
			goto step;
		}
		else if(choice == "rmp"){
			string title,nameOfParticipator;
			cout << endl << "[remove participator] [meeting title] [participator username]" << endl;
			cout << "[remove participator] ";
			cin >> title >> nameOfParticipator;
			if(service.removeMeetingParticipator(user_name,title,nameOfParticipator))
				cout << "[remove participator] succeed!" << endl;
			else cout << "[remove participator] error!" << endl;
			goto step;
		}
		else if(choice == "rqm"){
			string title;
			cout << endl << "[quit meeting] [meeting title]" << endl;
			cout << "[quit meeting] ";
			cin >> title;
			if(service.quitMeeting(user_name,title))
				cout << "[quit meeting] succeed!" << endl;
			else
				cout << "[quit meeting] error!" << endl;
			goto step;
		}
		else if(choice == "la"){
			cout << endl << "list all meetings" << endl;
			cout << "title       sponsor     start time         end time          participators" << endl;
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
			goto step;
		}
		else if(choice == "las"){
			cout << endl << "list all sponsor meetings" << endl;
			cout << "title       sponsor     start time         end time          participators" << endl;
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
			goto step;
		}
		else if(choice == "lap"){
			cout << endl << "list all participator meetings" << endl;
			cout << "title       sponsor     start time        end time          participators" << endl;
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
			goto step;
		}
		else if(choice == "qm"){
			string title;
			cout << endl << "[query meeting] [title] " << endl;
			cout << "[query meeting] ";
			cin >> title;
			cout << "title       sponsor     start time        end time          participators" << endl;
			list<Meeting> meetingList = service.meetingQuery(user_name,title);
			if(meetingList.empty()) goto step;
			cout << std::left << setw(12) << meetingList.front().getTitle() << std::left << setw(12) << meetingList.front().getSponsor() << std::left << setw(18) << Date::dateToString(meetingList.front().getStartDate()) << std::left << setw(18) << Date::dateToString(meetingList.front().getEndDate());
			std::vector<string> participators = meetingList.front().getParticipator();
				for(auto it = participators.begin();it != participators.end();it++)
					if(it != --participators.end())
						cout  << (*it) << "," ;
					else cout << (*it) << endl;
			cout << endl;
			goto step;
		}
		else if(choice == "qt"){
			string startDate,endDate;
			cout << "[query meetings] [start time(xxxx-xx-xx/xx:xx)] [end time(xxxx-xx-xx/xx:xx)]" << endl;
			cout << "[query meetings] ";
			cin >> startDate >> endDate;
			cout << "[query meetings] " << endl << endl;
			cout << "title       sponsor     start time        end time          participators" << endl;
			list<Meeting> meetingList = service.meetingQuery(user_name,startDate,endDate);
			for(auto i : meetingList){
				cout << std::left << setw(12) << i.getTitle() << std::left << setw(12) << i.getSponsor() << std::left << setw(18) <<  Date::dateToString(i.getStartDate()) << std::left << setw(18) <<  Date::dateToString(i.getEndDate());
				std::vector<string> participators = i.getParticipator();
				for(auto it = participators.begin();it != participators.end();it++)
					if(it != --participators.end())
						cout  << (*it) << "," ;
					else cout << (*it) << endl;
			}
			cout << endl;
			goto step;
		}
		else if(choice == "dm"){
			string title;
			cout <<  endl << "[delete meeting] [title]" << endl;
			cout << "[delete meeting] " ;
			cin >> title;
			if(service.deleteMeeting(user_name,title))
				cout << "[delete meeting] succeed!" << endl;
			else cout << "[delete meeting] delete meeting fail!" << endl;
			goto step;
		}
		else if(choice == "da"){
			if(service.deleteAllMeetings(user_name))
				cout << endl << "[delete all meeting] succeed!" << endl;
			goto step;
		}
		else if(choice == "o")	return;
		else if(choice == "help") goto Step;
		else{
			cout << "Sorry!We do not have such action!" << endl;
			goto step;
		}
	}

	void userRegister_UI(){
		string name,password,email,phone;
		cout << endl << "[register] [username] [password] [email] [phone]" << endl;
		cout << "[register] ";
		cin >> name >> password >> email >> phone;
		if(service.userRegister(name,password,email,phone))
			cout << "[register] succeed!" << endl;
		else cout << "[register] This username has been registered!" << endl;
	}
};



#endif