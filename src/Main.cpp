#include <Application.hpp>
#include <ShitVM.hpp>
#include <Window.hpp>
#include <window/MainWindow.hpp>

#include <cstdlib>

#include <Windows.h>

MainWindow* mainWindow;
int counter = 0;

int APIENTRY WinMain(HINSTANCE instance, HINSTANCE, LPSTR, int cmdShow) {
	if (!Initialize(instance, cmdShow)) {
		MessageBox(nullptr, "Failed to initialize", Title, MB_OK | MB_ICONERROR);
		return EXIT_FAILURE;
	}

	mainWindow = new MainWindow();
	mainWindow->Show(cmdShow);

	return Run([]() {
		++counter;
		if (counter == 10000) {
			counter = 0;
			if (!CheckShitVMProcessValid()) {
				mainWindow->Invalidate();
				MessageBox(nullptr, "ShitVM Process is dead", Title, MB_OK | MB_ICONINFORMATION);
			}
		}
	});
}