/* Copyright (c) 1997-2017, FenixOS Developers
   All Rights Reserved.

   This file is subject to the terms and conditions defined in
   file 'LICENSE', which is part of this source code package.
 */

/*! \file video.c This file holds implementations of functions
  presenting output to the VGA screen. */
#include <stdint.h>

/*! Max number of columns in the VGA buffer. */
#define MAX_COLS                (80)
/*! Max number of columns in the VGA buffer. */
#define MAX_ROWS                (25)

struct screen_position
{
 unsigned char character; /*!< The character part of the byte tuple used for
                               each screen position. */
 unsigned char attribute; /*!< The character part of the byte tuple used for
                               each screen position. */
};
/*!< Defines a VGA text mode screen position. */

struct screen
{
 struct screen_position positions[MAX_ROWS][MAX_COLS];
 /*!< The VGA screen. It is organized as a two dimensional array. */
};
/*!< Defines a VGA text mode screen. */

/*! points to the VGA screen. */
static struct screen* const
screen_pointer = (struct screen*) 0xB8000;

int counter = -1; //Means screen wasn't initialised


//Print a string
void
kprints(const char* string)
{
  int i, colour;
  colour = 0x0f; //White text on black background

  if(counter == -1){ //Initialise the screen - all characters on it set to whitespace
    for(i=0;i<MAX_ROWS*MAX_COLS; i++){
      screen_pointer->positions[i/MAX_COLS][i%MAX_COLS].character = ' ';
      screen_pointer->positions[i/MAX_COLS][i%MAX_COLS].attribute = colour;
  }
    counter = 0; //We start counting characters on the screen
  }

  //If not enough place to print the string - clear the screen and try again
  if(counter >= MAX_COLS*MAX_ROWS){
  	  counter = -1;
  	  kprints(string);
      return;
    }
  
  //Go through all characters in the string
  for(i=0;string[i]!='\0';i++){
    //For non-newline characters: set the character on a given position, increment the counter
    if(string[i] != '\n'){
    screen_pointer->positions[counter/MAX_COLS][counter%MAX_COLS].character = string[i];
    screen_pointer->positions[counter/MAX_COLS][counter%MAX_COLS].attribute = colour;
    counter++;
    } 
    //For newline character: move counter to next line
    else{
      counter = ((counter/MAX_COLS)+1)*MAX_COLS;
    }
    //If we can't print further: clear the screen and print the whole string
    if(counter >= MAX_COLS*MAX_ROWS){
	  counter = -1;
	  kprints(string);
      return;
    }
  }
}

//Print a hexadecimal number
void
kprinthex(const register uint32_t value)
{
  int mask, i, copy, current;
  char string[11] = "0xgggggggg"; //8 digits + "0x" + '\0', 'g' used to mark an error
  mask = 15; //We need four bits to get a value for each position
  copy = value;

  //Transform the number into a string
  for(i=0;i<8;i++){
  	//Get least significant four bits
    current = copy & mask;
    //Set string character on last non-initialised position according to the result
    if(current < 10){
      string[9-i] = '0' + current;
    } else{
      string[9-i] = 'A' - 10 + current;
    }
    //Get rid of used bits
    copy = copy >> 4;
  }

  //Print the string
  kprints(string);
  
}
