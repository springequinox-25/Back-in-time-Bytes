/**
 * @file tictactoesetting.cpp
 * @brief Implementation of the TicTacToeSetting class.
 * This file provides the functionality to select game settings for Tic Tac Toe, including game mode, difficulty, and grid size.
 */
#include "tictactoesetting.h"

/**
 * @brief Constructor for TicTacToeSetting. Initializes the settings dialog with options for game mode, difficulty, and grid size.
 * @param parent The parent widget. Defaults to nullptr.
 */
TicTacToeSetting::TicTacToeSetting(QWidget *parent) : QDialog(parent) {
    gameModeCombo = new QComboBox();
    gameModeCombo->addItem("Player vs Player", "PVP");
    gameModeCombo->addItem("Player vs Machine", "PVM");

    difficultyCombo = new QComboBox();
    difficultyCombo->addItem("Easy", "Easy");
    difficultyCombo->addItem("Hard", "Hard");

    gridSizeCombo = new QComboBox();
    gridSizeCombo->addItem("3x3", 3);
    gridSizeCombo->addItem("4x4", 4);
    gridSizeCombo->addItem("5x5", 5);

    auto *layout = new QFormLayout();
    layout->addRow(new QLabel("Game Mode:"), gameModeCombo);
    layout->addRow(new QLabel("Difficulty:"), difficultyCombo);
    layout->addRow(new QLabel("Grid Size:"), gridSizeCombo);

    auto *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);

    layout->addWidget(buttons);
    setLayout(layout);

    setWindowTitle("Tic Tac Toe Settings");
}

/**
 * @brief Returns the selected game mode.
 * @return QString representing the selected game mode.
 */
QString TicTacToeSetting::getGameMode() const {
    return gameModeCombo->currentData().toString();
}

/**
 * @brief Returns the selected difficulty level.
 * @return QString representing the selected difficulty level.
 */
QString TicTacToeSetting::getDifficulty() const {
    return difficultyCombo->currentData().toString();
}

/**
 * @brief Returns the selected grid size.
 * @return int representing the selected grid size.
 */
int TicTacToeSetting::getGridSize() const {
    return gridSizeCombo->currentData().toInt();
}
