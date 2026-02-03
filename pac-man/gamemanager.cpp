#include "gamemanager.h"
#include <QDebug>

GameManager::GameManager(QObject *parent)
    : QObject(parent),
    scene(nullptr),
    view(nullptr),
    world(nullptr),
    pacman(nullptr),
    scoreText(nullptr),
    debugRect(nullptr),
    gameTimer(nullptr),
    powerUpTimer(nullptr),
    powerUpActive(false),
    frameCounter(0)
{
    // Create game world
    world = new GameWorld();

    // Setup everything
    setupScene();
    setupView();
    setupPacMan();
    setupGhosts();
    // Create timers
    gameTimer = new QTimer(this);
    connect(gameTimer, &QTimer::timeout, this, &GameManager::gameLoop);

    modeTimer = new QTimer(this);
    modeTimer->setSingleShot(false);  // REPEATING
    connect(modeTimer, &QTimer::timeout, this, &GameManager::switchGhostMode);
    modeTimer->start(7000);  // 7 seconds

    qDebug() << "Mode timer started - will switch every 7 seconds";

    powerUpTimer = new QTimer(this);
    powerUpTimer->setSingleShot(true);
    connect(powerUpTimer, &QTimer::timeout, this, &GameManager::powerUpEnded);


    // Background music
    backgroundMusic = new QMediaPlayer(this);
    audioOutput = new QAudioOutput(this);
    backgroundMusic->setAudioOutput(audioOutput);
    backgroundMusic->setSource(QUrl::fromLocalFile("K:/programing/Lodz/c++/projects/Pac-man/pac-man/sounds/PacManMusic.mp3"));
    audioOutput->setVolume(0.3);
    backgroundMusic->setLoops(QMediaPlayer::Infinite);
    backgroundMusic->play();

    // Eat dot sound
    eatDotSound = new QMediaPlayer(this);
    eatDotAudio = new QAudioOutput(this);
    eatDotSound->setAudioOutput(eatDotAudio);
    eatDotSound->setSource(QUrl::fromLocalFile("K:/programing/Lodz/c++/projects/Pac-man/pac-man/sounds/eatDot.mp3"));
    eatDotAudio->setVolume(0.5);

    // Eat power dot sound
    eatPowerDotSound = new QMediaPlayer(this);
    eatPowerDotAudio = new QAudioOutput(this);
    eatPowerDotSound->setAudioOutput(eatPowerDotAudio);
    eatPowerDotSound->setSource(QUrl::fromLocalFile("K:/programing/Lodz/c++/projects/Pac-man/pac-man/sounds/EatingThePac-dots.mp3"));
    eatPowerDotAudio->setVolume(0.5);

    // Eat ghost sound
    eatGhostSound = new QMediaPlayer(this);
    eatGhostAudio = new QAudioOutput(this);
    eatGhostSound->setAudioOutput(eatGhostAudio);
    eatGhostSound->setSource(QUrl::fromLocalFile("K:/programing/Lodz/c++/projects/Pac-man/pac-man/sounds/EatingTheGhost.mp3"));

    // Death sound
    deathSound = new QMediaPlayer(this);
    deathAudio = new QAudioOutput(this);
    deathSound->setAudioOutput(deathAudio);
    deathSound->setSource(QUrl::fromLocalFile("K:/programing/Lodz/c++/projects/Pac-man/pac-man/sounds/lose.mp3"));
}

GameManager::~GameManager()
{
    delete view;
    delete scene;
    delete world;
    // Qt parent-child system deletes timers, pacman, scoreText automatically
}

void GameManager::setupScene()
{
    const int cols = world->getCols();
    const int rows = world->getRows();
    const int tileSize = world->TitleSize();
    const int width = cols * tileSize;
    const int height = rows * tileSize;

    scene = new QGraphicsScene();
    scene->setSceneRect(0, 0, width, height);

    // Draw map and dots
    world->drawMap(scene);
    world->drawDots(scene);

    //create score display at top-left
    scoreText = new QGraphicsTextItem();
    scoreText->setPlainText("SCORE:0");
    scoreText->setDefaultTextColor(Qt::yellow);
    scoreText->setFont(QFont("Arial", 16, QFont::Bold));
    scoreText->setPos(10, 0); // Above the game area
    scoreText->setZValue(10000); // Always on top
    scene->addItem(scoreText);
}

void GameManager::setupView()
{
    const int width = world->getCols() * world->TitleSize();
    const int height = world->getRows() * world->TitleSize();

    view = new QGraphicsView(scene);
    view->setFrameStyle(QFrame::NoFrame);
    view->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    view->setFixedSize(width, height);

    view->show();
    QCoreApplication::processEvents();

   //adjust for viewport chrome
    QSize viewportSize = view->viewport()->size();
    int extraW = view->width() - viewportSize.width();
    int extraH = view->height() - viewportSize.height();

    if (viewportSize.width() != width || viewportSize.height() != height) {
        view->setFixedSize(width + extraW, height + extraH + 30); // +30 for score text
        QCoreApplication::processEvents();
    }

    view->setAlignment(Qt::AlignLeft | Qt::AlignTop);
}

void GameManager::setupPacMan()
{
    QString openPac("K:/programing/Lodz/c++/projects/Pac-man/pac-man/pic/pac.png");
    QString closedPac("K:/programing/Lodz/c++/projects/Pac-man/pac-man/pic/fullpac.png");

    pacman = new PacMan(world, openPac, closedPac);
    pacman->setTransformOriginPoint(pacman->boundingRect().center());
    pacman->setPos(pacman->iniPosition);
    pacman->setFlag(QGraphicsItem::ItemIsFocusable);
    pacman->setFocus();

    scene->addItem(pacman);

    // Debug rectangle
    debugRect = scene->addRect(
        pacman->pos().x(),
        pacman->pos().y(),
        pacman->boundingRect().width(),
        pacman->boundingRect().height(),
        QPen(Qt::red)
        );
    debugRect->setZValue(9999);
}

void GameManager::setupGhosts() {
    // Ghost starting positions
    QVector<QPointF> startPositions = {
        QPointF(9 * world->TitleSize(), 5 * world->TitleSize()),   // Red
        QPointF(9 * world->TitleSize(), 5 * world->TitleSize()),   // Pink
        QPointF(9 * world->TitleSize(), 5 * world->TitleSize()),  // Cyan
        QPointF(9 * world->TitleSize(), 5 * world->TitleSize())   // Orange
    };

    QVector<QPointF> scatterTargets = {
        QPointF(17 * world->TitleSize(), 1 * world->TitleSize()),  // Red: Top-Right
        QPointF(1 * world->TitleSize(), 1 * world->TitleSize()),   // Pink: Top-Left
        QPointF(17 * world->TitleSize(), 13 * world->TitleSize()), // Cyan: Bottom-Right
        QPointF(1 * world->TitleSize(), 13 * world->TitleSize())   // Orange: Bottom-Left
    };

    QVector<QString> ghostPaths = {
        "K:/programing/Lodz/c++/projects/Pac-man/pac-man/pic/red_ghost.png",
        "K:/programing/Lodz/c++/projects/Pac-man/pac-man/pic/pink_ghost.png",
        "K:/programing/Lodz/c++/projects/Pac-man/pac-man/pic/cyan_ghost.png",
        "K:/programing/Lodz/c++/projects/Pac-man/pac-man/pic/orange_ghost.png"
    };
    QVector<GhostType> types = {BLINKY, PINKY, INKY, CLYDE};

    for (int i = 0; i < 4; i++) {
        Ghost* ghost = new Ghost(world, ghostPaths[i], startPositions[i],
                                 scatterTargets[i], types[i]);  //Pass type
        ghosts.append(ghost);
        scene->addItem(ghost);
        ghost->setPos(startPositions[i]);
    }
}

void GameManager::switchGhostMode() {
    if (powerUpActive) return;

    static bool inChaseMode = false;
    inChaseMode = !inChaseMode;

    qDebug() << "=== MODE SWITCH ===" << (inChaseMode ? "CHASE" : "SCATTER");  // DEBUG

    for (Ghost* ghost : ghosts) {
        if (ghost->mode != DEAD && ghost->mode != FRIGHTENED) {
            ghost->setMode(inChaseMode ? CHASE : SCATTER);
            qDebug() << "  Ghost" << ghost->type << "now in" << (inChaseMode ? "CHASE" : "SCATTER");
        }
    }
}
void GameManager::start()
{
    gameTimer->start(16);

    // Start in SCATTER mode explicitly
    for (Ghost* ghost : ghosts) {
        ghost->setMode(SCATTER);
    }

    qDebug() << "Game started - ghosts in SCATTER mode";
}

void GameManager::gameLoop()
{
    pacman->move();

    //  DEBUG: Print ghost modes every 60 frames
    static int debugCounter = 0;
    debugCounter++;
    if (debugCounter % 60 == 0) {
        qDebug() << "=== Ghost Modes ===";
        for (int i = 0; i < ghosts.size(); i++) {
            QString modeStr = ghosts[i]->mode == CHASE ? "CHASE" :
                                  ghosts[i]->mode == SCATTER ? "SCATTER" :
                                  ghosts[i]->mode == FRIGHTENED ? "FRIGHTENED" : "DEAD";
            qDebug() << "  Ghost" << i << ":" << modeStr;
        }
    }

    //Get Pac-Man info for ghost AI
    QPointF pacmanCenter = pacman->getCurrentCenter();
    QPointF pacmanVelocity = pacman->currentVelocity;

    // Blinky is first ghost (index 0)
    Ghost* blinky = ghosts.isEmpty() ? nullptr : ghosts[0];
    
    for (Ghost* ghost : ghosts) {
        ghost->move(pacmanCenter, pacmanVelocity, blinky);
    }

    checkGhostCollisions();

    checkDotCollection();

    // animate mouth every 10 frames
    frameCounter++;
    if (frameCounter % 10 == 0) {
        pacman->DrawPacMan();
    }

    // Update debug rectangle position
    debugRect->setRect(
        pacman->pos().x(),
        pacman->pos().y(),
        pacman->boundingRect().width(),
        pacman->boundingRect().height()
        );

    // Debug output
    /*qDebug() << "currPosition:" << pacman->pos();
    qDebug() << "Score:" << pacman->Score; //690 is the max from Dots
    updateScoreDisplay();*/
}

void GameManager::checkDotCollection()
{
    //  game logic coordination

    // Calculate pacmans current tile
    QPointF currentCenter = pacman->pos() + pacman->boundingRect().center();
    int tileX = currentCenter.x() / world->TitleSize();
    int tileY = currentCenter.y() / world->TitleSize();

    // Clamp to grid bounds
    tileX = qMax(0, qMin(tileX, world->getCols()-1));
    tileY = qMax(0, qMin(tileY, world->getRows()-1));

    int prizeValue = world->prizeGrid[tileY][tileX];

    if (prizeValue == 1) {
        // Small dot
        // remove visual dot
        if (world->dotItems[tileY][tileX] != nullptr) {
            scene->removeItem(world->dotItems[tileY][tileX]);
            delete world->dotItems[tileY][tileX];
            world->dotItems[tileY][tileX] = nullptr;
        }

        world->prizeGrid[tileY][tileX] = 0;
        pacman->Score += 10;
        updateScoreDisplay();
        eatDotSound->stop();
        eatDotSound->play();

    } else if (prizeValue == 2) {
        //big dot - Power pellet!
        if (world->dotItems[tileY][tileX] != nullptr) {
            scene->removeItem(world->dotItems[tileY][tileX]);
            delete world->dotItems[tileY][tileX];
            world->dotItems[tileY][tileX] = nullptr;
        }

        world->prizeGrid[tileY][tileX] = 0;
        pacman->Score += 50;
        updateScoreDisplay();
        eatPowerDotSound->stop();
        eatPowerDotSound->play();
        for (Ghost* ghost : ghosts) {
            ghost->setMode(FRIGHTENED);
        }

        // Activate power-up mode
        powerUpActive = true;
        powerUpTimer->start(8000); // 8 seconds

        qDebug() << "POWER UP ACTIVATED!";
        // TODO: Tell ghosts to run away (when you implement them)
    }
}

void GameManager::checkGhostCollisions() {
    for (Ghost* ghost : ghosts) {
        if (ghost->collidesWithItem(pacman)) {
            if (powerUpActive && ghost->mode != DEAD) {
                ghost->setMode(DEAD);
                ghost->setPos(ghost->iniPosition);
                pacman->Score += 200;
                updateScoreDisplay();
                eatGhostSound->stop();
                eatGhostSound->play();
            } else if (ghost->mode != DEAD) {
                qDebug() << "GAME OVER!";
                gameTimer->stop();
                backgroundMusic->stop();
                deathSound->play();
            }
        }
    }
}

void GameManager::updateScoreDisplay()
{
    scoreText->setPlainText(QString("SCORE: %1").arg(pacman->Score));
}

void GameManager::powerUpEnded()
{
    for (Ghost* ghost : ghosts) {
        if (ghost->mode == FRIGHTENED) {
            ghost->setMode(CHASE);
        }
    }
    powerUpActive = false;
    qDebug() << "Power up ended!";
}
