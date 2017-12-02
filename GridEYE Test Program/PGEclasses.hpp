//
//  PGEclasses.hpp
//  GridEYE Test Program
//
//  Created by Richard Oman on 12/1/17.
//  Copyright © 2017 Richard Oman. All rights reserved.
//

#ifndef PGEclasses_hpp
#define PGEclasses_hpp

#include <iostream>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>

using namespace std;

// GrideEYE Class
//-----------------------------------------------------------------
class GridEYE{
friend class video;
private:
    int FPS;        // 1 or 10 FPS
    int runtime;    // Run Time in seconds
    bool DR;        // True: TRUE, Human: FALSE
    int fd;         // File Descriptor for I2C functions

public:
    GridEYE();
    GridEYE(int address); //Hint: My board has it set at 0x68 :)
    ~GridEYE();
    
    void reset();
    void wake();
    int read( int pixAddr );
    
    void setFPS( int temp );
    void setRunTime( int newTime );

    int getfd();
    int getFPS();
};

// Frame Class
//-----------------------------------------------------------------
class frame{                        //      Stores sensor data
                                    //     0  1  2  3  4  5  6  7
protected:                          // 0  [] [] [] [] [] [] [] []
    float mean;                     // 1  [] [] [] [] [] [] [] []
    short max;                      // 3  [] [] [] [] [] [] [] []
    short sensor_values[8][8];      // 4  [] [] [] [] [] [] [] []
                                    // 4  [] [] [] [] [] [] [] []
    void set_max();         // 5  [] [] [] [] [] [] [] []
    void set_mean();        // 6  [] [] [] [] [] [] [] []
    // 7  [] [] [] [] [] [] [] []
public:
    
    frame();
    frame(GridEYE* gPtr);
    ~frame();
    
    short access( short row , short col );
    short get_max();
    float get_mean();
    
    void new_max( short newMax );
    void new_mean( float newMean );
    
    void print();
    
};

// Video Class
//-----------------------------------------------------------------
class video{
private:
    short frameCount; // frameCount = FPS * runtime
    short FPS;
    int runtime;
    
    short max;
    float mean;
    
    vector< frame* > data;  // Storing up to 31,800 frames maximum
    
    void set_max();
    void set_mean();
    
public:
    video();
    video( GridEYE* gPtr );
    ~video();
    
    void addFrame( frame* fPtr );
    frame* getFrame( int index );
    
    void set_runtime( int val );
    short getframeCount();
    void exportVideo( string filename );
    void print();
    
};

// Session Class
//-----------------------------------------------------------------
class session{
private:
    vector< video* > current; // session.current[vCount]
    int vCount;
    
public:
    session();
    session( video* newVid );
    ~session();
    
    void addVideo( video* newVid ); // Implement Save button
    void undoRec();                 // Implement Delete Button
    void print();
};
#endif /* PGEclasses_hpp */
