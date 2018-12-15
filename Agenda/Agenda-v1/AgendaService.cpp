#include "AgendaService.hpp"
using std::list;
using std::string;

AgendaService::AgendaService(){
	startAgenda();
}

AgendaService::~AgendaService(){
	quitAgenda();
}

bool AgendaService::userLogIn(const std::string &userName, const std::string &password){
	std::list<User> tmp = m_storage->queryUser([userName](const User &x){ if(x.getName() == userName)	return true;
														else return false;});
	if(tmp.size() == 0)	return false;
	else if(tmp.front().getPassword() != password)	return false;
	else return true;
}

bool AgendaService::userRegister(const std::string &userName, const std::string &password,
                const std::string &email, const std::string &phone){
	std::list<User> tmp = m_storage->queryUser([userName](const User &x){ if(x.getName() == userName)	return true;
														else return false;});
	if(tmp.size() != 0)	return false;
	else{
		m_storage->createUser(User(userName,password,email,phone));
		return true;
	}
}

bool AgendaService::deleteUser(const std::string &userName, const std::string &password){
	int count = m_storage->deleteUser([userName](const User& x){ if(x.getName() == userName)	return true;
												else return false;});
	if(count == 0)	return false;
	else{
		m_storage->deleteMeeting([userName](const Meeting& m){ if(m.getSponsor() == userName) return true;	
															      else 	return false; });
		m_storage->updateMeeting([userName](const Meeting& m){ if(m.isParticipator(userName))	return true;
																   else return false;},
								[&](Meeting &m){
									 m.removeParticipator(userName);
									  if(m.getParticipator().size() == 0){
									  	 std::string title = m.getTitle();
									 	 m_storage->deleteMeeting([title](const Meeting& newm){ 
									 		 if(newm.getTitle() == title) return true;	
											 else 	return false; });
									  }
								 });
																	
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
	if(participator.empty())	return false;
	for(auto it : participator)
		if(it == userName)
			return false;
	std::list<User> userList = listAllUsers();
	for(auto i : participator){
		int ans = 0;
		for(auto newi : userList)
			if(newi.getName() == i)	ans = 1;
		if(ans == 0)	return false;
	}
	if(m_storage->queryUser([userName](const User& x){if(x.getName() == userName)	return true; else return false;}).size() == 0)	return false;
	else if(endDate <= startDate || Date::dateToString(Date::stringToDate(startDate)) == "0000-00-00/00:00" || Date::dateToString(Date::stringToDate(endDate)) == "0000-00-00/00:00") return false;
	else if(m_storage->queryMeeting([title](const Meeting& m){ if(m.getTitle() == title) return true; else return false; }).size() != 0)	return false;
	else if(!(m_storage->queryMeeting([userName,startDate,endDate](const Meeting& m){
								if((m.getSponsor() == userName || m.isParticipator(userName)) && !(Date(endDate) <= m.getStartDate() || Date(startDate) >= m.getEndDate()))	return true;
																	else return false;	})).empty())	return false;
	else if((m_storage->queryMeeting([title](const Meeting&m){ if(m.getTitle() == title) return true; else return false;})).size() != 0) return false;
	else{
		std::vector<std::string> v;
		Meeting newMeeting(userName,v,Date::stringToDate(startDate),Date::stringToDate(endDate),title);
		for(auto it : participator)
			if(newMeeting.isParticipator(it))	return false;
			else{
				if(!m_storage->queryMeeting([it,startDate,endDate](const Meeting& m){
								if((m.getSponsor() == it || m.isParticipator(it)) && !(Date(endDate) <= m.getStartDate() || Date(startDate) >= m.getEndDate()))	return true;
																	else return false;	}).empty())	return false;
				else	newMeeting.addParticipator(it);
			}
		m_storage->createMeeting(newMeeting);
		return true;
	}
}

bool AgendaService::addMeetingParticipator(const std::string &userName,
                          const std::string &title,
                          const std::string &participator){
	if(m_storage->queryUser([participator](const User& x){return (x.getName() == participator);}).empty() || participator == userName)	return false;
	for(auto &it : listAllSponsorMeetings(userName))
		if(it.getTitle() == title && !it.isParticipator(participator)){
			string startDate = Date::dateToString(it.getStartDate());
			string endDate = Date::dateToString(it.getEndDate());
			if(!m_storage->queryMeeting([participator,startDate,endDate](const Meeting& m){if((m.getSponsor() == participator || m.isParticipator(participator)) && !(Date(endDate) <= m.getStartDate() || Date(startDate) >= m.getEndDate()))return true;else return false;}).empty())	return false;
			m_storage->updateMeeting([userName,title](const Meeting& m){ return (m.getTitle() == title && m.getSponsor() == userName);},[participator](Meeting &m){ m.addParticipator(participator);});
			return true;
		}
	return false;
}

bool AgendaService::removeMeetingParticipator(const std::string &userName,
                             const std::string &title,
                             const std::string &participator){
	for(auto it : listAllSponsorMeetings(userName))
		if(it.getTitle() == title)
			return quitMeeting(participator,title);
	return false;
}

bool AgendaService::quitMeeting(const std::string &userName, const std::string &title){
	if (m_storage->updateMeeting([userName,title](const Meeting& m){ if(m.isParticipator(userName) && m.getTitle()==title)	return true;
																   else return false;},
								[&](Meeting &m){ m.removeParticipator(userName);})){
		m_storage->deleteMeeting([](const Meeting& m){return m.getParticipator().empty();});
		return true;
	}
	else return false;
}

std::list<Meeting> AgendaService::meetingQuery(const std::string &userName,
                              const std::string &title) const{
	std::list<Meeting> meetingList = m_storage->queryMeeting([title,userName](const Meeting& m){ if(m.getTitle() == title && (m.getSponsor() == userName || m.isParticipator(userName)))	return true;
																								else return false;});
	return meetingList;
}

std::list<Meeting> AgendaService::meetingQuery(const std::string &userName,
                              const std::string &startDate,
                              const std::string &endDate) const{
	if(Date::dateToString(Date::stringToDate(startDate)) == "0000-00-00/00:00" || Date::dateToString(Date::stringToDate(endDate)) == "0000-00-00/00:00"){ 
			std::list<Meeting> meetingList;
			return meetingList;
	}
	std::list<Meeting> meetingList = m_storage->queryMeeting([userName,startDate,endDate](const Meeting& m){
								if((m.getSponsor() == userName || m.isParticipator(userName)) && !(Date(endDate) < m.getStartDate() || Date(startDate) > m.getEndDate()))	return true;
																	else return false;	});
	return meetingList;	
}

std::list<Meeting> AgendaService::listAllMeetings(const std::string &userName) const{
	std::list<Meeting> meetingList = m_storage->queryMeeting([userName](const Meeting& m){ if(m.isParticipator(userName) || m.getSponsor() == userName) 
																							return true;
																						 else return false;});
	return meetingList;
}

std::list<Meeting> AgendaService::listAllSponsorMeetings(const std::string &userName) const{
	std::list<Meeting> meetingList = m_storage->queryMeeting([userName](const Meeting& m){ if(m.getSponsor() == userName) 	return true;
																						 else return false;});
	return meetingList;
}

std::list<Meeting> AgendaService::listAllParticipateMeetings(
  const std::string &userName) const{
	std::list<Meeting> meetingList = m_storage->queryMeeting([userName](const Meeting& m){ if(m.isParticipator(userName))  return true;
																						 else return false;});
	return meetingList;
}

bool AgendaService::deleteMeeting(const std::string &userName, const std::string &title){
	int count = m_storage->deleteMeeting([title,userName](const Meeting& m){ if(userName == m.getSponsor() && title == m.getTitle()) return true;
																	else return false;	});
	return count;
}

bool AgendaService::deleteAllMeetings(const std::string &userName){
	int count = m_storage->deleteMeeting([userName](const Meeting& m){ if(m.getSponsor() == userName)	return true;
																else return false;});
	return count;
}

void AgendaService::startAgenda(void){
	m_storage = Storage::getInstance();
}

void AgendaService::quitAgenda(void){
	m_storage->sync();
}