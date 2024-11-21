/**
 * @file fifteenpuzzle.cpp
 * @brief Implementation of the FifteenPuzzle class.
 *
 * This file provides detailed implementations of the FifteenPuzzle functionalities including game initialization,
 * handling user interactions, updating the game state, and managing game settings and high scores.
 */
#include "fifteenpuzzle.h"
#include "mainmenu.h"
#include <QGridLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QInputDialog>
#include <algorithm>
#include <random>
#include <chrono>
#include <QLabel>
#include <QMediaPlayer>
#include <QSoundEffect>
#include <QAudioOutput>


/**
 * @brief Constructor for FifteenPuzzle. Initializes game components and selects the game's difficulty.
 * @param parent The parent widget. Defaults to nullptr.
 */
FifteenPuzzle::FifteenPuzzle(QWidget *parent) : QWidget(parent) {
    fifteensound = new QMediaPlayer;
    audioOutput = new QAudioOutput;
    fifteensound->setAudioOutput(audioOutput);
    fifteensound->setSource(QUrl("qrc:/sound/rain1.wav"));
    audioOutput->setVolume(10);
    connect(fifteensound, &QMediaPlayer::mediaStatusChanged, this, &FifteenPuzzle::musicStateChanged);

    fifteensound->play();

    QDialog dialog(this);

    dialog.setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint);

    QVBoxLayout layout(&dialog);
    QString difficulty;

    QLabel label("Choose the level of difficulty:");
    layout.addWidget(&label);

    QPushButton *easyButton = new QPushButton("Easy", &dialog);
    easyButton->setStyleSheet("QPushButton { background-color: white; border: 2px solid gray; padding: 10px; }"
                              "QPushButton:hover { background-color: gold; }"
                              "QPushButton:pressed { background-color: darkgray; }");
    easyButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    connect(easyButton, &QPushButton::clicked, [&dialog, &difficulty](){
        QSoundEffect *button = new QSoundEffect;
        button->setSource(QUrl("qrc:/sound/m_pushbutton.wav"));
        button->setVolume(0.2f); // Set volume from 0.0 to 1.0
        button->play();
        difficulty = "Easy";
        dialog.accept();
    });
    layout.addWidget(easyButton);


    QPushButton *hardButton = new QPushButton("Hard", &dialog);
    hardButton->setStyleSheet("QPushButton { background-color: white; border: 2px solid gray; padding: 10px; }"
                              "QPushButton:hover { background-color: gold; }"
                              "QPushButton:pressed { background-color: darkgray; }");
    hardButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    connect(hardButton, &QPushButton::clicked, [&dialog, &difficulty](){
        QSoundEffect *button = new QSoundEffect;
        button->setSource(QUrl("qrc:/sound/m_pushbutton.wav"));
        button->setVolume(0.2f); // Set volume from 0.0 to 1.0
        button->play();
        difficulty = "Hard";
        dialog.accept();
    });
    layout.addWidget(hardButton);

    dialog.setLayout(&layout);

    dialog.exec();
    currentDifficulty = (difficulty == "Easy") ? Easy : Hard;
    setupGame();
    initialState.clear();
    for (int i = 0; i < gridSize * gridSize; ++i) {
        initialState.push_back(buttons[i]->text());
    }

    updateButtonColors();

    //Initialize the button sound effect
    buttonSoundEffect = new QSoundEffect(this);
    buttonSoundEffect->setSource(QUrl("qrc:/sound/m_pushbutton.wav"));
    buttonSoundEffect->setVolume(0.2f);
}

/**
 * @brief Handles state changes in the background music player, restarting the music when it ends.
 * @param status The current media status of the music player.
 */
void FifteenPuzzle::musicStateChanged(QMediaPlayer::MediaStatus status) {
    // Slot implementation for handling music state changes omitted for brevity
    if (status == QMediaPlayer::EndOfMedia) {
        fifteensound->setPosition(0); // Restart the music
        fifteensound->play();
    }
}

/**
 * @brief Sets up the game board, including grid layout and button initialization.
 */
void FifteenPuzzle::setupGame() {
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    movesLabel = new QLabel("Current Moves: 0", this);
    movesLabel->setStyleSheet("font-size: 20pt; font-weight: bold;");
    movesLabel->setAlignment(Qt::AlignCenter);

    QGridLayout *gridLayout = new QGridLayout();
    gridLayout->setSpacing(2);
    gridLayout->setVerticalSpacing(15);
    gridLayout->setContentsMargins(1, 1, 1, 1);

    int buttonSize = std::min(this->width(), this->height()) / gridSize - 2;

    // Initialize buttons
    for (int i = 0; i < gridSize * gridSize; ++i) {
        buttons[i] = new QPushButton(QString::number(i + 1), this);
        buttons[i]->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        buttons[i]->setFixedSize(buttonSize, buttonSize);
        buttons[i]->setStyleSheet("font-size: 30pt;font-weight: bold;");
        gridLayout->addWidget(buttons[i], i / gridSize, i % gridSize);
        connect(buttons[i], &QPushButton::clicked, this, &FifteenPuzzle::buttonClicked);
    }
    buttons[gridSize * gridSize - 1]->setText("");

    for (int i = 0; i < gridSize; ++i) {
        gridLayout->setRowStretch(i, 1);
        gridLayout->setColumnStretch(i, 1);
    }

    mainLayout->addLayout(gridLayout);

    // Initialize Reset and New Game buttons
    QPushButton *showHighScoreButton = new QPushButton("Highest Score", this);
    connect(showHighScoreButton, &QPushButton::clicked, this, &FifteenPuzzle::showHighScore);


    resetButton = new QPushButton("Reset", this);
    connect(resetButton, &QPushButton::clicked, this, &FifteenPuzzle::resetButtonClicked);

    newGameButton = new QPushButton("New Game", this);
    connect(newGameButton, &QPushButton::clicked, this, &FifteenPuzzle::newGameButtonClicked);

    menuButton = new QPushButton("Main Menu", this);
    connect(menuButton, &QPushButton::clicked, this, &FifteenPuzzle::onGoBackClicked);

    helpButton = new QPushButton("Get Help", this);
    connect(helpButton, &QPushButton::clicked, this, &FifteenPuzzle::showHelp);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    mainLayout->addWidget(movesLabel);
    mainLayout->addWidget(resetButton);
    buttonLayout->addWidget(newGameButton);
    buttonLayout->addWidget(menuButton);
    buttonLayout->addWidget(helpButton);
    mainLayout->addWidget(showHighScoreButton);

    mainLayout->addLayout(buttonLayout);

    shuffleTiles(currentDifficulty);
}


/**
 * @brief Shuffles the tiles on the game board based on the selected difficulty level.
 * @param difficulty The difficulty level of the puzzle (Easy or Hard).
 */
void FifteenPuzzle::shuffleTiles(Difficulty difficulty) {
    std::random_device rd;
    std::mt19937 g(rd());

    if (difficulty == Hard) {
        std::shuffle(buttons, buttons + (gridSize * gridSize) - 1, g); // Shuffle buttons except the last one
    } else {
        setupEasyPuzzle();
    }

    // Ensure buttons are placed correctly after shuffling
    for (int i = 0; i < gridSize * gridSize; ++i) {
        QGridLayout *layout = static_cast<QGridLayout*>(this->layout()->itemAt(0)->layout());
        layout->addWidget(buttons[i], i / gridSize, i % gridSize);
    }
}

/**
 * @brief Resets the game board to its initial state.
 */
void FifteenPuzzle::resetButtonClicked() {
    buttonSoundEffect->play();
    for (int i = 0; i < gridSize * gridSize; ++i) {
        buttons[i]->setText(initialState.at(i));
    }
    movesCount = 0;
    movesLabel->setText("Current Moves: 0");
    updateButtonColors();
}

/**
 * @brief Starts a new game, allowing the player to select a difficulty before shuffling the board.
 */
void FifteenPuzzle::newGameButtonClicked() {
    buttonSoundEffect->play();
    QDialog dialog(this);

    dialog.setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint);

    QVBoxLayout layout(&dialog);
    QString difficulty;

    QLabel label("Choose the level of difficulty:");
    layout.addWidget(&label);

    QPushButton *easyButton = new QPushButton("Easy", &dialog);
    easyButton->setStyleSheet("QPushButton { background-color: white; border: 2px solid gray; padding: 10px; }"
                              "QPushButton:hover { background-color: gold; }"
                              "QPushButton:pressed { background-color: darkgray; }");
    easyButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    connect(easyButton, &QPushButton::clicked, [&dialog, &difficulty](){
        QSoundEffect *button = new QSoundEffect;
        button->setSource(QUrl("qrc:/sound/m_pushbutton.wav"));
        button->setVolume(0.2f); // Set volume from 0.0 to 1.0
        button->play();
        difficulty = "Easy";
        dialog.accept();
    });
    layout.addWidget(easyButton);


    QPushButton *hardButton = new QPushButton("Hard", &dialog);
    hardButton->setStyleSheet("QPushButton { background-color: white; border: 2px solid gray; padding: 10px; }"
                              "QPushButton:hover { background-color: gold; }"
                              "QPushButton:pressed { background-color: darkgray; }");
    hardButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    connect(hardButton, &QPushButton::clicked, [&dialog, &difficulty](){
        QSoundEffect *button = new QSoundEffect;
        button->setSource(QUrl("qrc:/sound/m_pushbutton.wav"));
        button->setVolume(0.2f); // Set volume from 0.0 to 1.0
        button->play();
        difficulty = "Hard";
        dialog.accept();
    });
    layout.addWidget(hardButton);

    dialog.setLayout(&layout);

    dialog.exec();
    currentDifficulty = (difficulty == "Easy") ? Easy : Hard;
    shuffleTiles(currentDifficulty);
    initialState.clear();
    for (int i = 0; i < gridSize * gridSize; ++i) {
        initialState.push_back(buttons[i]->text());
    }
    movesCount = 0;
    movesLabel->setText("Current Moves: 0");
    updateButtonColors();
}

/**
 * @brief Sets up an easy puzzle configuration that can be solved in a few moves.
 */
void FifteenPuzzle::setupEasyPuzzle() {
    for (int i = 0; i < gridSize * gridSize - 1; ++i) {
        buttons[i]->setText(QString::number(i + 1));
    }
    buttons[gridSize * gridSize - 1]->setText(""); // Set the last button as empty

    std::random_device rd;
    std::mt19937 gen(rd());

    std::uniform_int_distribution<> distrib(0, 1);
    int ran = distrib(gen);

    if(ran == 1){
        buttons[10]->setText(QString::number(12));
        buttons[11]->setText(QString::number(15));
        buttons[14]->setText(QString::number(11));
    }else{
        buttons[10]->setText(QString::number(15));
        buttons[11]->setText("");
        buttons[14]->setText(QString::number(12));
        buttons[15]->setText(QString::number(11));
    }
}

/**
 * @brief Responds to button clicks by moving tiles if the move is valid.
 */
void FifteenPuzzle::buttonClicked() {
    buttonSoundEffect->play();
    QPushButton *clickedButton = qobject_cast<QPushButton*>(sender());
    int clickedIndex = findButtonIndex(clickedButton->text());
    int emptyIndex = findButtonIndex("");

    if (isAdjacent(clickedIndex, emptyIndex)) {
        updateButtonColors();
        movesCount++;
        movesLabel->setText(QString("Current Moves: %1").arg(movesCount));
        QString tempText = buttons[clickedIndex]->text();
        buttons[clickedIndex]->setText(buttons[emptyIndex]->text());
        buttons[emptyIndex]->setText(tempText);

        if (isSolved()) {
            updateHighScore();
            updateButtonColors();

            // Winning QMessageBox
            QMessageBox *msgBox = new QMessageBox(this);
            msgBox->setWindowTitle("Congratulations!");
            msgBox->setText("Congratulations! \n\nYou've successfully solved the puzzle.");
            msgBox->setStandardButtons(QMessageBox::Ok);

            // Set win icon pixmap
            QPixmap winPix(":/image/tictactoeWin.png");
            msgBox->setIconPixmap(winPix.scaled(64, 64, Qt::KeepAspectRatio, Qt::SmoothTransformation));

            // Show the message box
            msgBox->exec();

            newGameButtonClicked();
        }
    }
    updateButtonColors();
}

/**
 * @brief Finds the index of a button based on its label.
 * @param number The label of the button to find.
 * @return The index of the button, or -1 if not found.
 */
int FifteenPuzzle::findButtonIndex(const QString &number) {
    for (int i = 0; i < gridSize * gridSize; ++i) {
        if (buttons[i]->text() == number) {
            return i;
        }
    }
    return -1;
}

/**
 * @brief Checks if two indices on the grid are adjacent to each other.
 * @param index1 The index of the first tile.
 * @param index2 The index of the second tile.
 * @return True if the tiles are adjacent, false otherwise.
 */
bool FifteenPuzzle::isAdjacent(int index1, int index2) {
    int row1 = index1 / gridSize, col1 = index1 % gridSize;
    int row2 = index2 / gridSize, col2 = index2 % gridSize;

    return (std::abs(row1 - row2) + std::abs(col1 - col2)) == 1;
}

/**
 * @brief Checks if the puzzle is solved.
 * @return True if the puzzle is in a solved state, false otherwise.
 */
bool FifteenPuzzle::isSolved() {
    for (int i = 0; i < gridSize * gridSize - 1; ++i) {
        if (buttons[i]->text().toInt() != i + 1) {
            return false;
        }
    }
    return buttons[gridSize * gridSize - 1]->text().isEmpty();
}

/**
 * @brief Handles the action of going back to the main menu.
 */
void FifteenPuzzle::onGoBackClicked(){
    buttonSoundEffect->play();
    this->deleteLater();
    auto mainMenu = new MainMenu();
    mainMenu->show();
    fifteensound->stop();
}

/**
 * @brief Displays help and instructions for the game.
 */
void FifteenPuzzle::showHelp() {
    buttonSoundEffect->play();
    QDialog dialog(this);
    dialog.setWindowTitle("Fifteen Puzzle Instructions");

    QVBoxLayout layout(&dialog);

    QLabel label(&dialog);
    label.setTextFormat(Qt::RichText);
    label.setText(
        "<h2>Welcome to Fifteen Puzzle! 😊</h2>"
        "<p><b>Objective:</b> Arrange the tiles in ascending numerical order from 1 to 15, leaving the last square empty. 🎯</p>"
        "<p><b>How to Play:</b> You can slide any tile adjacent to the empty space into the empty space. Use this to gradually organize the board. 🖱️↔️</p>"
        "<p><b>Tips and Tricks:</b> Try solving the puzzle one row or column at a time. Start with the first two rows, then the first two columns, and so on. Planning your moves ahead can minimize unnecessary tile movements. 🤔💡</p>"
        "<p>Not all puzzle configurations are solvable. If you find yourself stuck, don't hesitate to start a new game. 🔄🆕</p>"
        "<p><b>Color Guide:</b> Tiles will be highlighted in <span style='color:green;'>green</span> if they are in the correct position, otherwise, they will be highlighted in <span style='color:purple;'>purple</span>. This can help you easily identify which tiles are correctly placed. 🟩🟪</p>"
        "<p>Good luck, and have fun solving the puzzle! 😄🍀</p>"
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
 * @brief Updates the high score for the current difficulty level, if the current game's move count is lower.
 */
void FifteenPuzzle::updateHighScore() {
    QString key = currentDifficulty == Easy ? "HighScoreEasy" : "HighScoreHard";
    qDebug() << "Current currentDifficulty : " << key, currentDifficulty;

    int currentHighScore = settings.value(key, INT_MAX).toInt();
    qDebug() << "Current currentHighScore : " << currentHighScore;

    if (movesCount < currentHighScore || currentHighScore ==0 ) {
        settings.setValue(key, movesCount);
        qDebug() << "The movesCount are: " << movesCount;
    }
}

/**
 * @brief Shows the high score for the current difficulty level.
 */
void FifteenPuzzle::showHighScore() {
    fifteensound->play();
    QString difficultyKey = currentDifficulty == Easy ? "HighScoreEasy" : "HighScoreHard";
    int highScore = settings.value(difficultyKey, INT_MAX).toInt();
    qDebug() << "Current highScore : " << highScore;

    // Create a win QMessageBox
    QMessageBox msgBox(this);
    msgBox.setWindowTitle("High Score");

    // Prepare the message content
    QString message = highScore == INT_MAX ? "No high score yet." : QString("High Score: %1 moves").arg(highScore);
    if(highScore ==0){
        message = "No high score yet.";
    }
    msgBox.setText(message);

    // Set win icon pixmap
    QPixmap winPix(":/image/higestscore.png");
    msgBox.setIconPixmap(winPix.scaled(64, 64, Qt::KeepAspectRatio, Qt::SmoothTransformation));

    // Set standard buttons on the message box
    msgBox.setStandardButtons(QMessageBox::Ok);

    // Show the message box
    msgBox.exec();
}

/**
 * @brief Updates the colors of the puzzle buttons based on their current positions.
 */
void FifteenPuzzle::updateButtonColors() {
    for (int i = 0; i < gridSize * gridSize; ++i) {
        if (buttons[i]->text().isEmpty()) {
            buttons[i]->setStyleSheet(""); // Resets to no specific background color
        } else {
            if (buttons[i]->text().toInt() == i + 1) {
                buttons[i]->setStyleSheet("background-color: #8FB3A5;font-weight: bold;font-size: 20pt;"); // Light green for correct position
            } else {
                buttons[i]->setStyleSheet("background-color: #A89AD4;font-weight: bold;font-size: 20pt;"); // Purple for incorrect position
            }
        }
    }
}
