#include "Application.h"
#include <Windows.h>

using namespace std;

int main() 
{
	auto& app = Application::Instance();
	app.Initialize();
	app.Run();
	app.Tarminate();
}