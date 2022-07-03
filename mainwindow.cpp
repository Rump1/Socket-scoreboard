#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <chrono>
#include <thread>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<stdio.h>
#include<unistd.h>
#include<fcntl.h>
#include<sys/types.h>
#include<string.h>
#include<stdlib.h>
#include <sstream>
#include <stdlib.h>
#include <iostream>
#define maxlen 70000
#define mlen 100000
#include <mutex>
std::mutex g_lock;

using namespace std;

bool pau = true;
int i = -1;
int mint = 0;
int sec = 0;
int goal1 = 0;
int goal2 = 0;
int period = 1;
QString temp;
QString mess = " ";
int check = 0;
int st = 0;

// QLineEdit *lineEdit_8, std::thread thread
void threadFunction(Ui::MainWindow *&ui)
{

    while (true) // Добавление времени, вывод в формате строки на табло
    {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        if (pau)
        {

            i++;
            if (i == 10){
                sec++;
                i=0;
                if (sec == 6 && i == 0){
                    mint++;
                    i=0;
                    sec = 0;
                }
            }
            temp = QVariant(mint).toString()+":"+QVariant(sec).toString()+QVariant(i).toString();
            QMetaObject::invokeMethod(ui->lineEdit_8, "setText", Q_ARG(QString, temp));
        }
    }
}
void threadFunction1(Ui::MainWindow *&ui)  // Прием ЮДП запроса и оправка своего айпи на клиент
{
    char fileName[1024];
    int sd;
    socklen_t len;

    for(int j=0;j<=100;j++){
        fileName[j]='\0';
    }
    struct sockaddr_in servaddr,cliaddr;

    sd = socket(AF_INET, SOCK_DGRAM, 0);

    bzero(&servaddr, sizeof(servaddr));

    int broadcastEnable=1;
    setsockopt(sd, SOL_SOCKET, SO_BROADCAST, &broadcastEnable, sizeof(broadcastEnable)); // Установка параметров для сокета
    servaddr.sin_family = AF_INET; // сокет в формате айпи
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY); // широковещательный
    servaddr.sin_port = htons(8000); // конкретный порт

    memset(&(servaddr.sin_zero),'\0',8);
    bind(sd, (struct sockaddr *)&servaddr, sizeof(servaddr)); // Блокировка текущего айпи и порта
    len=sizeof(cliaddr);
    //string temp2;
    while(1)
    {

      char num;
      recvfrom(sd,&num,sizeof(num),0,(struct sockaddr *)&cliaddr, &len); // Ожидание запроса
      if (num == 1){
          g_lock.lock();
          recvfrom(sd,fileName,1024,0,(struct sockaddr *)&cliaddr, &len);
          char num=1;
          char digs[100];
          string utf8_text = temp.toUtf8().constData(); // Перевод таймера в ютф-8
          utf8_text = utf8_text + "," + to_string(period) + "," + to_string(goal1) + "," +to_string(goal2)+ "," + mess.toStdString() + "," + "Barselona" + "," + "Real Madrid" + "," + to_string(st);
          //sprintf(digs,"%d",i); // , в качестве разделителя
          strcpy(digs, utf8_text.c_str()); // Перевод строки в чар массив

          sendto(sd, &num, sizeof(num), 0,(struct sockaddr *)&cliaddr, sizeof(struct sockaddr));
          sendto(sd, digs, sizeof(digs), 0,(struct sockaddr *)&cliaddr, sizeof(struct sockaddr)); // отправка чаровского массива на клиент
          //mess = " ";
          g_lock.unlock();
      }
    }
}

MainWindow::MainWindow(QWidget *parent) // Запуск формы
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //std::thread thread(threadFunction, std::ref(ui));
    //thread.detach();

    std::thread thread1(threadFunction1,std::ref(ui)); // Запуск потока ожидания сообщений от клиента
    thread1.detach();
}

MainWindow::~MainWindow() // Закрытие формы
{
    delete ui;
}

void MainWindow::on_pushButton_3_clicked() // Кнопка паузы
{
    if (pau) {
        pau = false;
        ui->pushButton_8->setEnabled(false);
        mess = "- Игра на паузе!\n";
        ui->textEdit->setText(ui->textEdit->toPlainText()+"- Игра на паузе!\n");
        ui->pushButton_3->setText("Продолжить");
    } else {
        pau = true;
        ui->pushButton_8->setEnabled(true);
        mess = "- Игра продолжается!\n";
        ui->textEdit->setText(ui->textEdit->toPlainText()+"- Игра продолжается!\n");
        ui->pushButton_3->setText("Пауза");
    }
}


void MainWindow::on_pushButton_clicked() // Забить гол 1
{
    goal1++;
    QString name = "Messi";
    QString t = "Barselona";
    mess = "- " + name + " из " + t +" забил "+QVariant(goal1).toString()+"-й гол!\n";
    ui->textEdit->setText(ui->textEdit->toPlainText()+"- " + QVariant(name + " из " + t + " забил "+QVariant(goal1).toString()+"-й гол!\n").toString());
    ui->lineEdit_3->setText(QVariant(goal1).toString());
}


void MainWindow::on_pushButton_2_clicked() // Забить гол 2
{
    goal2++;
    QString name = "Ronaldo";
    QString t = "Real Madrid";
    mess = "- " + name + " из " + t + " забил " + QVariant(goal2).toString()+"-й гол!\n";
    ui->textEdit->setText(ui->textEdit->toPlainText()+"- " + QVariant(name + " из " + t + " забил "+QVariant(goal2).toString()+"-й гол!\n").toString());
    ui->lineEdit_4->setText(QVariant(goal2).toString());
}

void MainWindow::on_pushButton_4_clicked() // Некст период
{
    i = -1;
    mint = 0;
    sec = 0;
    mess = "- Период "+QVariant(period).toString()+" сменен на период "+QVariant(period+1).toString()+"\n";
    period++;
    ui->textEdit->setText(ui->textEdit->toPlainText()+"- Период "+QVariant(period-1).toString()+" сменен на период "+QVariant(period).toString()+"\n");
    ui->lineEdit_6->setText(QVariant(period).toString());
}


void MainWindow::on_pushButton_5_clicked() // Отменить гол команда 1
{
    if (goal1>0) {
        QString t = "Barselona";
        ui->textEdit->setText(ui->textEdit->toPlainText()+"- " + QVariant(goal1).toString()+"-й гол " + t + " отменен.\n");
        mess = "- " + QVariant(goal1).toString()+"-й гол " + t + " отменен.\n";
        goal1--;
        ui->lineEdit_3->setText(QVariant(goal1).toString());
    }
}


void MainWindow::on_pushButton_6_clicked() // Отменить гол 2
{
    if (goal2>0) {
        QString t = "Real Madrid";
        ui->textEdit->setText(ui->textEdit->toPlainText()+"- "+QVariant(goal2).toString()+"-й гол " + t + " отменен.\n");
        mess = "- " + QVariant(goal2).toString()+"-й гол " + t + " отменен.\n";
        goal2--;
        ui->lineEdit_4->setText(QVariant(goal2).toString());
    }
}


void MainWindow::on_pushButton_7_clicked() // Start
{
    st = 0;
    ui->pushButton_7->setEnabled(false);
    ui->pushButton_8->setEnabled(true);
    ui->textEdit->setText("");
    i = -1;
    sec = 0;
    mint = 0;
    goal1 = 0;
    goal2 = 0;
    period = 1;
    pau = true;

    ui->lineEdit_3->setText("0");
    ui->lineEdit_4->setText("0");
    ui->lineEdit_6->setText("1");

    //ui->pushButton_7->repaint();
    if (check == 0){
        std::thread thread(threadFunction, std::ref(ui));
        thread.detach();
        check++;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    if (st == 0){
        st = 1;
    }
    mess = "- Игра началась!\n";
    ui->textEdit->setText(ui->textEdit->toPlainText()+"- Игра началась!\n");
}


void MainWindow::on_pushButton_8_clicked() // End game
{
    pau = false;
    ui->pushButton_7->setEnabled(true);
    ui->pushButton_8->setEnabled(false);
    QString t;
    if (goal1 > goal2){
        t = "Выиграла команда Barselona";
    }
    else{
        if (goal1 < goal2) t = "Выиграла команда Real Madrid";
        else t = "Ничья";
    }
    mess = "- Игра завершена! " + t + "\n";
    ui->textEdit->setText(ui->textEdit->toPlainText() + "- Игра окончена! " + t + "\n");

}
