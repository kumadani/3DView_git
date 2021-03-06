//
//  main.cpp
//  3DView
//
//  Created by 熊谷　和哉 on 2018/01/08.
//  Copyright © 2018年 熊谷　和哉. All rights reserved.
//

/*******************************************************************************
 *                                                                              *
 *   PrimeSense NiTE 2.0 - Simple Skeleton Sample                               *
 *   Copyright (C) 2012 PrimeSense Ltd.                                         *
 *                                                                              *
 *******************************************************************************/

#include <NiTE.h>
#include "NiteSampleUtilities.h"
#include <OpenNI.h>
#include <chrono>


#if (ONI_PLATFORM == ONI_PLATFORM_MACOSX)
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#define Scale 1000.0
#define LineSize 2
#define PointSize 2
#define WindowWidth 1024
#define WindowHeight 1024

bool Rotation = false;
unsigned int Front = 4;
bool PointCloud = true;
bool Skelton = true;
unsigned int DisplayMode = 0;

std::chrono::system_clock::time_point start;
unsigned long frame = 0;

//nite::UserTracker userTracker,userTracker2;
//openni::Device device,device2;
//openni::VideoStream colorStream,colorStream2;
//openni::VideoStream depthStream,depthStream2;
nite::UserTracker userTracker[2];
openni::Device device[2];
openni::VideoStream colorStream[2];
openni::VideoStream depthStream[2];
openni::Array<openni::DeviceInfo> deviceInfo;

#define MAX_USERS 10
bool g_visibleUsers[MAX_USERS] = {false};
nite::SkeletonState g_skeletonStates[MAX_USERS] = {nite::SKELETON_NONE};

#define USER_MESSAGE(msg) \
{printf("[%08llu] User #%d:\t%s\n",ts, user.getId(),msg);}

void updateUserState(const nite::UserData& user, unsigned long long ts)
{
    if (user.isNew())
        USER_MESSAGE("New")
        else if (user.isVisible() && !g_visibleUsers[user.getId()])
            USER_MESSAGE("Visible")
            else if (!user.isVisible() && g_visibleUsers[user.getId()]){
                USER_MESSAGE("Out of Scene")
                printf("%f\n",user.getSkeleton().getJoint(nite::JOINT_TORSO).getPosition().z);
            }
                else if (user.isLost())
                    USER_MESSAGE("Lost")
                    
                    g_visibleUsers[user.getId()] = user.isVisible();
    
    
    if(g_skeletonStates[user.getId()] != user.getSkeleton().getState())
    {
        switch(g_skeletonStates[user.getId()] = user.getSkeleton().getState())
        {
            case nite::SKELETON_NONE:
                USER_MESSAGE("Stopped tracking.")
                printf("%f\n",user.getSkeleton().getJoint(nite::JOINT_TORSO).getPosition().z);
                break;
            case nite::SKELETON_CALIBRATING:
                USER_MESSAGE("Calibrating...")
                break;
            case nite::SKELETON_TRACKED:
                USER_MESSAGE("Tracking!")
                break;
            case nite::SKELETON_CALIBRATION_ERROR_NOT_IN_POSE:
            case nite::SKELETON_CALIBRATION_ERROR_HANDS:
            case nite::SKELETON_CALIBRATION_ERROR_LEGS:
            case nite::SKELETON_CALIBRATION_ERROR_HEAD:
            case nite::SKELETON_CALIBRATION_ERROR_TORSO:
                USER_MESSAGE("Calibration Failed... :-|")
                break;
        }
    }
}

void Finalize(void) {
	for (int i = 0;i < deviceInfo.getSize(); i++) {
		colorStream[i].stop();
		depthStream[i].stop();
		userTracker[i].destroy();
		colorStream[i].destroy();
		depthStream[i].destroy();
		nite::NiTE::shutdown();
		openni::OpenNI::shutdown();
		device[i].close();
	}
	exit(0);
}


void drawLine(nite::SkeletonJoint joint1,nite::SkeletonJoint joint2){
    

    glColor3d(1.0, 0.0, 0.0);
    glLineWidth(LineSize);
    if(joint1.getPositionConfidence() > .2 && joint2.getPositionConfidence() > .2){
        glBegin(GL_LINES);
        glVertex3d(joint1.getPosition().x, joint1.getPosition().y, joint1.getPosition().z);
        glVertex3d(joint2.getPosition().x, joint2.getPosition().y, joint2.getPosition().z);
        glEnd();
    }
    
}


void drawPoint(nite::SkeletonJoint joint){
    glColor3d(1.0, 0.0, 0.0);
    glPointSize(PointSize);
    if(joint.getPositionConfidence()>.2){
        glBegin(GL_POINTS);
        glVertex3d(joint.getPosition().x, joint.getPosition().y, joint.getPosition().z);
        glEnd();
    }
}
 

void drawSkelton(nite::UserData user){
    const nite::SkeletonJoint& head = user.getSkeleton().getJoint(nite::JOINT_HEAD);
    const nite::SkeletonJoint& neck = user.getSkeleton().getJoint(nite::JOINT_NECK);
    const nite::SkeletonJoint& left_shoulder = user.getSkeleton().getJoint(nite::JOINT_LEFT_SHOULDER);
    const nite::SkeletonJoint& right_shoulder = user.getSkeleton().getJoint(nite::JOINT_RIGHT_SHOULDER);
    const nite::SkeletonJoint& left_elbow = user.getSkeleton().getJoint(nite::JOINT_LEFT_ELBOW);
    const nite::SkeletonJoint& right_elbow = user.getSkeleton().getJoint(nite::JOINT_RIGHT_ELBOW);
    const nite::SkeletonJoint& left_hand = user.getSkeleton().getJoint(nite::JOINT_LEFT_HAND);
    const nite::SkeletonJoint& right_hand = user.getSkeleton().getJoint(nite::JOINT_RIGHT_HAND);
    const nite::SkeletonJoint& torso = user.getSkeleton().getJoint(nite::JOINT_TORSO);
    const nite::SkeletonJoint& left_hip = user.getSkeleton().getJoint(nite::JOINT_LEFT_HIP);
    const nite::SkeletonJoint& right_hip = user.getSkeleton().getJoint(nite::JOINT_RIGHT_HIP);
    const nite::SkeletonJoint& left_knee = user.getSkeleton().getJoint(nite::JOINT_LEFT_KNEE);
    const nite::SkeletonJoint& right_knee = user.getSkeleton().getJoint(nite::JOINT_RIGHT_KNEE);
    const nite::SkeletonJoint& left_foot = user.getSkeleton().getJoint(nite::JOINT_LEFT_FOOT);
    const nite::SkeletonJoint& right_foot = user.getSkeleton().getJoint(nite::JOINT_RIGHT_FOOT);
    
    
    drawPoint(head);
    drawPoint(neck);
    drawPoint(left_shoulder);
    drawPoint(left_elbow);
    drawPoint(left_hand);
    drawPoint(right_shoulder);
    drawPoint(right_elbow);
    drawPoint(right_hand);
    drawPoint(torso);
    drawPoint(left_hip);
    drawPoint(right_hip);
    drawPoint(left_knee);
    drawPoint(left_foot);
    drawPoint(right_knee);
    drawPoint(right_foot);
    
    
    drawLine(head,neck);
    
    drawLine(left_shoulder,left_elbow);
    drawLine(left_elbow,left_hand);
    
    drawLine(right_shoulder, right_elbow);
    drawLine(right_elbow, right_hand);
    
    drawLine(left_shoulder, right_shoulder);
    
    drawLine(left_shoulder, torso);
    drawLine(right_shoulder, torso);
    
    drawLine(torso, left_hip);
    drawLine(torso, right_hip);
    
    drawLine(left_hip, left_knee);
    drawLine(left_knee, left_foot);
    
    drawLine(right_hip, right_knee);
    drawLine(right_knee, right_foot);

}


void idle(void)
{
    glutPostRedisplay();
}

void display(void)
{

    static int r = 0;
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();

    switch(Front%6){
        case 0:
            gluLookAt(3.0, 5.0, -3.0,0.0, 0.0, 5.0,0.0, 1.0, 0.0);  //左斜め上前面
            break;
        case 1:
            gluLookAt(-10.0, 10.0, -10.0,0.0, 0.0, 0.0,0.0, 1.0, 0.0);  //右斜め上前面
            break;
        case 2:
            gluLookAt(10.0, 10.0, 10.0,0.0, 0.0, 0.0,0.0, 1.0, 0.0);  //左斜め上背面
            break;
        case 3:
            gluLookAt(-10.0, 10.0, 10.0,0.0, 0.0, 0.0,0.0, 1.0, 0.0);  //右斜め上背面
            break;
        case 4:
            gluLookAt(0.0, 0.0, -5.0,0.0, 0.0, 0.0,0.0, 1.0, 0.0);  //正面
            break;
        case 5:
            gluLookAt(5.0, 0.0, 3.0,0.0, 0.0, 3.0,0.0, 1.0, 0.0);  //側面
            break;
        default:
            break;
    }
    
    /*
    glColor3d(1.0, 0.0, 0.0);
    glBegin(GL_LINES);
    glVertex3d(0.0, 0.0, 0.0);
    glVertex3d(1.0, 0.0, 0.0);
    glEnd();
    glColor3d(0.0, 1.0, 0.0);
    glBegin(GL_LINES);
    glVertex3d(0.0, 0.0, 0.0);
    glVertex3d(0.0, 1.0, 0.0);
    glEnd();
    glColor3d(0.0, 0.0, 1.0);
    glBegin(GL_LINES);
    glVertex3d(0.0, 0.0, 0.0);
    glVertex3d(0.0, 0.0, 1.0);
    glEnd();
     */
    
    glPushMatrix();
    glScalef(1.0/Scale, 1.0/Scale, 1.0/1000.0);

	/*
    //骨格の表示
    nite::UserTrackerFrameRef userTrackerFrame;
    nite::Status niteRc;
    niteRc = userTracker.readFrame(&userTrackerFrame);
    if (niteRc != nite::STATUS_OK)
        printf("Get next user frame failed\n");
    
    if(Skelton == true){
        const nite::Array<nite::UserData>& users = userTrackerFrame.getUsers();
        for (int i = 0; i < users.getSize(); ++i)
        {
            const nite::UserData& user = users[i];
            updateUserState(user,userTrackerFrame.getTimestamp());
            if (user.isNew())
            {
                userTracker.startSkeletonTracking(user.getId());
            }
            else if (user.getSkeleton().getState() == nite::SKELETON_TRACKED)
            {
                drawSkelton(user);
            }
        }
    }
	*/

	//骨格の表示
	nite::UserTrackerFrameRef userTrackerFrame;
	nite::Status niteRc;
	for (int i = 0;i<deviceInfo.getSize(); i++) {
		niteRc = userTracker[i].readFrame(&userTrackerFrame);
		if (niteRc != nite::STATUS_OK)
			printf("Get next user frame failed\n");

		if (Skelton == true) {
			const nite::Array<nite::UserData>& users = userTrackerFrame.getUsers();
			for (int j = 0; j < users.getSize(); j++)
			{
				const nite::UserData& user = users[j];
				updateUserState(user, userTrackerFrame.getTimestamp());
				if (user.isNew())
				{
					//userTracker[i].startSkeletonTracking(user.getId());
				}
				else if (user.getSkeleton().getState() == nite::SKELETON_TRACKED)
				{
					drawSkelton(user);
				}
			}
		}
	}
    
	/*
    //点群の表示
    openni::VideoFrameRef colorFrame;
    openni::VideoFrameRef depthFrame;
    openni::Status openniRc;
    openniRc = colorStream.readFrame(&colorFrame);
    if(openniRc != openni::STATUS_OK)
        printf("Get next color frame failed\n");
    openniRc = depthStream.readFrame(&depthFrame);
    if(openniRc != openni::STATUS_OK)
        printf("Get next depth frame failed\n");
    
    
    int x,y;
    openni::RGB888Pixel* color = (openni::RGB888Pixel*)colorFrame.getData();
    openni::DepthPixel* depth = (openni::DepthPixel*)depthFrame.getData();
    const nite::UserId* pLabels = userTrackerFrame.getUserMap().getPixels();
    
    if(PointCloud == true){
        for(y=0;y<depthFrame.getHeight();y++){
            for(x=0;x<depthFrame.getWidth();x++){
                //openni::RGB888Pixel* color = (openni::RGB888Pixel*)colorFrame.getData();
                //int colorx,colory;
                //openni::CoordinateConverter::convertDepthToColor(depthStream, colorStream, x, y, *depth, &colorx, &colory);
                //int i=0;
                //while(i<colory*colorFrame.getWidth()+colorx){
                //    color++;
                //    i++;
                //}
                if(pLabels[y*depthFrame.getWidth()+x] != 0){
                float Worldx,Worldy,Worldz;
                openni::CoordinateConverter::convertDepthToWorld(depthStream, x, y, *depth, &Worldx, &Worldy, &Worldz);
                glColor3d(color->r/255.0, color->g/255.0, color->b/255.0);
                glPointSize(PointSize);
                glBegin(GL_POINTS);
                if(PointCloud == true)
                    //glVertex3d(x,(-y+colorFrame.getHeight()),(double)*depth);
                    glVertex3f(Worldx, Worldy, Worldz);
                glEnd();
                }
                color++;
                depth++;
            }
        }
    }
	*/

    glPopMatrix();
    
    glutSwapBuffers();
    frame++;
}

void resize(int w, int h)
{
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(30.0, (double)w / (double)h, 0.1, 1000.0);
    glMatrixMode(GL_MODELVIEW);
}
void keyboard(unsigned char key, int x, int y)
{
    switch (key) {
        case 'q':
        case 'Q':
        case '\033':  /* '\033' は ESC の ASCII コード */
        {
            auto end = std::chrono::system_clock::now();
            auto dur = end - start;
            auto msec = std::chrono::duration_cast<std::chrono::milliseconds>(dur).count();
            printf("%lld & %ld & %f\n",msec,frame,(double)frame/(double)msec*1000);
            
			//colorStream.stop();
			//depthStream.stop();
            //userTracker.destroy();
            //colorStream.destroy();
            //depthStream.destroy();
            //nite::NiTE::shutdown();
            //openni::OpenNI::shutdown();
			//device.close();
            //exit(0);
			Finalize();
            break;
        }
        case 'r':
        case 'R':
            Rotation = !Rotation;
            break;
        case 'f':
        case 'F':
            Front++;
            break;
        case 'P':
        case 'p':
            PointCloud = !PointCloud;
            break;
        case 's':
        case 'S':
            Skelton = !Skelton;
            break;
        default:
            break;
    }
}

void init(void)
{
    glClearColor(1.0, 1.0, 1.0, 1.0);
    
}




int main(int argc, char *argv[])
{
	
	//extern nite::UserTracker userTracker;
	//extern openni::Device device;
	//extern openni::VideoStream colorStream;
	//extern openni::VideoStream depthStream;
	
    openni::OpenNI::initialize();
    openni::Status openniRc =openni::STATUS_OK;



    //const char* deviceURI = NULL;
	const char* deviceURI[2] = { NULL,NULL };
	//const char* deviceURI2 = NULL;


    //openni::Array<openni::DeviceInfo> deviceInfo;
    openni::OpenNI::enumerateDevices(&deviceInfo);
    
	switch (deviceInfo.getSize())
	{
	case 0:
		printf("Coudn't find device.\n");
		return 0;
		break;
	case 1:
		printf("find 1 device.\n");
		//deviceURI[0] = deviceInfo[0].getUri();
		deviceURI[0] = deviceInfo[0].getUri();
		break;
	case 2:
		printf("find 2 devices.\n");
		deviceURI[0] = deviceInfo[0].getUri();
		deviceURI[1] = deviceInfo[1].getUri();
		//deviceURI = deviceInfo[0].getUri();
		//deviceURI2 = deviceInfo[1].getUri();
		break;
	default:
		break;
	}

	
	for (int i = 0; i < deviceInfo.getSize(); i++)
	{
		openniRc = device[i].open(deviceURI[i]);
		if (openniRc != openni::STATUS_OK) {
			printf("openni::device::open() failed.");
			//device[i].close();
			//nite::NiTE::shutdown();
			//openni::OpenNI::shutdown();
			//exit(0);
			Finalize();
		}
	}

	/*
	openniRc = device[0].open(deviceURI);
	if (openniRc != openni::STATUS_OK) {
		printf("openni::device::open() failed.");
		//device[i].close();
		//nite::NiTE::shutdown();
		//openni::OpenNI::shutdown();
		//exit(0);
		Finalize();
	}

	openniRc = device[1].open(deviceURI2);
	if (openniRc != openni::STATUS_OK) {
		printf("openni::device::open() failed.");
		//device[i].close();
		//nite::NiTE::shutdown();
		//openni::OpenNI::shutdown();
		//exit(0);
		Finalize();
	}
	*/
	for (int i = 0; i < deviceInfo.getSize(); i++)
	{
		//カラーストリームの生成・開始
		printf("Create color stream...");
		openniRc = colorStream[i].create(device[i], openni::SENSOR_COLOR);
		if (openniRc != openni::STATUS_OK) {
			printf("Couldn't create color stream");
			//colorStream.destroy();
			//device.close();
			//nite::NiTE::shutdown();
			//openni::OpenNI::shutdown();
			//exit(0);
			Finalize();
		}
		else {
			printf("Success!!\n Start color stream...");
			openniRc = colorStream[i].start();
			if (openniRc != openni::STATUS_OK) {
				printf("Couldn't start color stream");
				//colorStream.destroy();
				//device.close();
				//nite::NiTE::shutdown();
				//openni::OpenNI::shutdown();
				//exit(0);
				Finalize();
			}
			else printf("Success!!\n");
		}
	}

	for (int i = 0; i < deviceInfo.getSize(); i++)
	{
		//デプスストリームの生成・開始
		printf("\n\n Create depth stream...");
		openniRc = depthStream[i].create(device[i], openni::SENSOR_DEPTH);
		if (openniRc != openni::STATUS_OK) {
			printf("Couldn't create depth stream");
			//colorStream.destroy();
			//depthStream.destroy();
			//device.close();
			//nite::NiTE::shutdown();
			//openni::OpenNI::shutdown();
			//exit(0);
			Finalize();
		}
		else {
			printf("Success!! \n Start depth stream...");
			openniRc = depthStream[i].start();
			if (openniRc != openni::STATUS_OK) {
				printf("Couldn't start depth stream");
				//colorStream.stop();
				//colorStream.destroy();
				//depthStream.destroy();
				//nite::NiTE::shutdown();
				//openni::OpenNI::shutdown();
				//exit(0);
				Finalize();
			}
			else printf("Success!!\n\n");
		}
	}


	nite::NiTE::initialize();
	nite::Status niteRc;


	for (int i = 0; i < deviceInfo.getSize(); i++)
	{
		//ユーザートラッカーの生成
		niteRc = userTracker[i].create(&device[i]);
		if (niteRc != nite::STATUS_OK) {
			printf("Couldn't create user tracker\n");
			//colorStream.stop();
			//depthStream.stop();
			//userTracker.destroy();
			//colorStream.destroy();
			//depthStream.destroy();
			//nite::NiTE::shutdown();
			//openni::OpenNI::shutdown();
			//exit(0);
			Finalize();
		}
	}
    
    printf("\nStart moving around to get detected...\n");
    
    glutInit(&argc, argv);
    glutInitWindowSize(WindowWidth, WindowHeight);
    glutCreateWindow(argv[0]);
    glutInitDisplayMode(GLUT_RGBA|GLUT_DOUBLE);
    glutDisplayFunc(display);
    glutIdleFunc(idle);
    glutReshapeFunc(resize);
    glutKeyboardFunc(keyboard);
    init();
    start = std::chrono::system_clock::now();
    glutMainLoop();
    return 0;
}