#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <QKeyEvent>
#include <QTimer>
#include <vector>
#include <string>
#include <fstream>

using namespace std;

vector<vector<int>> fileToVector(string);

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    vector<vector<int>> array = fileToVector("../practica5/BD/map.txt");

    scene = new QGraphicsScene(this);
    ui->graphicsView->setScene(scene);

    QBrush redBrush(Qt::red), grayBrush(Qt::gray),
            darkGrayBrush(Qt::darkGray), brownBrush(Qt::darkRed),yellowBrush(Qt::yellow);
    QPen blackPen(Qt::black);

    blackPen.setWidth(1);

    key = scene->addEllipse(50,0,25,25,blackPen,darkGrayBrush);
    door = scene->addRect(200,-15,40,40,blackPen,brownBrush);

    int sqrRows=array.size(), sqrsNumb=array[1].size(),numbEnemies=8;

    for(int i=0;i<sqrRows;i++){
        int rectX=-(9*sqrSize/4);
        static int rectY=-(5*sqrSize/4);
        for(int j=0;j<sqrsNumb;j++){
            rectX+=sqrSize;
            if(array[i][j]==1){
                gameMap.append(scene->addRect(rectX,rectY,sqrSize,sqrSize,blackPen,grayBrush));
            }
            else if(array[i][j]==0&&(i!=1)){
                int random = rand()%5;
                if(random==1){
                    destructBlocks.append(scene->addRect(rectX,rectY,sqrSize,sqrSize,blackPen,darkGrayBrush));
                }
                else if(random==0&&enemies.size()!=numbEnemies&&i%2==0){
                    enemies.append(scene->addEllipse(rectX+(sqrSize/4),rectY+(sqrSize/4),30,30,blackPen,redBrush));
                }
            }
        }
        rectY+=sqrSize;

    }

    ellipse = scene->addEllipse(xPos,yPos,ellipseRad*2,ellipseRad*2,blackPen,yellowBrush);

    ui->lives->display(lives);
    timer = new QTimer(this);
    connect(timer,SIGNAL(timeout()),this,SLOT(showTimer()));
    timer->start(1000);

    timer = new QTimer(this);
    connect(timer,SIGNAL(timeout()),this,SLOT(moveObjects()));
    timer->start(100);

    timer = new QTimer(this);
    connect(timer,SIGNAL(timeout()),this,SLOT(moveEnemies()));
    timer->start(500);



}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::moveObjects(){
    ellipse->setPos(xPos,yPos);
    if(xPos>340) ui->graphicsView->setSceneRect(xPos-(9*sqrSize/4),-(5*sqrSize/4),700,700);
}

void MainWindow::moveEnemies(){
    qreal movSpeed=5;
    for(auto en:qAsConst(enemies)){
        if(detectColision(en)||direction=='L'){
            en->setPos(en->x()-movSpeed,en->y());
            direction = 'L';
        }
        if(detectColision(en)||direction=='U'){
            en->setPos(en->x(),en->y()-movSpeed);
            direction = 'U';
        }
        if(detectColision(en)||direction=='D'){
            en->setPos(en->x(),en->y()+(movSpeed));
            direction = 'D';
        }
        if(detectColision(en)||direction=='R'){
            en->setPos(en->x()+movSpeed,en->y());
            direction = 'R';
        }
    }
}

void MainWindow::takeALive(){
    ui->lives->display(--lives);
    xPos=0,yPos=0;
    ellipse->setPos(xPos,yPos);
}

bool MainWindow::detectColision(QGraphicsItem* item){
    ellipse->setPos(xPos,yPos);

    for(auto mapItem : qAsConst(gameMap)){
        if(item->collidesWithItem(mapItem)){
            if(item!=ellipse) item->setPos(item->x()-qreal(1),item->y()-qreal(1));
            return true;
        }
    }
    for(auto mapItem : qAsConst(destructBlocks)){
        if(item->collidesWithItem(mapItem)){
            if(item!=ellipse) item->setPos(item->x()-qreal(1),item->y()-qreal(1));
            return true;
        }
    }
    if(ellipse->collidesWithItem(key)){
        ellipseGotKey=true;
        scene->removeItem(key);
    }
    for(auto en : qAsConst(enemies)){
        if(ellipse->collidesWithItem(en)){
            takeALive();
        }
    }
    if((ellipse->collidesWithItem(door))&&ellipseGotKey){
        door->hide();
        QString text = "Winner!";
        ui->label->setText(text);
        scene->~QGraphicsScene();
    }


    return false;
}

void MainWindow::keyPressEvent(QKeyEvent *event){
    int movingSpace = 7;
    if(event->key()==Qt::Key_A){
        xPos-=movingSpace;
        if(detectColision(ellipse)) xPos+=movingSpace;
    }
    else if(event->key()==Qt::Key_W){
        yPos-=movingSpace;
        if(detectColision(ellipse)) yPos+=movingSpace;
    }
    else if(event->key()==Qt::Key_D){
        xPos+=movingSpace;
        if(detectColision(ellipse)) xPos-=movingSpace;
    }
    else if(event->key()==Qt::Key_S){
        yPos+=movingSpace;
        if(detectColision(ellipse)) yPos-=movingSpace;
    }
    else if(event->key()==Qt::Key_Space){
        QPen blackPen(Qt::black);
        QBrush blackBrush(Qt::black);
        xBomb=xPos+(ellipseRad/2), yBomb=yPos+(ellipseRad/2);
        bomb = scene->addEllipse(xBomb,yBomb,bombSize,bombSize,blackPen,blackBrush);
        timer = new QTimer(this);
        connect(timer,SIGNAL(timeout()),this,SLOT(explode()));
        timer->start(1000);
    }
}

vector<vector<int>> fileToVector(string fLocation){

    vector<vector<int>> ultimateVctr {};
    vector<int> auxVctr {};

    ifstream inFile;
    inFile.open(fLocation);

    string data;
    inFile >> data;

    for(unsigned long long i=0;i<data.length();i++){
        if(data[i]=='1'||data[i]=='0') auxVctr.push_back(data[i]-48);
        else if(data[i]=='}'&&auxVctr.size()!=0){
            ultimateVctr.push_back(auxVctr);
            auxVctr.clear();
        }
    }

    return ultimateVctr;
}

void MainWindow::showTimer(){
    if(gameTime==-1){
        takeALive();
        gameTime=20;
    }
    ui->timer->display(gameTime--);
}

void MainWindow::explode()
{
    QPen redPen(Qt::red);
    QBrush yellowBrush(Qt::yellow);
    int expDistance=20, c=2;
    bool liveTkenByExplosion=false;
    for(int hor=0;hor<2;hor++){
        if(hor==1) c=(-c);
        explosion.append(scene->addRect(xBomb+(bombSize/2),yBomb+(bombSize/2),expDistance*c,expDistance,redPen,yellowBrush));
        explosion.append(scene->addRect(xBomb+(bombSize/2),yBomb+(bombSize/2),expDistance,expDistance*c,redPen,yellowBrush));
    }

    for(auto exp : qAsConst(explosion)){
        for(auto block:qAsConst(destructBlocks)){
            if(exp->collidesWithItem(block)){
                points+=200;
                ui->points->display(points);
                scene->removeItem(block);
                destructBlocks.removeAt(destructBlocks.indexOf(block));
            }
        }
        for(auto en:qAsConst(enemies)){
            if(exp->collidesWithItem(en)){
                points+=500;
                ui->points->display(points);
                scene->removeItem(en);
                enemies.removeAt(enemies.indexOf(en));
            }
        }
        if(exp->collidesWithItem(ellipse)&&liveTkenByExplosion==false){
            takeALive();
            liveTkenByExplosion=true;
        }
        scene->removeItem(exp);
    }

    scene->removeItem(bomb);
    explosion.clear();
    timer->~QTimer();
}
