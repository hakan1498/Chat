#ifndef REGISTRATION_H
#define REGISTRATION_H

#include <QDialog>
#include <QKeyEvent>
#include "newcontact.h"

namespace Ui {
class registration;
}

class registration : public QWidget
{
    Q_OBJECT

public:
    explicit registration(QWidget *parent = 0);
    virtual void keyPressEvent(QKeyEvent* event) { key = event->key(); }
    virtual void keyReleaseEvent(QKeyEvent* event);

    ~registration();
    bool ok;
    Ui::registration *ui;
    NewContact *reg;
private:
     int key;

signals:
    void sendData(QString str, QString pas);
    void sendFindContact(QString str);


private slots:
    void onButtonSend();
    void onButtonSendUser();
    void recieveData(QString str);

private slots:
    void on_pushButton_clicked();
    void on_reg_button_clicked();
};

#endif // REGISTRATION_H
