#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H

#include <QObject>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsTextItem>
#include <QTimer>
#include <QMediaPlayer>
#include <QAudioOutput>
#include "World.h"
#include "pacMan.h"
#include "ghost.h"

class GameManager : public QObject
{
    Q_OBJECT

public:
    explicit GameManager(QObject *parent = nullptr);
    ~GameManager();

    void start();
    void pause();
    void resume();
    void setupGhosts();
    void checkGhostCollisions();

private slots:
    void gameLoop();           // Called every frame
    void powerUpEnded();       // Called when power-up expires

private:
    // Game objects
    QGraphicsScene* scene;
    QGraphicsView* view;
    GameWorld* world;
    PacMan* pacman;
    QVector<Ghost*> ghosts;

    // UI elements
    QGraphicsTextItem* scoreText;
    QGraphicsRectItem* debugRect;  // Your debug rectangle

    // Timers
    QTimer* gameTimer;
    QTimer* powerUpTimer;
    QTimer* modeTimer;

    //sounds
    QMediaPlayer* backgroundMusic;
    QAudioOutput* audioOutput;

    QMediaPlayer* eatDotSound;
    QAudioOutput* eatDotAudio;

    QMediaPlayer* eatPowerDotSound;
    QAudioOutput* eatPowerDotAudio;

    QMediaPlayer* eatGhostSound;
    QAudioOutput* eatGhostAudio;

    QMediaPlayer* deathSound;
    QAudioOutput* deathAudio;

    // Game state
    bool isPaused;
    bool powerUpActive;
    int frameCounter;

    // Helper methods
    void setupScene();
    void setupView();
    void setupPacMan();
    void updateScoreDisplay();
    void handlePowerPelletCollected();
    void checkDotCollection();
    void switchGhostMode();
};

#endif // GAMEMANAGER_H
