#include "kinectreader.h"
#include <QPainter>
#include <math.h>
#define MAX_DEPTH 10000
void KinectReader::timerEvent( QTimerEvent *event )
{
  // Read OpenNI data
  m_OpenNI.updateData();
  updateSkeletonInfo();
  // convert to RGBA format
  const XnDepthPixel*  pDepth = m_OpenNI.m_DepthMD.Data();
  unsigned int iSize=m_OpenNI.m_DepthMD.XRes()*m_OpenNI.m_DepthMD.YRes();

  // find the max value
  XnDepthPixel tMax = *pDepth;
  for( unsigned int i = 1; i < iSize; ++ i )
  {
    if( pDepth[i] > tMax )
      tMax = pDepth[i];
  }

  // redistribute data to 0-255
  int idx = 0;
  for( unsigned int i = 1; i < iSize; ++ i )
  {
    if( (*pDepth) != 0)
    {
      m_pDepthRGB[ idx++ ] = 255;
      m_pDepthRGB[ idx++ ] = 255 * (tMax - *pDepth)  / tMax;
      m_pDepthRGB[ idx++ ] = 255 * (tMax - *pDepth) / tMax;
      m_pDepthRGB[ idx++ ] = 255;
    }
    else
    {
      m_pDepthRGB[ idx++ ] = 255;
      m_pDepthRGB[ idx++ ] = 0;
      m_pDepthRGB[ idx++ ] = 0;
      m_pDepthRGB[ idx++ ] = 255;
    }
    ++pDepth;
  }
    //draw skeleton joints
  QImage img(m_pDepthRGB, m_OpenNI.m_DepthMD.XRes(), m_OpenNI.m_DepthMD.YRes(), QImage::Format_RGB32);
  QPainter painter(&img);
  QPen pen(QColor::fromRgb(255,0,0));
  pen.setWidth(5);
  painter.setPen(pen);
  for(int i = 0; i < JOINT_SIZE-1; i++){
      painter.drawLine((int)(jointsProj[i].X), (int)(jointsProj[i].Y), (int)(jointsProj[i+1].X), (int)(jointsProj[i+1].Y));
  }
  // Update Depth data
  m_pItemDepth->setPixmap( QPixmap::fromImage(img));

  // Update Image data
//  m_pItemImage->setPixmap( QPixmap::fromImage(
//    QImage( m_OpenNI.m_ImageMD.Data(),
//            m_OpenNI.m_ImageMD.XRes(), m_OpenNI.m_ImageMD.YRes(),
//            QImage::Format_RGB888 ) )
//  );
}

bool KinectReader::init()
{
    m_OpenNI.start();

    // add an empty Image to scene
  //  m_pItemImage = m_Scene.addPixmap( QPixmap() );
  //  m_pItemImage->setZValue( 1 );

    // add an empty Depth to scene
    m_pItemDepth = m_Scene.addPixmap( QPixmap() );
    m_pItemDepth->setZValue( 1 );

    // update first to get the depth map size
    m_OpenNI.updateData();
    m_pDepthRGB
       = new uchar[4*m_OpenNI.m_DepthMD.XRes()*m_OpenNI.m_DepthMD.YRes()];
    return true;
}
/* Start to update Qt Scene from OpenNI device */
void KinectReader::start( int iInterval)
{
  timerID = startTimer( iInterval );
}
void KinectReader::stop()
{
    killTimer(timerID);
}
void KinectReader::updateSkeletonInfo()
{
    user_num = MAX_NUM_USERS;
    xn::UserGenerator generator = m_OpenNI.getUserGenerator();
    generator.GetUsers(users, user_num);
    for(int i = 0; i < user_num; i++){
        if(generator.GetSkeletonCap().IsTracking(users[i]) == false){
            continue;
        }
        jointsReal[0] = getSkeletonPos(users[i], XN_SKEL_LEFT_HAND);
        jointsReal[1] = getSkeletonPos(users[i], XN_SKEL_LEFT_ELBOW);
        jointsReal[2] = getSkeletonPos(users[i], XN_SKEL_LEFT_SHOULDER);
        jointsReal[3] = getSkeletonPos(users[i], XN_SKEL_RIGHT_SHOULDER);
        jointsReal[4] = getSkeletonPos(users[i], XN_SKEL_RIGHT_ELBOW);
        jointsReal[5] = getSkeletonPos(users[i], XN_SKEL_RIGHT_HAND);
        m_OpenNI.getDepthGenerator().ConvertRealWorldToProjective(JOINT_SIZE, jointsReal, jointsProj);
        updateSkeletonSeq();
        break;
    }
}
void KinectReader::updateSkeletonSeq()
{
    double centerX = (jointsReal[2].X + jointsReal[3].X) / 2;
    double centerY = (jointsReal[2].Y + jointsReal[3].Y) / 2;
    double shouderDist = sqrt(pow(jointsReal[2].X-jointsReal[3].X, 2)+pow(jointsReal[2].Y-jointsReal[3].Y,2));
    for(int i = 0; i < JOINT_SIZE; i++){
        seq[i*2] = (jointsReal[i].X - centerX) / shouderDist;
        seq[i*2+1] = (jointsReal[i].Y - centerY) / shouderDist;
    }
    emit frameReady();
}
XnVector3D KinectReader::getSkeletonPos(XnUserID user, XnSkeletonJoint jointName)
{
    XnSkeletonJointPosition pos;
    m_OpenNI.getUserGenerator().GetSkeletonCap().GetSkeletonJointPosition(user, jointName, pos);
    return xnCreatePoint3D(pos.position.X, pos.position.Y, pos.position.Z);
}
