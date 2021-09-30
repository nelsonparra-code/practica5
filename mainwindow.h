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
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void keyPressEvent(QKeyEvent *e);

protected slots:
    void showTimer();
    void explode();
    void takeALive();
    //void hideObject(QGraphicsItem*);

private slots:
    void moveObjects();
    bool detectColision();


private:
    Ui::MainWindow *ui;

    QGraphicsScene *scene;
    QGraphicsEllipseItem *ellipse;
    QGraphicsEllipseItem *bomb;
    QGraphicsEllipseItem *key;
    QRectF *rect;
    QGraphicsRectItem *door;

    bool ellipseGotKey=false;

    int gameTime=20, lives=3;
    int xBomb,yBomb;

    QTimer *timer;

    QList<QGraphicsRectItem*> gameMap;
    QList<QGraphicsRectItem*> destructBlocks;
    QList<QGraphicsRectItem*> explosion;

    float xPos=0, yPos=0;
};
#endif // MAINWINDOW_H
