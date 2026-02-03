#include <QApplication>
#include "gamemanager.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    GameManager* game = new GameManager();
    game->start();

    return a.exec();
}
