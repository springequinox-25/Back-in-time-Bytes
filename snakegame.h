/**
 * @file SnakeGame.h
 * @brief Header file for the SnakeGame class, which implements the main game logic and user interface for a Snake game.
 */


#ifndef SNAKEGAME_H
#define SNAKEGAME_H

#include <QWidget>
#include <QTimer>
#include <QVector>
#include <QPoint>
#include <QKeyEvent>
#include <QInputDialog>
#include <QMessageBox>
#include <QRandomGenerator>
#include <QPushButton>
#include <QMediaPlayer>
#include <QShowEvent>
#include <QLabel>

/**
 * @brief The SnakeGame class represents the main game logic and GUI for a Snake game.
 * @details This class manages the game's state, including the snake, food, obstacles, and controls the game flow.
 */
class SnakeGame : public QWidget {
    Q_OBJECT
    QMap<QString, int> highestScores; ///< Stores the highest scores achieved in different difficulty levels.
    QString currentDifficulty; ///< Stores the current difficulty level.
    QLabel *highestScoreLabel;

public:
    /**
     * @brief Constructor for the SnakeGame class.
     * @param parent The parent widget.
     */
    explicit SnakeGame(QWidget *parent = nullptr);

signals:
    /**
     * @brief Signal emitted when the user wants to go back to the main menu.
     */
    void goBackToMainMenu();

protected:
    void paintEvent(QPaintEvent *) override;
    void keyPressEvent(QKeyEvent *event) override;

private slots:
    void updateGame();
    void pauseGame();
    void resumeGame();
    void hideSilverApple(); // Added this slot
    void handleProtectionTimeout();

private:
    enum Direction { UP, DOWN, LEFT, RIGHT };

    QTimer *protectionTimer;
    QTimer* blinkTimer;
    QPixmap stoneImage;
    QPixmap poisonImage;
    QPixmap foodImage;
    QPixmap backgroundImage;
    QPixmap snakeImage;

    QPoint goldenApple; ///< Stores the position of the golden apple.
    QPixmap goldenAppleImage;
    QTimer *goldenAppleTimer;
    bool hasGoldenApple = false; ///< Flag indicating whether the golden apple is present.

    QPoint silverApple; ///< Stores the position of the silver apple.
    QPixmap silverAppleImage;
    QTimer* silverAppleTimer;
    bool hasSilverApple = false; ///< Flag indicating whether the silver apple is present.
    bool hasProtection = false; ///< Flag indicating whether the snake has protection.

    QVector<QPoint> snake;
    QVector<QPoint> foods;
    QVector<QPoint> poisons;
    QVector<QPoint> obstacles;
    Direction direction;
    QTimer *timer;
    int gridSize;
    int numFoods;
    int score;
    bool isGameOver;
    bool isPaused;
    bool instructionsShown; ///< Flag indicating whether instructions have been shown to the user.
    QPushButton *pauseButton;
    QPushButton *resumeButton;
    QPushButton *restartButton;
    QPushButton *getHelpButton;
    QMediaPlayer *snakegamesound;
    QAudioOutput *audioOutput;

    QTimer *scoreAnimationTimer;
    int targetFontSize;
    int currentFontSize;
    int n;

    void showSilverApple();
    void showGoldenApple();
    void hideGoldenApple();
    void startGame();
    void restartGame();
    void gameOver();
    void initGame();
    void generateFood();
    void generateObstacles(const QString& difficulty);
    void generatePoisons(int numPoisons);
    void onGoBackClicked();
    bool isValidPosition(const QPoint &pos);
    void updateScoreFontSize();
    void showPlusOneEffect(const QPoint& position, int n);
    bool isGoodPosition(const QPoint& pos);
    void showInstructions();
    void musicStateChanged(QMediaPlayer::MediaStatus status);
    void handleWin();
};

#endif // SNAKEGAME_H
