#ifndef STRUCTURES_H
  #define STRUCTURES_H

  struct locomotive 
  {
    uint8_t dir = 1;
    uint8_t speed;
  };
  typedef struct locomotive loco;

  #define NUM_FSLOTS 10
  #define PULSED 128

  // Structure used for the "Hard_Coded_Values" feature when loading pre-configured Locos
  
  struct HCLoco 
  {
    char *RoadName;          //Max 8 Characters
    char *LocoType;          //Max 8 Characters
    uint16_t RoadNumber;    
    uint16_t LocoAddress;
    String FavSlot;                         //Slot (0-9) for selectedIDs[] if present
    uint8_t FSlot[NUM_FSLOTS][4];         //10 Functions, then Function Slot number, Function Number, Function Image for each function
  };

  // Structure used for the "Hard_Coded_Values" feature when loading pre-configured Accessories

  struct HCAcc
  {
    char *AccName;
    uint16_t AccAddress;
    uint8_t AccImage;
    uint8_t AccType;
  };

  // Structure used for the "Hard_Coded_Values" feature when loading pre-configured Routes

  struct HCRoute
  {
    uint8_t AccIDs[6][2];
  };  
#endif
