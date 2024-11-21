/**
 * @file fifteenpuzzle.h
 * @brief Defines the FifteenPuzzle class, a QWidget-based class to play the Fifteen Puzzle game.
 *
 * The Fifteen Puzzle game consists of a frame of numbered square tiles in random order with one tile missing.
 * The object of the puzzle is to place the tiles in order by making sliding moves that use the empty space.
 */
#ifndef FIFTEENPUZZLE_H
#define FIFTEENPUZZLE_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QSettings>
#include <QMediaPlayer>
#include <QSoundEffect>
#include <QAudioOutput>

/**
 * @class FifteenPuzzle
 * @brief The FifteenPuzzle class implements the logic and UI for the Fifteen Puzzle game.
 *
 * This class handles game setup, user interactions, game logic, and UI updates. It supports different difficulty levels.
 */
class FifteenPuzzle : public QWidget {
    Q_OBJECT

public:
    explicit FifteenPuzzle(QWidget *parent = nullptr);
    // Difficulty levels for the FifteenPuzzle
    enum Difficulty { Easy, Hard };

private:
    QVector<QString> initialState;
    static const int gridSize = 4; // 4x4 grid
    QPushButton *buttons[gridSize * gridSize];
    QPushButton *resetButton;
    QPushButton *newGameButton;
    QPushButton *menuButton;
    Difficulty currentDifficulty;
    QPushButton *helpButton;
    int movesCount = 0;
    QLabel* movesLabel;
    QSettings settings;
    QMediaPlayer *fifteensound;
    QAudioOutput *audioOutput;
    QSoundEffect *buttonSoundEffect;

    void setupGame();
    void shuffleTiles(Difficulty difficulty = Hard);
    void resetGame();
    void newGame();
    int findButtonIndex(const QString &number);
    bool isAdjacent(int index1, int index2);
    bool isSolved();
    void setupEasyPuzzle();
    void onGoBackClicked();
    void showHelp();
    void updateHighScore();
    void showHighScore();
    void updateButtonColors();

private slots:
    void buttonClicked();
    void resetButtonClicked();
    void newGameButtonClicked();
    void musicStateChanged(QMediaPlayer::MediaStatus status);
};

#endif // FIFTEENPUZZLE_H
