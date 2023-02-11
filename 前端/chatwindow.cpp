#include "chatwindow.h"
#include "ui_chatwindow.h"

const int WAIT_FOR_CONNECT_FILE_RECEIVER_SEC = 60;
const int CHAR_LENFTH_LIMITH = 2000;

ChatWindow::ChatWindow(QWidget *parent, TcpConnection *_conn) :
    QWidget(parent),
    ui(new Ui::ChatWindow),
    receiver_(nullptr)
{
    ui->setupUi(this);
    this->conn = _conn;
    QString title = "Fake Chat";
    title += this->conn->getUserName();
    this->setWindowTitle(title);
    ui->sendmsgbut->setStyleSheet("QPushButton{background-color: rgb(255, 255, 255) ;border:2px groove gray;border-radius:10px;padding:2px 4px;border-style: outset;}"
                                  "QPushButton:hover{background-color:rgb(229, 241, 251); color: black;}"
                                  "QPushButton:pressed{background-color:rgb(219, 231, 255);border-style: inset;}");
    ui->sendfile->setStyleSheet("QPushButton{background-color: rgb(255, 255, 255) ;border:2px groove gray;border-radius:10px;padding:2px 4px;border-style: outset;}"
                                "QPushButton:hover{background-color:rgb(229, 241, 251); color: black;}"
                                "QPushButton:pressed{background-color:rgb(219, 231, 255);border-style: inset;}");
    //ui->textBrowser->hide();
    ui->sendfile->setEnabled(false);
    connect(this->conn, SIGNAL(receiveMsg()), this, SLOT(receiveMsgCallback()));
    connect(this->conn, SIGNAL(memberChange()), this, SLOT(memberChangeCallback()));
    connect(this->conn, SIGNAL(fileRecive()), this, SLOT(receiveFileCallback()));
}

ChatWindow::~ChatWindow()
{
    delete ui;
}

void ChatWindow::on_sendmsgbut_clicked()
{
    QString msg = ui->sendEdit->document()->toPlainText();
    if(msg.length() >= CHAR_LENFTH_LIMITH)
    {
        QMessageBox::information(this, "提示",tr("信息长度应小于%1个字符").arg(CHAR_LENFTH_LIMITH));
        return;
    }
    QString time = QDateTime::currentDateTime().toString("yyyy/MM/dd hh:mm:ss");
    if(this->conn)
    {
        QJsonObject json;
        json.insert("sender",this->conn->getUserName());
        json.insert("receiver",this->topChatwindow_);
        json.insert("label","message");
        json.insert("time",time);
        json.insert("content",msg);
        QJsonDocument doc(json);
        this->conn->sendMessage(doc.toJson());
        this->conn->addMessageList(time, this->conn->getUserName(), this->topChatwindow_, msg);
        ui->sendEdit->clear();
        QString display = time;
        display += " - [";
        display += this->conn->getUserName();
        display += "]: ";
        ui->textBrowser->append(display);
        ui->textBrowser->append(msg);
        ui->textBrowser->append("");
    }
}

void ChatWindow::on_listWidget_itemClicked(QListWidgetItem *item)
{
    //切换显示信息
    if(this->topChatwindow_ != item->text())
    {
        ui->textBrowser->clear();
        this->topChatwindow_ = item->text();
        if(this->topChatwindow_ == "public")
        {
            ui->sendfile->setEnabled(false);
            for(const auto &m : this->conn->getMsgList())
            {
                if(m.receiver=="public")
                {
                    QString txt;
                    txt += m.time;
                    txt += " - [";
                    txt += m.sender;
                    txt += "]: ";
                    ui->textBrowser->append(txt);
                    ui->textBrowser->append(m.content);
                    ui->textBrowser->append("");
                }
            }
        }
        else
        {
            ui->sendfile->setEnabled(true);
            for(const auto &m : this->conn->getMsgList())
            {
                if((m.sender == this->topChatwindow_ && m.receiver == this->conn->getUserName()) || (m.sender == this->conn->getUserName() && m.receiver == this->topChatwindow_))
                {
                    QString txt;
                    txt += m.time;
                    txt += " - [";
                    txt += m.sender;
                    txt += "]: ";
                    ui->textBrowser->append(txt);
                    ui->textBrowser->append(m.content);
                    ui->textBrowser->append("");
                }
            }
        }
    }
}


void ChatWindow::receiveMsgCallback()
{
    Message newmsg = this->conn->getNewestMsg();
    if(newmsg.receiver == "public" && topChatwindow_ == "public")
    {
        QString txt;
        txt += newmsg.time;
        txt += " - [";
        txt += newmsg.sender;
        txt += "]";
        ui->textBrowser->append(txt);
        ui->textBrowser->append(newmsg.content);
        ui->textBrowser->append("");
    }
    else if(newmsg.sender == topChatwindow_ && newmsg.receiver == this->conn->getUserName())
    {
        QString txt;
        txt += newmsg.time;
        txt += " - [";
        txt += newmsg.sender;
        txt += "]: ";
        ui->textBrowser->append(txt);
        ui->textBrowser->append(newmsg.content);
        ui->textBrowser->append("");
    }
}

void ChatWindow::memberChangeCallback()
{
    ui->listWidget->clear();
    ui->listWidget->addItem("public");
    for(auto const & m : this->conn->getMemberList())
    {
        ui->listWidget->addItem(m);
    }
}

void ChatWindow::on_sendfile_clicked()
{
    QString path = QFileDialog::getOpenFileName(this,QStringLiteral("选择文件"),"F:",QStringLiteral("所有文件(*)"));
    if(this->conn)
    {
        FileSender sender = FileSender(this);
        sender.selectFile(path);
        sender.listen();
        QJsonObject json;
        json.insert("sender",this->conn->getUserName());
        json.insert("receiver",this->topChatwindow_);
        json.insert("label","file");
        json.insert("ip", sender.getIP());
        json.insert("port",sender.getPort());
        json.insert("file name",sender.getFileName());
        json.insert("file size",sender.getFileSize());
        QJsonDocument doc(json);
        this->conn->sendMessage(doc.toJson());
        if(sender.getServerSocket()->waitForNewConnection(WAIT_FOR_CONNECT_FILE_RECEIVER_SEC * 1000))
        {
            if(sender.sendFile())
            {

            }
        }
    }
}




void ChatWindow::receiveFileCallback()
{
    QMessageBox::information(this,"提示","收到文件");
    receiver_ = new FileReceiver(this);
    receiver_ -> connectHost(this->conn->getFileInfo().ip, this->conn->getFileInfo().port);
    receiver_ -> creatFile(this->conn->getFileInfo().fileName, this->conn->getFileInfo().fileSize);
    connect(receiver_, SIGNAL(receiveFinished()), this, SLOT(freeFileReceiver()));
}

void ChatWindow::freeFileReceiver()
{
    delete receiver_;
    receiver_ = nullptr;
}
