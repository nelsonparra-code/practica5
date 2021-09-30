#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QTimer>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{

    friend class bomber;
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void keyPressEvent(QKeyEvent *e);
    QGraphicsScene *scene;

protected slots:
    void showTimer();
    void explode();
    void takeALive();

private slots:
    bool detectColision(QGraphicsItem*);
    void moveObjects();
    void moveEnemies();

private:
    Ui::MainWindow *ui;

    QGraphicsEllipseItem *ellipse;
    QGraphicsEllipseItem *bomb;
    QGraphicsEllipseItem *key;
    QRectF *rect;
    QGraphicsRectItem *door;

    bool ellipseGotKey=false;
    char direction = 'L';

    int sqrSize=50, ellipseRad=15, bombSize=20;
    int points=0, gameTime=20, lives=3;
    int xBomb,yBomb;

    QTimer *timer;

    QList<QGraphicsRectItem*> gameMap;
    QList<QGraphicsRectItem*> destructBlocks;
    QList<QGraphicsRectItem*> explosion;
    QList<QGraphicsEllipseItem*> enemies;

    float xPos=0, yPos=0;
};
#endif // MAINWINDOW_H
