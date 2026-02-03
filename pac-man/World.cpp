#include <World.h>

const int GameWorld::Grid[rows][cols]= {
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,0,0,0,0,0,0,1,0,0,0,1,0,0,0,0,0,0,1},
    {1,0,1,1,1,1,0,1,0,1,0,1,0,1,1,1,1,0,1},
    {1,0,1,0,0,0,0,0,0,1,0,0,0,0,0,0,1,0,1},
    {1,0,1,0,1,1,0,1,1,1,1,1,0,1,1,0,1,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},

    // ─── Ghost House ───
    {1,0,1,1,1,1,0,1,1,2,1,1,0,1,1,0,1,0,1},
    {0,0,1,0,0,0,0,1,0,0,0,1,0,0,0,0,1,0,0}, // teleport gates (left & right)
    {1,0,1,0,1,1,0,1,1,1,1,1,0,1,1,1,1,0,1},
    // ──────────────────

    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,1,0,1,1,0,1,1,1,1,1,0,1,1,0,1,0,1},
    {1,0,1,0,0,0,0,0,0,1,0,0,0,0,0,0,1,0,1},
    {1,0,1,1,1,1,0,1,0,1,0,1,0,1,1,1,1,0,1},
    {1,0,0,0,0,0,0,1,0,0,0,1,0,0,0,0,0,0,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
};

int GameWorld::prizeGrid[rows][cols] = {
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,2,1,1,1,1,1,0,1,1,1,0,1,1,1,1,1,2,0},
    {0,1,0,0,0,0,1,0,1,0,1,0,1,0,0,0,0,1,0},
    {0,1,0,1,1,1,1,1,1,0,1,1,1,1,1,1,0,1,0},
    {0,1,0,1,0,0,1,0,0,0,0,0,1,0,0,1,0,1,0},
    {0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0},

    // ─── Ghost House ───
    {0,1,0,0,0,0,1,0,0,0,0,0,1,0,0,0,0,1,0},
    {0,1,0,1,1,1,1,0,0,0,0,0,1,1,1,1,0,1,0}, // teleport gates (left & right) are walkable -> show dots
    {0,1,0,0,0,0,1,0,0,0,0,0,1,0,0,0,0,1,0},
    // ─────────────────────

    {0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0},
    {0,1,0,1,0,0,1,0,0,0,0,0,1,0,0,1,0,1,0},
    {0,1,0,1,1,1,1,1,1,0,1,1,1,1,1,1,0,1,0},
    {0,1,0,0,0,0,1,0,1,0,1,0,1,0,0,0,0,1,0},
    {0,2,1,1,1,1,1,0,1,1,1,0,1,1,1,1,1,2,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
};

GameWorld::GameWorld(){
    sDots_size = 6;
    bDots_size = 12;

    for (int y = 0; y < rows; ++y) {
        for (int x = 0; x < cols; ++x) {
            dotItems[y][x] = nullptr;
        }
    }
}

/*GameWorld::GameWorld(const GameWorld& other)
        : TILE_SIZE(other.TILE_SIZE),
          row(other.row),
          col(other.col) {}*/
int GameWorld::getCols(){
    return cols;
}

int GameWorld::getRows(){
    return rows;
}

int GameWorld::TitleSize(){
    return TILE_SIZE;
}


void GameWorld ::drawMap(QGraphicsScene *scene){
    QPen wallPen(Qt::blue);
    QPen ghostGatePen(Qt::red);
    wallPen.setWidth(2);

    QBrush wallBrush(Qt::darkBlue);
    QBrush gateBrush(Qt::darkRed);
    QBrush pathBrush(Qt::black);

    for (int y = 0; y < rows; ++y) {
        for (int x = 0; x < cols; ++x) {

            QRectF tileRect(
                x * TILE_SIZE,
                y * TILE_SIZE,
                TILE_SIZE,
                TILE_SIZE
                );

            if (Grid[y][x] == 1) {
                scene->addRect(tileRect, wallPen, wallBrush);
            } else if (Grid[y][x] == 2){
                scene->addRect(tileRect, ghostGatePen, gateBrush);
            }else {
                // optional: draw paths lightly so you see the grid
                scene->addRect(tileRect, QPen(Qt::gray), pathBrush);
            }
        }
    }

}

void GameWorld::drawDots(QGraphicsScene *scene) {
    QBrush smallDotBrush(Qt::white);
    QBrush bigDotBrush(QColor("gold"));

    QPen noPen(Qt::NoPen);

    for (int y = 0; y < rows; ++y) {
        for (int x = 0; x < cols; ++x) {

            // Calculate the center of the current tile
            int centerX = x * TILE_SIZE + (TILE_SIZE / 2);
            int centerY = y * TILE_SIZE + (TILE_SIZE / 2);

            if (prizeGrid[y][x] == 1) {
                //Small Dot (Size: 4x4)
                int radius = sDots_size;
                dotItems[y][x] = scene->addEllipse(centerX - radius, centerY - radius,
                                  radius * 2, radius * 2,
                                  noPen, smallDotBrush);

            } else if (prizeGrid[y][x] == 2) {
                //big Power Pellet (Size: 12x12)
                int radius = bDots_size;
                dotItems[y][x] = scene->addEllipse(centerX - radius, centerY - radius,
                                  radius * 2, radius * 2,
                                  noPen, bigDotBrush);
            }
        }
    }
}
