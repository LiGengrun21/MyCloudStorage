#ifndef ONLINE_H
#define ONLINE_H

#include <QWidget>
#include "protocol.h"

// online users
namespace Ui {
class Online;
}

class Online : public QWidget
{
    Q_OBJECT

public:
    explicit Online(QWidget *parent = nullptr);
    ~Online();
    void showUsers(PDU *pdu);

private:
    Ui::Online *ui;
};

#endif // ONLINE_H
