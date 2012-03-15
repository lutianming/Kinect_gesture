#ifndef OPENNICONTROLER_H
#define OPENNICONTROLER_H
#include <XnCppWrapper.h>
#include <iostream>
void XN_CALLBACK_TYPE new_user_callback(xn::UserGenerator& generator, XnUserID user, void* pCookie);
void XN_CALLBACK_TYPE lost_user_callback(xn::UserGenerator& generator, XnUserID user, void* pCookie);
void XN_CALLBACK_TYPE pose_detect_callback(xn::PoseDetectionCapability& capabiluty, const char* strPose, XnUserID user, void* pCookie);
void XN_CALLBACK_TYPE calibration_start_callback(xn::SkeletonCapability& capability, XnUserID user, void* pCookie);
void XN_CALLBACK_TYPE calibration_end_callback(xn::SkeletonCapability& capability, XnUserID user, XnCalibrationStatus status, void* pCookie);
class OpenNIControler
{
public:
    OpenNIControler();
    ~OpenNIControler()
    {
      m_Context.Release();
    }
    xn::DepthMetaData  m_DepthMD;
    xn::ImageMetaData  m_ImageMD;
     /* Initial OpenNI context and create nodes. */
    bool initial();
    bool release();
     /* Start to get the data from device */
    bool start();
     /* Update / Get new data */
    bool updateData();
    xn::UserGenerator getUserGenerator(){ return m_User;}
    xn::DepthGenerator getDepthGenerator(){ return m_Depth;}
    static XnBool needPose;
private:
    XnStatus           m_eResult;
    xn::Context        m_Context;
    xn::DepthGenerator m_Depth;
    xn::ImageGenerator m_Image;
    xn::UserGenerator   m_User;
    XnCallbackHandle hUserCallbacks;
    XnCallbackHandle hCalibrationStart;
    XnCallbackHandle hCalibrationComplete;
    XnCallbackHandle hPoseCallbacks;
    /* Check return status m_eResult.
     * return false if the value is XN_STATUS_OK, true for error */
    bool CheckError( const char* sError );

};

#endif // OPENNICONTROLER_H
