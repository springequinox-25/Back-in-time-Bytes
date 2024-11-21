#include "game2048.h"
#include <QApplication>
#include <QDialog>
#include <QGridLayout>
#include <QPushButton>
#include <QKeyEvent>
#include <QDebug>
#include <QMessageBox>
#include <QLabel>
#include "settingswindow.h"
#include <QString>
#include <QColor>
#include <QFile>
#include <QMediaPlayer>
#include <QSettings>
#include <QMediaPlayer>
#include <QSoundEffect>
#include <QAudioOutput>

int bestScore;
/**
 * @brief Constructor for the game2048 class.
 * @param parent The parent widget.
 */
game2048::game2048(QWidget *parent)
    : QMainWindow(parent), settingsWindow(new SettingsWindow(this)), bestScore(0)
{
    // BGM
    bgSound = new QMediaPlayer(this);
    audioOutput = new QAudioOutput(this);
    bgSound->setAudioOutput(audioOutput);
    bgSound->setSource(QUrl("qrc:/sound/jazzbgm.wav"));
    audioOutput->setVolume(0.15);
    connect(bgSound, &QMediaPlayer::mediaStatusChanged, this, &game2048::musicStateChanged);
    bgSound->play();

    showHelp();
    setWindowTitle("MiniGame2048");

    // Slide sound effect
    slideSoundEffect.setSource(QUrl("qrc:/sound/slide.wav"));
    slideSoundEffect.setVolume(100);

    // Vertical layout
    QVBoxLayout *verticalLayout = new QVBoxLayout(this);

    // Score
    score = 0;
    scoreLabel = new QLabel("Score: " + QString::number(score));
    bestScoreLabel = new QLabel("Best Score: " + QString::number(bestScore));
    verticalLayout->addWidget(scoreLabel);
    verticalLayout->addWidget(bestScoreLabel);

    loadHighestScore();

    // Central widget
    QWidget *centralWidget = new QWidget();
    centralWidget->setLayout(verticalLayout);
    setCentralWidget(centralWidget);

    // Grid layout
    int buttonSize = qMin(centralWidget->geometry().width() / 4, centralWidget->geometry().height() / 4);
    QGridLayout *layout = new QGridLayout(centralWidget);
    layout->setSpacing(3);
    layout->setVerticalSpacing(15);

    for(int i = 0; i < 4; ++i) {
        for(int j = 0; j < 4; ++j) {
            grid[i][j] = 0;
            gridButtons[i][j] = new QPushButton("");
            gridButtons[i][j]->setFixedSize(buttonSize, buttonSize);
            // font size
            QFont font = gridButtons[i][j]->font();
            font.setPointSize(buttonSize/4);
            gridButtons[i][j]->setFont(font);
            // font color
            gridButtons[i][j]->setStyleSheet("color: #8E753D;");
            layout->addWidget(gridButtons[i][j], i, j);
        }
    }

    // Sound effect
    soundEffect = new QSoundEffect(this);
    soundEffect->setSource(QUrl("qrc:/sound/jazzbgm.wav"));

    // Buttons
    QPushButton *resetButton = new QPushButton("Start/Restart");
    connect(resetButton, &QPushButton::clicked, this, &game2048::resetGame);

    QPushButton *exitButton = new QPushButton("Back");
    connect(exitButton, &QPushButton::clicked, this, &game2048::exit);

    QPushButton *helpButton = new QPushButton("Get Help");
    connect(helpButton, &QPushButton::clicked, this, &game2048::showHelp);

    QPushButton *settingsButton = new QPushButton("Settings");
    connect(settingsButton, &QPushButton::clicked, this, &game2048::showSettings);

    // set theme
    int randTheme = rand() % 4 + 1;
    changeTheme(randTheme);
    changeButtonColor(randTheme);

    // Connect
    connect(settingsWindow, &SettingsWindow::changeThemeClicked, this, &game2048::changeTheme);
    connect(settingsWindow, &SettingsWindow::changeButtonClicked, this, &game2048::changeButtonColor);
    connect(exitButton, &QPushButton::clicked, this, &game2048::actionExitClicked);

    // Add widget
    verticalLayout->addWidget(bestScoreLabel);
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(helpButton);
    buttonLayout->addWidget(settingsButton);
    verticalLayout->addLayout(buttonLayout);
    layout->addWidget(resetButton, 5, 0, 1, 2);
    layout->addWidget(exitButton, 5, 2, 1, 2);
    verticalLayout->addLayout(layout);
}

/**
 * @brief Destructor for the game2048 class.
 */
game2048::~game2048()
{
}

/**
 * @brief Handles key press events.
 * @param event The key event.
 */
void game2048::keyPressEvent(QKeyEvent *event)
{
    // Check if started
    if (!gameStarted) {
        QMessageBox warningMessage;
        warningMessage.setWindowTitle("Game Not Started");
        warningMessage.setText("Please click Start or Restart to begin the game.");

        // Load the warning image from the resource file
        QPixmap warningPixmap(":/image/warning.png"); // Replace with your actual resource path
        warningMessage.setIconPixmap(warningPixmap.scaled(64, 64, Qt::KeepAspectRatio, Qt::SmoothTransformation)); // Scale the image if necessary

        warningMessage.exec();
        return;
    }

    bool moved = false;
    bool mergePossible = false;

    switch (event->key()) {
    case Qt::Key_W:
    case Qt::Key_Up:
        if (!checkLose()) {
            // Attempt merging and move up
            mergePossible = merge('w');
            moved = moveUp();

            //updateGameState();
        }
        break;
    case Qt::Key_S:
    case Qt::Key_Down:
        if (!checkLose()) {
            mergePossible = merge('s');
            moved = moveDown();

            //updateGameState();
        }
        break;
    case Qt::Key_A:
    case Qt::Key_Left:
        if (!checkLose()) {
            mergePossible = merge('a');
            moved = moveLeft();

            //updateGameState();
        }
        break;
    case Qt::Key_D:
    case Qt::Key_Right:
        if (!checkLose()) {
            mergePossible = merge('d');
            moved = moveRight();

            //updateGameState();
        }
        break;
    default:
        QMainWindow::keyPressEvent(event);
        return;
    }

    // If merging didn't occur but moving did, attempt merging again
    if (!mergePossible && moved) {
        switch (event->key()) {
        case Qt::Key_W:
        case Qt::Key_Up:
            mergePossible = merge('w');
            break;
        case Qt::Key_S:
        case Qt::Key_Down:
            mergePossible = merge('s');
            break;
        case Qt::Key_A:
        case Qt::Key_Left:
            mergePossible = merge('a');

            break;
        case Qt::Key_D:
        case Qt::Key_Right:
            mergePossible = merge('d');
            break;
        }
    }

    if (mergePossible || moved) {
        generateRandomNumber();
        updateGrid();
        updateGameState();
        // // Debugging statements
        // qDebug() << "Moved: " << moved;
        // qDebug() << "Merge possible: " << mergePossible;
        // qDebug() << "Lose state: " << checkLose();

    }


}

/**
 * @brief Resets the game.
 */
void game2048::resetGame()
{
    score = 0; // Reset the score to 0
    scoreLabel->setText("Score: " + QString::number(score));
    // Reset the game grid to initial state
    for(int i = 0; i < 4; ++i) {
        for(int j = 0; j < 4; ++j) {
            grid[i][j] = 0;
        }
    }

    generateRandomNumber();
    generateRandomNumber();

    updateGrid();
    gameStarted = true;
    win = 0;
}

/**
 * @brief Handles the state change of background music.
 * @param status The new media status.
 */
void game2048::musicStateChanged(QMediaPlayer::MediaStatus status) {
    if (status == QMediaPlayer::EndOfMedia) {
        bgSound->setPosition(0); // Restart the music
        bgSound->play();
    }
}

/**
 * @brief Updates the grid display.
 */
void game2048::updateGrid()
{
    for(int i = 0; i < 4; ++i) {
        for(int j = 0; j < 4; ++j) {
            if(grid[i][j] == 0) {
                gridButtons[i][j]->setText(""); // Set empty string if the number is 0
            } else {
                gridButtons[i][j]->setText(QString::number(grid[i][j])); // Set number as text

            }
        }
    }
}

/**
 * @brief Generates a random number in the grid.
 */
void game2048::generateRandomNumber()
{
    bool isEmptyCellAvailable = false;
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            if (grid[i][j] == 0) {
                isEmptyCellAvailable = true;
                break;
            }
        }
        if (isEmptyCellAvailable) {
            break;
        }
    }
    if (!isEmptyCellAvailable) {
        return;
    }
    int row, col;
    do {
        row = rand() % 4;
        col = rand() % 4;
    } while (grid[row][col] != 0);
    int randomNumber = (rand() % 10 < 9) ? 2 : 4;
    grid[row][col] = randomNumber;
}

/**
 * @brief Moves tiles upward in the grid.
 * @return True if any tile moved, false otherwise.
 */
bool game2048::moveUp()
{
    bool moved = false;

    for (int j = 0; j < 4; ++j) {
        int index = 0;
        for (int i = 0; i < 4; ++i) {
            if (grid[i][j] != 0) {
                if (index != i) {
                    grid[index][j] = grid[i][j];
                    grid[i][j] = 0;
                    moved = true;
                }
                ++index;
            }
        }
    }
    return moved;
}


/**
 * @brief Moves tiles downward in the grid.
 * @return True if any tile moved, false otherwise.
 */
bool game2048::moveDown()
{
    bool moved = false;

    for (int j = 0; j < 4; ++j) {
        int index = 3;
        for (int i = 3; i >= 0; --i) {
            if (grid[i][j] != 0) {
                if (index != i) {
                    grid[index][j] = grid[i][j];
                    grid[i][j] = 0;
                    moved = true;
                }
                index--;
            }
        }
    }

    return moved;
}

/**
 * @brief Moves tiles leftward in the grid.
 * @return True if any tile moved, false otherwise.
 */
bool game2048::moveLeft()
{
    bool moved = false;

    for (int i = 0; i < 4; ++i) {
        int index = 0;
        for (int j = 0; j < 4; ++j) {
            if (grid[i][j] != 0) {
                if (index != j) {
                    grid[i][index] = grid[i][j];
                    grid[i][j] = 0;
                    moved = true;
                }
                index++;
            }
        }
    }
    return moved;
}

/**
 * @brief Moves tiles rightward in the grid.
 * @return True if any tile moved, false otherwise.
 */
bool game2048::moveRight()
{
    bool moved = false;

    for (int i = 0; i < 4; ++i) {
        int index = 3;
        for (int j = 3; j >= 0; --j) {
            if (grid[i][j] != 0) {
                if (index != j) {
                    grid[i][index] = grid[i][j];
                    grid[i][j] = 0;
                    moved = true;
                }
                index--;
            }
        }
    }
    return moved;
}

/**
 * @brief Checks if there are adjacent duplicates in the grid.
 * @return True if adjacent duplicates are found, false otherwise.
 */
bool game2048::hasAdjacentDuplicates() const
{
    // Check horizontally
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 3; ++j) {
            if (grid[i][j] == grid[i][j+1]) {
                return true;
            }
        }
    }

    // Check vertically
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 4; ++j) {
            if (grid[i][j] == grid[i+1][j]) {
                return true;
            }
        }
    }

    // No adjacent duplicates found
    return false;
}


/**
 * @brief Merges tiles in the grid in the specified direction.
 * @param direction The direction to merge ('w', 's', 'a', 'd').
 * @return True if any merge occurred, false otherwise.
 */
bool game2048::merge(char direction)
{
    bool merged = false;
    int mergedNumber = 0;

    switch (direction) {
    case 'w': // Merge Up
        for (int j = 0; j < 4; ++j) {
            for (int i = 0; i < 3; ++i) {
                if (grid[i][j] != 0 && grid[i][j] == grid[i + 1][j]) {
                    // Merge if adjacent numbers are equal
                    merged = true;
                    mergedNumber = grid[i][j] * 2;
                    grid[i][j] = mergedNumber;
                    grid[i + 1][j] = 0;
                    updateScore(mergedNumber);
                }
            }
        }
        slideSoundEffect.play();
        break;
    case 's': // Merge Down
        for (int j = 0; j < 4; ++j) {
            for (int i = 3; i > 0; --i) {
                if (grid[i][j] != 0 && grid[i][j] == grid[i - 1][j]) {
                    merged = true;
                    mergedNumber = grid[i][j] * 2;
                    grid[i][j] = mergedNumber;
                    grid[i - 1][j] = 0;
                    updateScore(mergedNumber);
                }
            }
        }
        slideSoundEffect.play();
        break;
    case 'a': // Merge Left
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 3; ++j) {
                if (grid[i][j] != 0 && grid[i][j] == grid[i][j + 1]) {
                    merged = true;
                    mergedNumber = grid[i][j] * 2;
                    grid[i][j] = mergedNumber;
                    grid[i][j + 1] = 0;
                    updateScore(mergedNumber);
                }
            }
        }
        slideSoundEffect.play();
        break;
    case 'd': // Merge Right
        for (int i = 0; i < 4; ++i) {
            for (int j = 3; j > 0; --j) {
                if (grid[i][j] != 0 && grid[i][j] == grid[i][j - 1]) {
                    merged = true;
                    mergedNumber = grid[i][j] * 2;
                    grid[i][j] = mergedNumber;
                    grid[i][j - 1] = 0;
                    updateScore(mergedNumber);
                }
            }
        }
        slideSoundEffect.play();
        break;
    default:
        break;
    }
    return merged;
}

/**
 * @brief Checks if the game has been won.
 * @return True if the game has been won, false otherwise.
 */
bool game2048::checkWin() const
{
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            if (grid[i][j] == 2048) {

                return true;
            }
        }
    }
    return false;
}



/**
 * @brief Checks if the game has been lost.
 * @return True if the game has been lost, false otherwise.
 */
bool game2048::checkLose() const
{
    // Check for adjacent duplicates
    bool hasDuplicates = hasAdjacentDuplicates();

    // Check for empty cells
    bool hasEmptyCells = false;
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            if (grid[i][j] == 0) {
                hasEmptyCells = true;
                break;
            }
        }
        if (hasEmptyCells) {
            break;
        }
    }

    // Return true if there are no adjacent duplicates and no empty cells
    return !hasDuplicates && !hasEmptyCells;
}





/**
 * @brief Updates the game state after a move.
 */
void game2048::updateGameState()
{
    if (checkWin()) {
        win += 1;
        if(win<=1){
            QMessageBox winMessage;
            winMessage.setText("Congratulations!");
            winMessage.setInformativeText("You have won! you can continue challenging yourself to achieve even higher scores");
            QPixmap winPixmap(":/image/2048win.png");
            winMessage.setIconPixmap(winPixmap.scaled(64, 64, Qt::KeepAspectRatio, Qt::SmoothTransformation)); // Scale the image if necessary
            winMessage.exec();
        }
    } else if (checkLose()) {
        QMessageBox loseMessage;
        loseMessage.setText("Sorry!");
        loseMessage.setInformativeText("Game Over. Good luck next time!");
        QPixmap losePixmap(":/image/2048lose.png");
        loseMessage.setIconPixmap(losePixmap.scaled(64, 64, Qt::KeepAspectRatio, Qt::SmoothTransformation)); // Scale the image if necessary
        loseMessage.exec();
    }
}

/**
 * @brief Updates the score and best score.
 * @param addedScore The score to add.
 */
void game2048::updateScore(int addedScore)
{
    score += addedScore;
    if (score > bestScore) {
        bestScore = score;
        bestScoreLabel->setText("Best Score: " + QString::number(bestScore));
        saveHighestScore();
    }
    scoreLabel->setText("Score: " + QString::number(score));
}

/**
 * @brief Saves the highest score to settings.
 */
void game2048::saveHighestScore() {
    QSettings settings("backIntimeBytes", "game2048");
    settings.setValue("HighestScore", bestScore);
    settings.sync();
}

/**
 * @brief Handles the exit action by saving the best score.
 */
void game2048::exit(){
    QSettings settings;
    settings.setValue("HighScore", bestScore);
    onGoBackClicked();
}

/**
 * @brief Loads the highest score from settings.
 */
void game2048::loadHighestScore() {
    QSettings settings("backIntimeBytes", "game2048");
    bestScore = settings.value("HighestScore", 0).toInt();
    bestScoreLabel->setText("Best Score: " + QString::number(bestScore));
}

/**
 * @brief Displays the game instructions in a dialog.
 */
void game2048::showHelp() {
    QDialog dialog(this);
    dialog.setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint);
    dialog.setWindowTitle("2048 game Instructions");
    QVBoxLayout layout(&dialog);
    QLabel label(&dialog);
    label.setTextFormat(Qt::RichText);
    label.setText(
        "<h2>Welcome to 2048 Game Instructions😊</h2>"
        "<p><b>🎮 Objective:</b> Create a tile with the number 2048.</p >"
        "<p><b>🕹 Gameplay:</b>"
        "    Use WASD keys to move tiles:\n"
        "      W: Up, \n"
        "      A: Left, \n"
        "      S: Down, \n"
        "      D: Right.\n"
        "   Merge same-number tiles and get new ones!"
        "<p><b>🏆 Winning:\n</b>"
        "   - Reach the 2048 tile!\n\n</p >"
        " After reaching the 2048 tile, you can continue challenging yourself to achieve even higher scores."
        "<p><b>😢 Losing:\n</b>"
        "   - No valid moves left.\n"
        "   - Keep tiles merging!\n\n"
        "<p><b>🔄 Restart:\n</b>"
        "   - Click 'Start/Restart'.\n"
        "   - Score resets.\n\n</p >"
        "<p><b>😊 Enjoy:\n</b>"
        "   - Have fun and aim for 2048!</p >"
        );
    label.setStyleSheet("QLabel {"
                        "font-family: 'Arial', sans-serif;"
                        "margin-bottom: 6px;"
                        "}");
    label.setWordWrap(true);
    label.setAlignment(Qt::AlignLeft | Qt::AlignTop);
    layout.addWidget(&label);

    QPushButton okButton("Got it!", &dialog);
    okButton.setStyleSheet("QPushButton {"
                           "color: white;"
                           "background-color: #337ab7;"
                           "border-style: none;"
                           "padding: 10px;"
                           "font-weight: bold;"
                           "font-size: 14px;"
                           "border-radius: 5px;"
                           "}"
                           "QPushButton:hover {"
                           "background-color: #286090;"
                           "}");
    connect(&okButton, &QPushButton::clicked, &dialog, &QDialog::accept);
    layout.addWidget(&okButton);

    dialog.setLayout(&layout);
    dialog.exec();
}

/**
 * @brief Shows the settings window.
 */
void game2048::showSettings() {
    SettingsWindow *settingsWindow = new SettingsWindow(this);
    connect(settingsWindow, &SettingsWindow::changeThemeClicked, this, &game2048::changeTheme);
    connect(settingsWindow, &SettingsWindow::changeButtonClicked, this, &game2048::changeButtonColor);
    settingsWindow->exec();
}

/**
 * @brief Changes the theme of the game.
 * @param theme The theme index.
 */
void game2048::changeTheme(int theme) {
    QPalette palette;
    switch (theme) {
    case 1:
        palette.setColor(QPalette::Window, QColor(245, 177, 156)); // F5B19C - pink
        break;
    case 2:
        palette.setColor(QPalette::Window, QColor(191, 208, 202)); // BFD0CA - green
        break;
    case 3:
        palette.setColor(QPalette::Window, QColor(207, 157, 240)); // #CF9DF0 - purple
        break;
    case 4:
        palette.setColor(QPalette::Window, QColor(170, 193, 208)); // AAC1D0 - blue
        break;
    case 5:
        palette.setColor(QPalette::Window, QColor(240, 240, 240)); // F0F0F0 - white
        break;
    default:
        // default
        break;
    }
    setPalette(palette);
}

/**
 * @brief Changes the color of the buttons.
 * @param color The color index.
 */
void game2048::changeButtonColor(int button)
{
    qDebug() << "changeButtonColor() method called.";
    QString buttonStyleSheet = "";
    switch(button){
    case 1:
        buttonStyleSheet = "background-color: #D49987";
        //pink
        break;
    case 2:
        buttonStyleSheet = "background-color: #8E9A6D";
        //green
        break;
    case 3:
        buttonStyleSheet = "background-color: #A981C4";
        //purple
        break;
    case 4:
        buttonStyleSheet = "background-color: #7D8E99";
        //blue
        break;
    case 5:
        buttonStyleSheet = "background-color: #FDFDFD";
        //grey
        break;
    default:
        break;
    }
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            gridButtons[i][j]->setStyleSheet(buttonStyleSheet);
        }
    }
}

/**
 * @brief Slot function called when the "Go Back" button is clicked.
 * Emits the signal to go back to the main menu and deletes the object later.
 */
void game2048::onGoBackClicked() {
    qDebug() << "Go Back clicked, emitting goBackToMainMenu";
    emit goBackToMainMenu();
    this->deleteLater();
}

/**
 * @brief Slot function called when the "Exit" button is clicked.
 * Calls the onGoBackClicked() function to handle the action.
 */
void game2048::actionExitClicked(){
    onGoBackClicked();
}


