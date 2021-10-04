#include "mainwindow.h"
#include "ui_mainwindow.h"

using namespace std;

/*
 * El programa aun contiene algunos problemas:
 * 1) Cuando se gana, o se pierde, el programa se cierra solo.
 * 2) El movimiento de los enemigos es muy limitado.
 * 3) la generacion de bloques, de la llave y la puerta, no son aleatorios.
 * 3) El seguimiento del personaje, se puede mejorar para mejor visualizacion.
 */

vector<vector<int>> fileToVector(string);

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //CARGANDO IMAGENES PARA LOS ITEMS
    QImage bomber("../practica5/BD/bomberman.png"),doorPic("../practica5/BD/door.png"),
            keyPic("../practica5/BD/key.png"),enemyPic("../practica5/BD/enemy.png"),
            bricks("../practica5/BD/bricks.png"),solid("../practica5/BD/solid.png");

    //VECTOR QUE CONTIENE EL FORMATO DEL MAPA
    vector<vector<int>> array = fileToVector("../practica5/BD/map.txt");

    //CREACION DE ESCENA
    scene = new QGraphicsScene(this);
    ui->graphicsView->setScene(scene);

    //BROCHAS Y LAPICES PARA LOS ITEMS
    QBrush grayBrush(solid.scaled(sqrSize,sqrSize)), doorBrush(doorPic.scaled(sqrSize,sqrSize)), keyBrush(keyPic.scaled(sqrSize,sqrSize)),
            bricksBrush(bricks.scaled(sqrSize,sqrSize)), bomberBrush(bomber.scaled(bomberSize,bomberSize)),
            enemyBrush(enemyPic.scaled(30,30,Qt::KeepAspectRatioByExpanding));
    QPen blackPen(Qt::black), transparentPen(Qt::transparent);
    blackPen.setWidth(2);

    scene->setBackgroundBrush(Qt::darkGreen);

    //SE GAREGA EL MAPA A LA ESCENA RECORRIENDO EL VECTOR Y GENERANDO BLOQUES DESTRUCTIBLES Y ENEMIGOS "ALEATORIAMENTE"
    int sqrRows=array.size(), sqrsNumb=array[1].size(),numbEnemies=8;
    for(int i=0;i<sqrRows;i++){
        int rectX=0;
        static int rectY=0;
        for(int j=0;j<sqrsNumb;j++){
            if(array[i][j]==1){
                gameMap.append(scene->addRect(rectX,rectY,sqrSize,sqrSize,transparentPen,grayBrush));
            }
            else if(i!=1){
                uniform_int_distribution<int> distribution(1,100);
                int random = distribution(*QRandomGenerator::global());
                if(random%5==0){
                    if((random+5)>15&&keyExists==false){
                        key = scene->addRect(0,0,sqrSize,sqrSize,transparentPen,keyBrush);
                        key->setPos(QPointF(rectX,rectY));
                        keyExists=true;
                    }
                    else if((random+5)>10&&doorExists==false){
                        door = scene->addRect(0,0,sqrSize,sqrSize,transparentPen,doorBrush);
                        door->setPos(QPointF(rectX,rectY));
                        doorExists=true;
                    }
                    destructBlocks.append(scene->addRect(rectX,rectY,sqrSize,sqrSize,transparentPen,bricksBrush));
                }
                else if(random%5==1&&enemies.size()!=numbEnemies&&i%2==0){
                    QGraphicsRectItem* temp = scene->addRect(0,0,32,32,transparentPen,enemyBrush);
                    temp->setPos(rectX+(sqrSize/4),rectY+(sqrSize/4));
                    enemies.append(temp);
                }
            }
            rectX+=sqrSize;
        }
        rectY+=sqrSize;

    }

    //SE AGREGAN ELEMENTOS A UN VECTOR QUE CONTIENE EL MOVIMIENTO DE LOS ENEMIGOS
    for(int i=0;i<enemies.size();i++){
        (*ptrMovmtSpeed).push_back(movSpeed);
    }

    //SE AGREGA EL PERSONAJE PRINCIPAL A LA ESCENA
    ellipse = scene->addRect(0,0,bomberSize,bomberSize,transparentPen,bomberBrush);
    ellipse->setPos(xPos,yPos);

    //SE INICIALIZA EL TIMER DEL JUEGO
    ui->lives->display(lives);
    timer = new QTimer(this);
    connect(timer,SIGNAL(timeout()),this,SLOT(showTimer()));
    timer->start(1000);

    //TIMER PARA MANTENER ENFOCADA LA ESCENA EN EL PERSONAJE
    timer = new QTimer(this);
    connect(timer,SIGNAL(timeout()),this,SLOT(moveObjects()));
    timer->start(1);

    //TIMER PARA MOVER ENEMIGOS
    timer = new QTimer(this);
    connect(timer,SIGNAL(timeout()),this,SLOT(moveEnemies()));
    timer->start(150);



}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::moveObjects(){
    ellipse->setPos(xPos,yPos);
    //ui->graphicsView->setSceneRect(350,0,700,700);
    if(xPos==xO&&yPos==yO) ui->graphicsView->setSceneRect(0,0,700,700);
    else if(xPos>=xO+(20*50)) ui->graphicsView->setSceneRect(xO+(14*50),0,700,700);
    else if(xPos-350>0 && xPos<xO+(20*50)) ui->graphicsView->setSceneRect(xPos-350,0,700,700);
}

void MainWindow::moveEnemies(){
    for(auto en:qAsConst(enemies)){
        qreal objMovement = movmtSpeed[enemies.indexOf(en)];
        en->setX(en->x()-objMovement);
        if(detectColision(en)){
            en->setPos(en->x()+objMovement,en->y()+objMovement);
            if(detectColision(en)){
                en->setY(en->y()-objMovement);
                objMovement-=(qreal(2)*objMovement);
                (*ptrMovmtSpeed)[enemies.indexOf(en)] = objMovement;
            }
        }
    }
}

void MainWindow::takeALive(){
    ui->lives->display(--lives);
    xPos=xO,yPos=yO;
    ellipse->setPos(xPos,yPos);
}

bool MainWindow::detectColision(QGraphicsItem* item){
    ellipse->setPos(xPos,yPos);

    for(auto mapItem : qAsConst(gameMap)){
        if(item->collidesWithItem(mapItem)){
            return true;
        }
    }
    for(auto mapItem : qAsConst(destructBlocks)){
        if(item->collidesWithItem(mapItem)){
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
        QString text = "Winner!";
        ui->label->setText(text);
        scene->clear();
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
        //SE CREA UNA BOMBA
        QImage bombPic("../practica5/BD/bomb.png");
        QBrush bombBrush(bombPic.scaled(bomberSize,bomberSize));
        QPen transparentPen(Qt::transparent);
        xBomb=xPos, yBomb=yPos;
        bomb = scene->addRect(0,0,bomberSize,bomberSize,transparentPen,bombBrush);
        bomb->setPos(xPos,yPos);
        //SE CREA UN TIMER PARA QUE LA BOMBA EXPLOTE EN 1.5s
        timer = new QTimer(this);
        connect(timer,SIGNAL(timeout()),this,SLOT(explode()));
        timer->start(1500);
    }
}

void MainWindow::removeExplosions(){
    for(auto expToRem : qAsConst(explosion)){
        scene->removeItem(expToRem);
    }
    explosion.clear();
    tempTimer->~QTimer();
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
    if(lives==0){
        QString text = "Game Over!";
        ui->label->setText(text);
        scene->clear();
    }
    if(gameTime==-1){
        takeALive();
        gameTime=definedGameTime;
    }
    ui->timer->display(gameTime--);
}

void MainWindow::explode()
{
    int y=50, x=30, xExp=xBomb, yExp=yBomb+(bomberSize/2);
    QPen transparentPen(Qt::transparent);
    QImage exp("../practica5/BD/explosion.png"), expR("../practica5/BD/explosionR.png");
    QBrush expBrushUp(exp.scaled(x,y)), expBrushDwn(exp.mirrored(false,true).scaled(x,y)),
            expBrushRight(expR.mirrored(false,false).scaled(y,x)),
            expBrushLeft(expR.mirrored(true,false).scaled(y,x));
    bool liveTkenByExplosion=false;
    //SE CREA LA EXPLOSION
    for(int i=0;i<2;i++){
        for(int j=-1;j<2;j+=2){
            QGraphicsRectItem* temp;
            if(i==0){
                if(j==-1) temp = scene->addRect(0,0,x,y*j,transparentPen,expBrushUp);
                else temp = scene->addRect(0,0,x,y*j,transparentPen,expBrushDwn);
            }
            else{
                if(j==-1) temp = scene->addRect(0,0,x*j,y,transparentPen,expBrushLeft);
                else temp = scene->addRect(0,0,x*j,y,transparentPen,expBrushRight);
            }
            temp->setPos(xExp,yExp);
            explosion.append(temp);
        }
        xExp=xBomb+(bomberSize/2),yExp=yBomb;
        int aux=y;
        y=x,x=aux;
    }


    //DETECCION DE COLLISION PARA SABER QUE SE DEBE ELIMINAR DE LA ESCENA
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
    }

    tempTimer = new QTimer();
    connect(tempTimer,SIGNAL(timeout()),this,SLOT(removeExplosions()));
    tempTimer->start(750);
    scene->removeItem(bomb);
    timer->~QTimer();
}
