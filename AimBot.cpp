#include <iostream>
#include <unistd.h>
#include <sys/ptrace.h>
#include <sys/uio.h>
#include <cmath>
#include <android/log.h>
#include <cmath>
#include <unistd.h>
#include <android/log.h>
#include <stdlib.h>
#include "TouchHelper.h"

#define LOG_TAG "AIM"
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)

#define AIM_SCALE 500//Sensi


struct FRotator {
    float Pitch;
    float Yaw;
    float Roll;
};


FRotator CalcAngle(Vector3 LocalHead, Vector3 EnemyHead) {
    Vector3 delta = LocalHead - EnemyHead;
    float hyp = sqrt(delta.x * delta.x + delta.y * delta.y);
    FRotator angle;
    angle.Pitch = -atan(delta.z / hyp) * 180.0f / M_PI;
    angle.Yaw = atan(delta.y / delta.x) * 180.0f / M_PI;
    angle.Roll = 0.0f;
    if (delta.x >= 0.0f) angle.Yaw += 180.0f;
    return angle;
}


void RunAimbotLoop(uintptr_t localPlayer, uintptr_t enemyList) {
while (Bools_tt:AIMBOT) {
    auto mainTransform = Read<uintptr_t>(localPlayer + 0x1AC);
    auto localPos = get_Position(mainTransform);

    auto headTransform = HeadTF(enemyList);
    auto headPos = get_BoneNode(headTransform);

	//Vector3 headPos
    float dx = headPos.x - localPos.x;
    float dy = headPos.y - localPos.y;
    float dz = headPos.z - localPos.z;

    float dist = sqrtf(dx * dx + dy * dy + dz * dz);
    if (dist < 20.0f){
	return;
	}
    dx /= dist;
    dy /= dist;
	
    int touchX = (int)(dx * AIM_SCALE + Width);
    int touchY = (int)(-dy * AIM_SCALE + Height);

    Touch_Down(touchX, touchY);
    usleep(10000);
    Touch_Up();
	}
}
