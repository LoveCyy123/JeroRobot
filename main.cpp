#include <iostream>
#include "base/utils/JLog.h"

int main(int argc, char **argv)
{

	// Main app;
	// if (!app.Start())
	// {
	// 	Grm::Exit(-1, "Bot start failed!");
	// 	return -1;
	// }
	// app.Wait();
	// app.Stop();
	// Grm::Exit(0, "Normal Exit");

	jr::JLog::Init("jero_robot");
    LOG_INFO("JeroRobot started");

	return 0;
}