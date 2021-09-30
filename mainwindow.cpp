#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <QKeyEvent>
#include <QTimer>
#include <vector>
#include <string>
#include <fstream>
//#include <rectangle.h>

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
            darkGrayBrush(Qt::darkGray), goldBrush(Qt::darkYellow),
            brownBrush(Qt::darkRed);
    QPen blackPen(Qt::black);

    blackPen.setWidth(1);

    key = scene->addEllipse(50,0,25,25,blackPen,darkGrayBrush);
    door = scene->addRect(200,-15,40,40,blackPen,brownBrush);

    int sqrSize=50, sqrRows=array.size(), sqrsNumb=array[1].size();

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
            }
        }
        rectY+=sqrSize;

    }

    ellipse = scene->addEllipse(xPos,yPos,30,30,blackPen,redBrush);
    scene->setFocusItem(ellipse);

    //scene->setSceneRect(50,50,50,50);

    ui->lives->display(lives);
    timer = new QTimer(this);
    connect(timer,SIGNAL(timeout()),this,SLOT(showTimer()));
    timer->start(1000);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::moveObjects(){
    ellipse->setPos(xPos,yPos);
}

void MainWindow::takeALive(){
    ui->lives->display(--lives);
}

bool MainWindow::detectColision(){
    QBrush redBrush(Qt::red);
    ellipse->setPos(xPos,yPos);
    QBrush blueBrush(Qt::gray);

    for(auto mapItem : qAsConst(gameMap)){
        if(ellipse->collidesWithItem(mapItem)){
            mapItem->setBrush(redBrush);
            return true;
        }
        else mapItem->setBrush(blueBrush);
    }
    for(auto mapItem : qAsConst(destructBlocks)){
        if(ellipse->collidesWithItem(mapItem)){
            return true;
        }
    }
    if(ellipse->collidesWithItem(key)){
        ellipseGotKey=true;
        scene->removeItem(key);
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
        if(detectColision()) xPos+=movingSpace;
    }
    else if(event->key()==Qt::Key_W){
        yPos-=movingSpace;
        if(detectColision()) yPos+=movingSpace;
    }
    else if(event->key()==Qt::Key_D){
        xPos+=movingSpace;
        if(detectColision()) xPos-=movingSpace;
    }
    else if(event->key()==Qt::Key_S){
        yPos+=movingSpace;
        if(detectColision()) yPos-=movingSpace;
    }
    else if(event->key()==Qt::Key_Space){
        QPen blackPen(Qt::black);
        QBrush blackBrush(Qt::black);
        xBomb=xPos, yBomb=yPos;
        bomb = scene->addEllipse(xBomb,yBomb,20,20,blackPen,blackBrush);
        timer = new QTimer(this);
        connect(timer,SIGNAL(timeout()),this,SLOT(explode()));
        timer->start(1500);
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
        if(hor==1) c=-2;
        explosion.append(scene->addRect(xBomb,yBomb,expDistance*c,expDistance,redPen,yellowBrush));
        explosion.append(scene->addRect(xBomb,yBomb,expDistance,expDistance*c,redPen,yellowBrush));
    }

    for(auto exp : qAsConst(explosion)){
        for(auto block:qAsConst(destructBlocks)){
            if(exp->collidesWithItem(block)){
                scene->removeItem(block);
                destructBlocks.removeAt(destructBlocks.indexOf(block));
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

//void MainWindow::hideObject(QGraphicsItem* item){}
