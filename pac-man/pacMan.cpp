#include<pacMan.h>


PacMan::PacMan(GameWorld* world, const QString &openPath, const QString &closedPath, QGraphicsItem *parent)
    : QGraphicsPixmapItem(parent), world_(world)
{
    // load once and scale to tile EXACTLY
    iniPosition = QPointF(((int)world_->getCols() /2)* world_->TitleSize(), ((int) world_->getRows()/2 + 2) * world_->TitleSize());
    QPixmap open(openPath);
    QPixmap closed(closedPath);

    openPixmap   = open.scaled(world_->TitleSize(), world_->TitleSize(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    closedPixmap = closed.scaled(world_->TitleSize(), world_->TitleSize(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

    // use one of them as initial pixmap
    setPixmap(openPixmap);
    mouthOpen = false;

    // Now that pixmap is set, set rotation origin to center (correct boundingRect)
    setTransformOriginPoint(boundingRect().center());

    speed = 2;
    Score = 0;
}

void PacMan::keyPressEvent(QKeyEvent *event) {
    switch (event->key()) {
    case Qt::Key_Left:
        desiredVelocity = QPointF(-speed, 0);
        break;
    case Qt::Key_A:
        desiredVelocity = QPointF(-speed, 0);
        break;
    case Qt::Key_Right:
        desiredVelocity = QPointF(speed,0);
        break;
    case Qt::Key_D:
        desiredVelocity = QPointF(speed,0);
        break;
    case Qt::Key_Down:
        desiredVelocity = QPointF(0, speed);
        break;
    case Qt::Key_S:
        desiredVelocity = QPointF(0, speed);
        break;
    case Qt::Key_Up:
        desiredVelocity = QPointF(0, -speed);
        break;
    case Qt::Key_W:
        desiredVelocity = QPointF(0, -speed);
        break;
    default:
        break;
    }
}

void PacMan::DrawPacMan() {

    if (mouthOpen) setPixmap(openPixmap);
    else          setPixmap(closedPixmap);

    mouthOpen = !mouthOpen;

    // ensure transform origin still correct after changing pixmap
    setTransformOriginPoint(boundingRect().center());
}

bool PacMan::canMove(int dx, int dy){
    // bounding rect in item coords, convert to scene coords at next position:
    QRectF nextBounds = boundingRect();
    nextBounds.translate(pos() + QPointF(dx, dy));

    // compute tile index range covered by the bounding box
    int leftTile  = std::floor(nextBounds.left()  / world_->TitleSize());
    int rightTile = std::floor((nextBounds.right() - 0.0001) / world_->TitleSize());
    int topTile   = std::floor(nextBounds.top()   / world_->TitleSize());
    int bottomTile= std::floor((nextBounds.bottom()- 0.0001) / world_->TitleSize());

    // clamp to grid bounds
    leftTile  = qMax(0, qMin(leftTile,  world_->getCols()-1));
    rightTile = qMax(0, qMin(rightTile, world_->getCols()-1));
    topTile   = qMax(0, qMin(topTile,  world_->getRows()-1));
    bottomTile= qMax(0, qMin(bottomTile, world_->getRows()-1));

    // If any covered tile is a wall (1), movement blocked
    for (int ty = topTile; ty <= bottomTile; ++ty) {
        for (int tx = leftTile; tx <= rightTile; ++tx) {
            if (world_->Grid[ty][tx] == 1 || world_->Grid[ty][tx] == 2) return false;
        }
    }
    return true;
}

QPointF PacMan::getCurrentCenter() {
    return pos() + boundingRect().center();// pos() = topleft of pacMan
}

/*void PacMan::checkSmallDotCollection(){
    QPointF pCenter = getCurrentCenter();
    int tileX = pCenter.x() / world_->TitleSize();
    int tileY = pCenter.y() / world_->TitleSize();

    // clamp to grid bounds
    tileX = qMax(0, qMin(tileX, world_->getCols()-1));
    tileY = qMax(0, qMin(tileY, world_->getRows()-1));
    if (world_->prizeGrid[tileY][tileX] == 1) {
        if (world_->dotItems[tileY][tileX] != nullptr) {
            scene()->removeItem(world_->dotItems[tileY][tileX]);
            delete world_->dotItems[tileY][tileX];
            world_->dotItems[tileY][tileX] = nullptr;
        }
        world_->prizeGrid[tileY][tileX] = 0;
        Score += 5;
    }
}*/

/*bool PacMan::checkBigDotCollection(){
    QPointF pCenter = getCurrentCenter();
    int tileX = pCenter.x() / world_->TitleSize();
    int tileY = pCenter.y() / world_->TitleSize();
    // clamp to grid bounds
    tileX = qMax(0, qMin(tileX, world_->getCols()-1));
    tileY = qMax(0, qMin(tileY, world_->getRows()-1));
    if (world_->prizeGrid[tileY][tileX] == 2) {
        if (world_->dotItems[tileY][tileX] != nullptr) {
            scene()->removeItem(world_->dotItems[tileY][tileX]);
            delete world_->dotItems[tileY][tileX];
            world_->dotItems[tileY][tileX] = nullptr;
        }
        world_->prizeGrid[tileY][tileX] = 0;
        Score += 10;
        return true;
    }
    return false;
}*/

void PacMan::move() {
    if (canMove(desiredVelocity.x(), desiredVelocity.y())){
        if (desiredVelocity != currentVelocity) {
            currentVelocity = desiredVelocity;
            if (currentVelocity.x() < 0) rotationAngle = 180;
            else if (currentVelocity.x() > 0) rotationAngle = 0;
            else if (currentVelocity.y() > 0) rotationAngle = 90;
            else if (currentVelocity.y() < 0) rotationAngle = 270;
            setRotation(rotationAngle);
        }
    }
    if (canMove(currentVelocity.x(), currentVelocity.y())){
        moveBy(currentVelocity.x(), currentVelocity.y());
    }
    teleportCase();
 /*   checkSmallDotCollection();
    checkBigDotCollection();*/
}

void PacMan::teleportCase(){
    QPointF pCenter = getCurrentCenter();
    qreal totalWidth = world_->getCols()*world_->TitleSize();

    qreal halfw = boundingRect().width() / 2.0; //pac half width

    // left teleport gate
    if (pCenter.x() < -halfw) {
        QPointF newCenter(pCenter.x() + totalWidth + halfw, pCenter.y());
        QPointF newTopLeft = newCenter - boundingRect().center();
        setPos(newTopLeft);
        return;
    }

    //right teleport gate
    if (pCenter.x() > halfw + totalWidth) {
        QPointF newCenter(pCenter.x() - totalWidth - halfw, pCenter.y());
        QPointF newTopLeft = newCenter - boundingRect().center();
        setPos(newTopLeft);
        return;
    }
    return;
}

