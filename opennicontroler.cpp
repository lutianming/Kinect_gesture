#include "opennicontroler.h"
//OpenNI驱动控制，不包含Qt元素，可以直接在VS中使用
XnBool OpenNIControler::needPose = false;
OpenNIControler::OpenNIControler()
{

}
/* Initial OpenNI context and create nodes. */
bool OpenNIControler::initial()
{
	// Initial OpenNI Context
	m_eResult = m_Context.Init();
	if( CheckError( "Context Initial failed" ) )
		return false;

	// create image node
	m_eResult = m_Image.Create( m_Context );
	if( CheckError( "Create Image Generator Error" ) )
		return false;

    // create depth node
    m_eResult = m_Depth.Create( m_Context );
    if( CheckError( "Create Depth Generator Error" ) )
        return false;

    //create user node
    m_eResult = m_User.Create( m_Context);
    if( CheckError("Create User Generator Error"))
        return false;
    //add user callbacks
    m_eResult = m_User.RegisterUserCallbacks(new_user_callback, lost_user_callback, &m_User, hUserCallbacks);
    if( CheckError("register user callback"))
        return false;
    m_eResult = m_User.GetSkeletonCap().RegisterToCalibrationStart(calibration_start_callback, &m_User, hCalibrationStart);
    if( CheckError("register calibration start"))
        return false;
    m_eResult = m_User.GetSkeletonCap().RegisterToCalibrationComplete(calibration_end_callback, &m_User, hCalibrationComplete);
    if( CheckError("register calibration complete"))
        return false;
    m_eResult = m_User.GetPoseDetectionCap().RegisterToPoseDetected(pose_detect_callback, &m_User, hPoseCallbacks);
    if( CheckError("register pose dectection"))
        return false;
    if( m_User.GetSkeletonCap().NeedPoseForCalibration())
    {
        needPose = true;
    }
    m_User.GetSkeletonCap().SetSkeletonProfile(XN_SKEL_PROFILE_UPPER);
    // set nodes
    m_eResult = m_Depth.GetAlternativeViewPointCap().SetViewPoint( m_Image );
    if(CheckError( "Can't set the alternative view point on depth generator" ))
        return false;

  return true;
}
bool OpenNIControler::release()
{
    m_Depth.Release();
    m_Image.Release();
    m_User.Release();
    m_Context.Release();
}
/* Start to get the data from device */
bool OpenNIControler::start()
{
  m_eResult = m_Context.StartGeneratingAll();
  return !CheckError( "Start Generating" );
}

/* Update / Get new data */
bool OpenNIControler::updateData()
{
  // update
  m_eResult = m_Context.WaitOneUpdateAll(m_User);
  if( CheckError( "Update Data" ) )
    return false;

  // get new data
  m_Depth.GetMetaData( m_DepthMD );
  m_Image.GetMetaData( m_ImageMD );

  return true;
}

void new_user_callback(xn::UserGenerator &generator, XnUserID user, void *pCookie)
{
	XnUInt32 epochTime = 0;
	xnOSGetEpochTime(&epochTime);
	std::cout << epochTime << " new user" << user << std::endl;
	if(OpenNIControler::needPose)
	{
		generator.GetPoseDetectionCap().StartPoseDetection("Psi", user);
	}
	else
	{
		generator.GetSkeletonCap().RequestCalibration(user, true);
	}
}
void lost_user_callback(xn::UserGenerator &generator, XnUserID user, void *pCookie)
{
	XnUInt32 epochTime = 0;
	xnOSGetEpochTime(&epochTime);
	std::cout << epochTime << " lost user" << user << std::endl;
}
void calibration_start_callback(xn::SkeletonCapability &capability, XnUserID user, void *pCookie)
{
	std::cout << "calibration start" << std::endl;
}
void calibration_end_callback(xn::SkeletonCapability &capability, XnUserID user, XnCalibrationStatus status, void *pCookie)
{
	if(status == XN_CALIBRATION_STATUS_OK)
	{
		std::cout << "start tracking" << std::endl;
		capability.StartTracking(user);
	}
	else
	{
		if(status == XN_CALIBRATION_STATUS_MANUAL_ABORT)
		{
			std::cout << "manual abort occured, stop" << std::endl;
			return;
		}
		if(OpenNIControler::needPose)
		{
			std::cout << "need pose" << std::endl;
			((xn::UserGenerator*)pCookie)->GetPoseDetectionCap().StartPoseDetection("Psi", user);
		}
		else
		{
			std::cout << "request calibration" << std::endl;
			capability.RequestCalibration(user, true);
		}
	}
}
void pose_detect_callback(xn::PoseDetectionCapability &capabiluty, const char *strPose, XnUserID user, void *pCookie)
{
	std::cout << "decect pose" << std::endl;
	capabiluty.StopPoseDetection(user);
	((xn::UserGenerator*)pCookie)->GetSkeletonCap().RequestCalibration(user, true);
}
/* Check return status m_eResult.
 * return false if the value is XN_STATUS_OK, true for error */
bool OpenNIControler::CheckError( const char* sError )
{
  if( m_eResult != XN_STATUS_OK )
  {
      std::cerr << sError << ": " << xnGetStatusString( m_eResult ) << std::endl;
    return true;
  }
  return false;
}
