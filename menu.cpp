#include "menu.h"
#include "ui_menu.h"
#include "QEvent"
#include "QMouseEvent"
#include "dlabel.h"
#include "QTcpSocket"

#include <qfontdatabase.h>
#include <qmessagebox.h>
#include <qpropertyanimation.h>
typedef QPropertyAnimation ani;
QString sty[10]={
    "border-radius:10px;color: rgb(13, 135, 17);border:2px solid rgb(13, 135, 17);background: qlineargradient(x1: 0, y1: 1, x2: 0, y2: 0,stop: 0 #96fbc4,stop: 1 #f9f586);",
    "border-radius:10px;color: rgb(104, 0, 104);border:2px solid rgb(104, 0, 104);background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:1, stop:0 rgba(250, 208, 196, 255), stop:1 rgba(255, 194, 255, 255));",
    "border-radius:10px;color: rgb(191, 0, 0);border:2px solid rgb(191, 0, 0);background: qlineargradient(x1: 0, y1: 1, x2: 1, y2: 0, stop: 0 #f6d365, stop: 1 #fda085);",
    "border-radius:10px;color: rgb(255, 85, 0);border:2px solid rgb(255, 85, 0);background: qlineargradient(x1: 0.928, y1: 1, x2: 0.071, y2: 0, stop: 0.1 #FFA8A8, stop: 1 #FCFF00);",
    "border-radius:10px;color: rgb(55, 51, 175);border:2px solid rgb(55, 51, 175);background: qlineargradient(x1: 0, y1: 1, x2: 1, y2: 0, stop: 0 #e0c3fc,  stop: 1 #8ec5fc);",
    "border-radius:10px;color: rgb(3, 84, 2);border:2px solid rgb(3, 84, 2);background: qlineargradient(x1: 0.928, y1: 1, x2: 0.071, y2: 0, stop: 0.1 #FFF720, stop: 1 #3CD500);",
    "border-radius:10px;color: rgb(0, 44, 166);border:2px solid rgb(0, 44, 166);background: qlineargradient(x1: 0, y1: 0, x2: 1, y2: 0, stop: 0 #4facfe, stop: 1 #00f2fe);",
    "border-radius:10px;color: rgb(63, 57, 95);border:2px solid rgb(63, 57, 95);background: qlineargradient(x1: 0, y1: 1, x2: 0, y2: 0, stop: 0 #37ecba, stop: 1 #72afd3);",
    "border-radius:10px;color: rgb(81, 81, 81);border:2px solid rgb(81, 81, 81);background: qlineargradient(x1: 0, y1: 1, x2: 0, y2: 0, stop: 0 #accbee, stop: 1 #e7f0fd);",
    "border-radius:10px;color: rgb(0, 0, 0);border:2px solid;background: qlineargradient(x1: 0, y1: 1, x2: 0, y2: 0, stop: 0 #f3e7e9, stop: 0.99 #e3eeff, stop: 1 #e3eeff);"
};
QString bk="border-radius:10px;background: qlineargradient(x1: 0, y1: 1, x2: 1, y2: 0, stop: 0 #434343, stop: 1 black);color: rgb(255, 255, 255);font: 500 81pt \"Ancient\";";
QString ft[2]={
    "font: 500 90pt \"Ancient\";",
    "font: 500 75pt \"Ancient\";"
};
QTcpSocket* socket;
QVector<DLabel*> cards(200),counts(200);
QVector<DLabel*> handCard[10];
QVector<QGroupBox*> gps;
QVector<QLabel*> nms,bgs;
int player_num=1;//游戏人数
int cur_player=0;//当前玩家
int me=0;
int max_Card=7;//手牌上限
int seed;

QVector<DLabel*> gameCard;//摸牌区
void turnOn(DLabel* label){//翻开
    label->isOpen=true;
    int id=label->id;
    label->setStyleSheet(sty[(id-1)/10]+ft[id>=10]);
    label->setText(QString::number(id));
}
void turnOff(DLabel* label){//关牌
    label->isOpen=false;
    label->setStyleSheet(bk);
    label->setText("?");
}
int ran(int l,int r){
    return rand()%(r-l+1)+l;
}
template <typename T>
void ma(T* label,QRect ed,int duration=300,QEasingCurve curve=QEasingCurve::OutCubic){
    ani* an=new ani(label,"geometry");
    an->setEndValue(ed);
    an->setDuration(duration);
    an->setEasingCurve(curve);
    an->start();
}
template <typename T>
void fadeLabel(T* label,int duration,float end){//透明度
    label->setVisible(true);
    QGraphicsOpacityEffect* opa=new QGraphicsOpacityEffect(label);
    label->setGraphicsEffect(opa);
    QPropertyAnimation* animation = new QPropertyAnimation(opa,"opacity",label);
    animation->setDuration(duration);
    animation->setEndValue(end);
    animation->start(QAbstractAnimation::DeleteWhenStopped);
    QObject::connect(animation,&QPropertyAnimation::finished,[=](){
        if(end==0)
            label->setVisible(false);
    });
}
void openCard(DLabel *label,bool isOn){//翻牌动画
    int wid=label->width();
    int aimw=0,aimx=label->x()+wid/2;
    QRect startRect=label->geometry();
    QRect aimRect={aimx,label->y(),aimw,label->height()};
    ma(label,aimRect,200,QEasingCurve::InBack);
    QTimer::singleShot(200,[=](){
        if(isOn) turnOn(label);
        else turnOff(label);
        ma(label,startRect,200,QEasingCurve::OutBack);
    });
}
void openMiniCard(DLabel *label){//翻牌动画
    if(label->isOpen) return;

    label->isOpen=1;
    int wid=label->width();
    int aimw=0,aimx=label->x()+wid/2;
    QRect startRect=label->geometry();
    QRect aimRect={aimx,label->y(),aimw,label->height()};
    ma(label,aimRect,200,QEasingCurve::InBack);
    QTimer::singleShot(200,[=](){
        int id=label->id;
        label->setText(" "+QString::number(id)+" ");
        label->setStyleSheet(sty[(id-1)/10]+"font: 500 "+QString::number(id<10?40:35)+"pt \"Ancient\";");

        ma(label,startRect,200,QEasingCurve::OutBack);
    });
}
QByteArray send(QString head,QString content){
    QString hd=head+content;
    QString len="000"+QString::number(hd.toUtf8().size()+3);
    hd=len.right(3)+hd;
    return hd.toUtf8();
}
void Menu::dealCard(){

    QTimer *timer=new QTimer();
    int startPlayer=0;
    connect(timer,&QTimer::timeout,this,[=]() mutable{
        bool all_full=1;
        for(int i=0;i<player_num;i++){
            if(handCard[i].size()<max_Card){
                all_full=0;
                break;
            }
        }
        if(!all_full){
            while(1){
                if(gameCard.size()==0){
                    break;
                }
                if(handCard[startPlayer].size()<max_Card){
                    gameCard.back()->raise();
                    handCard[startPlayer].push_back(gameCard.back());
                    gameCard.pop_back();
                    ui->gamebar->setValue(gameCard.size());
                    if(startPlayer==me) modifyMe();
                    else modifyPlayer(startPlayer);
                    ui->gamebar->setValue(gameCard.size());
                    startPlayer+=1;
                    startPlayer%=player_num;
                    break;
                }else{
                    startPlayer+=1;
                    startPlayer%=player_num;
                }

            }
        }else{
            QTimer::singleShot(300,this,[=](){
                QTimer* timer2=new QTimer;
                int i=0;
                connect(timer2,&QTimer::timeout,this,[=]() mutable{//翻开未翻开的手牌
                    if(i<(int)handCard[me].size()){
                        if(!handCard[me][i]->isOpen){
                            openCard(handCard[me][i],1);
                        }
                    }else{
                        timer2->stop();
                        timer2->deleteLater();
                        timer->deleteLater();
                    }
                    i++;
                });
                timer2->start(40);
            });
            timer->stop();

        }
    });
    timer->start(120/player_num);

}
void Menu::modifyMe(){
    int allW=this->width();
    int oneW=123;
    int n=handCard[me].size();
    for(int i=n-1;i>=0;i--){
        int x= (allW-oneW*n)/2 + oneW*i;
        int y=840,w=121,h=161;
        ma(handCard[me][i],{x,y,w,h},300);
    }
}
void Menu::modifyPlayer(int player){
    QRect loc[4];
    loc[(me+3)%player_num]={660,-180,121,161};
    loc[(me+2)%player_num]={360,-180,121,161};
    loc[(me+1)%player_num]={60,-180,121,161};
    qDebug()<<player;
    for(auto i:handCard[player]) ma(i,loc[player],300);

}

void Menu::init(){
    ui->finishButton->setVisible(false);
    ui->sortButton->setVisible(false);
    ui->startgame->setVisible(false);
    ui->title_2->setVisible(false);
    ui->viewer->setVisible(false);
    installEventFilter(this);
    gps.append(ui->playergroup);gps.append(ui->playergroup_2);
    gps.append(ui->playergroup_3);gps.append(ui->playergroup_4);
    nms.append(ui->playername);nms.append(ui->playername2);
    nms.append(ui->playername3);nms.append(ui->playername4);
    bgs.append(ui->playerbg1);bgs.append(ui->playerbg2);
    bgs.append(ui->playerbg3);bgs.append(ui->playerbg4);
    for(auto i:gps) i->setVisible(false);
    for(auto i:bgs) i->setVisible(false);
    ui->roomtitle->setVisible(false);
    cur_player=0;
    for(int i=2;i<=99;i++){
        DLabel* label=new DLabel(ui->centralwidget);
        label->id=i;
        label->setGeometry(80,360,121,161);
        label->installEventFilter(this);
        label->lower();
        label->setVisible(true);
        turnOff(label);
        cards[i]=label;
    }

    for(int i=1;i<=10;i++){
        for(int j=1;j<=10;j++){
            DLabel* label = new DLabel(ui->viewer);

            int padw=480;//左间隔
            int padh=10;
            int padww=5;//横排缝隙
            int padhh=8;
            int w=(this->width()-padw*2-9*padww)/10;
            int h=(this->height()-padh*2-9*padhh)/10;

            int x=padw+(w+padww)*(i-1);
            int y=padh+(h+padhh)*(j-1);
            label->setGeometry(x,y,w,h);
            int tt=(j-1)*10+i;
            label->id=tt;
            label->setText("?");
            if(tt==1){
                label->setText(" "+QString::number(tt)+" ");
                label->setStyleSheet(ui->up_1->styleSheet()+"color:rgb(255,255,255);font: 500 40pt \"Ancient\";");
            }else if(tt==100){
                label->setText(" "+QString::number(tt)+" ");
                label->setStyleSheet(ui->dw_1->styleSheet()+"color:rgb(255,255,255);font: 500 30pt \"Ancient\";");
            }else label->setStyleSheet(bk+"color:rgb(255,255,255);font: 500 40pt \"Ancient\";");
            counts[tt]=label;
        }
    }


    ui->bg->raise();
    ui->group->raise();
    ui->title->raise();
    ui->roomtitle->raise();
    ui->startgame->raise();
}
void Menu::setup(){
    srand(seed);
    for(int i=2;i<=99;i++){

        if(cards[i]->isOpen){
            openCard(cards[i],0);
        }
        cards[i]->dragable=false;
        cards[i]->inHand=true;
        cards[i]->used=false;
        counts[i]->used=0;
        counts[i]->setStyleSheet(bk+"color:rgb(255,255,255);font: 500 40pt \"Ancient\";border:2px solid rgb(80,80,80);");
        counts[i]->setText("?");
    }
    QTimer::singleShot(400,[=](){
        for(int i=2;i<=99;i++){
            cards[i]->dragable=false;
            ma(cards[i],QRect(80,360,121,161),200);

        }
        //随机化
        for(int i=0;i<player_num;i++) handCard[i].clear();
        gameCard.clear();
        for(int i=2;i<=99;i++) gameCard.append(cards[i]);

        int n=gameCard.size();
        for(int i=0;i<n;i++) std::swap(gameCard[i],gameCard[ran(0,n-1)]);
        for(int i=0;i<n;i++) gameCard[i]->raise();
        playerStart();
        dealCard();
    });


}

void Menu::playerStart(){
    ui->finishButton->setVisible(true);
    ui->sortButton->setVisible(true);
    ui->finishButton->setEnabled(me==cur_player);

    if(me==cur_player){

        fadeLabel(ui->title_2,500,1);
        ui->title_2->raise();
        QTimer::singleShot(900,[=](){
            fadeLabel(ui->title_2,400,0);
        });
    }

}
Menu::Menu(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Menu)
{
    QFontDatabase::addApplicationFont(QCoreApplication::applicationDirPath()+"/font.ttf");
    QFontDatabase::addApplicationFont(QCoreApplication::applicationDirPath()+"/AGENCYB.TTF");
    QFontDatabase::addApplicationFont(QCoreApplication::applicationDirPath()+"/AGENCYR.TTF");
    ui->setupUi(this);
    init();




}

Menu::~Menu()
{
    delete ui;
}
void Menu::move(int id){//鼠标进入
    if(!cards[id]->used && cards[id]->isOpen){
        int allW=this->width();
        int oneW=123;
        int n=handCard[me].size();
        cards[id]->raise();
        for(int i=0;i<n;i++){
            int x= (allW-oneW*n)/2 + oneW*i;
            int y=760;
            if(handCard[me][i]->id==id){
                ma(handCard[me][i],{x,y,121,161},250,QEasingCurve::OutBack);
            }
        }
    }


}
void Menu::leave(int id){//鼠标退出
    if(!cards[id]->used && cards[id]->isOpen){
        int allW=this->width();
        int oneW=123;
        int n=handCard[me].size();
        cards[id]->raise();
        for(int i=0;i<n;i++){
            int x= (allW-oneW*n)/2 + oneW*i;
            int y=840;
            if(handCard[me][i]->id==id){
                ma(handCard[me][i],{x,y,121,161},250);
            }
        }
    }

}
void Menu::click(int id){//鼠标点击
    if(cards[id]->isOpen && cur_player==me){
        cards[id]->raise();
        for(auto i:handCard[me]){
            if(i->id==id){
                i->dragable=1;
            }
        }
    }

}
void Menu::releaseFun(int id,QPoint loc){
    cards[id]->used=1;
    turnOn(cards[id]);
    cards[id]->raise();
    for(int ii=0;ii<player_num;ii++){
        int n=handCard[ii].size();
        for(int i=0;i<n;i++){
            if(handCard[ii][i]->id==id){
                handCard[ii].erase(handCard[ii].begin()+i);
                break;
            }
        }
    }
    ma(cards[id],{loc.x(),loc.y(),121,161},300);
    counts[id]->used=1;
}
void Menu::release(int id,QPointF p,QPoint loc){
    if(cur_player==me){
        if(p.y()<=790 || cards[id]->used){
            QString s="";
            s=QString::number(id)+","+QString::number(loc.x())+","+QString::number(loc.y());
            socket->write(send("SHW",s));

        }
        modifyMe();
    }

}
void Menu::mousemove(int id,QPointF p){
    if(cards[id]->dragable && !cards[id]->used && cur_player==me){
        if(p.y()<=790){//场外
            if(cards[id]->inHand){//从手牌区移动到场上
                cards[id]->inHand=false;
                for(int i=0;i<handCard[me].size();i++){
                    if(handCard[me][i]->id==id){
                        handCard[me].erase(handCard[me].begin()+i);
                        break;
                    }
                }

                modifyMe();
            }
        }else{
            bool tt=0;
            if(!cards[id]->inHand){//从手牌区移动到场上
                cards[id]->inHand=true;
                handCard[me].append(cards[id]);
                tt=1;
            }

            int allW=this->width();
            int oneW=123;
            int n=handCard[me].size();

            int idx=handCard[me].indexOf(cards[id]);
            int moveidx=0;

            if(p.x()<(allW-oneW*n)/2) moveidx=0;
            else if(p.x()>=(allW+oneW*n)/2) moveidx=n-1;
            else{
                for(int i=0,j=1;j<=n;i++,j++){
                    int l=(allW-oneW*n)/2 + oneW*i;
                    int r=(allW-oneW*n)/2 + oneW*j;
                    if(l<=p.x() && p.x()<r){
                        moveidx=i;
                    }
                }
            }
            if(idx!=moveidx || tt){
                handCard[me].erase(handCard[me].begin()+idx);
                handCard[me].insert(moveidx,cards[id]);

                for(int i=0;i<n;i++){
                    if(i==moveidx) continue;
                    int x= (allW-oneW*n)/2 + oneW*i;
                    int y=840,w=121,h=161;
                    ma(handCard[me][i],{x,y,w,h},250);
                }
            }

        }
    }

}
bool Menu::eventFilter(QObject *obj, QEvent *event) {
    // 处理Tab键按下事件
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
        if (keyEvent->key() == 16777249) {
            ui->viewer->raise();
            ui->viewer->setVisible(true);
            fadeLabel(ui->viewer,100,1);
            QTimer::singleShot(100,[=](){
                for(int i=2;i<=99;i++){
                    if(cards[i]->used) openMiniCard(counts[i]);
                }
            });

            return true; // 阻止事件继续传播
        }
    }
    // 处理Tab键释放事件
    else if (event->type() == QEvent::KeyRelease) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
        if (keyEvent->key() == 16777249) {
            ui->viewer->setVisible(false);
            fadeLabel(ui->viewer,100,0);
            return true; // 阻止事件继续传播
        }
    }

    // 处理DLabel相关事件
    DLabel* label=qobject_cast<DLabel*>(obj);
    if(label){

        if (event->type() == QEvent::MouseButtonPress) {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
            if (mouseEvent->buttons() == Qt::LeftButton){
                click(label->id);
            }
        }else if (event->type() == QEvent::Enter) {
            move(label->id);
        } else if (event->type() == QEvent::Leave) {
            leave(label->id);
        }else if(event->type() == QEvent::MouseButtonRelease) {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
            release(label->id,mouseEvent->scenePosition(),label->pos());
        }else if(event->type() == QEvent::MouseMove){
            QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
            mousemove(label->id,mouseEvent->scenePosition());
        }

        return QObject::eventFilter(obj, event);
    }

    // 其他未处理的事件交给基类处理
    return QObject::eventFilter(obj, event);
}
void Menu::finish(){
    for(auto i:handCard[cur_player])
        i->dragable=false;
    (cur_player+=1)%=player_num;

    dealCard();
    playerStart();
}




QByteArray buffer;
void Menu::connect_server(QString name,QString ip,QString room){
    socket = new QTcpSocket(this);
    int port=8080;
    socket->connectToHost(ip,port);
    connect(socket,&QTcpSocket::connected,this,[=](){//连接成功
        ui->roomtitle->setText("Room : "+ui->room->text());
        ui->connect->setEnabled(false);
        ui->connect->setText("Connected!");
        socket->write(send("HEL",name+","+room));//发送身份验证

        //动画
        fadeLabel(ui->group,700,0);
        QTimer::singleShot(300,[=](){
            for(auto i:gps) fadeLabel(i,500,1),i->raise();
            fadeLabel(ui->roomtitle,500,1);
        });

    });
    connect(socket,&QTcpSocket::readyRead,this,[=](){
        buffer.append(socket->readAll());
        qDebug()<<"receive:"<<buffer;
        while(1){
            if(buffer.size()<3) return;
            bool ok=false;
            int len=buffer.mid(0,3).toInt(&ok);
            if(!ok) return;
            if(buffer.size()<len) return;
            QString category=buffer.mid(3,3);
            QString data=buffer.mid(6,len-6);
            buffer=buffer.mid(len);

            if(category=="ROM"){//房间信息

                QStringList strs=data.split(",");
                player_num=strs.size();
                for(int i=0;i<4;i++){
                    if(i<strs.size()){
                        if(strs[i]==name){
                            me=i;
                            if(me==0 && player_num>=1){//房主
                                fadeLabel(ui->startgame,300,1);
                            }
                        }
                        nms[i]->setVisible(true);
                        nms[i]->setText(strs[i]);
                        bgs[i]->setVisible(true);
                    }else{
                        nms[i]->setVisible(false);
                        bgs[i]->setVisible(false);
                    }


                }

            }else if(category=="SED"){
                seed=data.toInt();
            }else if(category=="STG"){//开始游戏
                fadeLabel(ui->bg,700,0);
                for(auto i:gps) fadeLabel(i,500,0);
                fadeLabel(ui->startgame,500,0);
                fadeLabel(ui->roomtitle,500,0);
                fadeLabel(ui->title,1000,0);
                QTimer::singleShot(500,[=](){
                    setup();
                });
            }else if(category=="SHW"){
                QStringList strs=data.split(",");
                releaseFun(strs[0].toInt(),{strs[1].toInt(),strs[2].toInt()});
            }else if(category=="DEF"){
                finish();
            }else if(category=="RST"){
                setup();
            }

        }
    });
}


void Menu::on_finishButton_clicked()
{
    socket->write(send("DEF",""));
}


void Menu::on_sortButton_clicked()
{
    std::sort(handCard[me].begin(),handCard[me].end(),[&](DLabel* a,DLabel* b){
        return a->id<b->id;
    });
    modifyMe();
}


void Menu::on_connect_clicked()
{
    if(ui->name->text()==""){
        QMessageBox::information(this,"","名字不能为空");
        return;
    }
    ui->table->lower();
    connect_server(ui->name->text(),ui->ip->text(),ui->room->text());
}


void Menu::on_startgame_clicked()
{

    socket->write(send("RDY",""));
}

