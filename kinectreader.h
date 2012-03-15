#ifndef KINECTREADER_H
#define KINECTREADER_H

#include <QObject>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <vector>
#include "opennicontroler.h"
#define MAX_NUM_USERS 10
#define JOINT_SIZE 6
class KinectReader : public QObject
{
    Q_OBJECT
public:
    KinectReader( OpenNIControler& controler)
        : m_OpenNI( controler ), seq(12,0)
    {

    }
    ~KinectReader()
    {
//        m_Scene.removeItem( m_pItemImage );
        m_Scene.removeItem( m_pItemDepth );
        delete [] m_pDepthRGB;
    }
    bool init();
      /* Start to update Qt Scene from OpenNI device */
    void start( int iInterval = 33 );
    void stop();
    void updateSkeletonSeq();
    std::vector<double> seq;
    QGraphicsScene& getScene(){return m_Scene;}
private:
    OpenNIControler&            m_OpenNI;
    QGraphicsScene      m_Scene;
    QGraphicsPixmapItem* m_pItemDepth;
//    QGraphicsPixmapItem* m_pItemImage;
    uchar*               m_pDepthRGB;
    XnUserID users[MAX_NUM_USERS];
    XnUInt16 user_num;
    XnVector3D jointsReal[JOINT_SIZE]; //left_hand,left_elbow,left_shoudler, right_shoudler, right_elbow, right hand
    XnVector3D jointsProj[JOINT_SIZE];
    int timerID;
    void timerEvent( QTimerEvent *event );
    void updateSkeletonInfo();
    XnVector3D getSkeletonPos(XnUserID user, XnSkeletonJoint jointName);
signals:
    void frameReady();
};

#endif // KINECTREADER_H
