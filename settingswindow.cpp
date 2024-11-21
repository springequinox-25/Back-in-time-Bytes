/**
 * @file settingswindow.cpp
 * @brief Implementation of the SettingsWindow class for managing application settings, including theme and button color changes for game 2048.
 */
#include "settingswindow.h"
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QDebug>

int theme;

SettingsWindow::SettingsWindow(QWidget *parent)
    : QDialog(parent)
{
    QVBoxLayout *layout = new QVBoxLayout(this);

    // Theme section
    QLabel *themeLabel = new QLabel("Theme");
    QPushButton *changeThemeButton = new QPushButton("Random Theme");
    // Connect the button clicked signal to the slot that emits the changeThemeClicked signal

    // connect(changeThemeButton, &QPushButton::clicked, this, &SettingsWindow::changeThemeButtonClicked);
    connect(changeThemeButton, &QPushButton::clicked, this, [=](){
        int randTheme = rand() % 4 + 1;
        changeButtonButtonClicked(randTheme);
        changeThemeButtonClicked(randTheme);
    });

    // Change background color section
    QLabel *backgroundColorLabel = new QLabel("Change Background Color");
    QPushButton *colorButton1 = new QPushButton("Color 1");
    connect(colorButton1, &QPushButton::clicked, this, [this]() {
        changeThemeButtonClicked(1);
    });
    QPushButton *colorButton2 = new QPushButton("Color 2");
    connect(colorButton2, &QPushButton::clicked, this, [this]() {
        changeThemeButtonClicked(2);
    });
    QPushButton *colorButton3 = new QPushButton("Color 3");
    connect(colorButton3, &QPushButton::clicked, this, [this]() {
        changeThemeButtonClicked(3);
    });
    QPushButton *colorButton4 = new QPushButton("Color 4");
    connect(colorButton4, &QPushButton::clicked, this, [this]() {
        changeThemeButtonClicked(4);
    });

    // Change button color section
    QLabel *buttonColorLabel = new QLabel("Change Button Color");
    QPushButton *buttonColorButton1 = new QPushButton("Color 1");
    connect(buttonColorButton1, &QPushButton::clicked, this, [this]() {
        changeButtonButtonClicked(1);
    });
    QPushButton *buttonColorButton2 = new QPushButton("Color 2");
    connect(buttonColorButton2, &QPushButton::clicked, this, [this]() {
        changeButtonButtonClicked(2);
    });
    QPushButton *buttonColorButton3 = new QPushButton("Color 3");
    connect(buttonColorButton3, &QPushButton::clicked, this, [this]() {
        changeButtonButtonClicked(3);
    });
    QPushButton *buttonColorButton4 = new QPushButton("Color 4");
    connect(buttonColorButton4, &QPushButton::clicked, this, [this]() {
        changeButtonButtonClicked(4);
    });

    // Add widgets to layout
    layout->addWidget(themeLabel);
    layout->addWidget(changeThemeButton);
    layout->addWidget(backgroundColorLabel);
    layout->addWidget(colorButton1);
    layout->addWidget(colorButton2);
    layout->addWidget(colorButton3);
    layout->addWidget(colorButton4);
    layout->addWidget(buttonColorLabel);
    layout->addWidget(buttonColorButton1);
    layout->addWidget(buttonColorButton2);
    layout->addWidget(buttonColorButton3);
    layout->addWidget(buttonColorButton4);
    layout->addWidget(new QLabel("")); // Blank line
}


void SettingsWindow::changeThemeButtonClicked(int theme) {
    emit changeThemeClicked(theme);
}

void SettingsWindow::changeButtonButtonClicked(int button) {
    qDebug() << "changeButtonButtonClicked signal emitted with button:" << button;
    emit changeButtonClicked(button);
}


