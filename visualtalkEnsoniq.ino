//#include <SoftwareSerial.h>

//SoftwareSerial mySerial(10, 11); // RX, TX
#include "U8glib.h"
U8GLIB_T6963_240X64 u8g(2, 3, 4, 5, 6, 7, 8, 9, A0, A1, A3, A4, A2); // 8Bit Com: D0..D7: 8,9,10,11,4,5,6,7, cs=14, a0=15, wr=17, rd=18, reset=16

#define bufflenm 128
#define fs 12
#define ss 20
#define ts 28
//  int fs = 12;
//  int ss = 20;
//  int ts = 28;

//#define FILLARRAY(a,n) a[0]=n, memcpy( ((char*)a)+sizeof(a[0]), a, sizeof(a)-sizeof(a[0]) );
//bool RdSer = true;
int inptln;   // for incoming serial data
byte MyBuff[bufflenm];
int filtered = 0;
byte BottomLine[24];
byte UnderLine[24];
byte ColonLine[24];

int BCursor;
int UCursor;

long flashcounter = 0;
bool flashstate = true;
bool underline = false;
//bool movecursor = true;
bool singleunder = false;
bool singledot = false;

bool LLoad = false;
bool LCmd = false;
bool LEdit = false;
bool LInst = false;
bool LSeq = false;
bool LMacro = false;
bool LMidi = false;
bool LSong = false;
bool LBank = false;
bool LSystem = false;
bool LPitch = false;
bool LLfo = false;

bool LLayer = false;
bool LFilter = false;
bool LWave = false;
bool LEnv = false;
bool LAmp = false;
bool LTrack = false;
bool LOdub = false;
bool LSong2 = false;
bool LRec = false;
bool LSeq2 = false;
bool LBar = false;
bool LPlay = false;

bool LStep = false;
bool LBeat = false;
bool LStop = false;
bool LRep = false;
bool LClock = false;

bool LRecF = false;
bool LLoadF = false;
bool FRec = false;
bool FLoad = false;
bool LOdubF = false;
bool FOdub = false;
bool LSeqF = false;
bool FSeq = false;
//int serl = 0;

void setup() {
  //  Serial.begin(57600);     // opens serial port, sets data rate to 9600 bps
  Serial.begin(62500, SERIAL_7N1);
  //  mySerial.begin(9600);

  BCursor = 0;
  UCursor = 0;

  //init txt buffers
  for (int ci = 0;  ci <= 23; ci++) {
    UnderLine[ci] = 0x20;
    ColonLine[ci] = 0x20;
  }
  UnderLine[23] = 0x00;
  ColonLine[23] = 0x00;
  u8g.setColorIndex(1);

  // flip screen, if required
  u8g.setRot180();
}
void ClearUnderline(void) {
  for (int ci = 0;  ci <= 22; ci++) {
    UnderLine[ci] = 0x20;
  }
}
void TxtOutByte(byte Txt) {
  BottomLine[BCursor] = Txt;
  if (underline) {
    UnderLine[BCursor] = 0x5F;

  }
  if (singleunder) {
    underline = false;

  }
  if (singledot) {
    singledot = false;
    ColonLine[BCursor] = 0x27;
  }
  //      if (movecursor) {
  BCursor++;

  //    } else movecursor = true;
  if (BCursor > 21) {
    BCursor = 0;
  }
}

void CurDebug(void) {
  u8g.setFont(u8g_font_babyr);
  u8g.setColorIndex(1);
  u8g.setPrintPos(45, fs);
  u8g.print("UCUR=");
  u8g.print(UCursor);
  u8g.print(" BCUR=");
  u8g.print(BCursor);


}

void drawBottom(void) {
  BottomLine[23] = 0x00;
  u8g.setFont(u8g_font_10x20r);
  String Bline = String((char*)BottomLine);
  u8g.setPrintPos(10, 44);
  u8g.print(Bline);
}

void drawUnder(void) {
  UnderLine[23] = 0x00;
  u8g.setFont(u8g_font_10x20r);
  String Uline = String((char*)UnderLine);
  u8g.setPrintPos(10, 46);
  u8g.print(Uline);
}

void drawColon(void) {
  ColonLine[23] = 0x00;
  u8g.setFont(u8g_font_10x20r);
  String Cline = String((char*)ColonLine);
  u8g.setPrintPos(15, 55);
  u8g.print(Cline);
}

void drawGTop(void) {

  u8g.setFont(u8g_font_babyr);
  u8g.setColorIndex(1);

  FLoad = LLoad;
  FOdub = LOdub;
  FRec = LRec;
  if (LLoadF) {
    FLoad = flashstate;
  }
  if (LOdubF) {
    FOdub = flashstate;
  }
  if (LRecF) {
    FRec = flashstate;
  }
  if (LLoad or LLoadF) {
    u8g.setColorIndex((int)FLoad);
    u8g.drawBox(4, fs - 6, 20, 7);
    u8g.setColorIndex((int)!FLoad);
    u8g.setPrintPos(5, fs);
    u8g.print("LOAD");
    u8g.setColorIndex(1);
  }

  if (LCmd) {
    u8g.drawBox(4, ss - 6, 17, 7);
    u8g.setColorIndex(0);
    u8g.setPrintPos(5, ss);
    u8g.print("CMD");
    u8g.setColorIndex(1);
  }
  if (LEdit) {
    u8g.drawBox(4, ts - 6, 19, 7);
    u8g.setColorIndex(0);
    u8g.setPrintPos(5, ts);
    u8g.print("EDIT");
    u8g.setColorIndex(1);
  }
  if (LInst) {
    u8g.setPrintPos(25, fs);
    u8g.print("INST");
  }
  if (LSeq) {
    u8g.setPrintPos(23, ss);
    u8g.print("SEQ");
  }
  if (LMacro) {
    u8g.setPrintPos(25, ts);
    u8g.print("MACRO");
  }
  if (LMidi) {

    u8g.setPrintPos(44, fs);
    u8g.print("MIDI");
  }
  if (LSong) {
    u8g.setPrintPos(40, ss);
    u8g.print("SONG");
  }
  if (LBank) {
    u8g.setPrintPos(52, ts);
    u8g.print("BANK");
  }

  if (LSystem) {
    u8g.setPrintPos(64, fs);
    u8g.print("SYSTEM");
  }
  if (LPitch) {
    u8g.setPrintPos(62, ss);
    u8g.print("PITCH");
  }
  if (LLfo) {
    u8g.setPrintPos(75, ts);
    u8g.print("LFO");
  }
  if (LLayer) {
    u8g.setPrintPos(94, fs);
    u8g.print("LAYER");
  }
  if (LFilter) {
    u8g.setPrintPos(90, ss);
    u8g.print("FILTER");
  }
  if (LWave) {
    u8g.setPrintPos(90, ts);
    u8g.print("WAVE");
  }
  if (LEnv) {
    u8g.setPrintPos(120, fs);
    u8g.print("ENV");
  }
  if (LAmp) {
    u8g.setPrintPos(119, ss);
    u8g.print("AMP");
  }
  if (LTrack) {
    u8g.setPrintPos(120, ts);
    u8g.print("TRACK");
  }
  if (LOdub or LOdubF) {
    u8g.setColorIndex((int)FOdub);
    u8g.drawBox(137, fs - 6, 22, 7);
    u8g.setColorIndex((int)!FOdub);
    u8g.setPrintPos(138, fs);
    u8g.print("ODUB");
    u8g.setColorIndex(1);
  }
  if (LSong2) {
    u8g.setPrintPos(138, ss);
    u8g.print("SONG");
  }
  FRec = LRec;
  if (LRecF) {
    FRec = flashstate;
  }
  if (LRec or LRecF) {
    u8g.setColorIndex((int)FRec);
    u8g.drawBox(160, fs - 6, 24, 7);
    u8g.setColorIndex((int)!FRec);
    u8g.setPrintPos(161, fs);
    u8g.print("REC");
    u8g.drawCircle(180, fs - 3, 2);
    u8g.setColorIndex(1);
  }
  if (LSeq2) {
    u8g.setPrintPos(170, ss);
    u8g.print("SEQ");
  }
  if (LBar) {
    u8g.setPrintPos(163, ts);
    u8g.print("BAR");
  }
  if (LPlay) {
    u8g.setPrintPos(190, fs);
    u8g.print("PLAY");
  }
  if (LStep) {
    u8g.setPrintPos(190, ss);
    u8g.print("STEP");
  }
  if (LBeat) {
    u8g.setPrintPos(190, ts);
    u8g.print("BEAT");
  }
  if (LStop) {
    u8g.setPrintPos(210, fs);
    u8g.print("STOP");
  }
  if (LRep) {
    u8g.setPrintPos(210, ss);
    u8g.print("REP");
  }
  if (LClock) {
    u8g.setPrintPos(213, ts);
    u8g.print("CLOCK");
  }
}


void DisplayOut(void) {
  u8g.firstPage();
  do {
    drawBottom();
    drawUnder();
    drawColon();
    drawGTop();
    //CurDebug();
  } while ( u8g.nextPage() );
}

int CmdDetect(byte Buff[bufflenm], int txtln) {

  int ii = 0;

  while (ii <= txtln) {
    //    if ((Buff[ii] >= 0x74) and (Buff[ii] <= 0x7C) and (Buff[ii + 1] >= 0x00) and (Buff[ii + 1] <= 0x0F)) {
    // --LED CLEAN
    //--- detect
    singledot = false;
    // LED off
    if (Buff[ii] == 0x74) {
      ii++;
    }
    //LED on
    else if (Buff[ii] == 0x75) {
      ii++;
    }
    //LED blink
    else if (Buff[ii] == 0x76) {
      ii++;
    }
    //end LED CLEAN---------------
    //--- detect
    //-----------------------------------------------------------------------------
    else if (Buff[ii] == 0x77) {
      bool on = true;
      if (Buff[ii + 1] == 0x00) {
        //gEfx(on);
      }
      if (Buff[ii + 1] == 0x01) {
        //gUnc(on);
      }
      if (Buff[ii + 1] == 0x02) {
        LSeq = on;
      }
      if (Buff[ii + 1] == 0x03) {
        LMidi = on;
      }
      if (Buff[ii + 1] == 0x04) {
        LSong = on;
      }
      if (Buff[ii + 1] == 0x05) {
        LEdit = on;
      }
      if (Buff[ii + 1] == 0x06) {
        LFilter = on;
      }
      if (Buff[ii + 1] == 0x07) {
        LBank = on;
        //gUnc(on);
      }
      if (Buff[ii + 1] == 0x08) {
        LWave = on;
      }
      if (Buff[ii + 1] == 0x09) {
        LLfo = on;
      }
      if (Buff[ii + 1] == 0x0A) {
        LPitch = on; //pitch
      }
      if (Buff[ii + 1] == 0x0B) {
        LLayer = on;
      }
      if (Buff[ii + 1] == 0x0C) {
        LSystem = on;
      }
      if (Buff[ii + 1] == 0x0D) {
        LCmd = on;
      }
      if (Buff[ii + 1] == 0x0E) {
        LInst = on;
      }
      if (Buff[ii + 1] == 0x0F) {
        LLoad = on;
      }

      ii++;
    }

    else if (Buff[ii] == 0x78) {
      bool on = false;
      if (Buff[ii + 1] == 0x00) {
        // gEfx(on);
      }
      if (Buff[ii + 1] == 0x01) {
        //gUnc(on);
      }
      if (Buff[ii + 1] == 0x02) {
        LSeq = on;
      }
      if (Buff[ii + 1] == 0x03) {
        LMidi = on;
      }
      if (Buff[ii + 1] == 0x04) {
        LSong = on;
      }
      if (Buff[ii + 1] == 0x05) {
        LEdit = on;
      }
      if (Buff[ii + 1] == 0x06) {
        LFilter = on;
      }
      if (Buff[ii + 1] == 0x07) {
        LBank = on;
        //gUnc(on);
      }
      if (Buff[ii + 1] == 0x08) {
        LWave = on;
      }
      if (Buff[ii + 1] == 0x09) {
        LLfo = on;
      }
      if (Buff[ii + 1] == 0x0A) {
        LPitch = on; //pitch
      }
      if (Buff[ii + 1] == 0x0B) {
        LLayer = on;
      }

      if (Buff[ii + 1] == 0x0C) {
        LSystem = on;
      }
      if (Buff[ii + 1] == 0x0D) {
        LCmd = on;
      }
      if (Buff[ii + 1] == 0x0E) {
        LInst = on;
      }
      if (Buff[ii + 1] == 0x0F) {
        LLoad = on;
        LLoadF = on;
      }
      ii++;
    }

    else if (Buff[ii] == 0x79) {
      if (Buff[ii + 1] == 0x0F) {
        LLoadF = true; //flash load?
        LLoad = false;
      }
      if (Buff[ii + 1] == 0x02) {
        LSeqF = true; //flash load?
        LSeq = false;
      }
      ii++;
    }

    else if (Buff[ii] == 0x7A) {
      bool on = true;
      if (Buff[ii + 1] == 0x01) {
        LBar = on; //not sure
      }
      if (Buff[ii + 1] == 0x02) {
        LSong2 = on;//right side Song
      }
      if (Buff[ii + 1] == 0x03) {
        LRec = on;
        LRecF = false;
      }
      if (Buff[ii + 1] == 0x04) {
        LSeq2 = on; //???? just idea, not tested
      }
      if (Buff[ii + 1] == 0x05) {
        LTrack = on;
      }
      if (Buff[ii + 1] == 0x06) {
        LRep = on; //not sure
      }
      if (Buff[ii + 1] == 0x07) {
        LBeat = on; //not sure
      }
      if (Buff[ii + 1] == 0x08) {
        LMacro = on; //???? just idea, not tested
      }
      if (Buff[ii + 1] == 0x09) {
        LClock = on;
      }
      if (Buff[ii + 1] == 0x0A) {
        LStep = on;
      }
      if (Buff[ii + 1] == 0x0B) {
        LStop = on;
      }
      if (Buff[ii + 1] == 0x0C) {
        LPlay = on; //play
      }
      if (Buff[ii + 1] == 0x0D) {
        LAmp = on;
      }
      if (Buff[ii + 1] == 0x0E) {
        LOdub = on;
      }
      if (Buff[ii + 1] == 0x0F) {
        LEnv = on; //env
      }


      ii++;
    }

    else if (Buff[ii] == 0x7B) {
      bool on = false;
      if (Buff[ii + 1] == 0x01) {
        LBar = on;
      }

      if (Buff[ii + 1] == 0x02) {
        LSong2 = on;
      }
      if (Buff[ii + 1] == 0x03) {
        LRec = on;
        LRecF = on;
      }
      if (Buff[ii + 1] == 0x04) {
        LSeq2 = on; //????
      }
      if (Buff[ii + 1] == 0x05) {
        LTrack = on;
      }
      if (Buff[ii + 1] == 0x06) {
        LRep = on;
      }
      if (Buff[ii + 1] == 0x07) {
        LBeat = on;
      }
      if (Buff[ii + 1] == 0x08) {
        LMacro = on;
      }
      if (Buff[ii + 1] == 0x09) {
        LClock = on;
      }
      if (Buff[ii + 1] == 0x0A) {
        LStep = on;
      }
      if (Buff[ii + 1] == 0x0B) {
        LStop = on;
      }
      if (Buff[ii + 1] == 0x0C) {
        LPlay = on; //ply
      }
      if (Buff[ii + 1] == 0x0D) {
        LAmp = on;
      }
      if (Buff[ii + 1] == 0x0E) {
        LOdub = on;
        LOdubF = on;
      }
      if (Buff[ii + 1] == 0x0F) {
        LEnv = on; //env
      }
      ii++;
    }
    else if (Buff[ii] == 0x7C) {
      if (Buff[ii + 1] == 0x03) {
        LRecF = true;
        LRec = false;
      }

      if (Buff[ii + 1] == 0x0E) {
        LOdubF = true;
        LOdub = false;
      }
      ii++;
    }
    //  }


    else if (Buff[ii] == 0x67) {
      if (Buff[ii + 1] == 0x71) {
        //display reset or initialise?
      }
      ii++;
    }
    else if (Buff[ii] == 0x71) {
      //      if (Buff[ii + 1] == 0x7E) {
      //        //unknown
      //      }
      //      ii++;
    }
    else if (Buff[ii] == 0x7C) {
      // unknown
      ii++;
    }
    //-------------------------------------------
        else if (Buff[ii] == 0x60) {
          if (Buff[ii + 1] == 0x01) {
            // set Underline cursor
    
            ii++;
//            UCursor = BCursor;
//            underline = true;
    
          }
          if (Buff[ii + 1] == 0x03) {
            // set Underline cursor
            ii++;
//            UCursor = BCursor;
//            underline = true;
            //underline = false;
          }
    
        }
    //---------------------------------------------
    else if ((Buff[ii] == 0x01) and (Buff[ii + 1] == 0x65)) {
      BottomLine[0] = Buff[ii + 2];
      BottomLine[1] = Buff[ii + 3];
      ii++; ii++; ii++;
    }
    //--------------------------------------------------------------------------------------
    //cursor detect c
    else if (Buff[ii] == 0x63) {
      //c

      if ((Buff[ii + 1] == 0x6A) and (Buff[ii + 2] == 0x67)) {
        //jgb cursor forward
        UCursor++;
        if (UCursor > 22) {
          UCursor = 22;
        }
        ii = ii + 2;
      }
      else if ((Buff[ii + 1] == 0x69) and (Buff[ii + 2] == 0x65) and (Buff[ii + 3] == 0x67)) {
        //iegb cursor back
        UCursor--;
        if (UCursor < 0) {
          UCursor = 0;
        }
        ii = ii + 3;

      }
      else if ((Buff[ii + 1] == 0x72) and (Buff[ii + 2] == 0x67) and (Buff[ii + 3] == 0x72)) {
        //rgr  show cursor?
        ii = ii + 3;

      }
      //      else {
      //        BCursor--;
      //      }
      //     CurDebug();
      //movecursor = false;
      BCursor = UCursor;
      ClearUnderline();
      UnderLine[UCursor] = 0x5F;
    }
    //-------------------------------------------------------------------------------------------
    else if (Buff[ii] == 0x62) {
      singleunder = false;
      UCursor = BCursor;
      if ((Buff[ii + 1] == 0x62) and (Buff[ii + 2] == 0x62)) {
        singleunder = true;
        ii++; ii++;

      }
      if ((Buff[ii + 1] == 0x60)) {
        underline = true;
        UCursor = BCursor;
        ii++; ii++;

      }

      ClearUnderline();
      //underline = true;

      //      for (int ci = 0;  ci <= 22; ci++) {
      //        UnderLine[ci] = 0x20;
      //      }
      UnderLine[UCursor] = 0x5F;
    }
    //-----------------------------------------------------------------------------
    else if (Buff[ii] == 0x72) {
      underline = false;

    }

     else if ((Buff[ii] >= 0x00) and (Buff[ii] <= 0x16)) {
      //locate cursor to 0f position

      BCursor = Buff[ii];
      UCursor = Buff[ii];
    }
      
//    else if (Buff[ii] == 0x14) {
//      if (BCursor > 1) {
//        BCursor--; BCursor--;
//      } else {
//        BCursor = 0;
//      }
//    }
//    else if (Buff[ii] == 0x15) {
//      //end filename string
//      //  for (int ci = BCursor;  ci <= 22; ci++) {
//      //   BottomLine[ci] = 0x20;
//      // }
//      BCursor = 0;
//      UCursor = 0;
//    }
//    else if (Buff[ii] == 0x0F) {
//      //locate cursor to 0f position
//
//      BCursor = 15;
//      UCursor = 15;
//    }
    // line clean, txt out f
    else if (Buff[ii] == 0x66) {
      //      movecursor = true;
      underline = false;
      singleunder = false;
      singledot = false;
      BCursor = 0;
      UCursor = 0;
      for (int ci = 0;  ci <= 23; ci++) {
        BottomLine[ci] = 0x20;
        UnderLine[ci] = 0x20;
        ColonLine[ci] = 0x20;
      }
    }
    else if (Buff[ii] == 0x00) {
      //    movecursor = true;
      //underline = false;
      //singleunder = false;
      BCursor = 0;
      UCursor = 0;
    }
    else if (Buff[ii] == 0x12) {
      //    movecursor = true;
      //underline = false;
      //singleunder = false;
      BCursor = 18;
      UCursor = 18;
    }
    else if (Buff[ii] == 0x21) {
      //0 with dot
      // Buff[ii] = 0x30;
      singledot = true;
      TxtOutByte(0x30);

    }
    else if (Buff[ii] == 0x23) {
      //1 with dot
      singledot = true;
      TxtOutByte(0x31);
    }
    else if (Buff[ii] == 0x25) {
      //2 with dot
      //Buff[ii] = 0x32;
      singledot = true;
      TxtOutByte(0x32);

    }
    else if (Buff[ii] == 0x28) {
      //3 with dot
      //Buff[ii] = 0x33;
      singledot = true;
      TxtOutByte(0x33);

    }
    else if (Buff[ii] == 0x29) {
      //4 with dot
      //Buff[ii] = 0x34;
      singledot = true;
      TxtOutByte(0x34);

    }
    else if (Buff[ii] == 0x3A) {
      //5 with dot
      singledot = true;
      TxtOutByte(0x35);

    }
    else if (Buff[ii] == 0x3B) {
      //6 with dot
      singledot = true;
      TxtOutByte(0x36);

    }
    else if (Buff[ii] == 0x5B) {
      //7 with dot
      singledot = true;
      TxtOutByte(0x37);

    }
    else if (Buff[ii] == 0x5C) {
      //8 with dot
      singledot = true;
      TxtOutByte(0x38);

    }
    else if (Buff[ii] == 0x5D) {
      //9 with dot
      singledot = true;
      TxtOutByte(0x39);

    }
    else if ((Buff[ii] >= 0x20) and (Buff[ii] <= 0x5A)) {
      if (BCursor < 0) {
        BCursor = 0;
      }
      TxtOutByte(Buff[ii]);
      //      BottomLine[BCursor] = Buff[ii];
      //      if (underline) {
      //        UnderLine[BCursor] = 0x5F;
      //      }
      //      if (singleunder) {
      //        underline = false;
      //      }
      //      if (singledot) {
      //        singledot = false;
      //        ColonLine[BCursor] = 0x2E;
      //      }
      //      //      if (movecursor) {
      //      BCursor++;
      //      //    } else movecursor = true;
      //      if (BCursor >= 23) {
      //        BCursor = 22;
      //      }
    }
    ii++ ;
  }
  return 1;
}



void outp(byte OutBuff2[bufflenm], int txtln) {
  //  OCmdDetect(OutBuff2, txtln);
  CmdDetect(OutBuff2, txtln);
  DisplayOut();
}



void loop() {
  flashcounter++;
  if (flashcounter >= 120000) {
    flashstate = !flashstate;
    flashcounter = 0;
    DisplayOut();
  }
  int i;
  i = 0;

  // serial input is basic of this device
  if (Serial.available()) {
    /* do {
        serl = Serial.available();
        delay(20);
      }    while (Serial.available() > serl);
      //RdSer = true;
    */

    do {
      MyBuff[i] = Serial.read();
      //Serial.write(MyBuff[i]);
      i++ ;
      //} while ((Serial.available() > 0) and (MyBuff[i] != 0x15));
      delay(10);

    }    while (Serial.available() > 0);
    //if buff not clean - make output
    if (i > 0) {
      outp(MyBuff, i - 1);
    }
  }
}





/*
  void drawTop(void) {
  // graphic commands to redraw the complete screen should be placed here
  //  TopLine[23] = 0x00;
  u8g.setFont(u8g_font_10x20);
  //u8g.setFont(u8g_font_osb21);
  String Bline = String((char*)TopLine);
  // String Bleene = "ggg";
  // u8g.drawStr( 10, 60, Bleene);
  u8g.setPrintPos(10, 30);
  u8g.print(Bline);
  }
*/


/*
  void PrintHex8(uint8_t *data, uint8_t length) // prints 8-bit data in hex with leading zeroes
  {
  char tmp[16];
  for (int i = 0; i < length; i++) {
    sprintf(tmp, "%.2X", data[i]);
    mySerial.print(tmp); mySerial.print(" ");
  }
  }
*/
