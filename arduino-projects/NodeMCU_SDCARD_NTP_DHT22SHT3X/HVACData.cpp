#include "Arduino.h"
#include "HVACData.h"

void HVACData::SetData(float data, String sensorType, String dataType)
{
  _data = data;
  _sensorType = sensorType;
  _dataType = dataType;
}
  
float HVACData::GetData()
{
  return _data;
}

String HVACData::GetSensorType()
{
  return _sensorType;
}

String HVACData::GetDataType()
{
  return _dataType;
}
