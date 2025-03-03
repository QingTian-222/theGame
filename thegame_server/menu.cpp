#include "menu.h"
#include "ui_menu.h"
#include "qtcpsocket"
#include "QTcpServer"
struct Player{
    QString name="unknown";//用户名
    QString ip;//ipv4
    int id;//几号位
    QString room="";//房间号
    QTcpSocket* socket;
};

struct Room{
    QString roomId;
    int seed;
    Room(QString id,Player* host){
        roomId=id;
        seed=(unsigned)time(0);
        players.append(host);
    }
    QVector<Player*> players;
};

QVector<Room*> rooms;
QVector<Player*> players;
QTcpServer *server;
QVector<QByteArray> buffers;
QPlainTextEdit* logger;
menu::menu(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::menu)
{
    ui->setupUi(this);
    logger=ui->log;
}

menu::~menu()
{
    delete ui;
}
void lg(QString s){
    QString str=logger->toPlainText();
    str+=s+'\n';
    logger->setPlainText(str);
}
QByteArray menu::send(QString head,QString content,QString name){
    QString hd=head+content;
    QString len="000"+QString::number(hd.toUtf8().size()+3);
    hd=len.right(3)+hd;
    lg("server->"+name+": "+hd);
    return hd.toUtf8();
}
Room* getRoomById(QString room){
    int roomIdx=-1;
    for(roomIdx=0;roomIdx<rooms.size();roomIdx++){
        if(rooms[roomIdx]->roomId==room) break;
    }
    if(roomIdx==-1){
        lg("房间"+room+"未找到");
        return NULL;
    }
    return rooms[roomIdx];
}
void menu::broadCast_roomData(QString room){
    Room* rm=getRoomById(room);
    if(rm==NULL) return;
    QString data;
    int n=rm->players.size();
    for(int i=0;i<n;i++){
        data+=rm->players[i]->name;
        if(i!=n-1) data+=',';
    }
    for(int i=0;i<n;i++){
        rm->players[i]->socket->write(send("ROM",data,players[i]->name));
    }
}
void menu::broadCast(QString category,QString data,QString room){
    Room* rm=getRoomById(room);
    if(rm==NULL) return;
    int n=rm->players.size();
    for(int i=0;i<n;i++){
        rm->players[i]->socket->write(send(category,data,players[i]->name));
    }
}

void menu::on_pushButton_clicked()
{
    server=new QTcpServer(this);

    server->listen(QHostAddress::Any,ui->data_2->text().toInt());
    ui->pushButton->setDisabled(true);
    ui->pushButton->setText("LISTENING...");

    connect(server,&QTcpServer::newConnection,this,[=](){
        QTcpSocket* socket=server->nextPendingConnection();
        Player *temp=new Player;
        temp->socket=socket;
        temp->id=players.indexOf(temp);
        players.append(temp);
        buffers.append("");
        connect(socket,&QTcpSocket::readyRead,this,[=](){
            int idx=players.indexOf(temp);
            buffers[idx].append(socket->readAll());
            while(1){
                if(buffers[idx].size()<3) return;
                bool ok=false;
                int len=buffers[idx].mid(0,3).toInt(&ok);
                if(!ok) return;
                if(buffers[idx].size()<len) return;
                QString category=buffers[idx].mid(3,3);
                QString data=buffers[idx].mid(6,len-6);
                buffers[idx]=buffers[idx].mid(len);
                lg(temp->name+": "+category+data);

                if(category=="HEL"){//基础信息(name,room)
                    QStringList strs=data.split(",");
                    temp->name=strs[0];
                    temp->room=strs[1];

                    //加入房间
                    bool haveCreated=false;
                    for(int i=0;i<rooms.size();i++){
                        if(rooms[i]->roomId==temp->room){
                            haveCreated=true;
                            temp->id=rooms[i]->players.size();
                            rooms[i]->players.append(temp);
                            lg(temp->name+"加入了房间"+temp->room);
                            break;
                        }

                    }
                    if(!haveCreated){//创建房间
                        Room* rm=new Room(temp->room,temp);
                        rooms.append(rm);
                        temp->id=0;
                        lg(temp->name+"创建了房间"+temp->room);
                    }


                    //广播房间信息
                    broadCast_roomData(temp->room);
                    broadCast("SED",QString::number(getRoomById(temp->room)->seed),temp->room);
                }
                else if(category=="RDY"){//准备好了,开始游戏
                    broadCast("STG","",temp->room);
                }else if(category=="SHW"){
                    broadCast("SHW",data,temp->room);
                }else if(category=="DEF"){
                    broadCast("DEF",data,temp->room);
                }

            }

        });

        connect(socket,&QTcpSocket::disconnected,this,[=](){
            Room* rm=getRoomById(temp->room);
            if(rm==NULL) return;
            for(int i=0;i<rm->players.size();i++){
                if(rm->players[i]->name==temp->name){
                    rm->players.erase(rm->players.begin()+i);
                    break;
                }
            }
            lg(temp->name+"断开连接");
            if(rm->players.empty()){
                rooms.erase(rooms.begin()+rooms.indexOf(rm));
                lg(rm->roomId+"解散了");
            }
            else{
                broadCast_roomData(temp->room);
            }

        });

    });
}


void menu::on_pushButton_2_clicked()
{
    ui->log->clear();
}


void menu::on_pushButton_3_clicked()
{
    auto rm =getRoomById(ui->room->text());
    rm->seed=(unsigned)time(0);
    broadCast("SED",QString::number(getRoomById(ui->room->text())->seed),ui->room->text());
    broadCast(ui->data->text().left(3),ui->data->text().right(ui->data->text().size()-3),ui->room->text());
}

