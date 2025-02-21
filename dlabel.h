#ifndef DLABEL_H
#define DLABEL_H

#include <QDir>
#include <QGraphicsDropShadowEffect>
#include <QLabel>
#include <QPainter>
#include <QPainterPath>
#include <qpropertyanimation.h>
#include "QEvent"
#include "QMouseEvent"
#include "QTimer"
class DLabel : public QLabel {
    Q_OBJECT
public:
    DLabel(QWidget* parent = nullptr) : QLabel(parent) {
        setAlignment(Qt::AlignCenter);
    }

    int id;
    bool dragable=false;//是否可移动
    bool isOpen=false;//是否翻面
    bool inHand=true;//是否在手牌中
    bool used=false;//是否打出





private:
    void paintEvent(QPaintEvent* event) override {

        QLabel::paintEvent(event); // 保留 QLabel 的默认绘制行为

        if(this->text()!="0" && this->text()!="100" && this->text()!="?"){
            QPainter painter(this);
            QFont font("Agency FB", 18);
            font.setBold(true);
            painter.setFont(font);
            painter.setPen(Qt::black);

            painter.drawText((this->text().size()==1?8:5), 25, this->text());
            painter.drawText(width() - (this->text().size()==1?15:25), height() - 6, this->text()); // 右下角
        }

    }
    void mousePressEvent(QMouseEvent* event) override {
        if(dragable){
            if (event->button() == Qt::LeftButton) {
                //this->raise();
                offset = event->position().toPoint();
                dragging = true;
            }
        }

    }

    void mouseMoveEvent(QMouseEvent* event) override {
        if(dragable){
            if (dragging && event->buttons() & Qt::LeftButton) {
                mouseAt=event->scenePosition().toPoint();
                move(mouseAt - offset);
            }
        }

    }

    void mouseReleaseEvent(QMouseEvent* event) override {
        if(dragable){
            if (event->button() == Qt::LeftButton) {
                dragging = false;
            }
        }

    }



private:
    bool dragging = false;  // 标志是否正在拖动
    QPoint offset,mouseAt;          // 鼠标相对标签的位置
    QTimer* aniTimer;

};
#endif // DLABEL_H
