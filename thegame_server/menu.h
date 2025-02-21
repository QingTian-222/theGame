#ifndef MENU_H
#define MENU_H

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class menu; }
QT_END_NAMESPACE

class menu : public QWidget
{
    Q_OBJECT

public:
    menu(QWidget *parent = nullptr);
    ~menu();


    void broadCast_roomData(QString room);

    QByteArray send(QString head, QString content, QString name);

    void broadCast(QString category, QString data, QString room);
private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

private:
    Ui::menu *ui;
};
#endif // MENU_H
