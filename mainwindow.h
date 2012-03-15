#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "kinectreader.h"
#include "GestureRecognizer.h"
namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(KinectReader& reader, QWidget *parent = 0);
    ~MainWindow();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::MainWindow *ui;
    KinectReader &m_reader;
    GestureRecognizer m_recognizer;
    //how many skeleton frames to ignore
    //1 = capture every frame, 2 = capture every second frame ...
    int ignore_frame;
    int flipflop;    //0 = use this frame, else drop
    int buffer_size;
    //the minimum number of frames in the buffer before attempting to start matching gestures
    int min_frame_size;
    int capture_count_down_second;
    //whether the recogniser is capturing new gesture
    bool capturing;
    //used for calculating frames per second
    int last_frames;
    vector<vector<double> > buffer;

private slots:
    void loadGestures();
    void saveGestures();
    void read();
    void capture();
    void listGestures();
public slots:
    void handleFrame();
};

#endif // MAINWINDOW_H
