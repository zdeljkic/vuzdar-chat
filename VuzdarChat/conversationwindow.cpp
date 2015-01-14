#include "conversationwindow.h"
#include "ui_conversationwindow.h"

ConversationWindow::ConversationWindow(QString title, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ConversationWindow)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_QuitOnClose, false);

    setWindowTitle(title);
    ui->messageText->installEventFilter(this);
}

ConversationWindow::~ConversationWindow()
{
    delete ui;
}

QString ConversationWindow::time()
{
    QString timeString = QTime::currentTime().toString("HH:mm");
    timeString.prepend("<font color = gray>");
    timeString.append("</font>&nbsp;");

    return timeString;
}

bool ConversationWindow::eventFilter(QObject *o, QEvent *e)
{
    if (o == ui->messageText && e->type() == QEvent::KeyPress) {
        QKeyEvent *ke = (QKeyEvent *) e;

        if (ke->key() == Qt::Key_Return && ke->modifiers() != Qt::ShiftModifier) {
            if (!ui->messageText->toPlainText().isEmpty())
                on_sendButton_clicked();
            return true;
        } else {
            return QWidget::eventFilter(o, e);
        }
    } else {
        return QWidget::eventFilter(o, e);
    }
}

void ConversationWindow::showSystemMessage(QString message, QString color)
{
    message = message.toHtmlEscaped();
    message.replace(QString("\n"), QString("<br>"));
    message.replace(QString(" "), QString("&nbsp;"));
    ui->conversationText->append(QString("<font color=" + color + ">" + message + "</font>"));
}

void ConversationWindow::showClientMessage(QString name, QString message, QString color)
{
    message = message.toHtmlEscaped();
    message.replace(QString("\n"), QString("<br>"));
    message.replace(QString(" "), QString("&nbsp;"));
    ui->conversationText->append(QString(time() + "<font color=" + color + ">" + name + ": " + "</font>" + message));
}

void ConversationWindow::on_saveConversationButton_clicked()
{
    emit saveHtmlConversation(ui->conversationText->toHtml());
}

void ConversationWindow::on_sendButton_clicked()
{
    emit sendMessage(ui->messageText->toPlainText().left(10000));
    ui->messageText->setHtml("");
}
