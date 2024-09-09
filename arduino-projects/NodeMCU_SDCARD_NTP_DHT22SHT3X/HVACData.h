#ifndef HVACDATA_H
#define HVACDATA_H

#include "Arduino.h"

class HVACData
{
  public:
    void SetData(float data, String sensorType, String dataType);
    float GetData();
    String GetSensorType();
    String GetDataType();
  private:
    float _data;
    String _sensorType;
    String _dataType;
};

#endif
