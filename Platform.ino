/****************************************************************************************************

RepRapFirmware - Platform: RepRapPro Mendel with Prototype Arduino Due controller

Platform contains all the code and definitons to deal with machine-dependent things such as control 
pins, bed area, number of extruders, tolerable accelerations and speeds and so on.

-----------------------------------------------------------------------------------------------------

Version 0.1

18 November 2012

Adrian Bowyer
RepRap Professional Ltd
http://reprappro.com

Licence: GPL

****************************************************************************************************/

#include "RepRapFirmware.h"

// Arduino initialise and loop functions
// Put nothing in these other than calls to the RepRap equivalents

void setup()
{
  reprap.Init();  
}
  
void loop()
{
  reprap.Spin();
}

//*************************************************************************************************

Platform::Platform(RepRap* r)
{
  reprap = r;
  active = false;
}

void Platform::Init()
{ 
  byte i;
  
  Serial.begin(BAUD_RATE);
  //Serial.println("\n\n\nPlatform constructor");
  
  lastTime = Time();
  
  if(!LoadFromStore())
  {     
  // DRIVES
  
    char stepPins[DRIVES] = STEP_PINS;
    char directionPins[DRIVES] = DIRECTION_PINS;
    char enablePins[DRIVES] = ENABLE_PINS;
    boolean disableDrives[DRIVES] = DISABLE_DRIVES;
    float maxFeedrates[DRIVES] = MAX_FEEDRATES;
    float maxAccelerations[DRIVES] = MAX_ACCELERATIONS;
    float driveStepsPerUnit[DRIVES] = DRIVE_STEPS_PER_UNIT;
    float jerks[DRIVES] = JERKS;
    boolean driveRelativeModes[DRIVES] = DRIVE_RELATIVE_MODES;
    
  // AXES
  
    char lowStopPins[AXES] = LOW_STOP_PINS;
    char highStopPins[AXES] = HIGH_STOP_PINS;
    float axisLengths[AXES] = AXIS_LENGTHS;
    float fastHomeFeedrates[AXES] = AXIS_LENGTHS;
   
  // HEATERS - Bed is assumed to be the first
  
    char tempSensePins[HEATERS] = TEMP_SENSE_PINS;
    char heatOnPins[HEATERS] = HEAT_ON_PINS;
    float thermistorBetas[HEATERS] = THERMISTOR_BETAS;
    float thermistorSeriesRs[HEATERS] = THERMISTOR_SERIES_RS;
    float thermistorInfRs[HEATERS] = THERMISTOR_25_RS;
    boolean usePid[HEATERS] = USE_PID;
    float pidKis[HEATERS] = PID_KIS;
    float pidKds[HEATERS] = PID_KDS;
    float pidKps[HEATERS] = PID_KPS;
    float pidILimits[HEATERS] = PID_I_LIMITS;
    
    // Files

    webDir = WEB_DIR;
    gcodeDir = GCODE_DIR;
    sysDir = SYS_DIR;
    tempDir = TEMP_DIR;
  }
  
  for(i = 0; i < DRIVES; i++)
  {
    if(stepPins[i] >= 0)
      pinMode(stepPins[i], OUTPUT);
    if(directionPins[i] >= 0)  
      pinMode(directionPins[i], OUTPUT);
    if(enablePins[i] >= 0)
    {  
      pinMode(enablePins[i], OUTPUT);
      digitalWrite(enablePins[i], ENABLE);
    }
  }
  
  for(i = 0; i < AXES; i++)
  {
    if(lowStopPins[i] >= 0)
    {
      pinMode(lowStopPins[i], INPUT);
      digitalWrite(lowStopPins[i], HIGH); // Turn on pullup
    }
    if(highStopPins[i] >= 0)
    {
      pinMode(highStopPins[i], INPUT);
      digitalWrite(highStopPins[i], HIGH); // Turn on pullup
    }
  }  
  
  
  for(i = 0; i < HEATERS; i++)
  {
    if(heatOnPins[i] >= 0)
      pinMode(heatOnPins[i], OUTPUT);
    //Serial.println(thermistorInfRs[i]);
    thermistorInfRs[i] = ( thermistorInfRs[i]*exp(-thermistorBetas[i]/(25.0 - ABS_ZERO)) );
    //Serial.println(thermistorInfRs[i]);
  }  

  // Files
 
  files = new File[MAX_FILES];
  inUse = new boolean[MAX_FILES];
  for(i=0; i < MAX_FILES; i++)
  {
    buf[i] = new byte[FILE_BUF_LEN];
    bPointer[i] = 0;
    inUse[i] = false;
  }
  
  // Network

  mac = MAC;
  server = new EthernetServer(HTTP_PORT);
  
  // disable SD SPI while starting w5100
  // or you will have trouble
  pinMode(SD_SPI, OUTPUT);
  digitalWrite(SD_SPI,HIGH);   

  Ethernet.begin(mac, *(new IPAddress(IP0, IP1, IP2, IP3)));
  server->begin();
  
  //Serial.print("server is at ");
  //Serial.println(Ethernet.localIP());
  
  // this corrects a bug in the Ethernet.begin() function
  // even tho the call to Ethernet.localIP() does the same thing
  digitalWrite(ETH_B_PIN, HIGH);
  
  clientStatus = 0;
  client = 0;
 
  if (!SD.begin(SD_SPI)) 
     Serial.println("SD initialization failed.");
  // SD.begin() returns with the SPI disabled, so you need not disable it here  
  
    // Reinitialise the message file
  
  DeleteFile(PrependRoot(GetWebDir(), MESSAGE_FILE));
  int m = OpenFile(PrependRoot(GetWebDir(), MESSAGE_TEMPLATE), false);
  int n = OpenFile(PrependRoot(GetWebDir(), MESSAGE_FILE), true);
  byte b;
  while (Read(m, b))
    Write(n,b);
  Close(m);  
  Close(n);
  
  active = true;
}

void Platform::Exit()
{
  active = false;
}

RepRap* Platform::GetRepRap()
{
  return reprap;
}


char* Platform::PrependRoot(char* root, char* fileName)
{
  strcpy(scratchString, root);
  return strcat(scratchString, fileName);
}


// Load settings from local storage; return true if successful, false otherwise

bool Platform::LoadFromStore()
{
  return false;
}

//===========================================================================
//=============================Thermal Settings  ============================
//===========================================================================

// See http://en.wikipedia.org/wiki/Thermistor#B_or_.CE.B2_parameter_equation

// BETA is the B value
// RS is the value of the series resistor in ohms
// R_INF is R0.exp(-BETA/T0), where R0 is the thermistor resistance at T0 (T0 is in kelvin)
// Normally T0 is 298.15K (25 C).  If you write that expression in brackets in the #define the compiler 
// should compute it for you (i.e. it won't need to be calculated at run time).

// If the A->D converter has a range of 0..1023 and the measured voltage is V (between 0 and 1023)
// then the thermistor resistance, R = V.RS/(1023 - V)
// and the temperature, T = BETA/ln(R/R_INF)
// To get degrees celsius (instead of kelvin) add -273.15 to T
//#define THERMISTOR_R_INFS ( THERMISTOR_25_RS*exp(-THERMISTOR_BETAS/298.15) ) // Compute in Platform constructor

// Result is in degrees celsius

float Platform::GetTemperature(byte heater)
{
  float r = (float)GetRawTemperature(heater);
  //Serial.println(r);
  return ABS_ZERO + thermistorBetas[heater]/log( (r*thermistorSeriesRs[heater]/(AD_RANGE - r))/thermistorInfRs[heater] );
}


// power is a fraction in [0,1]

void Platform::SetHeater(byte heater, const float& power)
{
  if(power <= 0)
  {
     digitalWrite(heatOnPins[heater], 0);
     return;
  }
  
  if(power >= 1.0)
  {
     digitalWrite(heatOnPins[heater], 1);
     return;
  }
  
  byte p = (byte)(255.0*power);
  analogWrite(heatOnPins[heater], p);
}


/*********************************************************************************

  Files & Communication
  
*/

// List the flat files in a directory.  No sub-directories or recursion.

char* Platform::FileList(char* directory)
{
  File dir, entry;
  dir = SD.open(directory);
  int p = 0;
  int q;
  int count = 0;
  while(entry = dir.openNextFile())
  {
    q = 0;
    count++;
    fileList[p++] = FILE_LIST_BRACKET;
    while(entry.name()[q])
    {
      fileList[p++] = entry.name()[q];
      q++;
      if(p >= FILE_LIST_LENGTH - 10) // Caution...
      {
        Message(HOST_MESSAGE, "FileList - directory: ");
        Message(HOST_MESSAGE, directory);
        Message(HOST_MESSAGE, " has too many files!<br>\n");
        return "";
      }
    }
    fileList[p++] = FILE_LIST_BRACKET;
    fileList[p++] = FILE_LIST_SEPARATOR;
    entry.close();
  }
  dir.close();
  
  if(count <= 0)
    return "";
  
  fileList[--p] = 0; // Get rid of the last separator
  return fileList;
}

// Delete a file
boolean Platform::DeleteFile(char* fileName)
{
  return SD.remove(fileName);
}

// Open a local file (for example on an SD card).

int Platform::OpenFile(char* fileName, boolean write)
{
  int result = -1;
  for(int i = 0; i < MAX_FILES; i++)
    if(!inUse[i])
    {
      result = i;
      break;
    }
  if(result < 0)
  {
      Message(HOST_MESSAGE, "Max open file count exceeded.<br>\n");
      return -1;    
  }
  
  if(!SD.exists(fileName))
  {
    if(!write)
    {
      Message(HOST_MESSAGE, "File not found for reading.<br>\n");
      return -1;
    }
    files[result] = SD.open(fileName, FILE_WRITE);
    bPointer[result] = 0;
  } else
  {
    if(write)
    {
      files[result] = SD.open(fileName, FILE_WRITE);
      bPointer[result] = 0;
    } else
      files[result] = SD.open(fileName, FILE_READ);
  }

  inUse[result] = true;
  return result;
}

void Platform::GoToEnd(int file)
{
  if(!inUse[file])
  {
    Message(HOST_MESSAGE, "Attempt to seek on a non-open file.<br>\n");
    return;
  }
  unsigned long e = files[file].size();
  files[file].seek(e);
}

void Platform::Close(int file)
{ 
  if(bPointer[file] != 0)
    files[file].write(buf[file], bPointer[file]);
  bPointer[file] = 0;
  files[file].close();
  inUse[file] = false;
}


boolean Platform::Read(int file, unsigned char& b)
{
  if(!inUse[file])
  {
    Message(HOST_MESSAGE, "Attempt to read from a non-open file.<br>\n");
    return false;
  }
    
  if(!files[file].available())
    return false;
  b = (unsigned char) files[file].read();
  return true;
}

void Platform::Write(int file, char b)
{
  if(!inUse[file])
  {
    Message(HOST_MESSAGE, "Attempt to write byte to a non-open file.<br>\n");
    return;
  }
  (buf[file])[bPointer[file]] = b;
  bPointer[file]++;
  if(bPointer[file] >= FILE_BUF_LEN)
  {
    files[file].write(buf[file], FILE_BUF_LEN);
    bPointer[file] = 0;
  } 
  //files[file].write(b);
}

void Platform::WriteString(int file, char* b)
{
  if(!inUse[file])
  {
    Message(HOST_MESSAGE, "Attempt to write string to a non-open file.<br>\n");
    return;
  }
  int i = 0;
  while(b[i])
    Write(file, b[i++]); 
  //files[file].print(b);
}


void Platform::Message(char type, char* message)
{
  switch(type)
  {
  case FLASH_LED:
  // Message that is to flash an LED; the next two bytes define 
  // the frequency and M/S ratio.
  
    break;
  
  case DISPLAY_MESSAGE:  
  // Message that is to appear on a local display;  \f and \n should be supported.
  case HOST_MESSAGE:
  default:
  
  
    int m = OpenFile(PrependRoot(GetWebDir(), MESSAGE_FILE), true);
    GoToEnd(m);
    WriteString(m, message);
    Serial.print(message);
    Close(m);
    
  }
}

// Send something to the network client

void Platform::SendToClient(char* message)
{
  if(client)
  {
    client.print(message);
    //Serial.print("Sent: ");
    //Serial.print(message);
  } else
    Message(HOST_MESSAGE, "Attempt to send string to disconnected client.<br>\n");
}

// Where the php/htm etc files are

char* Platform::GetWebDir()
{
  return webDir;
}

// Where the gcodes are

char* Platform::GetGcodeDir()
{
  return gcodeDir;
}

// Where the system files are

char* Platform::GetSysDir()
{
  return sysDir;
}

// Where the temporary files are

char* Platform::GetTempDir()
{
  return tempDir;
}


//***************************************************************************************************




void Platform::Spin()
{
  if(!active)
    return;
    
   ClientMonitor();
   if(Time() - lastTime < 2000000)
     return;
   lastTime = Time();
   //Serial.print("Client status: ");
   //Serial.println(clientStatus);
}


















