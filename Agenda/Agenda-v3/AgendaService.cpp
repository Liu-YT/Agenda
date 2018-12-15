#include "AgendaService.hpp"
#include "CatchError.hpp"
using std::list;
using std::string;

AgendaService::AgendaService(){
	startAgenda();
}

AgendaService::~AgendaService(){
	quitAgenda();
}

bool AgendaService::userLogIn(const std::string &userName, const std::string &password){
	std::list<User> tmp = m_storage->queryUser([userName](const User &x){ return (x.getName() == userName); });
	if(tmp.size() == 0)	throw Error(string("No such a user!"));
	else if(tmp.front().getPassword() != password)	throw Error(string("The password is't correct!"));
	else return true;
}

bool AgendaService::userRegister(const std::string &userName, const std::string &password,
                const std::string &email, const std::string &phone){
	std::list<User> tmp = m_storage->queryUser([userName](const User &x){ return (x.getName() == userName); });
	if(tmp.size() != 0)	throw Error(string("This username has been registered!"));
	else{
		m_storage->createUser(User(userName,password,email,phone));
		return true;
	}
}

bool AgendaService::deleteUser(const std::string &userName, const std::string &password){
	int count = m_storage->deleteUser([userName](const User& x){ return (x.getName() == userName); });
	if(count == 0)	throw Error(string("No such a user!"));
	else{
		m_storage->deleteMeeting([userName](const Meeting& m){ return (m.getSponsor() == userName); });
		m_storage->updateMeeting([userName](const Meeting& m){ return (m.isParticipator(userName));},
								[&](Meeting &m){m.removeParticipator(userName);});
		m_storage->deleteMeeting([](const Meeting& m){ return m.getParticipator().empty(); });
																	
		return true;
	}
}

std::list<User> AgendaService::listAllUsers(void) const{
	list<User> tmp = m_storage->queryUser([](const User &x){ return true; });
	return tmp;
}

bool AgendaService::createMeeting(const std::string &userName, const std::string &title,
                 const std::string &startDate, const std::string &endDate,
                 const std::vector<std::string> &participator){
	if(m_storage->queryUser([userName](const User& x){return (x.getName() == userName);}).size() == 0)
		throw Error(string("The sponsor is't a user of Agenda!"));
	if(participator.empty())	throw Error(string("No participators in this meeting!"));
	for(auto it : participator)
		if(it == userName)
			throw Error(string("Can't be the participator of your sponsoring meeting!"));
	std::list<User> userList = listAllUsers();
	for(auto i : participator){
		int ans = 0;
		for(auto newi : userList)
			if(newi.getName() == i)	ans = 1;
		if(ans == 0)	throw Error(string("Some of the participators are not a user of Agenda!"));
	}
	if(endDate <= startDate)
		throw Error(string("The time is not reasonable!"));
	else if( Date::dateToString(Date::stringToDate(startDate)) == "0000-00-00/00:00" || Date::dateToString(Date::stringToDate(endDate)) == "0000-00-00/00:00")
		throw Error(string("The time input is invalid!"));
	else if(m_storage->queryMeeting([title](const Meeting& m){ return (m.getTitle() == title); }).size() != 0)
		throw Error(string("The title has been used"));
	else if(!(m_storage->queryMeeting([userName,startDate,endDate](const Meeting& m){
								return ((m.getSponsor() == userName || m.isParticipator(userName)) && !(Date(endDate) <= m.getStartDate() || Date(startDate) >= m.getEndDate()));	})).empty())
		throw Error(string("The sponsor is busy at that time!"));
	else{
		std::vector<std::string> v;
		Meeting newMeeting(userName,v,Date::stringToDate(startDate),Date::stringToDate(endDate),title);
		for(auto it : participator)
			if(newMeeting.isParticipator(it))	throw (Error(string("Some participators want to join the meeting twice!")));
			else{
				if(!m_storage->queryMeeting([it,startDate,endDate](const Meeting& m){
								return ((m.getSponsor() == it || m.isParticipator(it)) && !(Date(endDate) <= m.getStartDate() || Date(startDate) >= m.getEndDate())); }).empty())	
						throw Error(string("Some participators are busy at that time!"));
				else	newMeeting.addParticipator(it);
			}
		m_storage->createMeeting(newMeeting);
		return true;
	}
}

bool AgendaService::addMeetingParticipator(const std::string &userName,
                          const std::string &title,
                          const std::string &participator){
	if(m_storage->queryUser([participator](const User& x){return (x.getName() == participator);}).empty())	throw Error(string("The participator is not a user of Agenda!"));
	else if(participator == userName)	throw Error(string("Can't add yourself be a participator!"));
	for(auto &it : listAllSponsorMeetings(userName))
		if(it.getTitle() == title && !it.isParticipator(participator)){
			string startDate = Date::dateToString(it.getStartDate());
			string endDate = Date::dateToString(it.getEndDate());
			if(!m_storage->queryMeeting([participator,startDate,endDate](const Meeting& m){ return ((m.getSponsor() == participator || m.isParticipator(participator)) && !(Date(endDate) <= m.getStartDate() || Date(startDate) >= m.getEndDate())); }).empty())	throw Error(string("The participator is busy at that time!"));
			m_storage->updateMeeting([userName,title](const Meeting& m){ return (m.getTitle() == title && m.getSponsor() == userName);},[participator](Meeting &m){ m.addParticipator(participator);});
			return true;
		}
	throw Error(string("You don't have sucn a meeting!"));
}

bool AgendaService::removeMeetingParticipator(const std::string &userName,
                             const std::string &title,
                             const std::string &participator){
	for(auto it : listAllSponsorMeetings(userName))
		if(it.getTitle() == title){
			if (m_storage->updateMeeting([participator,title](const Meeting& m){ return (m.isParticipator(participator) && m.getTitle()==title);},
								[&](Meeting &m){ m.removeParticipator(participator);})){
				m_storage->deleteMeeting([](const Meeting& m){return m.getParticipator().empty();});
				return true;
			}
			throw Error(string("This user is not a participator of thie meeting!"));
		}
	throw Error(string("You may not the sponsor of thie meeting or this participator is not a participator of this meeting!"));
}

bool AgendaService::quitMeeting(const std::string &userName, const std::string &title){
	if (m_storage->updateMeeting([userName,title](const Meeting& m){ return (m.isParticipator(userName) && m.getTitle()==title);},
								[&](Meeting &m){ m.removeParticipator(userName);})){
		m_storage->deleteMeeting([](const Meeting& m){return m.getParticipator().empty();});
		return true;
	}
	throw Error(string("You may not a participator of thie meeting!"));
}

std::list<Meeting> AgendaService::meetingQuery(const std::string &userName,
                              const std::string &title) const{
	std::list<Meeting> meetingList = m_storage->queryMeeting([title,userName](const Meeting& m){ return (m.getTitle() == title && (m.getSponsor() == userName || m.isParticipator(userName)));});
	if(meetingList.empty())
		throw Error(string("May be you are't a member of this meeting or no such a meeting!"));
	return meetingList;

}

std::list<Meeting> AgendaService::meetingQuery(const std::string &userName,
                              const std::string &startDate,
                              const std::string &endDate) const{
	if(Date::dateToString(Date::stringToDate(startDate)) == "0000-00-00/00:00" || Date::dateToString(Date::stringToDate(endDate)) == "0000-00-00/00:00")
		throw Error(string("The time input is invalid!"));
	std::list<Meeting> meetingList = m_storage->queryMeeting([userName,startDate,endDate](const Meeting& m){
								return ((m.getSponsor() == userName || m.isParticipator(userName)) && !(Date(endDate) < m.getStartDate() || Date(startDate) > m.getEndDate()));	});
	return meetingList;	
}

std::list<Meeting> AgendaService::listAllMeetings(const std::string &userName) const{
	std::list<Meeting> meetingList = m_storage->queryMeeting([userName](const Meeting& m){ return (m.isParticipator(userName) || m.getSponsor() == userName); });
	return meetingList;
}

std::list<Meeting> AgendaService::listAllSponsorMeetings(const std::string &userName) const{
	std::list<Meeting> meetingList = m_storage->queryMeeting([userName](const Meeting& m){ return(m.getSponsor() == userName); });
	return meetingList;
}

std::list<Meeting> AgendaService::listAllParticipateMeetings(
  const std::string &userName) const{
	std::list<Meeting> meetingList = m_storage->queryMeeting([userName](const Meeting& m){ return(m.isParticipator(userName)); });
	return meetingList;
}

bool AgendaService::deleteMeeting(const std::string &userName, const std::string &title){
	int count = m_storage->deleteMeeting([title,userName](const Meeting& m){ return (userName == m.getSponsor() && title == m.getTitle());	});
	if(count == 0)
		throw Error(string("You are not the sponsor of this meeting!"));
	return count;
}

bool AgendaService::deleteAllMeetings(const std::string &userName){
	int count = m_storage->deleteMeeting([userName](const Meeting& m){ return(m.getSponsor() == userName);});
	return count;
}

void AgendaService::startAgenda(void){
	m_storage = Storage::getInstance();
}

void AgendaService::quitAgenda(void){
	m_storage->sync();
}
