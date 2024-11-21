/**
 * @file mainmenu.cpp
 * @brief Implementation of the MainMenu class to create and manage the main menu interface of the application.
 *
 * The MainMenu class extends QMainWindow and is responsible for initializing the application's main menu interface.
 * This interface includes game selection buttons for Snake, 2048, Tic Tac Toe, and the Fifteen Puzzle games, each represented
 * by a QPushButton with customized styling and icons. The class manages the setup and behavior of these buttons, including
 * their signal-slot connections to launch the respective games.
 */

#include "mainmenu.h"
#include "snakegame.h"
#include "TicTacToe.h"
#include "game2048.h"
#include "fifteenpuzzle.h"
#include "tictactoesetting.h"
#include <QMediaPlayer>
#include <QSoundEffect>
#include <QAudioOutput>
#include <QWidget>
#include <QPainter>


/**
 * @brief Constructs the main menu with game selection options.
 * Initializes the main menu window, sets up game selection buttons with icons and styles,
 * and connects button clicks to their respective game launch functions.
 * It also initializes and plays background music for the main menu.
 * @param parent The parent widget, typically null for main windows.
 */

MainMenu::MainMenu(QWidget *parent) : QMainWindow(parent) {


    // Set a fixed size for the main window.
    setFixedSize(400, 450);


    // Initialize the background music player.
    mainmenusound = new QMediaPlayer;
    // Initialize the audio output for the music player.
    audioOutput = new QAudioOutput;
    // Set the audio output device for the music player.
    mainmenusound->setAudioOutput(audioOutput);
    // Set the source file for the background music.
    mainmenusound->setSource(QUrl("qrc:/sound/m_mainmenu.wav"));
    audioOutput->setVolume(50);// Set the volume for the background music.
    connect(mainmenusound, &QMediaPlayer::mediaStatusChanged, this, &MainMenu::musicStateChanged);
    mainmenusound->play();

    // Initialize buttons with three games
    snakeGameButton = new QPushButton(this);
    game2048Button = new QPushButton(this);
    ticTacToeButton = new QPushButton(this);
    fifteenPuzzleButton = new QPushButton(this);


    // Set icons for the game selection buttons.
    QIcon snakeIcon(":/image/snake_name.png");
    snakeGameButton->setIcon(snakeIcon);
    snakeGameButton->setIconSize(QSize(90, 50));  // Adjust size as needed
    //snakeGameButton->setText("Snake");

    QIcon game2048Icon(":/image/2048.png");  // Replace with the correct path to 2048 game logo
    game2048Button->setIcon(game2048Icon);
    game2048Button->setIconSize(QSize(120, 80));

    QIcon ticTacToeIcon(":/image/tic_tac_toe.png");  // Replace with the correct path to Tic-Tac-Toe game logo
    ticTacToeButton->setIcon(ticTacToeIcon);
    ticTacToeButton->setIconSize(QSize(130, 90));

    QIcon fifteenPuzzleIcon(":/image/15puzzle.png");  // Replace with the correct path to fifteen puzzle game logo
    fifteenPuzzleButton->setIcon(fifteenPuzzleIcon);
    fifteenPuzzleButton->setIconSize(QSize(130, 95));


    // Set button colors and styles
    QString buttonStyle = "QPushButton {"
                          "background-color: white;"
                          "color: black;"
                          "border: 2px solid lightgrey;"
                          "border-radius: 10px;"
                          "}"
                          "QPushButton:hover {"
                          "background-color: gold;"
                          "}"
                          "QPushButton:pressed { "
                          "background-color: darkgrey; "
                          "}";


    // Apply the style and set up event filters for hover effects.
    snakeGameButton->setStyleSheet(buttonStyle);
    snakeGameButton->installEventFilter(this);
    game2048Button->setStyleSheet(buttonStyle);
    game2048Button->installEventFilter(this);
    ticTacToeButton->setStyleSheet(buttonStyle);
    ticTacToeButton->installEventFilter(this);
    fifteenPuzzleButton->setStyleSheet(buttonStyle);
    fifteenPuzzleButton->installEventFilter(this);

    // Position the game selection buttons within the window.
    snakeGameButton->setGeometry(QRect(QPoint(100, 95), QSize(200, 50)));
    game2048Button->setGeometry(QRect(QPoint(100, 155), QSize(200, 50)));
    ticTacToeButton->setGeometry(QRect(QPoint(100, 215), QSize(200, 50)));
    fifteenPuzzleButton->setGeometry(QRect(QPoint(100, 275), QSize(200, 50)));

    // Connect buttons to their respective slot functions
    connect(snakeGameButton, &QPushButton::clicked, this, &MainMenu::clickSnake);
    connect(game2048Button, &QPushButton::clicked, this, &MainMenu::click2048);
    connect(ticTacToeButton, &QPushButton::clicked, this, &MainMenu::clickTicTacToe);
    connect(fifteenPuzzleButton, &QPushButton::clicked, this, &MainMenu::clickFifteenPuzzle);
}



/**
 * @brief Paints the main menu's background image.
 * Overrides the paintEvent to draw a custom background image for the main menu.
 * @param event The paint event.
 */
void MainMenu::paintEvent(QPaintEvent *event) {
    QMainWindow::paintEvent(event); // Call the base class implementation first

    QPainter painter(this);
    QImage backgroundImage(":/image/background.png");
    QRect targetRect(0, 0, width(), height()); // Scale the image to fill the window
    QRect sourceRect(0, 0, backgroundImage.width(), backgroundImage.height()); // Use the entire source image
    painter.drawImage(targetRect, backgroundImage, sourceRect);
}



/**
 * @brief Filters events for game selection buttons to change icons on hover.
 * Watches for enter and leave events on game selection buttons to dynamically change the button icons,
 * providing a visual cue to the user that the button is interactive.
 * @param watched The object being observed for events.
 * @param event The event that occurred.
 * @return true if the event was handled, false otherwise.
 */
bool MainMenu::eventFilter(QObject *watched, QEvent *event) {
    // Check if the mouse is hovering over the Snake Game button.
    if (watched == snakeGameButton) {
        if (event->type() == QEvent::Enter) { // Mouse hover begins
            // Change the icon to a highlighted version for visual feedback.
            snakeGameButton->setIcon(QIcon(":/image/snake_logo.png"));
        } else if (event->type() == QEvent::Leave) { // Mouse hover ends
            // Revert the icon back to the default state.
            snakeGameButton->setIcon(QIcon(":/image/snake_name.png"));
        }
    }
    // Similar checks and icon changes for the Tic Tac Toe button.
    if (watched == ticTacToeButton) {
        if (event->type() == QEvent::Enter) { // Mouse hover begins
            ticTacToeButton->setIcon(QIcon(":/image/tic_tac_toe_logo.png"));
            ticTacToeButton->setIconSize(QSize(90, 40));
        } else if (event->type() == QEvent::Leave) { // Mouse hover ends
            ticTacToeButton->setIcon(QIcon(":/image/tic_tac_toe.png"));
            ticTacToeButton->setIconSize(QSize(130, 90));
        }
    }

    // Checking and adjusting the 2048 Game button's icon on mouse hover.
    if (watched == game2048Button) {
        if (event->type() == QEvent::Enter) { // Mouse hover begins
            game2048Button->setIcon(QIcon(":/image/finger.png"));
            game2048Button->setIconSize(QSize(90, 70));
        } else if (event->type() == QEvent::Leave) { // Mouse hover ends
            game2048Button->setIcon(QIcon(":/image/2048.png"));
            game2048Button->setIconSize(QSize(120, 80));
        }
    }

    // Handling hover events for the Fifteen Puzzle game button.
    if (watched == fifteenPuzzleButton) {
        if (event->type() == QEvent::Enter) { // Mouse hover begins
            fifteenPuzzleButton->setIcon(QIcon(":/image/15_puzzle_logo.png"));
            fifteenPuzzleButton->setIconSize(QSize(50, 50));
        } else if (event->type() == QEvent::Leave) { // Mouse hover ends
            fifteenPuzzleButton->setIcon(QIcon(":/image/15puzzle.png"));
            fifteenPuzzleButton->setIconSize(QSize(120, 80));
        }
    }
    // Call the base class event filter to ensure standard event processing continues.
    return QMainWindow::eventFilter(watched, event);
}




/**
 * @brief Handles the state change of the background music player.
 * Restarts the background music when it finishes playing to ensure continuous playback.
 * @param status The new media status of the background music player.
 */
void MainMenu::musicStateChanged(QMediaPlayer::MediaStatus status) {
    if (status == QMediaPlayer::EndOfMedia) {
        mainmenusound->setPosition(0); // Restart the music
        mainmenusound->play();
    }
}

/**
 * @brief Displays the main menu.
 * Shows the main menu window and starts playing the background music.
 */

void MainMenu::showMainMenu() {
    //qDebug() << "Showing Main Menu";
    mainmenusound->play();
    this->show(); // Show the main menu
}

/**
 * @brief Closes the main menu.
 * Stops the background music and deletes the main menu instance, typically to exit the application.
 */
void MainMenu::closeMainMenu() {
    mainmenusound->stop();
    this->deleteLater();
}

/**
 * @brief MainMenu::onSnakeGameClicked Once user click on the snakegame push button, will create a instance of snake and show the game page
 * when user quit the snake game, there is option allow user to choose go back to the mainmenu to select other games if they want
 */

void MainMenu::clickSnake() {
    mainmenusound->stop();
    // Hide the main menu immediately
    this->hide();

    // Create the Snake game instance
    SnakeGame *snakeGame = new SnakeGame();

    // Show the Snake game
    snakeGame->show();

    // When going back to the main menu, show the main menu again
    connect(snakeGame, &SnakeGame::goBackToMainMenu, this, &MainMenu::showMainMenu);
}
/**
 * @brief MainMenu::click2048 Once user click on the button of click2048, will create a instance of 2048 and how the game page
 * When user want quit the game, there is a option to go back to the mainmenu.
 */

void MainMenu::click2048() {
    mainmenusound->stop();
    //Hide the main menu immediately
    this->hide();

    // Create the Snake game instance
    game2048 *gameOption2048 = new game2048();

    // Showthe Snake game
    gameOption2048->show();

    // When going back to the main menu, show the main menu again
    connect(gameOption2048, &game2048::goBackToMainMenu, this, &MainMenu::showMainMenu);

}
/**
 * @brief Slot for handling the Tic Tac Toe game button click. Opens the Tic Tac Toe settings dialog.
 */
void MainMenu::clickTicTacToe() {
    buttonSoundEffect = new QSoundEffect(this);
    buttonSoundEffect->setSource(QUrl("qrc:/sound/m_pushbutton.wav"));
    buttonSoundEffect->setVolume(0.2f);
    mainmenusound -> stop();
    tictactoesound = new QMediaPlayer;
    audioOutput = new QAudioOutput;
    tictactoesound->setAudioOutput(audioOutput);
    tictactoesound->setSource(QUrl("qrc:/sound/guzheng1.wav"));
    audioOutput->setVolume(10);

    tictactoesound->play();

    this->hide();

    TicTacToeSetting settingsDialog;
    if (settingsDialog.exec() == QDialog::Accepted) {
        buttonSoundEffect->play();
        QString gameMode = settingsDialog.getGameMode();
        QString difficulty = settingsDialog.getDifficulty();
        int gridSize = settingsDialog.getGridSize();

        TicTacToe *tictactoe = new TicTacToe(gameMode, difficulty, gridSize, this);
        tictactoe->show();
        connect(tictactoe, &TicTacToe::goBackToMainMenu, this, &MainMenu::showMainMenu);
        tictactoesound->stop();
    } else {
        buttonSoundEffect->play();
        tictactoesound->stop();
        mainmenusound -> play();
        this->show();
    }
}

/**
 * @brief Slot for handling the Fifteen Puzzle game button click. Launches the Fifteen Puzzle game.
 */
void MainMenu::clickFifteenPuzzle() {
    mainmenusound->stop();
    //Hide the main menu immediately
    this->hide();

    // Create the Snake game instance
    FifteenPuzzle *fifteenpuzzle = new FifteenPuzzle();

    // Showthe Snake game
    fifteenpuzzle->show();

}


