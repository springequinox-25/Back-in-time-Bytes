/**
 * @file tictactoesetting.h
 * @brief Defines the TicTacToeSetting class, a QDialog used to configure game settings for Tic Tac Toe,
 * including game mode, difficulty, and grid size.
 */

#ifndef TICTACTOESETTING_H
#define TICTACTOESETTING_H

#include <QDialog>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QLabel>


/**
 * @class TicTacToeSetting
 * @brief The TicTacToeSetting class provides a dialog to select game settings for Tic Tac Toe.
 */
class TicTacToeSetting : public QDialog {
    Q_OBJECT

public:
    explicit TicTacToeSetting(QWidget *parent = nullptr);

    QString getGameMode() const;
    QString getDifficulty() const;
    int getGridSize() const;

private:
    QComboBox *gameModeCombo;
    QComboBox *difficultyCombo;
    QComboBox *gridSizeCombo;
};

#endif // TICTACTOESETTING_H
