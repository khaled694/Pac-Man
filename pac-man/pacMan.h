#include <World.h>

#ifndef PACMAN_H
#define PACMAN_H

using namespace std;

class PacMan : public QObject, public QGraphicsPixmapItem{
public:

    PacMan(GameWorld* world,
           const QString &openPath,
           const QString &closedPath,
           QGraphicsItem *parent = nullptr);
    int Score;
    QPointF iniPosition;
    int rotationAngle;
    QPointF currentVelocity {0, 0};
    QPointF desiredVelocity {0, 0};
    QPointF getCurrentCenter();
    void DrawPacMan();
    void move();
protected:
    void keyPressEvent(QKeyEvent *event) override;
    bool canMove(int dx, int dy);
private:
    int speed;
    GameWorld* world_;
    QPixmap openPixmap;
    QPixmap closedPixmap;
    bool mouthOpen = true;
    void teleportCase();
    /*void checkSmallDotCollection();
    bool checkBigDotCollection();*/
};

#endif // PACMAN_H
