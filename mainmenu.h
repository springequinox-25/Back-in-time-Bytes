/**
 * @file MainMenu.h
 * @brief Header file for the MainMenu class, serving as the entry point for selecting different games in the application.
 */

#ifndef MAINMENU_H
#define MAINMENU_H

#include <QMainWindow>
#include <QPushButton>
#include <QFile>
#include <QTextStream>
#include <QMediaPlayer>
#include <QShowEvent>
#include <QSoundEffect>

class MainMenu : public QMainWindow {
    Q_OBJECT


public:
    explicit MainMenu(QWidget *parent = nullptr);
    QMediaPlayer *tictactoesound;

private slots:
    /**
     * @brief MainMenu::onSnakeGameClicked Once user click on the snakegame push button, will create a instance of snake and show the game page
     * when user quit the snake game, there is option allow user to choose go back to the mainmenu to select other games if they want
     */
    void clickSnake();
    void click2048();
    void clickTicTacToe();
    void clickFifteenPuzzle();
    void showMainMenu();
    void closeMainMenu();


private:
    QPushButton *snakeGameButton;
    QPushButton *game2048Button;
    QPushButton *ticTacToeButton;
    QPushButton *fifteenPuzzleButton;
    QMediaPlayer *mainmenusound;
    QAudioOutput *audioOutput;
    QPushButton *fifteenpuzzle;
    QSoundEffect *buttonSoundEffect;

    void musicStateChanged(QMediaPlayer::MediaStatus status);
    void paintEvent(QPaintEvent *event) override;
    bool eventFilter(QObject *watched, QEvent *event) override;
};

#endif // MAINMENU_H
