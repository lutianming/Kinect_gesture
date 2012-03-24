#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QString>
#include <QFileDialog>
#include <fstream>
//主界面，需要改写，Qt读取mainwindow.ui文件确定界面样式
//建议安装Qt creator后查看该项目以方便改写
MainWindow::MainWindow(KinectReader &reader, QWidget *parent) :
    QMainWindow(parent), m_reader( reader), m_recognizer(12, 0.6, 2, 2, 10),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->graphicsView->setScene(&(m_reader.getScene()));
    //连接reader的frameReady信号和handleFrame函数
    //每次reader读取帧数据后发送frameReady信号，然后handleFrame执行处理
    connect(&m_reader, SIGNAL(frameReady()), this, SLOT(handleFrame()));
    ui->comboBox->addItem(QString::fromStdString(GESTURE_LEFT_SWIPE_LEFT));
    ui->comboBox->addItem(QString::fromStdString(GESTURE_RIGHT_SWIPE_LEFT));
    ui->comboBox->addItem(QString::fromStdString(GESTURE_UNKNOWN));
    ignore_frame = 2;
    flipflop = 0;
    buffer_size = 32;
    min_frame_size = 6;
    capture_count_down_second = 3;
    capturing = false;
}

MainWindow::~MainWindow()
{
    delete ui;
}

//不用管它
void MainWindow::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}
//
void MainWindow::handleFrame()
{
    //如果buffer内有足够多帧并且不在录制手势，则开始识别
    if(buffer.size() > min_frame_size && capturing == false){
        string s = m_recognizer.recognize(buffer);
        if(s != "UNKNOWN"){
         ui->outputTextEdit->appendPlainText(QString("gesture").append(s.c_str()));
         std::cout << "gesture" << s << std::endl;
        }
        if(s != GESTURE_UNKNOWN){
            buffer.clear();
        }
    }
    if(buffer.size() > buffer_size){
        //如果在录制手势，则停止录制，将该录制的手势加入识别器中
        if(capturing){
            capturing = false;
            m_recognizer.addOrUpdate(buffer, ui->comboBox->currentText().toStdString());
            buffer.clear();
            ui->outputTextEdit->appendPlainText("gesture "+ ui->comboBox->currentText() + " added\n");
            read();
        }
        //否则，删除第一个值，
        else{
            buffer.erase(buffer.begin());
        }
    }
    flipflop = (flipflop+1) % ignore_frame;
    if(flipflop == 0){
        buffer.push_back(m_reader.seq);
    }
}
void MainWindow::loadGestures()
{
    m_reader.stop();
    QString file = QFileDialog::getOpenFileName(this);
    if(file != NULL){
        m_recognizer.loadGestures(file.toAscii());
    }
    m_reader.start();
}
void MainWindow::saveGestures()
{
    m_reader.stop();
    QString file = QFileDialog::getSaveFileName(this);
    if(file != NULL){
        m_recognizer.saveGestures(file.toAscii());
    }
    m_reader.start();
}
//读取，开始识别，主状态
void MainWindow::read()
{
    ui->readButton->setEnabled(false);
    ui->captureButton->setEnabled(true);
    capturing = false;
    ui->outputTextEdit->appendPlainText("start reading");
}

//开始录制手势，32帧完后自动转回read状态
void MainWindow::capture()
{
    ui->readButton->setEnabled(false);
    ui->captureButton->setEnabled(false);
    capturing = true;
    ui->outputTextEdit->appendPlainText("start capture gesture");
    buffer.clear();
}
void MainWindow::listGestures()
{
    map<string, Sequence >::iterator it;
    for(it = m_recognizer.getGestures().begin(); it != m_recognizer.getGestures().end(); it++){
        ui->outputTextEdit->appendPlainText(QString::fromStdString(it->first));
        ui->outputTextEdit->appendPlainText(QString::number((it->second).size()) + "\n");
    }
}
