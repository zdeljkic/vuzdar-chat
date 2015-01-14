#ifndef CONVERSATIONWINDOW_H
#define CONVERSATIONWINDOW_H

#include <QWidget>
#include <QEvent>
#include <QKeyEvent>
#include <QDebug>

namespace Ui {
class ConversationWindow;
}

class ConversationWindow : public QWidget
{
    Q_OBJECT

public:
    explicit ConversationWindow(QString title,QWidget *parent = 0);
    ~ConversationWindow();

private:
    Ui::ConversationWindow *ui;

protected:
    virtual bool eventFilter(QObject *o, QEvent *e);

signals:
    void saveHtmlConversation(QString conversation);
    void sendMessage(QString message);

public slots:
    void showSystemMessage(QString message, QString color = QString("black"));
    void showClientMessage(QString name, QString message, QString color = QString("black"));

private slots:
    void on_saveConversationButton_clicked();
    void on_sendButton_clicked();
};

#endif // CONVERSATIONWINDOW_H
