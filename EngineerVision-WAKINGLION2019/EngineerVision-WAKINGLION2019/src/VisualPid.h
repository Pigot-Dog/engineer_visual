#ifndef VISUALPID_H
#define VISUALPID_H

#include<stdio.h>
//visual pid control yunTai by machining angle_slover
class VisualPid
{
private:
    double errorSum=0;
    double lastAngle=0;
    double lastError=0;
    unsigned int controlTimes=0;
    float p=1;
    float i=1;
    float d=1;
    unsigned int angleDivision=25;
    unsigned int stabilityCount=0;

public:
    VisualPid(float param_p,float param_i,float param_d,unsigned int param_angleDivision)
    {
        p=param_p;
        i=param_i;
        d=param_d;
        angleDivision=param_angleDivision;
    }

    double limitValueRange(double angle,double mini,double maxi)
    {
        if(angle>maxi) angle=maxi;
        else if(angle<mini) angle=mini;
        return angle;
    }

    /**
     * @brief  setParam
     * @return void
     * @autor  weiTao
     * @date   2019.4.25
     */
    void setPIDAndDivision(float param_p,float param_i,float param_d,unsigned int param_angleDivision)
    {
        p=param_p;
        i=param_i;
        d=param_d;
        angleDivision=param_angleDivision;
    }

    /**
     * @brief  calaculate control angle by sloverAngle
     * @return new angle
     * @autor  weiTao
     * @date   2019.4.25
     */
    double calAngle(double sloverAngle,int controlMethond=1)
    {
        double error=0;
        double newAngle=0;
        if(controlMethond==1)
        {
            switch(controlTimes)
            {
            case 2:
                error=sloverAngle-lastAngle;
                //printf("error=%f\n",error);
                newAngle=p*(sloverAngle/(angleDivision+1))+i*(error)+d*(error-lastError);
                lastAngle=sloverAngle;
                lastError=error;
                break;
            case 1:
                error=sloverAngle-lastAngle;
                newAngle=p*(sloverAngle/(angleDivision+1))+i*(error);
                lastAngle=sloverAngle;
                lastError=error;
                controlTimes+=1;
                break;
            case 0:
                newAngle=p*(sloverAngle/(angleDivision+1));
                lastAngle=sloverAngle;
                controlTimes+=1;
                break;

            }
        }
        else
        {
            switch(controlTimes)
            {
            case 2:
                error=sloverAngle-lastAngle;
                errorSum=limitValueRange(errorSum+sloverAngle,-10,10);
                newAngle=p*(sloverAngle/(angleDivision+1))+i*(errorSum)+d*(error-lastError);
                lastAngle=sloverAngle;
                lastError=error;
                break;
            case 1:
                error=sloverAngle-lastAngle;
                errorSum=limitValueRange(errorSum+sloverAngle,-10,10);
                newAngle=p*(sloverAngle/(angleDivision+1))+i*errorSum;
                lastAngle=sloverAngle;
                lastError=error;
                controlTimes+=1;
                break;
            case 0:
                newAngle=p*(sloverAngle/(angleDivision+1));
                lastAngle=sloverAngle;
                controlTimes+=1;
                break;

            }
        }
        stabilityCount=0;
        return newAngle;
    }

    double getAngle()
    {
        return lastAngle;
    }


    void cleanControlTimes()
    {
        controlTimes=0;
        lastAngle=0;
        lastError=0;
        errorSum=0;
    }

    void adaptiveCleanControlTimes()
    {
        stabilityCount++;
        if(stabilityCount==3)
        {
            cleanControlTimes();
        }
        if(stabilityCount>=65535)
        {
            stabilityCount=0;
        }
    }
};

#endif // VISUALPID_H
