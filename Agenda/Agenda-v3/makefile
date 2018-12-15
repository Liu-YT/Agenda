
out : main.o User.o Date.o Meeting.o Storage.o AgendaService.o 
	g++ -g -o out User.o Date.o Storage.o Meeting.o  AgendaService.o main.o  -std=c++11

main.o : main.cpp AgendaUI.hpp 
	g++ -g -c main.cpp AgendaUI.hpp -std=c++11

User.o : User.cpp User.hpp
	g++ -g -c User.cpp -std=c++11

Date.o : Date.cpp 
	g++ -g -c Date.cpp -std=c++11

Meeting.o : Meeting.hpp Meeting.cpp
	g++ -g -c Meeting.cpp -std=c++11

Storage.o :Storage.hpp Storage.cpp
	g++ -g -c  Storage.cpp -std=c++11

AgendaService.o : AgendaService.hpp AgendaService.cpp
	g++ -g -c  AgendaService.cpp -std=c++11

clean:
	@rm out main.o User.o Date.o Meeting.o Storage.o AgendaService.o 