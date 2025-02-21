#ifndef MENU_H
#define MENU_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class Menu; }
QT_END_NAMESPACE

class Menu : public QMainWindow
{
    Q_OBJECT

public:
    Menu(QWidget *parent = nullptr);
    ~Menu();

    void init();
    void setup();
    void dealCard();
    void modifyMe();
    void modifyPlayer(int player);
    bool eventFilter(QObject *obj, QEvent *event);
    void move(int id);
    void leave(int id);
    void click(int id);

    void finish();
    void mousemove(int id, QPointF p);
    void playerStart();
    void connect_server(QString name, QString ip, QString room);
    void release(int id, QPointF p, QPoint loc);
    void releaseFun(int id, QPoint loc);
private slots:
    void on_finishButton_clicked();

    void on_sortButton_clicked();

    void on_connect_clicked();

    void on_startgame_clicked();

private:
    Ui::Menu *ui;
};
#endif // MENU_H
