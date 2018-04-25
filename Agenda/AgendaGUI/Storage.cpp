#include "Storage.hpp"
#include <algorithm>
#include <iostream>
#include <fstream>

using std::string;
using std::endl;
using std::function;

std::shared_ptr<Storage> Storage::m_instance = nullptr;

Storage::Storage(){
 readFromFile();
 m_dirty = false;
}

bool Storage::readFromFile(){
 std::ifstream file("tmp/user.csv");
 if(!file.is_open())  return false;
 string _name = "";
 string password = "";
 string email = "";
 string phone = "";
 while(file.peek() != EOF){
  getline(file,_name,',') , _name = _name.substr(1,_name.size()-2);
  getline(file,password,',') , password = password.substr(1,password.size()-2);
  getline(file,email,',') , email = email.substr(1,email.size()-2);
  getline(file,phone) , phone = phone.substr(1,phone.size()-2) , m_userList.push_back(User(_name,password,email,phone));
 }
 file.close();

 file.open("tmp/meeting.csv");
 if(!file.is_open())  return false;
 string s_name = "";
 string p_name = "";
 password = "";
 string m_startDate;
   string m_endDate;
   string m_title;
 while(file.peek() != EOF){
  std::vector<std::string> name;
  getline(file,s_name,',') , s_name = s_name.substr(1,s_name.size()-2);
  getline(file,p_name,',');
   p_name = p_name.substr(1,p_name.size()-2);
   int i = 0;
   int ans = 0;
   string str = "";
   for(int j = 0;j < p_name.size();j++)
    if(p_name[j] == '&'){
     str = p_name.substr(i,ans);
     name.push_back(str);
     i = j+1;
     ans = 0;
    }
    else ans++;
   str = p_name.substr(i,ans);
   name.push_back(str);
  getline(file,m_startDate,',') , m_startDate = m_startDate.substr(1,m_startDate.size()-2);
  getline(file,m_endDate,',') , m_endDate = m_endDate.substr(1,m_endDate.size()-2);
  getline(file,m_title), m_title = m_title.substr(1,m_title.size()-2),m_meetingList.push_back(Meeting(s_name,name,Date(m_startDate),Date(m_endDate),m_title));
 }
 file.close();
 return true;
}

bool Storage::writeToFile(){
 std::ofstream file("tmp/user.csv");
 if(!file.is_open()) return false;
 for(auto it = m_userList.begin();it != m_userList.end();it++)
   file << "\"" << it->getName() << "\"" << "," << "\"" << it->getPassword() << "\"" << "," << "\"" << it->getEmail() << "\"" << "," << "\"" << it->getPhone() << "\"" << endl;
 file.close();

 file.open("tmp/meeting.csv");
 if(!file.is_open()) return false;
 for(auto it = m_meetingList.begin();it != m_meetingList.end();it++){
  file << "\"" << it->getSponsor() << "\"" << "," << "\"";
  for(int i = 0;i < it->getParticipator().size();i++){
   file << (it->getParticipator())[i];
   if(i < it->getParticipator().size()-1)
    file << "&" ;
   else
    file << "\"" << ",";
  }
  file << "\"" << Date::dateToString(it->getStartDate()) << "\"" << "," << "\"" << Date::dateToString(it->getEndDate()) << "\"" << "," << "\"" <<it->getTitle() << "\"" << endl;
 }
 file.close();
 m_dirty = false;
 return true;
}

std::shared_ptr<Storage> Storage::getInstance(){
 if(m_instance == nullptr) {
  std::shared_ptr<Storage> new_instance(new Storage());
  m_instance = new_instance;
 }
 return m_instance;
}

Storage::~Storage(){
 writeToFile();
 m_instance = nullptr;
}

void Storage::createUser(const User &t_user){
 //if(find(m_userList.begin(),m_userList.end(),t_user) == m_userList.end())
 m_userList.push_back(t_user);
 m_dirty = true;
}

std::list<User> Storage::queryUser(std::function<bool(const User &)> filter) const{
 std::list<User> otherUserList;
 for(auto &i : m_userList)
  if(filter(i))
   otherUserList.push_back(i);
 return otherUserList;
}

int Storage::updateUser(std::function<bool(const User &)> filter,std::function<void(User &)> switcher){
 int ans = 0;
 for(auto &i : m_userList)
  if(filter(i))
   switcher(i),++ans;
 if(ans > 0)  m_dirty = true;
 return ans;
}

int Storage::deleteUser(std::function<bool(const User &)> filter){
 int ans = 0;
 for(auto it = m_userList.begin();it != m_userList.end();)
  if(filter(*it))
   it = m_userList.erase(it) , ++ans;
  else ++it;
 if(ans > 0)  m_dirty = true;
 return ans;
}

void Storage::createMeeting(const Meeting &t_meeting){
 //if(find(m_meetingList.begin(),m_meetingList.end(),t_meeting) == m_meetingList.end())
 m_meetingList.push_back(t_meeting);
 m_dirty = true;
}

std::list<Meeting> Storage::queryMeeting(std::function<bool(const Meeting &)> filter) const{
 std::list<Meeting> otherMeeting;
 for(auto &i : m_meetingList)
  if(filter(i))
   otherMeeting.push_back(i);
 return otherMeeting;
}

int Storage::updateMeeting(std::function<bool(const Meeting &)> filter,std::function<void(Meeting &)> switcher){
 int ans = 0;
 for(auto &i : m_meetingList)
  if(filter(i))
   switcher(i),ans++;
 if(ans > 0)  m_dirty = true;
 return ans;
}

int Storage::deleteMeeting(std::function<bool(const Meeting &)> filter){
 int ans = 0;
 for(auto it = m_meetingList.begin();it != m_meetingList.end();)
  if(filter(*it)){
   it = m_meetingList.erase(it);
   ans++;
  }
  else
   ++it;
 if(ans > 0)  m_dirty = true;
 return ans;
}

bool Storage::sync(void){
 if(m_dirty)
  return writeToFile();
}
