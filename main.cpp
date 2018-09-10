#include <iostream>
#include "Application.h"
#include <Windows.h>

using namespace std;

int main() 
{
	auto& app = Application::Instance();
	app.InitWindow();
	app.Run();
	app.Tarminate();
	/*cout << "Fuck you" << endl;
	getchar();
	return 0;*/
}