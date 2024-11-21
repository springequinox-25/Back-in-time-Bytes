/**
 * @file TicTacToe.cpp
 * @brief Implementation of the TicTacToe class.
 *
 * Provides detailed functionalities for initializing the game, handling user interactions, managing game state,
 * and rendering the game UI. Supports various game modes, difficulty levels, and includes AI for the machine opponent.
 */
#include "TicTacToe.h"
#include "mainmenu.h"
#include <QStandardPaths>
#include <QMessageBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QTimer>
#include <QFile>
#include <QSettings>
#include <QCoreApplication>
#include <QMediaPlayer>
#include <QSoundEffect>
#include <QAudioOutput>

/**
 * @brief Constructs a TicTacToe object with specified game mode, difficulty, and grid size.
 * @param gameMode The game mode (Player vs. Machine or Player vs. Player).
 * @param difficulty The game difficulty (Easy, Medium, Hard).
 * @param gridSize The size of the game grid (e.g., 3x3).
 * @param parent The parent widget.
 */
TicTacToe::TicTacToe(QString gameMode, QString difficulty, int gridSize, QWidget *parent)
    : QMainWindow(parent), gameMode(gameMode), difficulty(difficulty), gridSize(gridSize), isHumanTurn(true) {
    tictactoesound = new QMediaPlayer;
    audioOutput = new QAudioOutput;
    tictactoesound->setAudioOutput(audioOutput);
    tictactoesound->setSource(QUrl("qrc:/sound/guzheng1.wav"));
    audioOutput->setVolume(10);
    connect(tictactoesound, &QMediaPlayer::mediaStatusChanged, this, &TicTacToe::musicStateChanged);
    tictactoesound->play();
    setWindowTitle("Tic Tac Toe");
    adjustWindowSize();

    QWidget* centralWidget = new QWidget(this);
    QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);

    winLose = new QLabel(this);
    winLose->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(winLose);
    updateWinLose();

    setCentralWidget(centralWidget);
    QGridLayout* layout = new QGridLayout(centralWidget);
    layout->setSpacing(10);

    board.resize(gridSize, std::vector<char>(gridSize, Player::NONE));
    buttons.resize(gridSize, std::vector<QPushButton*>(gridSize));

    for (int i = 0; i < gridSize; ++i) {
        for (int j = 0; j < gridSize; ++j) {
            QPushButton* button = new QPushButton(" ");
            button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
            button->setMinimumSize(100, 100);
            QFont font = button->font();
            font.setPointSize(24);
            button->setFont(font);
            layout->addWidget(button, i, j);
            buttons[i][j] = button;
            connect(button, &QPushButton::clicked, [this, i, j]() { this->buttonClicked(i, j); });
        }
    }
    mainLayout->addLayout(layout);

    statusLabel = new QLabel("Player X's turn", this); // Initial message
    statusLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(statusLabel);

    // Bottom buttons
    QHBoxLayout* bottomLayout = new QHBoxLayout;
    QPushButton* resetButton = new QPushButton("Reset");
    QPushButton* hintButton = new QPushButton("Get Hint");
    QPushButton* exitButton = new QPushButton("Back to main menu");
    QPushButton* settingsButton = new QPushButton("Settings");
    highScoreButton = new QPushButton("Show High Score", this);
    helpButton = new QPushButton("Help", this);

    bottomLayout->addWidget(resetButton);
    bottomLayout->addWidget(hintButton);
    bottomLayout->addWidget(exitButton);
    bottomLayout->addWidget(settingsButton);
    bottomLayout->addWidget(highScoreButton);
    bottomLayout->addWidget(helpButton);

    connect(resetButton, &QPushButton::clicked, this, &TicTacToe::on_actionNew_Game_triggered);
    connect(hintButton, &QPushButton::clicked, this, &TicTacToe::on_actionGet_Hint_triggered);
    connect(exitButton, &QPushButton::clicked, this, &TicTacToe::on_actionExit_triggered);
    connect(settingsButton, &QPushButton::clicked, this, &TicTacToe::openSettingsDialog);
    connect(highScoreButton, &QPushButton::clicked, this, &TicTacToe::displayHighScore);
    connect(helpButton, &QPushButton::clicked, this, &TicTacToe::showHelp);

    mainLayout->addLayout(bottomLayout);
    setCentralWidget(centralWidget);

    mainLayout->setAlignment(bottomLayout, Qt::AlignHCenter);

    //Initialize the button sound effect
    buttonSoundEffect = new QSoundEffect(this);
    buttonSoundEffect->setSource(QUrl("qrc:/sound/m_pushbutton.wav"));
    buttonSoundEffect->setVolume(0.2f);
}

/**
 * @brief Adjusts the main window size based on the grid size.
 *
 * Calculates and sets the main window size to ensure all game tiles are properly displayed,
 * considering the number of tiles and their spacing.
 */
void TicTacToe::adjustWindowSize() {
    int baseWidth = 100; // Minimum width per button
    int baseHeight = 100; // Minimum height per button
    int spacing = 10; // Assume some spacing between buttons
    int padding = 20; // Extra space for margins
    int bottomUIHeight = 100; // Adjust based on the height of status label and bottom buttons layout

    // Calculate total window size based on grid size
    int width = gridSize * baseWidth + (gridSize - 1) * spacing + 2 * padding;
    int height = gridSize * baseHeight + (gridSize - 1) * spacing + bottomUIHeight + 2 * padding;

    // Ensure minimum size is respected
    width = qMax(width, 500); // Minimum width of the window
    height = qMax(height, 600); // Minimum height of the window

    resize(width, height);
}

/**
 * @brief Handles the state changes of the background music player.
 * @param status The new status of the media player.
 */
void TicTacToe::musicStateChanged(QMediaPlayer::MediaStatus status) {
    if (status == QMediaPlayer::EndOfMedia) {
        tictactoesound->setPosition(0); // Restart the music
        tictactoesound->play();
    }
}

/**
 * @brief Opens the settings dialog for the game, allowing the player to adjust game settings.
 */
void TicTacToe::openSettingsDialog() {
    buttonSoundEffect->play();
    QDialog settingsDialog(this);
    settingsDialog.setWindowTitle("Settings");

    // Create combo boxes for settings
    QComboBox *gameModeCombo = new QComboBox();
    gameModeCombo->addItem("Player vs Player", "PVP");
    gameModeCombo->addItem("Player vs Machine", "PVM");

    QComboBox *difficultyCombo = new QComboBox();
    difficultyCombo->addItem("Easy", "Easy");
    difficultyCombo->addItem("Hard", "Hard");

    QComboBox *gridSizeCombo = new QComboBox();
    gridSizeCombo->addItem("3x3", 3);
    gridSizeCombo->addItem("4x4", 4);
    gridSizeCombo->addItem("5x5", 5);

    // Layout for the form
    QFormLayout *layout = new QFormLayout();
    layout->addRow(new QLabel("Game Mode:"), gameModeCombo);
    layout->addRow(new QLabel("Difficulty:"), difficultyCombo);
    layout->addRow(new QLabel("Grid Size:"), gridSizeCombo);

    // Buttons for dialog actions
    QDialogButtonBox *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    layout->addWidget(buttons);
    settingsDialog.setLayout(layout);

    // Connect buttons to dialog slots
    connect(buttons, &QDialogButtonBox::accepted, &settingsDialog, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, &settingsDialog, &QDialog::reject);
    if (settingsDialog.exec() == QDialog::Accepted) {
        this->deleteLater();
        buttonSoundEffect->play();
        tictactoesound->stop();
        QString gameMode = gameModeCombo->currentData().toString();
        QString difficulty = difficultyCombo->currentData().toString();
        int gridSize = gridSizeCombo->currentData().toInt();

        updateAndSaveStats(false, false, false);
        saveHighScore(player1Wins, player2Wins, ties);
        auto *tictactoe = new TicTacToe(gameMode, difficulty, gridSize);
        tictactoe->show();
    } else {
        buttonSoundEffect->play();
    }
}

/**
 * @brief Updates the win/loss statistics and displays them on the UI.
 */
void TicTacToe::updateWinLose() {
    QString statsText;
    if(gameMode == "PVP"){
        statsText = QString("PlayerX Win:  %1  | PlayerO Win:  %2  |   Ties:  %3  ")
                        .arg(wins).arg(losses).arg(ties);
    }else{
        statsText = QString("Win:  %1  | Lose:  %2  |   Ties:  %3  ")
                        .arg(wins).arg(losses).arg(ties);
    }
    winLose->setText(statsText);
    // Adjust font size and style as needed
    winLose->setStyleSheet("font-size: 16pt; font-weight: bold;");

    // Position the statsLabel at the top center
    int labelWidth = this->width(); // Make the label span the entire window width
    int labelHeight = 30; // Adjust as needed
    winLose->setGeometry(0, 10, labelWidth, labelHeight); // Adjust the y position and height as needed
}

/**
 * @brief Responds to a button click on the game grid, marking the cell and checking for a win or tie.
 * @param x The x-coordinate (row) of the clicked button.
 * @param y The y-coordinate (column) of the clicked button.
 */
void TicTacToe::buttonClicked(int x, int y) {
    buttonSoundEffect->play();
    if (board[x][y] != Player::NONE) return;

    QString playerSymbol;
    QString colorStyle;
    // Assume buttons are square for simplicity in calculating font size
    int fontSize = static_cast<int>(buttons[x][y]->height() * 0.8 / 1.33); // Approximation to keep font size ~80% of button height

    if (gameMode == "PVP") {
        playerSymbol = isHumanTurn ? "X" : "O";
        colorStyle = isHumanTurn ? "color: blue;" : "color: red;";

        board[x][y] = isHumanTurn ? Player::HUMAN : Player::Machine;
        buttons[x][y]->setStyleSheet(colorStyle + QString(" font-size: %1pt;").arg(fontSize));
        buttons[x][y]->setText(playerSymbol);
        buttons[x][y]->setDisabled(true);

        if (checkWin(isHumanTurn ? Player::HUMAN : Player::Machine, board)) {
            openMsgBox(QString("%1 Wins!").arg(isHumanTurn ? "Player X" : "Player O"));
            if(isHumanTurn){
                updateAndSaveStats(true, false, false);
                saveHighScore(player1Wins, player2Wins, ties);
            }else{
                updateAndSaveStats(false, true,false);
                saveHighScore(player1Wins, player2Wins, ties);
            }
            on_actionNew_Game_triggered();
            return;
        }
        isHumanTurn = !isHumanTurn;
        statusLabel->setText(isHumanTurn ? "Player X's turn" : "Player O's turn");
    } else {
        playerSymbol = "X";
        colorStyle = "color: blue;";
        board[x][y] = Player::HUMAN;
        buttons[x][y]->setStyleSheet(colorStyle + QString(" font-size: %1pt;").arg(fontSize));
        buttons[x][y]->setText(playerSymbol);
        buttons[x][y]->setDisabled(true);

        if (checkWin(Player::HUMAN, board)) {
            openMsgBox("You win!");
            updateAndSaveStats(true, false,false);
            saveHighScore(player1Wins, player2Wins, ties);
            on_actionNew_Game_triggered();
            return;
        }
        if (!isGameOver(board)) {
            QTimer::singleShot(500, this, &TicTacToe::playTheBest);
        }
    }

    if (isGameOver(board)) {
        openMsgBox("It's a tie!");
        updateAndSaveStats(false, false, true);
        saveHighScore(player1Wins, player2Wins, ties);
        on_actionNew_Game_triggered();
    }
}

/**
 * @brief Initiates the best possible move for the machine based on the current state of the game.
 */
void TicTacToe::playTheBest() {
    int fontSize = static_cast<int>(buttons[0][0]->height() * 0.8 / 1.33);
    if (difficulty == "Easy" || gridSize >3) {
        // Make a random or less optimal move
        std::vector<std::pair<int, int>> availableMoves;
        for (int i = 0; i < gridSize; ++i) {
            for (int j = 0; j < gridSize; ++j) {
                if (board[i][j] == Player::NONE) {
                    availableMoves.push_back({i, j});
                }
            }
        }

        if (!availableMoves.empty()) {
            // Select a random move from available ones
            buttonSoundEffect->play();
            int moveIndex = rand() % availableMoves.size();
            auto [row, col] = availableMoves[moveIndex];
            board[row][col] = Player::Machine;
            buttons[row][col]->setText("O");
            QString colorStyle = "color: red;";
            buttons[row][col]->setStyleSheet(colorStyle + QString(" font-size: %1pt;").arg(fontSize));
            buttons[row][col]->setDisabled(true);
        }
        statusLabel->setText("Player X's turn");
    } else if (difficulty == "Hard" && gridSize <4) {
        // Use miniMax algorithm to find the best move
        int bestScore = INT_MIN;
        std::pair<int, int> bestMove(-1, -1);

        for (int i = 0; i < gridSize; ++i) {
            for (int j = 0; j < gridSize; ++j) {
                if (board[i][j] == Player::NONE) {
                    board[i][j] = Player::Machine;
                    int score = miniMax(board, 0, false); // Assuming false means it's the AI's turn in your implementation
                    board[i][j] = Player::NONE;
                    if (score > bestScore) {
                        bestScore = score;
                        bestMove = {i, j};
                    }
                }
            }
        }

        if (bestMove.first != -1) {
            board[bestMove.first][bestMove.second] = Player::Machine;
            buttons[bestMove.first][bestMove.second]->setText("O");
            buttons[bestMove.first][bestMove.second]->setDisabled(true);
            QString colorStyle = "color: red;";
            buttons[bestMove.first][bestMove.second]->setStyleSheet(colorStyle + QString(" font-size: %1pt;").arg(fontSize));
        }
        buttonSoundEffect->play();
        statusLabel->setText("Player X's turn");
    }

    // After making a move, check for a win or tie
    if (checkWin(Player::Machine, board)) {
        openMsgBox("You lose, machine wins!");
        updateAndSaveStats(false, true, false);
        saveHighScore(player1Wins, player2Wins, ties);
        on_actionNew_Game_triggered();
    } else if (isGameOver(board)) {
        openMsgBox("It's a tie!");
        updateAndSaveStats(false, false, true);
        saveHighScore(player1Wins, player2Wins, ties);
        on_actionNew_Game_triggered();
    } else {
        isHumanTurn = true; // Hand the turn back to the player
    }
}

/**
 * @brief Implements the minimax algorithm to determine the best move for the machine.
 * @param gameBoard The current state of the game board.
 * @param depth The current depth of the recursion.
 * @param isMachine Flag indicating if it's the machine's turn.
 * @return The score of the board configuration.
 */
int TicTacToe::miniMax(std::vector<std::vector<char>> gameBoard,
                       int depth,
                       bool isMachine) const
{
    int topScore  {0};
    int bestScore {0};
    if (isGameOver(gameBoard))
        return (int) calculateScore(gameBoard);

    bestScore = (isMachine) ? INT_MIN : INT_MAX;
    for(auto &row: gameBoard)
        for(char &item: row)
            if (item == Player::NONE) {
                item = (isMachine) ? Player::Machine : Player::HUMAN;
                topScore = miniMax(gameBoard, depth + 1, !isMachine);
                item = Player::NONE;
                bestScore = (isMachine)                        ?
                                std::max(topScore, bestScore) :
                                std::min(topScore, bestScore) ;
            }
    return bestScore;
}

/**
 * @brief Calculates the score for the current game board.
 * @param gameBoard The game board to evaluate.
 * @return The state of the game from the human player's perspective (win, lose, tie).
 */
HumanState TicTacToe::calculateScore(std::vector<std::vector<char>> gameBoard) const
{
    return (checkWin(Player::HUMAN, gameBoard)) ?
               HumanState::WIN                      :
               (checkWin(Player::Machine, gameBoard))?
                   HumanState::LOSE                     :
                   HumanState::TIE                      ;
}

/**
 * @brief Checks if the game is over based on the current board state.
 * @param gameBoard The current game board.
 * @return True if the game is over, otherwise false.
 */
bool TicTacToe::isGameOver(std::vector<std::vector<char>> gameBoard) const
{
    if (checkWin(Player::HUMAN, gameBoard) || checkWin(Player::Machine, gameBoard)) return true;
    for (const auto &row : gameBoard) {
        for (char cell : row) {
            if (cell == Player::NONE) return false; // Game is not over if any cell is empty
        }
    }
    return true; // No empty cells, game is over
}

/**
 * @brief Checks for a win condition for the specified player.
 * @param player The player to check for a winning condition.
 * @param gameBoard The game board to check.
 * @return True if the specified player has won, otherwise false.
 */
bool TicTacToe::checkWin(Player player, const std::vector<std::vector<char>>& gameBoard) const {
    char symbol = static_cast<char>(player);
    bool win;

    // Check rows and columns
    for (int i = 0; i < gridSize; ++i) {
        win = true;
        for (int j = 0; j < gridSize; ++j) {
            if (gameBoard[i][j] != symbol) {
                win = false;
                break;
            }
        }
        if (win) return true; // Winner found in a row

        win = true;
        for (int j = 0; j < gridSize; ++j) {
            if (gameBoard[j][i] != symbol) {
                win = false;
                break;
            }
        }
        if (win) return true; // Winner found in a column
    }

    // Check main diagonal (\)
    win = true;
    for (int i = 0; i < gridSize; ++i) {
        if (gameBoard[i][i] != symbol) {
            win = false;
            break;
        }
    }
    if (win) return true;

    // Check secondary diagonal (/)
    win = true;
    for (int i = 0; i < gridSize; ++i) {
        if (gameBoard[i][gridSize - 1 - i] != symbol) {
            win = false;
            break;
        }
    }
    if (win) return true;

    return false; // No winner found
}

/**
 * @brief Displays a message box with a given message, such as the outcome of the game.
 * @param msg The message to display in the message box.
 */
void TicTacToe::openMsgBox(QString msg) {
    QMessageBox* msgBox = new QMessageBox(this);
    msgBox->setText(msg);

    if(msg.contains("lose", Qt::CaseInsensitive)){
        QPixmap losePix(":/image/tictactoelose.png");
        msgBox->setIconPixmap(losePix.scaled(100, 100, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }else if (msg.contains("win", Qt::CaseInsensitive)) {
        QPixmap winPix(":/image/tictactoeWin.png");
        msgBox->setIconPixmap(winPix.scaled(64, 64, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }else{
        QPixmap tiePix(":/image/tieGame.png");
        msgBox->setIconPixmap(tiePix.scaled(64, 64, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }

    msgBox->exec();
}

TicTacToe::~TicTacToe()
{

}

/**
 * @brief Resets the game to its initial state, ready for a new game.
 * This function clears the game board, resets the UI, and prepares for a new game round.
 */
void TicTacToe::on_actionNew_Game_triggered(){
    buttonSoundEffect->play();
    updateWinLose();
    statusLabel->setText("Player X's turn");
    // Reset the game to its initial state, considering gridSize and gameMode
    for (int i = 0; i < gridSize; ++i) {
        for (int j = 0; j < gridSize; ++j) {
            board[i][j] = Player::NONE;
            buttons[i][j]->setText(" ");
            buttons[i][j]->setEnabled(true);
        }
    }
    isHumanTurn = true; // Player always starts in PvM
}

/**
 * @brief Handles the action to exit the game and return to the main menu.
 */
void TicTacToe::on_actionExit_triggered(){
    buttonSoundEffect->play();
    updateAndSaveStats(false, false, false);
    saveHighScore(player1Wins, player2Wins, ties);
    tictactoesound->stop();
    this->deleteLater();
    auto mainMenu = new MainMenu();
    mainMenu->show();
}

/**
 * @brief Provides a hint for the next move by highlighting a recommended cell.
 * This function is particularly useful in training or assisting new players.
 */
void TicTacToe::on_actionGet_Hint_triggered() {
    buttonSoundEffect->play();
    if (gridSize == 3) {
        // gridSize is 3, use the miniMax function to find the best move
        int bestScore = INT_MIN;
        std::pair<int, int> bestMove(-1, -1);

        for (int i = 0; i < gridSize; ++i) {
            for (int j = 0; j < gridSize; ++j) {
                if (board[i][j] == Player::NONE) {
                    board[i][j] = Player::HUMAN;
                    int score = miniMax(board, 0, false); // Assuming false is for the machine's turn
                    board[i][j] = Player::NONE;
                    if (score > bestScore) {
                        bestScore = score;
                        bestMove = {i, j};
                    }
                }
            }
        }

        // Highlight the best move found by miniMax
        if (bestMove.first != -1) {
            buttons[bestMove.first][bestMove.second]->setStyleSheet("background-color: yellow;");
            buttons[bestMove.first][bestMove.second]->setDisabled(true);
            QTimer::singleShot(300, [this, bestMove]() { buttons[bestMove.first][bestMove.second]->setStyleSheet(""); buttons[bestMove.first][bestMove.second]->setDisabled(false); });
        }
    } else {
        // gridSize is larger than 3, make a random or less optimal move
        std::vector<std::pair<int, int>> availableMoves;
        for (int i = 0; i < gridSize; ++i) {
            for (int j = 0; j < gridSize; ++j) {
                if (board[i][j] == Player::NONE) {
                    availableMoves.push_back({i, j});
                }
            }
        }

        if (!availableMoves.empty()) {
            int moveIndex = rand() % availableMoves.size();
            auto [row, col] = availableMoves[moveIndex];
            // Highlight the selected move
            buttons[row][col]->setStyleSheet("background-color: yellow;");
            buttons[row][col]->setDisabled(true);
            QTimer::singleShot(300, [this, row, col]() { buttons[row][col]->setStyleSheet(""); buttons[row][col]->setDisabled(false); });
        }
    }
}

/**
 * @brief Updates and saves game statistics, such as wins, losses, and ties.
 * @param player1Win Indicates if player 1 has won the game.
 * @param player2Win Indicates if player 2 (or the machine in PvM mode) has won the game.
 * @param tie Indicates if the game ended in a tie.
 */
void TicTacToe::updateAndSaveStats(bool player1Win, bool player2Win, bool tie) {
    // Update statistics based on the game outcome
    if (tie) {
        ties++;
    } else if (player1Win) {
        wins++;
        player1Wins++;
    } else if (player2Win) {
        losses++;
        player2Wins++;
    }
}

/**
 * @brief Saves the high score to persistent storage.
 * @param player1Wins The number of wins by player 1.
 * @param player2Wins The number of wins by player 2 (or the machine in PvM mode).
 * @param ties The number of tied games.
 */
void TicTacToe::saveHighScore(int player1Wins, int player2Wins, int ties) {
    QSettings settings;
    // Increment and save new stats. You might want to load current stats and then increment them.
    settings.setValue(getWinsKey(), player1Wins);
    settings.setValue(getLossesKey(), player2Wins);
    settings.setValue(getTiesKey(), ties);
}

/**
 * @brief Displays the high score/statistics to the player.
 */
void TicTacToe::displayHighScore() {
    buttonSoundEffect->play();

    QSettings settings;
    int wins = settings.value(getWinsKey(), 0).toInt();
    int losses = settings.value(getLossesKey(), 0).toInt();
    int ties = settings.value(getTiesKey(), 0).toInt();

    // Create a win QMessageBox
    QMessageBox msgBox(this);
    msgBox.setWindowTitle("Statistics");

    // Prepare the message content
    if (gameMode == "PVP") {
        msgBox.setText(QString("PlayerX's win: %1\nPlayerO's Win: %2\nTies: %3").arg(wins).arg(losses).arg(ties));
    } else {
        msgBox.setText(QString("Wins: %1\nLosses: %2\nTies: %3").arg(wins).arg(losses).arg(ties));
    }

    // Set win icon pixmap
    QPixmap winPix(":/image/higestscore.png");
    msgBox.setIconPixmap(winPix.scaled(64, 64, Qt::KeepAspectRatio, Qt::SmoothTransformation));

    // Set standard buttons on the message box
    msgBox.setStandardButtons(QMessageBox::Ok);

    // Show the message box
    msgBox.exec();
}

/**
 * @brief Generates the storage key for tracking the number of wins.
 * @return A QString representing the key used for storing the win count in the application settings.
 */
QString TicTacToe::getWinsKey() const {
    return "Stats/" + gameMode + "/" + difficulty + "/Wins";
}

/**
 * @brief Generates the storage key for tracking the number of losses.
 * @return A QString representing the key used for storing the loss count in the application settings.
 */
QString TicTacToe::getLossesKey() const {
    return "Stats/" + gameMode + "/" + difficulty + "/Losses";
}

/**
 * @brief Generates the storage key for tracking the number of ties.
 * @return A QString representing the key used for storing the tie count in the application settings.
 */
QString TicTacToe::getTiesKey() const {
    return "Stats/" + gameMode + "/" + difficulty + "/Ties";
}

/**
 * @brief Shows the game instructions to the player in a dialog.
 * This function helps new players understand the game rules and controls.
 */
void TicTacToe::showHelp() {
    buttonSoundEffect->play();
    QDialog dialog(this);
    dialog.setWindowTitle("TicTacToe Instructions");

    QVBoxLayout layout(&dialog);

    QLabel label(&dialog);
    label.setTextFormat(Qt::RichText);
    label.setText(
        "<h2>Welcome to TicTacToe! 🎉</h2>"
        "<p><b>Instructions:</b> Get ready to challenge a friend or the computer in this classic game.</p>"
        "<p><b>Game Modes:</b> Duel a friend or face the computer AI. 🤖👥</p>"
        "<p><b>Winning:</b> Align 3 of your marks (X or O) vertically, horizontally, or diagonally to win. 🏆</p>"
        "<p><b>Turns:</b> Player 1 is X, and Player 2 (or computer) is O. Take turns to place your mark. 🔁</p>"
        "<p><b>Tie:</b> If the board fills up and no one wins, it's a tie. 🤝</p>"
        "<p>Use the 'Settings' menu to customize your game mode and difficulty. ⚙️</p>"
        "<p><b>Tips and Tricks:</b> Owning the center gives you more winning opportunities. Block your opponent's moves if they are close to winning. Think ahead! 🧠</p>"
        "<p>Wishing you good luck and great fun! 🍀</p>"
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
 * @brief Handles the event triggered when the TicTacToe window is closed.
 * @param event Pointer to the QCloseEvent that contains event details. Though unused, it's part of the function signature for event handlers.
 */
void TicTacToe::closeEvent(QCloseEvent *event)
{
    Q_UNUSED(event); // Suppress unused parameter warning
    QCoreApplication::quit();
}



