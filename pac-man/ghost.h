#include <World.h>
#ifndef GHOST_H
#define GHOST_H

using namespace std;

enum GhostMode { CHASE, SCATTER, FRIGHTENED, DEAD };

enum GhostType {
    BLINKY,  // Red - Direct chaser
    PINKY,   // Pink - Ambusher
    INKY,    // Cyan - Unpredictable
    CLYDE    // Orange - Shy
};
class Ghost : public QObject, public QGraphicsPixmapItem{
    Q_OBJECT
public:
    Ghost(GameWorld* world,
           const QString &openPath,
           QPointF startPos,
           QPointF scatterTarget,
          GhostType type,
           QGraphicsItem *parent = nullptr);

    QPointF iniPosition;
    GhostMode mode;
    GhostType type;
    int decisionCooldown;

    void move(QPointF pacmanPos, QPointF pacmanVelocity, Ghost* blinky);  // Pass pacman position
    void setMode(GhostMode newMode);
    QPointF getCurrentCenter();
private:
    int speed;
    int lastDecisionTileX;  // Track where we last made a decision
    int lastDecisionTileY;
    GameWorld* world_;
    QPixmap normalPixmap;
    QPixmap frightenedPixmap;
    QPointF currentVelocity {0, 0};
    QPointF scatterTarget_;
    bool hasLeftHouse;

    bool canMove(int dx, int dy);
    void chooseDirection(QPointF pacmanPos, QPointF pacmanVelocity, Ghost* blinky);
    void teleportCase();
    bool isAtIntersection();
    bool isInGhostHouse();
    QPointF calculateChaseTarget(QPointF pacmanPos, QPointF pacmanVelocity, Ghost* blinky);
};


#endif // GHOST_H
