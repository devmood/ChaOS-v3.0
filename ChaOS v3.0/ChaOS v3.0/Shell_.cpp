// Shell_.cpp : Defines the entry point for the console application.
//
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <iomanip>
#include <time.h>
#include <ctime>
#include <chrono>
#include <conio.h>
#include <cstring>
#include <Windows.h>
#include <string>
#include <fstream>
#include <algorithm>
#include <vector>
#include "Headers\Process.h"
#include "Headers\ProcessesManager.h"
#include "Headers\Interpreter.h"
#include "Headers\MemoryManager.h"
#include "Headers\ChaOS_filesystem.h"
#include "Headers\ConditionVariable.h"
#include "Headers\Siec.h"
#include "Headers\ProcessScheduler.h"
#include "Headers\table_maker.h"
#include "locale.h"
#include "Headers\rlutil.h"

using namespace std;
std::string shellReturnPath = "/root";
PCB* ActiveProcess;

ProcessesManager *pm;
ProcessScheduler *ps;
MemoryManager *mm;
Siec *s;
ChaOS_filesystem *fs;
Interpreter *i;

std::vector<std::string> ErrorsTab{

	//Prosze o dopisywanie nastepnika z wlasnym opisem bledu. W komentarzu piszcie nr zebysmy sie nie pogubili.
	"Brak bledow w obsludze.",//(0)
	"Przykladowy blad ktory opisuje w tym miejscu jakas osoba. Mozecie go zmienic na wlasny.", //(1)
	"Brak wolnych sektorow na dysku.",//(2)
	"Obiekt o podanej nazwie juz istnieje.",//(3)
	"Obiekt o podanej nazwie nie zostal odnaleziony.",//(4)
	"Brak otwartego pliku do wykonania operacji.",//(5)
	"Podany plik musi byc otwarty, by wykonac operacje.", //(6)
	"Aktywny proces ma juz otwarty plik.", //(7)
	"Typ pliku, ktory zostal podany jest inny niz plik lub folder", //(8)
	"Odwolanie do pamieci spoza obszaru adresowego procesu.", //(9)
	"Brak wolnej pamieci.", //(10)
	"Usuwany folder musi byc pusty.", //(11)
	"Podany tekst jest zbyt dlugi, by zapisac go do pliku.", //(12)
	"Zly kod rozkazu.", //(13)
	"Nie znaleziono odbiorcy o podanym PID.",	//(14)
	"Wiadomosc jest zbyt dluga.", //(15)
	"Brak wiadomosci w kolejce.", //(16)
	"" //(17)
};

class main_loop {
private:
	vector<string> command;
	string line;
	void choose_function();
	void initialize();
	void prepare_string();
	void help();


public:
	main_loop();
	void run();
	void big_letter(vector<string> &pom);
};

void main_loop::big_letter(vector<string> &pom) {
	std::transform(pom[0].begin(), pom[0].end(), pom[0].begin(), ::toupper);
}

void main_loop::initialize() {
	//uruchmienie logo
	rlutil::locate(2, 10);
	cout << "      /\\\\\\\\\\\\\\\\\\   /\\\\\\                               /\\\\\\\\\          /\\\\\\\\\\\\\\\\\\\\\\           " << endl
		 << "      /\\\\\\////////  \\/\\\\\\                            /\\\\\\///\\\\\\      /\\\\\\/////////\\\\\\        " << endl
		 << "     /\\\\\\/           \\/\\\\\\                          /\\\\\\/  \\///\\\\\\   \\//\\\\\\      \\///        " << endl
		 << "     /\\\\\\             \\/\\\\\\          /\\\\\\\\\\\\\\\\\\     /\\\\\\      \\//\\\\\\   \\////\\\\\\              " << endl
		 << "     \\/\\\\\\             \\/\\\\\\\\\\\\\\\\\\\\  \\////////\\\\\\   \\/\\\\\\       \\/\\\\\\      \\////\\\\\\          " << endl
		 << "      \\//\\\\\\            \\/\\\\\\/////\\\\\\   /\\\\\\\\\\\\\\\\\\\\  \\//\\\\\\      /\\\\\\          \\////\\\\\\      "<<endl
		 << "        \\///\\\\\\          \\/\\\\\\   \\/\\\\\\  /\\\\\\/   \\\\\\   \\///\\\\\\  /\\\\\\     /\\\\\\      \\//\\\\\\    "<<endl
		 << "           \\////\\\\\\\\\\\\\\\\\\ \\/\\\\\\   \\/\\\\\\ \\//\\\\  \\\\\\/\\\\    \\///\\\\\\\\\\/     \\///\\\\\\\\\\\\\\\\\\\\\\/    "<<endl
		 << "               \\/////////  \\///    \\///   \\////////\\//       \\/////         \\///////////     " << endl<<endl<<"\n";

	mm = new MemoryManager();
	ps = new ProcessScheduler();
	pm = new ProcessesManager();
	i = new Interpreter();
	fs = new ChaOS_filesystem();
	s = new Siec();
	
	rlutil::hidecursor();
	char a = 177, b = 219;
	cout << "\t\t\t\t\t";
	for (int i = 0; i <= 25; i++) 
		cout << a;
	rlutil::msleep(150);
	cout << "\r";
	cout << "\t\t\t\t\t";
	for (int i = 0; i <= 25; i++) {
		cout << b;
		rlutil::msleep(100);
	}
	system("cls");
	rlutil::locate(95, 1);
	rlutil::setColor(rlutil::LIGHTMAGENTA);
	auto end = std::chrono::system_clock::now();
	std::time_t end_time = std::chrono::system_clock::to_time_t(end);
	std::cout<<"  " << std::ctime(&end_time);
	rlutil::setColor(rlutil::LIGHTGREEN);
	rlutil::locate(0, 0);
	rlutil::showcursor();
}


void main_loop::prepare_string() {
	command.clear();
	size_t pos = 0;
	string delimiter = " ";
	string token;
	int pom = 0;
	while (((pos = line.find(delimiter)) != string::npos + 1) && pom < 4) {
		token = line.substr(0, pos);
		command.push_back(token);
		line.erase(0, pos + delimiter.length());
		pom++;
		if (token == "ap" || token == "write" || token == "google" || token == "browse") {
			//cout << line << endl;
			command.push_back(line);
			break;
		}
	}
}


void main_loop::run() {
	initialize();
	while (true) {
		try {

			rlutil::setColor(rlutil::WHITE);
			cout <<shellReturnPath<< "> ";
			rlutil::setColor(rlutil::YELLOW);
			getline(cin, line);
			rlutil::setColor(rlutil::LIGHTMAGENTA);
			auto end = std::chrono::system_clock::now();
			std::time_t end_time = std::chrono::system_clock::to_time_t(end);
			std::cout<<"\t\t\t\t\t\t\t\t\t\t\t\t" << std::ctime(&end_time);
			rlutil::setColor(rlutil::LIGHTGREEN);
			if (line == "close")break;
			else {
				prepare_string();
				choose_function();
			}
		}
		catch (exception e) {
			//cout << e.what();
		}
		catch (...) {
			rlutil::setColor(rlutil::LIGHTRED);
			cout << "Bledna nazwa rozkazu. \nWpisz \"help\" zeby zobaczyc liste rozkazow\n";
			rlutil::setColor(rlutil::LIGHTGREEN);
		}
	}
}

main_loop::main_loop() {

}



void main_loop::choose_function() {
	if (command[0] == "help") {
		help();
	}
	else if (command[0] == "xD") {
		rlutil::setColor(rlutil::YELLOW);
		cout << "           /$$$$$$$ " << endl
			<< "          | $$__  $$" << endl
			<< " /$$   /$$| $$  \\ $$" << endl
			<< "|  $$ /$$/| $$  | $$" << endl
			<< " \\  $$$$/ | $$  | $$" << endl
			<< "  >$$  $$ | $$  | $$" << endl
			<< " /$$/\\  $$| $$$$$$$/" << endl
			<< "|__/  \\__/|_______/ " << "\n";
		cout << "\n";
		rlutil::setColor(rlutil::LIGHTGREEN);
	}
	else if (command[0] == "cf") {
		/*
		char tab[1024];
		strcpy_s(tab, command[1].c_str());
		if (command[2] == "plik") {
			fs->create(tab, ChaOS_filesystem::type::file);
		}
		else if (command[2] == "folder") {
			fs->create(tab, ChaOS_filesystem::type::dir);
		}
		else {
			throw 1;
		}
	*/
		main_loop::big_letter(command);
		i->DoShellCommand(command);
		cout << "\n";
	}
	else if (command[0] == "rf") {
		/*char tab[1024];
		strcpy_s(tab, command[1].c_str());
		fs->remove(tab);
		*/ 
		main_loop::big_letter(command);
		i->DoShellCommand(command);
		cout << "\n";
		//nie wiemy czy wyrzuca blad
	}
	else if (command[0] == "ls") {
		//fs->listDirectory(ActiveProcess);
		command[0] = "ld";
		main_loop::big_letter(command);
		i->DoShellCommand(command);
		cout << "\n";
	}
	else if (command[0] == "cd") {
		char tab[1024];
		strcpy_s(tab, command[1].c_str());
		main_loop::big_letter(command);
		i->DoShellCommand(command);
		//fs->changeDirectory(tab);
		cout << "\n";
	}
	else if (command[0] == "rd") {
		main_loop::big_letter(command);
		i->DoShellCommand(command);
		cout << "\n";
	}
	else if (command[0] == "bd") {
		main_loop::big_letter(command);
		i->DoShellCommand(command);
		cout << "\n";
	}
	else if (command[0] == "ren") {
		/*
		char tab[1024];
		strcpy_s(tab, command[1].c_str());
		char tab2[1024];
		strcpy_s(tab2, command[2].c_str());
		fs->rename(tab, tab2);
		*/
		main_loop::big_letter(command);
		i->DoShellCommand(command);
		cout << "\n";
	}
	else if (command[0] == "of") {
		/*
		char tab[1024];
		strcpy_s(tab, command[1].c_str());
		fs->openFile(tab);
		*/		
		main_loop::big_letter(command);
		i->DoShellCommand(command);
		cout << "\n";
	}
	else if (command[0] == "clf") {
		main_loop::big_letter(command);
		i->DoShellCommand(command);
		cout << "\n";
	}
	else if (command[0] == "pds") {
		std::cout << fs->printDiskStats() <<"\n";
		cout << "\n";
	}
	else if (command[0] == "pdss") {
		int pom = stoi(command[1]);
		std::cout << fs->printSectorsChain(pom) << std::endl<<"\n";
	}
	else if (command[0] == "read") {
		//fs->readFile();
		main_loop::big_letter(command);
		i->DoShellCommand(command);
		cout << "\n";
	}
	else if (command[0] == "ap") {
		main_loop::big_letter(command);
		i->DoShellCommand(command);
		cout << "\n";
	}
	else if (command[0] == "write") {
		//fs->writeFile(command[1]);
		main_loop::big_letter(command);
		i->DoShellCommand(command);
		cout << "\n";
	}
	else if (command[0] == "cp") {
		//pm->createProcess(command[1], stoi(command[2]));
		main_loop::big_letter(command);
		i->DoShellCommand(command);
		cout << "\n";
	}
	else if (command[0] == "cpa") {
		//pm->createProcess(command[1], stoi(command[2]));
		main_loop::big_letter(command);
		i->DoShellCommand(command);
		cout << "\n";
	}
	else if (command[0] == "kp") {
		//pm->killProcess(stoi(command[1]));
		main_loop::big_letter(command);
		i->DoShellCommand(command);
		cout << "\n";
	}
	else if (command[0] == "dap") {
		pm->displayAllProcesses();
		cout << "\n";
	}
	else if (command[0] == "dwp") {
		pm->displayWaitingProcesses();
		cout << "\n";
	}
	else if (command[0] == "drp") {
		pm->displayReadyProcesses();
		cout << "\n";
	}
	else if (command[0] == "pap") {
		if (ActiveProcess == nullptr)
		{
			cout << "Nullptr\n";
		}
		else
		{
			TextTable t(196, 179, 197);
			t.add("PID");
			t.add(to_string(ActiveProcess->GetPID()));
			t.endOfRow();
			t.add("GID");
			t.add(to_string(ActiveProcess->GetGID()));
			t.endOfRow();
			t.add("Program");
			t.add(ActiveProcess->GetFileName());
			t.endOfRow();
			t.add("Ilosc stron");
			t.add(to_string(ActiveProcess->getPageTableSize()));
			t.endOfRow();
			t.add("Licznik rozkazow");
			t.add(to_string(ActiveProcess->GetInstructionCounter()));
			t.endOfRow();
			t.add("Burst Time");
			t.add(to_string(ActiveProcess->GetProcesBurstTime()));
			t.endOfRow();
			t.add("Stan");
			switch (ActiveProcess->GetState())
			{
			case 0:
				t.add("New");
				break;
			case 1:
				t.add("Ready");
				break;
			case 2:
				t.add("Waiting");
				break;
			case 3:
				t.add("Running");
				break;
			case 4:
				t.add("Terminated");
				break;
			default:
				t.add("Terminated");
				break;
			}
			t.endOfRow();
			t.setAlignment(2, TextTable::Alignment::RIGHT);
			std::cout << t << "\n";
		}
	}
	else if (command[0] == "mem") {
		if (command[2] == "-a") {
			mm->printPCBframes(pm->findPCBbyPID(stoi(command[1])), false);
		}
		else if (command[2] == "-r") {
			mm->printPCBframes(pm->findPCBbyPID(stoi(command[1])), true);
		}
		else if (command[2] == "-t") {
			mm->printPageTable(pm->findPCBbyPID(stoi(command[1])));
		}
		else if (command[2] == "-f") {
			mm->printFIFO();
		}
		else {
			mm->printMemoryConnetent();
		}
		cout << "\n";
	}
	else if (command[0] == "sms") {
		s->wyswietlwiad();
		cout << "\n";
	}
	else if (command[0] == "go") {
		try {
			int digit = stoi(command[1]);
			ps->RunProcess(digit);
		}catch (invalid_argument) {
			ps->RunProcess();
		}
		cout << "\n";
	}
	else if (command[0] == "clear") {
		system("cls");
	}
	else if (command[0] == "google") {
		string pom = "https://www.google.pl/search?safe=off&ei=DuJUWqCNHZGVsAeP25qgAQ&q=" + command[1] + "&oq = cosiek & gs_l = psy - ab.3..0l7.14897.15571.0.15717.6.6.0.0.0.0.113.523.5j1.6.0....0...1c..64.psy - ab..0.6.521...35i39k1j0i131k1j0i67k1j0i10k1.0.64prie2y8O0";
		ShellExecute(0, 0, pom.c_str(), 0, 0, SW_SHOW);
	}
	else if (command[0] == "browse") {
		ShellExecute(0, 0, command[1].c_str(), 0, 0, SW_SHOW);
	}
	else {
		throw 1;
	}


}


void main_loop::help() {
	fstream file;
	std::string line;

	TextTable t(196, 179, 197);

	t.add("KOMENDA");
	t.add("OPIS");
	t.endOfRow();
	t.add("help");
	t.add("Wyswietla liste komend");
	t.endOfRow();
	t.add("close");
	t.add("Konczy prace systemu");
	t.endOfRow();
	t.add("cf [nazwa] [typ(plik/folder)]");
	t.add("Tworzy plik/folder");
	t.endOfRow();
	t.add("rf [nazwa]");
	t.add("Usuwa plik/folder");
	t.endOfRow();
	t.add("ls");
	t.add("Wypisuje zawartosc aktualnego folderu");
	t.endOfRow();
	t.add("cd [nazwa]");
	t.add("Przechodzi do podanego folderu(nizszego w hierarchii)");
	t.endOfRow();
	t.add("rd");
	t.add("Przechodzi do folderu root");
	t.endOfRow();
	t.add("bd");
	t.add("Wraca do folderu poprzedniego w hierarchii");
	t.endOfRow();
	t.add("ren [nazwa1] [nazwa2]");
	t.add("Zmienia nazwe podanego pliku/folderu");
	t.endOfRow();
	t.add("of [nazwa]");
	t.add("Otwiera plik");
	t.endOfRow();
	t.add("clear");
	t.add("Zamyka otwarty plik");
	t.endOfRow();
	t.add("pds");
	t.add("Wyswietla statystyki dysku");
	t.endOfRow();
	t.add("pdss [nr_sektora]");
	t.add("Wyswietla zawartosc sektora");
	t.endOfRow();
	t.add("read");
	t.add("Wyswietla zawartosc pliku");
	t.endOfRow();
	t.add("write [tekst]");
	t.add("Zapisuje tekst do pliku");
	t.endOfRow();
	t.add("ap [tekst]");
	t.add("Dodaje tekst do pliku");
	t.endOfRow();
	t.add("cp [nazwa_pliku] [nr_grupy]");
	t.add("Tworzy proces");
	t.endOfRow();
	t.add("kp [numer]");
	t.add("Zabija proces");
	t.endOfRow();
	t.add("dap");
	t.add("Wyswietla wszystkie procesy(blok kontrolny)");
	t.endOfRow();
	t.add("dwp");
	t.add("Wyswietla oczekujace procesy");
	t.endOfRow();
	t.add("drp");
	t.add("Wyswietla gotowe procesy");
	t.endOfRow();
	t.add("mem [proces] -r");
	t.add("Wypisze strony procesu ktore sa tylko w ramkach RAM");
	t.endOfRow();
	t.add("mem [proces] -a");
	t.add("Wypisze wszystkie ramki");
	t.endOfRow();
	t.add("mem");
	t.add("Wyswietla RAM");
	t.endOfRow();
	t.add("sms");
	t.add("Wyswietla liste wszystkich wiadomosci");
	t.endOfRow();
	t.add("go");
	t.add("Wykonuje kolejny rozkaz z pliku z aktywnego procesu");
	t.endOfRow();
	t.add("pap");
	t.add("Pokazuje informacje o aktywnym procesie");
	t.endOfRow();
	t.add("google");
	t.add("Wyszukuje frazy w google");
	t.endOfRow();
	t.add("browse");
	t.add("Otwiera strone www");
	t.endOfRow();
	t.setAlignment(2, TextTable::Alignment::RIGHT);
	std::cout << t<<"\n";
}


int main() {

	system("chcp 437 >nul");
	rlutil::setColor(rlutil::LIGHTGREEN); 
	rlutil::setConsoleTitle("ChaOS - system na 5 <3");
	main_loop m;
	m.run();
	delete ps;
	delete pm;
	delete mm;
	delete s;
	delete fs;
	delete i;
	return EXIT_SUCCESS;
}




