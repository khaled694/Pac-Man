#include "ghost.h"
#include <QRandomGenerator>
#include <cmath>

Ghost::Ghost(GameWorld* world, const QString &imagePath, QPointF startPos,
             QPointF scatterTarget, GhostType type, QGraphicsItem *parent)
    : QObject(), QGraphicsPixmapItem(parent), mode(CHASE), type(type)
    ,decisionCooldown(0),world_(world), scatterTarget_(scatterTarget),hasLeftHouse(false)
{
    iniPosition = startPos;

    QPixmap img(imagePath);
    normalPixmap = img.scaled(world_->TitleSize(), world_->TitleSize(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

    // Blue ghost for frightened mode
    QString blueGhostPath = "K:/programing/Lodz/c++/projects/Pac-man/pac-man/pic/blue_ghost.png";
    QPixmap blueImg(blueGhostPath);
    frightenedPixmap = blueImg.scaled(world_->TitleSize(), world_->TitleSize(),
                                      Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

    setPixmap(normalPixmap);
    setTransformOriginPoint(boundingRect().center());

    speed = 1;  // Slower than pacman

    // Start moving up
    currentVelocity = QPointF(0, -speed);
}

void Ghost::setMode(GhostMode newMode) {
    mode = newMode;

    if (mode == FRIGHTENED) {
        setPixmap(frightenedPixmap);
        speed = 1;
    } else if (mode == CHASE || mode == SCATTER) {
        setPixmap(normalPixmap);
        speed = 1;
    } else if (mode == DEAD) {
        setPixmap(normalPixmap);
        speed = 3;
        hasLeftHouse = true;
    }
}

QPointF Ghost::getCurrentCenter() {
    return pos() + boundingRect().center();
}

bool Ghost::isInGhostHouse() {
    QPointF center = getCurrentCenter();
    int tileX = center.x() / world_->TitleSize();
    int tileY = center.y() / world_->TitleSize();

    //Ghost house is around rows 6-8, cols 7-11
    return (tileY >= 6 && tileY <= 8 && tileX >= 7 && tileX <= 11);
}



bool Ghost::canMove(int dx, int dy) {
    QRectF nextBounds = boundingRect();
    nextBounds.translate(pos() + QPointF(dx, dy));

    int leftTile = std::floor(nextBounds.left() / world_->TitleSize());
    int rightTile = std::floor((nextBounds.right() - 0.0001) / world_->TitleSize());
    int topTile = std::floor(nextBounds.top() / world_->TitleSize());
    int bottomTile = std::floor((nextBounds.bottom() - 0.0001) / world_->TitleSize());

    leftTile = qMax(0, qMin(leftTile, world_->getCols()-1));
    rightTile = qMax(0, qMin(rightTile, world_->getCols()-1));
    topTile = qMax(0, qMin(topTile, world_->getRows()-1));
    bottomTile = qMax(0, qMin(bottomTile, world_->getRows()-1));

    for (int ty = topTile; ty <= bottomTile; ++ty) {
        for (int tx = leftTile; tx <= rightTile; ++tx) {
            if (world_->Grid[ty][tx] == 1) return false;  // Wall only, ghosts can pass gate (2)

            // Ghost gate (2) blocks ONLY f
            // ghost is alive (not DEAD) AND has already left house
            if (world_->Grid[ty][tx] == 2) {
                if (mode != DEAD && hasLeftHouse) {
                    return false;  // Can't go back in!
                }
            }
        }
    }
    return true;
}

bool Ghost::isAtIntersection() {
    QPointF center = getCurrentCenter();
    int tileSize = world_->TitleSize();

    int tileX = center.x() / tileSize;
    int tileY = center.y() / tileSize;

    QPointF tileCenter(tileX * tileSize + tileSize/2.0,
                       tileY * tileSize + tileSize/2.0);

    qreal dist = std::sqrt(std::pow(center.x() - tileCenter.x(), 2) +
                           std::pow(center.y() - tileCenter.y(), 2));

    // Must be close to tile center
    return dist < 3;  // Very strict
}

void Ghost::chooseDirection(QPointF pacmanPos, QPointF pacmanVelocity, Ghost* blinky) {
    if (!isAtIntersection()) return;

    if (!hasLeftHouse && !isInGhostHouse()) {
        hasLeftHouse = true;
    }

    QPointF center = getCurrentCenter();

    QVector<QPointF> directions = {
        QPointF(speed, 0),
        QPointF(-speed, 0),
        QPointF(0, speed),
        QPointF(0, -speed)
    };

    QPointF bestDir = currentVelocity;
    qreal bestScore = 999999;
    QVector<QPointF> validDirs;

    // Collect valid directions (no reversing)
    for (const QPointF& dir : directions) {
        if (dir == -currentVelocity) continue;

        if (canMove(dir.x() * 3, dir.y() * 3)) {
            validDirs.append(dir);
        }
    }

    // Emergency: allow reverse if stuck
    if (validDirs.isEmpty()) {
        for (const QPointF& dir : directions) {
            if (canMove(dir.x() * 3, dir.y() * 3)) {
                validDirs.append(dir);
            }
        }
    }

    // If only one option, take it
    if (validDirs.size() == 1) {
        currentVelocity = validDirs[0];
        return;
    }

    QPointF target;

    if (mode == FRIGHTENED) {
        // Random
        if (!validDirs.isEmpty()) {
            int randIdx = QRandomGenerator::global()->bounded(validDirs.size());
            bestDir = validDirs[randIdx];
        }

    } else if (mode == DEAD) {
        target = iniPosition;

        for (const QPointF& dir : validDirs) {
            QPointF nextPos = center + dir * 5;
            qreal dist = std::sqrt(std::pow(nextPos.x() - target.x(), 2) +
                                   std::pow(nextPos.y() - target.y(), 2));

            if (dist < bestScore) {
                bestScore = dist;
                bestDir = dir;
            }
        }

        qreal distToSpawn = std::sqrt(std::pow(center.x() - iniPosition.x(), 2) +
                                      std::pow(center.y() - iniPosition.y(), 2));
        if (distToSpawn < world_->TitleSize() * 1.5) {
            setMode(SCATTER);
            hasLeftHouse = false;
        }

    } else if (mode == SCATTER) {
        target = scatterTarget_;

        qDebug() << "Ghost" << type << "SCATTER to corner:" << target;  //DEBUG

        for (const QPointF& dir : validDirs) {
            QPointF nextPos = center + dir * 20;  // Look further ahead
            qreal dist = std::sqrt(std::pow(nextPos.x() - target.x(), 2) +
                                   std::pow(nextPos.y() - target.y(), 2));

            if (dist < bestScore) {
                bestScore = dist;
                bestDir = dir;
            }
        }

    } else { // CHASE
        target = calculateChaseTarget(pacmanPos, pacmanVelocity, blinky);

        qDebug() << "Ghost" << type << "CHASE target:" << target;  // dEBUG

        for (const QPointF& dir : validDirs) {
            QPointF nextPos = center + dir * 20;  // look further ahead
            qreal dist = std::sqrt(std::pow(nextPos.x() - target.x(), 2) +
                                   std::pow(nextPos.y() - target.y(), 2));

            if (dist < bestScore) {
                bestScore = dist;
                bestDir = dir;
            }
        }
    }

    currentVelocity = bestDir;
}
QPointF Ghost::calculateChaseTarget(QPointF pacmanPos, QPointF pacmanVelocity, Ghost* blinky) {
    int tileSize = world_->TitleSize();
    QPointF target = pacmanPos;

    switch(type) {
    case BLINKY:
        // BLINKY (Red): Direct chase - target Pac-Man's exact position
        target = pacmanPos;
        break;

    case PINKY:
        // PINKY (Pink): Ambush - target 4 tiles ahead of Pac-Man
        {
            // Normalize velocity to get direction
            qreal velLen = std::sqrt(pacmanVelocity.x() * pacmanVelocity.x() +
                                     pacmanVelocity.y() * pacmanVelocity.y());
            if (velLen > 0) {
                QPointF direction(pacmanVelocity.x() / velLen, pacmanVelocity.y() / velLen);
                target = pacmanPos + direction * (4 * tileSize);
            } else {
                target = pacmanPos;  // Pac-Man not moving
            }
        }
        break;

    case INKY:
        // INKY (Cyan): Unpredictable - uses Blinky's position
        {
            if (blinky) {
                // Look 2 tiles ahead of Pac-Man
                qreal velLen = std::sqrt(pacmanVelocity.x() * pacmanVelocity.x() +
                                         pacmanVelocity.y() * pacmanVelocity.y());
                QPointF ahead = pacmanPos;
                if (velLen > 0) {
                    QPointF direction(pacmanVelocity.x() / velLen, pacmanVelocity.y() / velLen);
                    ahead = pacmanPos + direction * (2 * tileSize);
                }

                // Vector from Blinky to that spot, then double it
                QPointF blinkyPos = blinky->getCurrentCenter();
                QPointF vector = ahead - blinkyPos;
                target = ahead + vector;  // Double the distance
            } else {
                target = pacmanPos;  // Fallback if no Blinky
            }
        }
        break;

    case CLYDE:
        // CLYDE (Orange): Shy - runs away when too close
        {
            QPointF myPos = getCurrentCenter();
            qreal distance = std::sqrt(std::pow(myPos.x() - pacmanPos.x(), 2) +
                                       std::pow(myPos.y() - pacmanPos.y(), 2));

            if (distance > 8 * tileSize) {
                // Far away: chase Pac-Man
                target = pacmanPos;
            } else {
                // Too close: run to scatter corner
                target = scatterTarget_;
            }
        }
        break;
    }

    return target;
}



void Ghost::move(QPointF pacmanPos, QPointF pacmanVelocity, Ghost* blinky) {
    chooseDirection(pacmanPos, pacmanVelocity, blinky);

    if (canMove(currentVelocity.x(), currentVelocity.y())) {
        moveBy(currentVelocity.x(), currentVelocity.y());
    }

    teleportCase();
}

void Ghost::teleportCase() {
    QPointF center = getCurrentCenter();
    qreal totalWidth = world_->getCols() * world_->TitleSize();
    qreal halfw = boundingRect().width() / 2.0;

    if (center.x() < -halfw) {
        QPointF newCenter(center.x() + totalWidth + halfw, center.y());
        QPointF newTopLeft = newCenter - boundingRect().center();
        setPos(newTopLeft);
    }
    else if (center.x() > halfw + totalWidth) {
        QPointF newCenter(center.x() - totalWidth - halfw, center.y());
        QPointF newTopLeft = newCenter - boundingRect().center();
        setPos(newTopLeft);
    }
}
