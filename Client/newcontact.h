#ifndef NEWCONTACT_H
#define NEWCONTACT_H

#include <QWidget>
#include <QTcpSocket>

namespace Ui {
class NewContact;
}

class NewContact : public QWidget
{
    Q_OBJECT

public:
    explicit NewContact(QWidget *parent = 0);
    ~NewContact();

private slots:
    void on_accept_button_clicked();
    void getMessage();

signals:
    void sendData(QString str);

private:
    QTcpSocket *socket;
    Ui::NewContact *ui;
};

#endif // NEWCONTACT_H
