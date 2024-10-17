#ifndef OPEWIDGET_H
#define OPEWIDGET_H
#include <QWidget>
#include <QListWidget>
#include "book.h"
#include "friend.h"
#include <QStackedWidget>

// operate widget, the main window of the applicaiton
class OpeWidget : public QWidget
{
    Q_OBJECT
public:
    explicit OpeWidget(QWidget *parent = nullptr);
    static OpeWidget &getInstance();
    Friend* getFriend();

private:
    QListWidget *m_pListWidget; // list widget on the left
    Friend *m_pFriend;
    Book *m_pBook;
    QStackedWidget *m_pSW; // stacked widget on the right, contaning friend and book, but only show one of them at one time.

signals:
};

#endif // OPEWIDGET_H
