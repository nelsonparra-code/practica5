#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QTimer>
#include <QImage>
#include <vector>

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
    friend QGraphicsScene::~QGraphicsScene();

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

    QGraphicsRectItem *ellipse;
    QGraphicsRectItem *bomb;
    QGraphicsRectItem *key;
    QRectF *rect;
    QGraphicsRectItem *door;

    bool ellipseGotKey=false;
    char direction = 'U';

    int sqrSize=50, bomberSize=30, bombSize=20;
    int points=0, definedGameTime=120, gameTime=definedGameTime, lives=3;
    int xBomb,yBomb;
    int xKey=250,yKey=50,xDoor=400,yDoor=50;

    QTimer *timer;

    QList<QGraphicsRectItem*> gameMap;
    QList<QGraphicsRectItem*> destructBlocks;
    QList<QGraphicsRectItem*> explosion;
    QList<QGraphicsRectItem*> enemies;

    qreal xO =60, yO =60, xPos=xO, yPos=yO;

    std::vector<qreal> movmtSpeed {};
    std::vector<qreal>* ptrMovmtSpeed=&movmtSpeed;
    qreal movSpeed=5;
};
#endif // MAINWINDOW_H
