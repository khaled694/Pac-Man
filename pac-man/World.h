#include <QGraphicsPixmapItem>
#include <QGraphicsPixmapItem>
#include<QKeyEvent>
#include <QGraphicsPixmapItem>
#include <QApplication>
#include <QGraphicsView>

#ifndef WORLD_H
#define WORLD_H

class GameWorld : public QGraphicsPixmapItem{
public:
    static const int TILE_SIZE = 64;
    static const int rows = 15;
    static const int cols = 19;
    static const int Grid[rows][cols];
    static int prizeGrid[rows][cols];
    QGraphicsEllipseItem* dotItems[rows][cols];
    int sDots_size;
    int bDots_size;
    GameWorld();
    GameWorld(const GameWorld& world);
    void drawMap(QGraphicsScene *scene);
    bool isWall(int row, int col) const;
    int getCols();
    int getRows();
    int TitleSize();
    void drawDots(QGraphicsScene *scene);

};

#endif // WORLD_H
