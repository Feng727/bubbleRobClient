// Make sure to have the server side running in CoppeliaSim!
// Start the server from a child script with following command:
// simExtRemoteApiStart(portNumber) -- starts a remote API server service on the specified port

#include <stdio.h>
#include <stdlib.h>

extern "C" {
    #include "extApi.h"
}

int main(int argc,char* argv[])
{
    int portNb=0;
    int leftMotorHandle;
    int rightMotorHandle;
    int sensorHandle;

    if (argc>=5)
    {
        portNb=atoi(argv[1]);
        leftMotorHandle=atoi(argv[2]);
        rightMotorHandle=atoi(argv[3]);
        sensorHandle=atoi(argv[4]);
    }
    else
    {
        printf("Indicate following arguments: 'portNumber leftMotorHandle rightMotorHandle sensorHandle'!\n");
        extApi_sleepMs(5000);
        return 0;
    }

    int clientID=simxStart((simxChar*)"127.0.0.1",portNb,true,true,2000,5);
    if (clientID!=-1)
    {
        int driveBackStartTime=-99000;
        float motorSpeeds[2];

        while (simxGetConnectionId(clientID)!=-1)
        {
            simxUChar sensorTrigger=0;
            if (simxReadProximitySensor(clientID,sensorHandle,&sensorTrigger,NULL,NULL,NULL,simx_opmode_streaming)==simx_return_ok)
            { // We succeeded at reading the proximity sensor
                int simulationTime=simxGetLastCmdTime(clientID);
                if (simulationTime-driveBackStartTime<3000)
                { // driving backwards while slightly turning:
                    motorSpeeds[0]=-3.1415f*0.5f;
                    motorSpeeds[1]=-3.1415f*0.25f;
                }
                else
                { // going forward:
                    motorSpeeds[0]=3.1415f;
                    motorSpeeds[1]=3.1415f;
                    if (sensorTrigger)
                        driveBackStartTime=simulationTime; // We detected something, and start the backward mode
                }
                simxSetJointTargetVelocity(clientID,leftMotorHandle,motorSpeeds[0],simx_opmode_oneshot);            
                simxSetJointTargetVelocity(clientID,rightMotorHandle,motorSpeeds[1],simx_opmode_oneshot);           
            }
            extApi_sleepMs(5);
        }
        simxFinish(clientID);
    }
    return(0);
}

