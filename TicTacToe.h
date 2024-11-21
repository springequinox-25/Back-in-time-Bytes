/**
 * @file TicTacToe.h
 * @brief Header file for the TicTacToe game class.
 *
 * Defines the TicTacToe class and its member variables and functions, providing the logic and UI for playing
 * a Tic Tac Toe game with support for different game modes and difficulties.
 */
#ifndef TICTACTOE_H
#define TICTACTOE_H

#include <QMainWindow>
#include <QPushButton>
#include <vector>
#include <QLabel>
#include <QMediaPlayer>
#include <QDialog>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QLabel>
#include <QMediaPlayer>
#include <QSoundEffect>

// Enumeration for human game state outcomes.
enum class HumanState : int {
    LOSE =  1,
    TIE  =  0,
    WIN  = -1
};

enum Player  {
    Machine = 'O',
    HUMAN = 'X',
    NONE  = ' '
};

/**
 * @class TicTacToe
 * @brief The TicTacToe class manages the game logic, UI, and interactions for a Tic Tac Toe game.
 *
 * Supports player vs machine and player vs player modes, variable grid sizes, and tracks game statistics.
 */
class TicTacToe : public QMainWindow {
    Q_OBJECT

public:
    explicit TicTacToe(QString gameMode = "PVM", QString difficulty = "Easy", int gridSize = 3, QWidget *parent = nullptr);
    ~TicTacToe();

signals:
    void goBackToMainMenu();

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void buttonClicked(int x, int y);
    void on_actionNew_Game_triggered();
    void on_actionExit_triggered();
    void on_actionGet_Hint_triggered();
    void openSettingsDialog();

private:
    //QString
    QString getWinsKey() const;
    QString getLossesKey() const;
    QString getTiesKey() const;
    QString mode;
    QString getSettingsKey() const;
    QString gameMode;
    QString difficulty;

    //QLabel
    QLabel* winLose;
    QLabel* statusLabel;

    //int
    int player1Wins = 0;
    int player2Wins = 0;
    int wins = 0;
    int ties = 0;
    int losses = 0;
    int gridSize;
    int highScore = 0;

    bool isHumanTurn;

    std::vector<std::vector<char>> board;
    std::vector<std::vector<QPushButton*>> buttons;

    QPushButton *highScoreButton;
    QPushButton *helpButton;


    QMediaPlayer *tictactoesound;
    QAudioOutput *audioOutput;
    QComboBox *gameModeCombo;
    QComboBox *difficultyCombo;
    QComboBox *gridSizeCombo;
    QSoundEffect *buttonSoundEffect;

    //Function
    HumanState calculateScore(std::vector<std::vector<char>> gameBoard) const;

    void updateAndSaveStats(bool player1Win, bool player2Win, bool tie);
    void initializeGame();
    void playTheBest();
    void openMsgBox(QString msg);
    void adjustForGameModeAndDifficulty();
    void setupBoard();
    void adjustWindowSize();
    void updateWinLose();
    void saveHighScore(int player1Wins, int player2Wins, int ties);
    void displayHighScore();
    void showHelp();
    void musicStateChanged(QMediaPlayer::MediaStatus status);

    int miniMax(std::vector<std::vector<char>> gameBoard, int depth, bool isMachine) const;

    bool isGameOver(std::vector<std::vector<char>> gameBoard) const;
    bool checkWin(Player player, const std::vector<std::vector<char>>& gameBoard) const;
};

#endif // TICTACTOE_H
