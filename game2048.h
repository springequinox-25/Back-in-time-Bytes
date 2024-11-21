/**
 * @file game2048.h
 * @brief Contains the declaration of the game2048 class.
 */

#ifndef GAME2048_H
#define GAME2048_H

#include <QMainWindow>
#include <QDialog>
#include <QGridLayout>
#include <QPushButton>
#include <QKeyEvent>
#include <QLabel>
#include <QVBoxLayout>
#include <QSoundEffect>
#include <QMap>
#include <QMediaPlayer>
#include <QAudioOutput>
#include "settingswindow.h"

/**
 * @class game2048
 * @brief The game2048 class represents the main game window for the 2048 game.
 */
class SettingsWindow;
class game2048 : public QMainWindow {
    Q_OBJECT
    QMediaPlayer *bgSound;
    QAudioOutput *audioOutput;

public:
    /**
     * @brief Constructor for the game2048 class.
     * @param parent The parent widget.
     */
    explicit game2048(QWidget *parent = nullptr);
    /**
     * @brief Destructor for the game2048 class.
     */
    ~game2048();
    /**
     * @brief Getter function to retrieve the highest score.
     * @return The highest score achieved.
     */
    int getHighestScore() const;

protected:
    /**
     * @brief Overridden function to handle key press events.
     * @param event The key event.
     */
    void keyPressEvent(QKeyEvent *event) override;

private slots:
    /**
     * @brief Slot function to show the settings window.
     */
    void showSettings();
    /**
     * @brief Slot function to show the game instructions dialog.
     */
    void showHelp();
    /**
     * @brief Slot function to change the theme of the game.
     * @param theme The index of the selected theme.
     */
    void changeTheme(int theme);
    /**
     * @brief Slot function to change the color of the buttons.
     * @param button The index of the selected button color.
     */
    void changeButtonColor(int button);

signals:
    /**
     * @brief Signal emitted when the user wants to go back to the main menu.
     */
    void goBackToMainMenu();
    /**
     * @brief Signal emitted when a sound effect needs to be played.
     */
    void playSoundEffect();

public slots:
    /**
     * @brief Slot function to handle going back to the main menu.
     */
    void onGoBackClicked();

private:
    SettingsWindow *settingsWindow;
    int bestScore;
    bool gameStarted = false;
    int grid[4][4];
    int score;
    int win = 0;

    /**
     * @brief Updates the grid representation on the UI.
     */
    void updateGrid();
    /**
     * @brief Generates a random number on the grid.
     */
    void generateRandomNumber();
    /**
     * @brief Moves the tiles upwards on the grid.
     * @return True if any tile was moved, false otherwise.
     */
    bool moveUp();
    /**
     * @brief Moves the tiles downwards on the grid.
     * @return True if any tile was moved, false otherwise.
     */
    bool moveDown();
    /**
     * @brief Moves the tiles to the left on the grid.
     * @return True if any tile was moved, false otherwise.
     */
    bool moveLeft();
    /**
     * @brief Moves the tiles to the right on the grid.
     * @return True if any tile was moved, false otherwise.
     */
    bool moveRight();
    /**
     * @brief Checks if there are adjacent duplicate tiles on the grid.
     * @return True if adjacent duplicates are found, false otherwise.
     */
    bool hasAdjacentDuplicates() const;
    /**
     * @brief Merges the tiles in the specified direction.
     * @param direction The direction of merging.
     * @return True if any tiles were merged, false otherwise.
     */
    bool merge(char direction);
    /**
     * @brief Checks if the game has been won.
     * @return True if the game has been won, false otherwise.
     */
    bool checkWin() const;
    /**
     * @brief Checks if the game has been lost.
     * @return True if the game has been lost, false otherwise.
     */
    bool checkLose() const;
    /**
     * @brief Updates the game state after a move.
     */
    void updateGameState();
    /**
     * @brief Updates the score and best score.
     * @param addedScore The score to add.
     */
    void updateScore(int addedScore);
    /**
     * @brief Resets the game to its initial state.
     */
    void resetGame();
    /**
     * @brief Handles the exit action by saving the best score.
     */
    void exit();
    /**
     * @brief Sets the background color of the game window.
     * @param palette The palette containing the desired background color.
     */
    void setBackgroundColor(const QPalette &palette);
    /**
     * @brief Loads the highest score from settings.
     */
    void loadHighestScore();
    /**
     * @brief Saves the highest score to settings.
     */
    void saveHighestScore();
    /**
     * @brief Slot function to handle the exit action.
     */
    void actionExitClicked();
    /**
     * @brief Slot function to handle the state change of background music.
     * @param status The new media status.
     */
    void musicStateChanged(QMediaPlayer::MediaStatus status);

    QPushButton *gridButtons[4][4];
    QLabel *scoreLabel;
    QLabel *bestScoreLabel;
    QSoundEffect *soundEffect;
    QSoundEffect slideSoundEffect;
};

#endif // GAME2048_H


