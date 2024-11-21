/**
 * @file snakegame.cpp
 * @brief Implementation file for the SnakeGame class.
 *
 * This file contains the implementation of the SnakeGame class, which
 * represents the main window and logic for the Snake game.
 */

#include "snakegame.h"
#include <QMediaPlayer>
#include <QSoundEffect>
#include <QAudioOutput>
#include <QPushButton>
#include <QApplication>
#include <QVBoxLayout>
#include <QPainter>
#include <QFontDatabase>
#include <QDebug>
#include <QPixmap>
#include <QGraphicsOpacityEffect>
#include <QLabel>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>
#include <QSettings>
#include <QMutex>
#include <algorithm>

int highestScore = 0;

/**
 * @brief Constructor for SnakeGame class.
 *
 * Initializes the game window and sets up necessary components such as buttons, images, and timers.
 *
 * @param parent Pointer to the parent widget.
 */
SnakeGame::SnakeGame(QWidget *parent) : QWidget(parent) {
    // Set window properties
    setWindowTitle("Snake");
    resize(500, 600);
    setFixedSize(600, 650);

    // Initialize audio components
    snakegamesound = new QMediaPlayer;
    audioOutput = new QAudioOutput;
    snakegamesound->setAudioOutput(audioOutput);
    snakegamesound->setSource(QUrl("qrc:/sound/m_snakegameBG.wav"));
    audioOutput->setVolume(15);
    connect(snakegamesound, &QMediaPlayer::mediaStatusChanged, this, &SnakeGame::musicStateChanged);

    // Play background music
    snakegamesound->play();

    // Load highest scores from settings
    QSettings settings;
    highestScores["Easy"] = settings.value("HighestScoreEasy", 0).toInt();
    highestScores["Medium"] = settings.value("HighestScoreMedium", 0).toInt();
    highestScores["Hard"] = settings.value("HighestScoreHard", 0).toInt();

    // Boolean variable for shown instruction
    instructionsShown = false;

    // Define button style
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

    // Create buttons
    QPushButton *goBackButton = new QPushButton("↩", this);
    connect(goBackButton, &QPushButton::clicked, this, &SnakeGame::onGoBackClicked);
    goBackButton->setFixedSize(30, 20);

    restartButton = new QPushButton("↻", this);
    connect(restartButton, &QPushButton::clicked, this, &SnakeGame::restartGame);
    restartButton->setFixedSize(30, 20);

    pauseButton = new QPushButton("⏸", this);
    connect(pauseButton, &QPushButton::clicked, this, &SnakeGame::pauseGame);
    pauseButton->setFixedSize(30, 20);

    resumeButton = new QPushButton("▶", this);
    connect(resumeButton, &QPushButton::clicked, this, &SnakeGame::resumeGame);
    resumeButton->setFixedSize(30, 20);

    getHelpButton = new QPushButton("?", this);
    connect(getHelpButton, &QPushButton::clicked, this, &SnakeGame::showInstructions);
    getHelpButton->setFixedSize(30, 20);

    // Apply button style
    pauseButton->setStyleSheet(buttonStyle);
    restartButton->setStyleSheet(buttonStyle);
    resumeButton->setStyleSheet(buttonStyle);
    goBackButton->setStyleSheet(buttonStyle);
    getHelpButton->setStyleSheet(buttonStyle);

    // Set initial positions and images
    goldenApple = QPoint(-1, -1);
    stoneImage = QPixmap(":/image/stone.png");
    poisonImage = QPixmap(":/image/poison.png");
    foodImage= QPixmap(":/image/apple.png");
    backgroundImage = QPixmap(":/image/desertBG.jpg");
    snakeImage =  QPixmap(":/image/snake.png");

    // Initialize the golden apple timer
    goldenAppleTimer = new QTimer(this);
    connect(goldenAppleTimer, &QTimer::timeout, this, &SnakeGame::hideGoldenApple);
    goldenAppleImage = QPixmap(":/image/goldenApple.png");

    // Load and initialize the silver apple image and timer
    silverAppleImage = QPixmap(":/image/silverApple.png");
    hasSilverApple = false;
    silverApple = QPoint(-1, -1);
    silverAppleTimer = new QTimer(this);
    connect(silverAppleTimer, &QTimer::timeout, this, &SnakeGame::hideSilverApple);

    // Create button layout
    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(goBackButton);
    buttonLayout->addStretch();
    buttonLayout->addWidget(restartButton);
    buttonLayout->addWidget(pauseButton);
    buttonLayout->addWidget(resumeButton);
    buttonLayout->addWidget(getHelpButton);

    // Create main layout
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addLayout(buttonLayout);
    mainLayout->addStretch();

    // Set main layout
    setLayout(mainLayout);

    timer = nullptr;
    isPaused = false;
    startGame();
}

/**
 * @brief Handles changes in music state.
 *
 * This function is called when the media state changes. If the end of media is reached,
 * it restarts the music.
 *
 * @param status The new media status.
 */
void SnakeGame::musicStateChanged(QMediaPlayer::MediaStatus status) {
    if (status == QMediaPlayer::EndOfMedia) {
        snakegamesound->setPosition(0); // Restart the music
        snakegamesound->play();
    }
}

/**
 * @brief Initializes the game by setting up the initial state of the snake.
 *
 * This function clears the snake's segments and sets its initial position at the center of the grid.
 * It also generates a random initial direction for the snake and adds segments accordingly.
 */
void SnakeGame::initGame() {
    snake.clear(); // Clear the snake's segments

    // Calculate the center of the entire grid
    int centerX = width() / (2 * gridSize);
    int centerY = (height() - 50) / (2 * gridSize);

    // Set the initial position of the snake at the center
    snake.append(QPoint(centerX, centerY));

    // Generate a random initial direction for the snake
    QRandomGenerator generator(QRandomGenerator::global()->generate()); // Initialize random number generator
    int initialDirection = generator.bounded(0, 4); // 0: UP, 1: DOWN, 2: LEFT, 3: RIGHT

    // Set the initial direction and add snake segments accordingly
    switch (initialDirection) {
    case 0:
        direction = UP;
        snake.append(QPoint(centerX, centerY + 1)); // Move up
        snake.append(QPoint(centerX, centerY + 2)); // Move up
        break;
    case 1:
        direction = DOWN;
        snake.append(QPoint(centerX, centerY - 1)); // Move down
        snake.append(QPoint(centerX, centerY - 2)); // Move down
        break;
    case 2:
        direction = LEFT;
        snake.append(QPoint(centerX + 1, centerY)); // Move left
        snake.append(QPoint(centerX + 2, centerY)); // Move left
        break;
    case 3:
        direction = RIGHT;
        snake.append(QPoint(centerX - 1, centerY)); // Move right
        snake.append(QPoint(centerX - 2, centerY)); // Move right
        break;
    default:
        direction = UP;
        snake.append(QPoint(centerX, centerY + 1)); // Default to moving up
        snake.append(QPoint(centerX, centerY + 2)); // Default to moving up
        break;
    }
}

/**
 * @brief Starts the game by resetting variables and showing the difficulty selection dialog.
 */
void SnakeGame::startGame() {
    // Reset game state variables
    score = 0;
    hasProtection = false;
    hasGoldenApple = false;
    hasSilverApple = false;
    snake.clear();
    foods.clear();
    poisons.clear();
    obstacles.clear();

    if (!instructionsShown) {
        showInstructions();
        instructionsShown = true;
    }

    // Create and configure the dialog for selecting difficulty
    QDialog dialog;
    dialog.setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint);
    QVBoxLayout layout(&dialog);
    QString difficulty;

    // Add label and buttons for each difficulty level
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
        button->setVolume(1.0f); // Set volume from 0.0 to 1.0
        button->play();
        difficulty = "Easy";
        dialog.accept();
    });
    layout.addWidget(easyButton);

    QPushButton *mediumButton = new QPushButton("Medium", &dialog);
    mediumButton->setStyleSheet("QPushButton { background-color: white; border: 2px solid gray; padding: 10px; }"
                                "QPushButton:hover { background-color: gold; }"
                                "QPushButton:pressed { background-color: darkgray; }");
    mediumButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    connect(mediumButton, &QPushButton::clicked, [&dialog, &difficulty](){
        QSoundEffect *button = new QSoundEffect;
        button->setSource(QUrl("qrc:/sound/m_pushbutton.wav"));
        button->setVolume(1.0f); // Set volume from 0.0 to 1.0
        button->play();
        difficulty = "Medium";
        dialog.accept();
    });
    layout.addWidget(mediumButton);

    QPushButton *hardButton = new QPushButton("Hard", &dialog);
    hardButton->setStyleSheet("QPushButton { background-color: white; border: 2px solid gray; padding: 10px; }"
                              "QPushButton:hover { background-color: gold; }"
                              "QPushButton:pressed { background-color: darkgray; }");
    hardButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    connect(hardButton, &QPushButton::clicked, [&dialog, &difficulty](){
        QSoundEffect *button = new QSoundEffect;
        button->setSource(QUrl("qrc:/sound/m_pushbutton.wav"));
        button->setVolume(1.0f); // Set volume from 0.0 to 1.0
        button->play();
        difficulty = "Hard";
        dialog.accept();
    });
    layout.addWidget(hardButton);

    // Set up layout and handle dialog result
    dialog.setLayout(&layout);

    if (dialog.exec() == QDialog::Rejected) {
        // User closed the dialog or clicked outside, go back to the main menu
        emit goBackToMainMenu();
        return;
    }

    // Set game parameters based on selected difficulty
    int speed = 0;
    int numPoisons = 0;
    if (difficulty == "Easy") {
        gridSize = 30;
        numFoods = 10;
        numPoisons = 2;
        speed = 260;
        currentDifficulty = "Easy";
        backgroundImage = QPixmap(":/image/grass1.png");
    } else if (difficulty == "Medium") {
        gridSize = 25;
        numFoods = 10;
        numPoisons = 3;
        speed = 220;
        currentDifficulty = "Medium";
        backgroundImage = QPixmap(":/image/sky.png");
    } else if (difficulty == "Hard") {
        gridSize = 20;
        numFoods = 10;
        numPoisons = 4;
        speed = 180;
        currentDifficulty = "Hard";
        backgroundImage = QPixmap(":/image/pinkBG.jpeg");
    }

    // Load the highest score for the chosen difficulty
    score = 0; // Reset the score for the new game
    highestScore = highestScores[currentDifficulty];

    initGame();
    generateFood();
    generateObstacles(difficulty);
    generatePoisons(numPoisons);
    showGoldenApple();
    showSilverApple();

    if (timer) {
        timer->stop();
        delete timer;
    }
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateGame()));
    timer->start(speed);

    currentFontSize = 20; // Initial font size
    targetFontSize = currentFontSize;

    scoreAnimationTimer = new QTimer(this);
    connect(scoreAnimationTimer, &QTimer::timeout, this, &SnakeGame::updateScoreFontSize);

    // Initialize the protection timer
    protectionTimer = new QTimer(this);
    connect(protectionTimer, &QTimer::timeout, this, &SnakeGame::handleProtectionTimeout);
    protectionTimer->setSingleShot(true);
}

/**
 * @brief Pauses the game.
 *
 * Plays a button sound effect and pauses the game by stopping all relevant timers.
 * Sets the isPaused flag to true.
 */
void SnakeGame::pauseGame() {
    // Play button sound effect
    QSoundEffect *button = new QSoundEffect;
    button->setSource(QUrl("qrc:/sound/m_pushbutton.wav"));
    button->setVolume(1.0f);
    button->play();

    // Pause game components if not already paused
    if (!isPaused) {
        if (timer) {
            timer->stop();
        }
        if (scoreAnimationTimer) {
            scoreAnimationTimer->stop();
        }
        if (protectionTimer) {
            protectionTimer->stop();
        }
        if (silverAppleTimer) {
            silverAppleTimer->stop();
        }
        if (goldenAppleTimer) {
            goldenAppleTimer->stop();
        }
        isPaused = true;
    }
}

/**
 * @brief Resumes the paused game.
 *
 * Plays a button sound effect and resumes the game by starting all relevant timers.
 * Sets the isPaused flag to false.
 */
void SnakeGame::resumeGame() {
    // Play button sound effect
    QSoundEffect *button = new QSoundEffect;
    button->setSource(QUrl("qrc:/sound/m_pushbutton.wav"));
    button->setVolume(1.0f);
    button->play();

    // Resume game components if paused
    if (isPaused) {
        if (timer) {
            timer->start();
        }
        if (scoreAnimationTimer) {
            scoreAnimationTimer->start();
        }
        if (protectionTimer) {
            protectionTimer->start();
        }
        if (silverAppleTimer) {
            silverAppleTimer->start();
        }
        if (goldenAppleTimer) {
            goldenAppleTimer->start();
        }
        isPaused = false;
    }
}

/**
 * @brief Restarts the game.
 *
 * This function restarts the game by resetting game variables and components.
 * It ensures all timers are stopped before restarting and plays a sound effect
 * to indicate the restart action.
 */
void SnakeGame::restartGame() {
    // Ensure all timers are stopped before restarting
    if (timer) {
        timer->stop();
        delete timer;
        timer = new QTimer(this);
        connect(timer, &QTimer::timeout, this, &SnakeGame::updateGame);
    }
    if (goldenAppleTimer) {
        goldenAppleTimer->stop();
    }
    if (silverAppleTimer) {
        silverAppleTimer->stop();
    }
    if (protectionTimer) {
        protectionTimer->stop();
    }
    if (scoreAnimationTimer) {
        scoreAnimationTimer->stop();
    }

    // Play button sound effect
    QSoundEffect *button = new QSoundEffect;
    button->setSource(QUrl("qrc:/sound/m_pushbutton.wav"));
    button->setVolume(1.0f);
    button->play();

    // Reset game state variables
    isGameOver = false;
    score = 0;
    hasProtection = false;

    // Delete existing timer if any
    if (timer) {
        timer->stop();
        delete timer;
        timer = nullptr;
    }

    // Reset golden apple state
    hasGoldenApple = false;
    goldenAppleTimer->stop();
    goldenApple = QPoint(-1, -1);

    // Reset silver apple state
    hasSilverApple = false;
    silverAppleTimer->stop();
    silverApple = QPoint(-1, -1);

    // Clear game elements and start a new game
    snake.clear();
    foods.clear();
    poisons.clear();
    startGame();
    update();
}

/**
 * @brief Hides the golden apple.
 *
 * This function hides the golden apple by adding it back to the list of foods,
 * resetting its position and stopping the golden apple timer. It then triggers
 * a repaint of the game window.
 */
void SnakeGame::hideGoldenApple() {
    if (hasGoldenApple) {
        // Add golden apple back to foods list
        foods.append(goldenApple);
        // Reset golden apple state
        hasGoldenApple = false;
        goldenApple = QPoint(-1, -1);
        // Stop golden apple timer
        goldenAppleTimer->stop();
        // Trigger repaint of game window
        update();
    }
}

/**
 * @brief Paints the game window.
 *
 * This function handles the painting of various game elements such as background,
 * score display, snake, foods, obstacles, and special items like golden apple and silver apple.
 * It overrides the QWidget's paintEvent.
 *
 * @param event The paint event.
 */
void SnakeGame::paintEvent(QPaintEvent *) {
    QPainter painter(this);
    QFont font;

    font.setBold(true);
    font.setPixelSize(currentFontSize); // Use current font size
    painter.setFont(font);
    painter.drawPixmap(0, 0, width(), height(), backgroundImage); // Draw background image
    QColor overlayColor(0, 0, 0, 64);
    painter.fillRect(0, 0, width(), 50, overlayColor); // Draw overlay for score display

    // Draw score
    QLinearGradient gradient(0, 0, width(), height());
    gradient.setColorAt(0, QColor(255, 215, 0));
    gradient.setColorAt(1, Qt::yellow);
    painter.setBrush(QBrush(gradient));
    painter.setPen(QColor(255, 215, 0));
    painter.drawText(285, 25, " " + QString::number(score));

    // Draw highest score for the current difficulty
    QFont smallerFont = font;
    smallerFont.setPixelSize(currentFontSize - 6);
    painter.setFont(smallerFont);
    painter.drawText(265, 40, "Highest: " + QString::number(highestScores[currentDifficulty]));


    // Draw the snake
    QPixmap snakeHead = hasProtection ? QPixmap(":/image/withProtectionHead2.png") : QPixmap(":/image/snakeBodyBigger1.png");
    QPixmap snakeTail = hasProtection ? QPixmap(":/image/withProtectionTail2.png") : QPixmap(":/image/snakeBodySmaller1.png");
    for (int i = 0; i < snake.size(); i++) {
        if (i == 0) {
            painter.drawPixmap(snake.at(i).x() * gridSize, snake.at(i).y() * gridSize + 50, gridSize, gridSize, snakeHead);
        }
        else if (i == snake.size() - 1) {
            painter.drawPixmap(snake.at(i).x() * gridSize, snake.at(i).y() * gridSize + 50, gridSize, gridSize, snakeTail);
        }
        else {
            painter.drawPixmap(snake.at(i).x() * gridSize, snake.at(i).y() * gridSize + 50, gridSize, gridSize, snakeTail);
        }
    }

    // Draw foods
    for (int i = 0; i < foods.size(); i++) {
        painter.drawPixmap(foods.at(i).x() * gridSize, foods.at(i).y() * gridSize + 50, gridSize, gridSize, foodImage);
    }

    // Draw obstacles
    for (const auto& obstacle : obstacles) {
        painter.drawPixmap(obstacle.x() * gridSize, obstacle.y() * gridSize + 50, gridSize, gridSize, stoneImage);
    }

    // Draw poisons
    for (int i = 0; i < poisons.size(); i++) {
        painter.drawPixmap(poisons.at(i).x() * gridSize, poisons.at(i).y() * gridSize + 50, gridSize, gridSize, poisonImage);
    }

    // Draw golden apple if present
    if (hasGoldenApple) {
        painter.drawPixmap(goldenApple.x() * gridSize, goldenApple.y() * gridSize + 50, gridSize, gridSize, goldenAppleImage);
    }

    // Draw silver apple if present
    if (hasSilverApple) {
        painter.drawPixmap(silverApple.x() * gridSize, silverApple.y() * gridSize + 50, gridSize, gridSize, silverAppleImage);
    }
}

/**
 * @brief Handles key press events.
 *
 * This function handles key press events for controlling the snake movement
 * and pausing/resuming the game. It overrides the QWidget's keyPressEvent.
 *
 * @param event The key event.
 */
void SnakeGame::keyPressEvent(QKeyEvent *event) {
    // Pause or resume game on spacebar press
    if (event->key() == Qt::Key_Space) {
        if (isPaused) {
            resumeGame();
        } else {
            pauseGame();
        }
        return;
    }

    // Handle snake movement keys
    switch (event->key()) {
    case Qt::Key_Up:
    case Qt::Key_W:
        if (direction != DOWN)
            direction = UP;
        break;
    case Qt::Key_Down:
    case Qt::Key_S:
        if (direction != UP)
            direction = DOWN;
        break;
    case Qt::Key_Left:
    case Qt::Key_A:
        if (direction != RIGHT)
            direction = LEFT;
        break;
    case Qt::Key_Right:
    case Qt::Key_D:
        if (direction != LEFT)
            direction = RIGHT;
        break;
    default:
        QWidget::keyPressEvent(event);
    }
}

/**
 * @brief Updates the font size of the score display.
 *
 * This function gradually increases or decreases the font size of the score display
 * to achieve an animation effect. It is called periodically by a timer.
 */
void SnakeGame::updateScoreFontSize() {
    int step = 2;
    if (currentFontSize < targetFontSize) {
        currentFontSize += step;
        if (currentFontSize >= targetFontSize) {
            currentFontSize = targetFontSize;
            targetFontSize -= 15; // Decrease target font size for next animation step
            scoreAnimationTimer->start(15); // Start next animation step
        }
    } else if (currentFontSize > targetFontSize) {
        currentFontSize -= step;
        if (currentFontSize <= targetFontSize) {
            currentFontSize = targetFontSize;
            scoreAnimationTimer->stop(); // Stop animation if reached target size
        }
    }
    update(); // Trigger repaint of game window
}

/**
 * @brief Displays a special effect at a given position.
 *
 * This function displays a special effect at a specified position on the game window.
 * The effect can be a simple "+1" text or various emojis based on the value of 'n'.
 *
 * @param position The position where the effect is displayed.
 * @param n The type of effect to display:
 *              - 1 for "+1"
 *              - 2 for "😫💨"
 *              - 3 for "WOW"
 *              - Others for "🛡️"
 */
void SnakeGame::showPlusOneEffect(const QPoint& position, int n) {
    QLabel *label = new QLabel(this);
    if (n == 1) {
        label->setText(" +1");
    } else if (n == 2){
        label->setText("😫💨");
    } else if (n == 3) {
        label->setText("WOW");
    } else {
        label->setText("🛡️");
    }
    QFont font("Arial", 14, QFont::Bold);
    label->setFont(font);
    label->move(position.x() * gridSize, position.y() * gridSize + 50);
    QColor randomColor = QColor::fromRgb(QRandomGenerator::global()->bounded(256),
                                         QRandomGenerator::global()->bounded(256),
                                         QRandomGenerator::global()->bounded(256));
    label->setStyleSheet("QLabel { color: " + randomColor.name() + "; }");
    label->show();

    // Animation for moving the label upwards and fading it out
    QPropertyAnimation *positionAnimation = new QPropertyAnimation(label, "pos", this);
    positionAnimation->setDuration(1000); // 1 second duration
    positionAnimation->setStartValue(label->pos());
    positionAnimation->setEndValue(label->pos() - QPoint(0, 50)); // Move the label up by 50 pixels
    positionAnimation->setEasingCurve(QEasingCurve::InOutQuad);

    QPropertyAnimation *opacityAnimation = new QPropertyAnimation(label, "windowOpacity", this);
    opacityAnimation->setDuration(1000); // 1 second duration
    opacityAnimation->setStartValue(1.0);
    opacityAnimation->setEndValue(0.0);
    opacityAnimation->setEasingCurve(QEasingCurve::InOutQuad);

    // Connect animation finished signal to delete the label and animations
    connect(opacityAnimation, &QPropertyAnimation::finished, [=]() {
        label->deleteLater();
        positionAnimation->deleteLater();
        opacityAnimation->deleteLater();
    });

    // Start animations
    positionAnimation->start();
    opacityAnimation->start();
}

/**
 * @brief Updates the state of the game based on player input and game rules.
 *
 * This function is called periodically by a timer to update the game state, including
 * moving the snake, handling collisions, and updating the score. It also checks for
 * win or lose conditions and performs appropriate actions.
 */
void SnakeGame::updateGame() {
    const int maxSpeed = 120; // Maximum speed limit for the game

    if (isGameOver || isPaused) return;

    QPoint newHead = snake.first(); // Get the position of the snake's head

    // Move the snake's head according to the current direction
    switch (direction) {
    case UP:
        newHead.ry()--;
        break;
    case DOWN:
        newHead.ry()++;
        break;
    case LEFT:
        newHead.rx()--;
        break;
    case RIGHT:
        newHead.rx()++;
        break;
    }

    int winningSize = 0;
    // Determine the winning size based on the current game difficulty
    if (currentDifficulty == "Easy") {
        winningSize = 300;
    } else if (currentDifficulty == "Medium") {
        winningSize = 400;
    } else if (currentDifficulty == "Hard") {
        winningSize = 500;
    }

    // Check if the snake has reached the winning size
    if (snake.size() >= winningSize) {
        timer->stop();
        handleWin();
        return;
    }

    // Check collision with the borders of the game window
    if (newHead.x() < 0 || newHead.x() >= width() / gridSize || newHead.y() < 0 || newHead.y() >= (height() - 50) / gridSize) {
        QSoundEffect *hitwall = new QSoundEffect;
        hitwall->setSource(QUrl("qrc:/sound/m_hitwall.wav"));
        hitwall->setVolume(1.0f);
        hitwall->play();
        timer->stop();
        gameOver();
        return;
    }

    // Check collision with poisons
    for (const auto& poison : poisons) {
        if (newHead == poison) {
            QSoundEffect *poisonsick = new QSoundEffect;
            poisonsick->setSource(QUrl("qrc:/sound/m_poisonsick.wav"));
            poisonsick->setVolume(1.0f);
            poisonsick->play();
            if (snake.size() > 2) {
                snake.removeLast();
                snake.removeLast();
                showPlusOneEffect(newHead, 2);
            } else {
                timer->stop();
                gameOver();
            }
            poisons.removeOne(poison); // Remove the poison
            generatePoisons(1);
            if (snake.isEmpty()) {
                timer->stop();
                gameOver();
                return;
            }
            break;
        }
    }

    // Check collision with obstacles
    bool obstacleHit = false;
    int obstacleHitGrid = -1; // Store the grid position of the obstacle that was hit
    for (int i = 0; i < obstacles.size(); ++i) {
        if (newHead == obstacles[i]) {
            obstacleHit = true;
            obstacleHitGrid = i;
            break;
        }
    }

    if (obstacleHit) {
        if (hasProtection) {
            showPlusOneEffect(newHead, 4);
            QSoundEffect *obstacleHitWithPro = new QSoundEffect;
            obstacleHitWithPro->setSource(QUrl("qrc:/sound/m_obstacleHitWithPro.wav"));
            obstacleHitWithPro->setVolume(1.0f);
            obstacleHitWithPro->play();
            hasProtection = false;
            protectionTimer->stop();
            obstacles.erase(obstacles.begin() + obstacleHitGrid);

        } else {
            QSoundEffect *hitwall = new QSoundEffect;
            hitwall->setSource(QUrl("qrc:/sound/m_hitwall.wav"));
            hitwall->setVolume(1.0f);
            hitwall->play();
            timer->stop();
            gameOver();
            return;
        }
    }

    // Check collision with the snake's own body
    for (int i = 1; i < snake.size(); i++) {
        if (newHead == snake.at(i)) {
            timer->stop();
            gameOver();
            return;
        }
    }

    // Check collision with the golden apple
    if (newHead == goldenApple) {
        QSoundEffect *gold = new QSoundEffect;
        gold->setSource(QUrl("qrc:/sound/m_golden.wav"));
        gold->setVolume(1.0f);
        gold->play();
        int oldSize = snake.size();
        for (int i = 0; i < oldSize; ++i) { // Double the snake's size
            snake.append(snake.last());
        }
        score += snake.size() / 2; // Score increments by the length of the snake
        targetFontSize += 15;
        scoreAnimationTimer->start(15);
        showPlusOneEffect(newHead, 3);
        updateScoreFontSize();
        hasGoldenApple = false;
        goldenAppleTimer->stop();
        hideGoldenApple();
    }

    // Check collision with the silver apple
    if (newHead == silverApple) {
        QSoundEffect *sil = new QSoundEffect;
        sil->setSource(QUrl("qrc:/sound/m_silver.wav"));
        sil->setVolume(1.0f);
        sil->play();
        score++;
        targetFontSize += 15;
        scoreAnimationTimer->start(15);
        showPlusOneEffect(newHead, 1);
        updateScoreFontSize();
        hasProtection = true;
        protectionTimer->start(10000);
        hasSilverApple = false;
        silverAppleTimer->stop();
        hideSilverApple();
    }

    // Check collision with food
    bool ateFood = false;
    for (int i = 0; i < foods.size(); i++) {
        if (newHead == foods.at(i)) {
            QSoundEffect *red = new QSoundEffect;
            red->setSource(QUrl("qrc:/sound/m_redapple.wav"));
            red->setVolume(1.0f);
            red->play();
            foods.removeAt(i);
            ateFood = true;
            generateFood();
            score++;

            targetFontSize = 20;
            targetFontSize += 15;
            scoreAnimationTimer->start(15);
            showPlusOneEffect(newHead, 1);
            updateScoreFontSize();

            // Decrease the timer interval to increase speed, respecting the maximum speed limit
            int currentSpeed = timer->interval();
            if (currentSpeed > maxSpeed) {
                currentSpeed -= 5; // Reduce the interval by 5 milliseconds to increase speed
                currentSpeed = std::max(currentSpeed, maxSpeed); // Ensure speed does not exceed max
                timer->setInterval(currentSpeed);
            }
            else{
                timer->setInterval(maxSpeed);
            }
            showSilverApple();
            break;
        }
    }

    // Update the score and check for new highest score
    if (score > highestScores[currentDifficulty]) {
        highestScores[currentDifficulty] = score;

        QSettings settings;
        settings.setValue("HighestScore" + currentDifficulty, highestScores[currentDifficulty]);
        settings.sync();
    }

    // Update the snake's position
    snake.prepend(newHead);
    if (!ateFood) {
        snake.removeLast();
    }

    update(); // Trigger repaint of the game window
}

/**
 * @brief Generates food items on the game grid.
 *
 * This function generates a specified number of food items randomly on the grid, ensuring they are not too close to the snake,
 * and do not overlap with existing food items or obstacles.
 */
void SnakeGame::generateFood() {
    QRandomGenerator generator = QRandomGenerator::securelySeeded(); // Initialize random number generator
    int numFoodsToGenerate = numFoods - foods.size(); // Calculate the number of foods to generate to reach the desired count
    for (int i = 0; i < numFoodsToGenerate; i++) {
        int x, y;
        bool validPosition;
        do {
            validPosition = true;
            x = generator.bounded(width() / gridSize); // Generate random x-coordinate within the grid
            y = generator.bounded((height() - 50) / gridSize); // Generate random y-coordinate within the grid

            // Check if the food is at least 5 grids away from the snake
            for (const auto& segment : snake) {
                if (abs(segment.x() - x) < 5 && abs(segment.y() - y) < 5) {
                    validPosition = false;
                    break;
                }
            }

            // Check if the food is on any existing food or obstacle
            for (const auto& existingFood : foods) {
                if (existingFood.x() == x && existingFood.y() == y) {
                    validPosition = false;
                    break;
                }
            }
            for (const auto& obstacle : obstacles) {
                if (obstacle.x() == x && obstacle.y() == y) {
                    validPosition = false;
                    break;
                }
            }
        } while (!validPosition);
        foods.append(QPoint(x, y)); // Add the generated food position to the list
    }

    // Generate a golden apple with a 10% chance
    if (generator.bounded(100) < 10) { // 10% chance
        showGoldenApple(); // Function to place a golden apple
    }
}

/**
 * @brief Generates obstacles on the game grid based on the chosen difficulty level.
 *
 * This function clears the existing obstacles and generates new ones according to the specified difficulty level.
 *
 * @param difficulty The chosen difficulty level ("Medium" or "Hard").
 */
void SnakeGame::generateObstacles(const QString& difficulty) {
    obstacles.clear(); // Clear existing obstacles

    int numObstacles = 0;
    if (difficulty == "Medium") {
        numObstacles = 3;
    } else if (difficulty == "Hard") {
        numObstacles = 6;
    }

    // Calculate the center of the grid
    int centerX = width() / (2 * gridSize);
    int centerY = (height() - 50) / (2 * gridSize);

    for (int i = 0; i < numObstacles; ++i) {
        int length = QRandomGenerator::global()->bounded(2, 6); // Generate a random length for the obstacle
        int x, y;

        // Generate a starting position that is not within a 10x10 grid space around the center
        do {
            x = QRandomGenerator::global()->bounded(0, width() / gridSize - length);
            y = QRandomGenerator::global()->bounded(0, (height() - 50) / gridSize);
        } while (qAbs(x - centerX) <= 5 && qAbs(y - centerY) <= 5);

        bool isVertical = QRandomGenerator::global()->bounded(2) == 0; // Randomly choose whether the obstacle is vertical or horizontal

        // Add the obstacle segments to the obstacles list
        for (int j = 0; j < length; ++j) {
            if (isVertical) {
                obstacles.append(QPoint(x, y + j));
            } else {
                obstacles.append(QPoint(x + j, y));
            }
        }
    }
}

/**
 * @brief Generates poison items on the game grid.
 *
 * This function generates poison items at random positions on the game grid, ensuring that they do not overlap with existing food items, obstacles, snake segments, or other poison items.
 *
 * @param numPoisons The number of poison items to generate.
 */
void SnakeGame::generatePoisons(int numPoisons) {
    for (int i = 0; i < numPoisons; ++i) {
        int x, y;
        bool validPosition;
        do {
            validPosition = true;
            x = QRandomGenerator::global()->bounded(0, width() / gridSize);
            y = QRandomGenerator::global()->bounded(0, (height() - 50) / gridSize);

            // Check if the generated position overlaps with existing game elements
            for (const auto& existingFood : foods) {
                if (existingFood.x() == x && existingFood.y() == y) {
                    validPosition = false;
                    break;
                }
            }
            for (const auto& obstacle : obstacles) {
                if (obstacle.x() == x && obstacle.y() == y) {
                    validPosition = false;
                    break;
                }
            }
            for (const auto& segment : snake) {
                if (segment.x() == x && segment.y() == y) {
                    validPosition = false;
                    break;
                }
            }
            for (const auto& poison : poisons) {
                if (poison.x() == x && poison.y() == y) {
                    validPosition = false;
                    break;
                }
            }
        } while (!validPosition);

        // Add the poison item to the list of poisons
        poisons.append(QPoint(x, y));
    }
}

/**
 * @brief Displays the golden apple on the game grid.
 *
 * This function determines valid positions around obstacles to place the golden apple. If there are valid positions available and no golden apple is currently present, it chooses a random valid position for the golden apple and starts a timer to hide it after 10 seconds.
 */
void SnakeGame::showGoldenApple() {
    // Check if there are obstacles or if a golden apple is already present
    if (obstacles.isEmpty() || hasGoldenApple) return;

    // Find potential positions around obstacles
    QVector<QPoint> potentialPositions;
    for (const QPoint& obstacle : obstacles) {
        QVector<QPoint> positionsAroundObstacle = {
            obstacle + QPoint(-1, 0), // left
            obstacle + QPoint(1, 0),  // right
            obstacle + QPoint(0, -1), // up
            obstacle + QPoint(0, 1)   // down
        };

        // Check if positions around the obstacle are valid
        for (const QPoint& pos : positionsAroundObstacle) {
            if (isValidPosition(pos)) {
                potentialPositions.push_back(pos);
            }
        }
    }

    // If there are no valid positions, don't show the golden apple
    if (potentialPositions.isEmpty()) return;

    // Choose a random valid position for the golden apple
    int positionIndex = QRandomGenerator::global()->bounded(potentialPositions.size());
    goldenApple = potentialPositions[positionIndex];
    hasGoldenApple = true;

    // Start the timer to hide the golden apple after 10 seconds
    goldenAppleTimer->start(10000);

    // Trigger repaint to display the golden apple
    update();
}

void SnakeGame::showSilverApple() {
    // Only show silver apple in Medium and Hard difficulty
    if (currentDifficulty == "Easy") {
        return; // Do not spawn silver apple in Easy mode
    }

    if (QRandomGenerator::global()->bounded(100) < 30) {
        QPoint potentialPosition;
        bool validPositionFound = false;
        int attempts = 0;

        while (!validPositionFound && attempts < 100) {
            int x = QRandomGenerator::global()->bounded(width() / gridSize);
            int y = QRandomGenerator::global()->bounded((height() - 50) / gridSize);
            potentialPosition = QPoint(x, y);

            // Check if the position is valid (not colliding with existing items)
            if (isValidPosition(potentialPosition) &&
                (!hasGoldenApple || potentialPosition != goldenApple)) {
                validPositionFound = true;
            }
            attempts++;
        }

        if (validPositionFound) {
            silverApple = potentialPosition;
            hasSilverApple = true;

            // Start or restart the timer for silver apple's visibility duration
            if (!silverAppleTimer) {
                silverAppleTimer = new QTimer(this);
                connect(silverAppleTimer, &QTimer::timeout, this, &SnakeGame::hideSilverApple);
            }
            silverAppleTimer->start(15000); // Silver apple disappears after 15 seconds if not eaten
        }
    }
}

/**
 * @brief Checks if a given position is valid for placing game elements.
 *
 * This function checks whether the given position is within the game bounds and at least 10 grids away from the snake. Additionally, it ensures that the position does not collide with existing foods, poisons, obstacles, golden apple, or silver apple.
 *
 * @param pos The position to be checked.
 * @return true if the position is valid, false otherwise.
 */
bool SnakeGame::isValidPosition(const QPoint &pos) {
    // Check bounds
    if (pos.x() < 0 || pos.x() >= width() / gridSize || pos.y() < 0 || pos.y() >= (height() - 50) / gridSize) {
        return false;
    }

    // Check if the position is at least 10 grids away from the snake
    for (const auto& segment : snake) {
        if (abs(segment.x() - pos.x()) < 10 && abs(segment.y() - pos.y()) < 10) {
            return false;
        }
    }

    // Check collision with foods, poisons, obstacles, golden apple, and silver apple
    if (std::find(foods.begin(), foods.end(), pos) != foods.end() ||
        std::find(poisons.begin(), poisons.end(), pos) != poisons.end() ||
        std::find(obstacles.begin(), obstacles.end(), pos) != obstacles.end() ||
        (hasGoldenApple && goldenApple == pos) || (hasSilverApple && silverApple == pos)) {
        return false;
    }
    return true;
}

/**
 * @brief Hides the silver apple from the game view.
 *
 * This function sets the `hasSilverApple` flag to false, indicating that the silver apple is no longer present in the game. It then triggers a refresh of the game view to reflect the change.
 */
void SnakeGame::hideSilverApple() {
    hasSilverApple = false;
    update(); // Refresh the game view
}

/**
 * @brief Handles the "Go Back" button click event.
 *
 * This function is called when the "Go Back" button is clicked. It plays a sound effect, stops the background game music, emits the signal to go back to the main menu, and deletes the current instance of SnakeGame.
 */
void SnakeGame::onGoBackClicked() {
    // Play button click sound effect
    QSoundEffect *button = new QSoundEffect;
    button->setSource(QUrl("qrc:/sound/m_pushbutton.wav"));
    button->setVolume(1.0f);
    button->play();

    // Stop the background game music
    snakegamesound->stop();

    // Emit signal to go back to the main menu
    qDebug() << "Go Back clicked, emitting goBackToMainMenu";
    emit goBackToMainMenu();

    // Delete the current instance of SnakeGame
    this->deleteLater();
}

/**
 * @brief Displays the game instructions in a dialog window.
 *
 * This function creates a QDialog window to display the game instructions using QLabel for rich text formatting.
 * It includes information on controls, apples, obstacles, poisons, silver apples, and golden apples.
 */
void SnakeGame::showInstructions() {
    pauseGame();
    QDialog dialog(this);
    dialog.setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint);
    dialog.setWindowTitle("Game Instructions");
    //dialog.setFixedSize(650, 400); // Set the fixed size here

    // Use a layout to organize widgets vertically
    QVBoxLayout layout(&dialog);


    // Use a QLabel for rich text, including HTML content
    QLabel label(&dialog);
    label.setTextFormat(Qt::RichText); // Set the label to use Rich Text
    label.setText(
        "<h2>Welcome to the Snake Game!</h2>"
        "<p><b>Control:</b> Use arrow keys or WASD</p >"
        "<p><b>Eat Apples:</b> < img src=':/image/apple.png' width='25' height='22'> Eat to grow and earn one point</p >"
        "<p><b>Obstacles:</b> < img src=':/image/stone.png' width='20' height='20'> Avoid unless protected</p >"
        "<p><b>Poison:</b> < img src=':/image/poison.png' width='25' height='25'> Shrinks by two segments without changing points</p >"
        "<p><b>Silver Apple:</b> < img src=':/image/silverApple.png' width='20' height='20'> Appears randomly for a 10-second shield. Note: Shield breaks after one collision.</p >"
        "<p><b>Golden Apple:</b> < img src=':/image/goldenApple.png' width='18' height='18'> A fleeting chance to double your size! It appears for 5 seconds, then reverts to a normal apple < img src=':/image/apple.png' width='25' height='22'>. Awards points for the added segments.</p >"
        "<p><b>Winning Conditions:</b>🏆 Reach 300 points for Easy mode, 400 for Medium, and 500 for Hard to win the game.</p >"
        "<p>Good luck and have fun!</p >"
        );

    // Style the label
    label.setStyleSheet("QLabel {"
                        "font-family: 'Arial', sans-serif;"
                        "margin-bottom: 6px;"
                        "}");

    // Adjust the label's properties as needed
    label.setWordWrap(true);
    label.setAlignment(Qt::AlignLeft | Qt::AlignTop);

    layout.addWidget(&label); // Add the label to the layout

    // Add an OK button to close the dialog
    QPushButton okButton("Got it!", &dialog);
    okButton.setStyleSheet("QPushButton {"
                           "color: white;"
                           "background-color: #5cb85c;"
                           "border-style: none;"
                           "padding: 10px;"
                           "font-weight: bold;"
                           "font-size: 14px;"
                           "border-radius: 5px;"
                           "}"
                           "QPushButton:hover {"
                           "background-color: #4cae4c;"
                           "}");

    connect(&okButton, &QPushButton::clicked, &dialog, &QDialog::accept);
    layout.addWidget(&okButton); // Add the button to the layout

    dialog.setLayout(&layout); // Set the dialog's layout
    if (dialog.exec() == QDialog::Rejected) {
        // If the dialog was closed using the X button, emit the signal to go back
        emit goBackToMainMenu();
        return; // Ensure the method exits here without proceeding further
        //this->deleteLater();
    }
    resumeGame();

}

/**
 * @brief Handles the expiration of the protection timer.
 *
 * This function is called when the protection timer expires. It removes the protection effect from the snake
 * and refreshes the game view to reflect the change.
 */
void SnakeGame::handleProtectionTimeout() {
    // Protection timer expired, remove the protection
    hasProtection = false;
    update(); // Refresh the game view
}

/**
 * @brief Handles the win condition when the player reaches the winning size.
 *
 * This function is called when the player wins the game by reaching the winning size. It displays a congratulatory
 * message box with the option to retry or go back to the main menu. It also updates the highest score if applicable.
 */
void SnakeGame::handleWin() {
    QTimer::singleShot(250, [=]() {
        QSoundEffect *effect = new QSoundEffect;
        effect->setSource(QUrl("qrc:/sound/winning.wav"));
        effect->setVolume(1.0f);
        effect->play();

        timer->stop();

        QMessageBox msgBox;
        msgBox.setText("CONGRATULATIONS! YOU WON!");

        QPushButton *retryButton = msgBox.addButton(tr("Retry"), QMessageBox::AcceptRole);
        QPushButton *backButton = msgBox.addButton(tr("Back"), QMessageBox::RejectRole);

        // Update highest score if current score exceeds it
        int currentHighestScore = highestScores[currentDifficulty];
        if (score > currentHighestScore) {
            highestScores[currentDifficulty] = score;
            QSettings settings;
            settings.setValue("HighestScore" + currentDifficulty, score);
            settings.sync();
        }

        msgBox.setInformativeText("Your score: " + QString::number(score) +
                                  "\nHighest score for " + currentDifficulty + ": " +
                                  QString::number(highestScores[currentDifficulty]));

        QPixmap explosionPixmap(":/image/winning.png"); // Adjust the path to your explosion image
        msgBox.setIconPixmap(explosionPixmap.scaledToWidth(64)); // Scale the image to fit

        msgBox.setDefaultButton(retryButton);
        msgBox.exec();

        if (msgBox.clickedButton() == retryButton) {
            effect->stop();
            QSoundEffect *button = new QSoundEffect;
            button->setSource(QUrl("qrc:/sound/m_pushbutton.wav"));
            button->setVolume(1.0f);
            button->play();
            restartGame();
        } else if (msgBox.clickedButton() == backButton) {
            effect->stop();
            onGoBackClicked();

        }

    });
}

/**
 * @brief Handles the game over state.
 *
 * Displays a game over message box with the option to retry or go back to the main menu.
 * Updates the highest score if applicable.
 */
bool gameOverDisplayed = false;
void SnakeGame::gameOver() {
    if (gameOverDisplayed) {
        // If game over window is already displayed, return immediately
        return;
    }

    // Set the flag to true to indicate that game over window is being displayed
    gameOverDisplayed = true;

    QTimer::singleShot(250, [=]() {
        // Play game over sound effect
        QSoundEffect *effect = new QSoundEffect;
        effect->setSource(QUrl("qrc:/sound/m_gameover.wav"));
        effect->setVolume(1.0f);
        effect->play();

        // Stop the timer
        timer->stop();

        // Display game over message after a delay
        QMessageBox msgBox;
        msgBox.setText("Game Over!");

        QPushButton *retryButton = msgBox.addButton(tr("Retry"), QMessageBox::AcceptRole);
        QPushButton *backButton = msgBox.addButton(tr("Back"), QMessageBox::RejectRole);

        int currentHighestScore = highestScores[currentDifficulty];
        if (score > currentHighestScore) {
            highestScores[currentDifficulty] = score;
            QSettings settings;
            settings.setValue("HighestScore" + currentDifficulty, score);
            settings.sync();
        }

        // Show the game over message box
        msgBox.setInformativeText("Your score: " + QString::number(score) +
                                  "\nHighest score for " + currentDifficulty + ": " +
                                  QString::number(highestScores[currentDifficulty]));

        QPixmap explosionPixmap(":/image/snakegameover.png"); // Adjust the path to your explosion image
        msgBox.setIconPixmap(explosionPixmap.scaledToWidth(64)); // Scale the image to fit

        msgBox.setDefaultButton(retryButton);
        msgBox.exec();

        if (msgBox.clickedButton() == retryButton) {
            effect->stop();
            // Play button sound effect
            QSoundEffect *button = new QSoundEffect;
            button->setSource(QUrl("qrc:/sound/m_pushbutton.wav"));
            button->setVolume(1.0f);
            button->play();
            restartGame();
        } else if (msgBox.clickedButton() == backButton) {
            effect->stop();
            onGoBackClicked();
        }

        // Reset the flag after the message box is closed
        gameOverDisplayed = false;
    });
}


