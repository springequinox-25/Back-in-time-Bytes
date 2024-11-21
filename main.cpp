/**
 * @file main.cpp
 * @brief Application entry point.
 */

#include <QApplication>
#include "mainmenu.h"

/**
 * @brief Main function.
 * @param argc Number of command line arguments.
 * @param argv Array of command line arguments.
 * @return Exit status.
 */
int main(int argc, char *argv[]) {
    QApplication app(argc, argv); ///< Application object.

    MainMenu mainMenu; ///< Main menu object.
    mainMenu.show(); ///< Show the main menu.

    return app.exec(); ///< Execute the application.
}


