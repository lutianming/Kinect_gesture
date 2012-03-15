#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QString>
#include <QFileDialog>
#include <fstream>
MainWindow::MainWindow(KinectReader &reader, QWidget *parent) :
    QMainWindow(parent), m_reader( reader), m_recognizer(12, 0.6, 2, 2, 10),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->graphicsView->setScene(&(m_reader.getScene()));
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
void MainWindow::handleFrame()
{
    if(buffer.size() > min_frame_size && capturing == false){
        string s = m_recognizer.recognize(buffer);
 //       std::cout << "gesture:" << s << std::endl;
        if(s != "UNKNOWN"){
         std::cout << "gesture" << s << std::endl;
        }
        if(s != GESTURE_UNKNOWN){
            buffer.clear();
        }
    }
    if(buffer.size() > buffer_size){
        if(capturing){
            capturing = false;
            m_recognizer.addOrUpdate(buffer, ui->comboBox->currentText().toStdString());
            buffer.clear();
            ui->outputTextEdit->appendPlainText("gesture "+ ui->comboBox->currentText() + " added\n");
            read();
        }
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
void MainWindow::read()
{
    ui->readButton->setEnabled(false);
    ui->captureButton->setEnabled(true);
    capturing = false;
    ui->outputTextEdit->appendPlainText("start reading");
}
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
