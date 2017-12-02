//
//  PGEclasses.cpp
//  GridEYE Test Program
//
//  Created by Richard Oman on 12/1/17.
//  Copyright © 2017 Richard Oman. All rights reserved.
//

#include "PGEclasses.hpp"

#define PGE     0x68    // Panasonic GridEye Address
#define PCR     0x00    // Power Control Register// Normal Mode Cmd: 0x00
#define RESET   0x01    // Reset Register
#define FrRate  0x02    // Frame Rate Register//    10/default: 0x00, 1: 0x01

int row, col;

//-----------------------------------------------------------------
// GridEYE Methods
//-----------------------------------------------------------------
GridEYE::GridEYE(){
    fd = wiringPiI2CSetup( PGE );
    wiringPiI2CWriteReg16(fd, PCR, 0);
}

int GridEYE::read( int pixAddr ){
    int temp = 0;
    wiringPiI2CWriteReg16( fd, pixAddr, 1 );    // Write to pixel, requests data
    temp = wiringPiI2CReadReg16( fd, pixAddr ); // Receive value from pixel
    temp = temp >> 4;                           // Thermistor has 12-bit data
                                                // Shift 4 removes precision Bits, makes short data 8-bit temperature
    return temp;
}

void GridEYE::reset(void){
    FPS = 10;
    wiringPiI2CWriteReg16( fd, 0x02, 0 ); // Resets Frame rate register to default
    DR = true;
    return;
}

int GridEYE::getfd(){
    return this->fd;
}

int GridEYE::getFPS(){
    return this->FPS;
}

void GridEYE::setFPS(int temp){
    this->FPS = temp;
    try{
        if( temp == 1 || temp == 10 )
            throw -1;
        if( temp == 1 )
            wiringPiI2CWriteReg16( fd, 0x02, 1 );   // Sets Frame rate register to 1 FPS
        if( temp == 10 )
            wiringPiI2CWriteReg16(fd, 0x02, 0);     // Sets Frame rate register to 10 FPS
        }
    catch( int ){
        cout << "Exception Handled: invalid setting value" << endl;
    }
    return;
}

void GridEYE::setRunTime( int newTime ){
    this->runtime = newTime;
}

//-----------------------------------------------------------------
// Frame Methods
//-----------------------------------------------------------------
frame::frame(){
    for( row=0 ; row < 8 ; row++ ){
        for( col=0 ; col < 8 ; col++){
            this->sensor_values[row][col] = 0;
        }
    }
    this->mean = 0;
    this->max = 0;
    return;
}

frame::frame(GridEYE* gPtr){
    int temp = 0;
    int pixAddr = 0x80;
    
    for( row = 0 ; row < 8 ; row++ ){
        for( col = 0 ; col < 8 ;  col++){
            temp = gPtr->read( pixAddr );                // Read Thermistor Data
            this->sensor_values[row][col] = (short)temp;    // Stores temp value in sensor table
            pixAddr += 2;                                   // Increment to next pixel
        }
    }
    this->print();
    set_max();
    set_mean();
    return;
    
    return;
}
//-----------------------------------------------------------------
// Frame Methods
//-----------------------------------------------------------------
void frame::set_max(){
    short temp = 0;
    
    for( row = 0 ; row < 8 ; row++ ){
        for( col = 0 ; col < 8 ;  col++){
            if( this->sensor_values[row][col] > temp )
                temp = this->sensor_values[row][col];
        }
    }
    this->max = temp;
}

void frame::set_mean(){
    float sum = 0;
    
    for( row = 0 ; row < 8 ; row++ ){
        for( col = 0 ; col < 8 ;  col++){
            sum += this->sensor_values[row][col];
        }
    }
    this->mean = sum/64;
}

short frame::get_max(){
    return this->max;
}

float frame::get_mean(){
    return this->mean;
}

void frame::print(){
    
    for( col = 0 ; col < 8 ; col++){                                        // Frame No. : 1
        cout << "\t"                                      // TAB [ 1] [ 2] [ 3] [ 4] [ 5] [ 6] [ 7] [ 8
        << "[ " << this->sensor_values[col][0] << " ] "   // TAB [ 9] [10] [11] [12] [13] [14] [15] [16]
        << "[ " << this->sensor_values[col][1] << " ] "   // TAB [17] [18] [19] [20] [21] [22] [23] [24]
        << "[ " << this->sensor_values[col][2] << " ] "   // TAB [25] [26] [27] [28] [29] [30] [31] [32]
        << "[ " << this->sensor_values[col][3] << " ] "   // TAB [33] [34] [35] [36] [37] [38] [39] [40]
        << "[ " << this->sensor_values[col][4] << " ] "   // TAB [41] [42] [43] [44] [45] [46] [47] [48]
        << "[ " << this->sensor_values[col][5] << " ] "   // TAB [49] [50] [51] [52] [53] [54] [55] [56]
        << "[ " << this->sensor_values[col][6] << " ] "   // TAB [57] [58] [59] [60] [61] [62] [63] [64]
        << "[ " << this->sensor_values[col][7] << " ] " << endl;
    }
    return;
}

short frame::access( short row , short col ){
    return this->sensor_values[row][col];    //Accesses data point in data array
}

frame::~frame(){
}
//-----------------------------------------------------------------

//-----------------------------------------------------------------
// Video Constructors
//-----------------------------------------------------------------
video::video(){
    this->frameCount = 0;
    
}
video::video( GridEYE* gPtr ){
    frame* temp;
    
    frameCount = (gPtr->FPS * gPtr->runtime);
    
    for( int x = 0 ; x < frameCount ; x++){
        temp = new frame( gPtr );       // Collect data and create frame
        this->data.push_back( temp );       // Store pointer in data Vector
    }
    this->set_max();
    this->set_mean();
    return;
}
//-----------------------------------------------------------------
// Video Methods
//-----------------------------------------------------------------
void video::addFrame(frame* fPtr){
    this->data.push_back(fPtr);
    this->frameCount++;
    this->set_max();
    this->set_mean();
}

void video::exportVideo( string filename ){
    fstream newOutput;                      // Creates/Opens new output file
    newOutput.open( filename, ios::out );
    
    newOutput << "*" << frameCount;    // Copies data from memory to file
    
    for( int x = 0; x < frameCount; x++ ){
        frame* temp = (data[x]);
        newOutput << "%" << x;                                              // Begin Packet % indicates frame number
        newOutput << "&" << temp->get_max() << "&" << temp->get_mean();     // & indicates sub-data value
        for( int y = 0; y < 8 ; y++){
            newOutput << "@" << "$" << temp->access( y, 0 )             // @ indicates row begin
            << "$" << temp->access( y, 1 )
            << "$" << temp->access( y, 2 )             // $ indicates data value
            << "$" << temp->access( y, 3 )
            << "$" << temp->access( y, 4 )
            << "$" << temp->access( y, 5 )
            << "$" << temp->access( y, 6 )
            << "$" << temp->access( y, 7 );
        }
        newOutput << "#"; // End Packet
    }
    newOutput.close( ); // Close file
    return;
}

void video::set_max(){
    short temp = 0;
    frame* framePtr = NULL;
    
    while(temp < this->frameCount){
        framePtr = data[temp];
        for( row = 0 ; row < 8 ; row++ ){
            for( col = 0 ; col < 8 ;  col++){
                if( framePtr->access(row,col) > temp )
                    temp = framePtr->access(row,col);
            }
        }
        temp++;
    }
    framePtr->new_max( temp );
    return;
}

void video::set_mean(){
    short temp = 0;
    float sum = 0;
    frame* framePtr = NULL;
    
    while(temp < this->frameCount){
        framePtr = data[temp];
        for( row = 0 ; row < 8 ; row++ ){
            for( col = 0 ; col < 8 ;  col++){
                sum += framePtr->get_mean();
            }
        }
        temp++;
    }
    framePtr->new_mean( sum / (64*frameCount) );
}

void video::print(){
    frame* temp;
    for( row = 0 ; row < frameCount ; row++ ){
        temp = this->data[row];
        cout << "Frame No. : " << row + 1 << endl;
        for( col = 0 ; col < 8 ; col++){                // Frame No. : 1
            cout << "\t"                                // TAB [ 1] [ 2] [ 3] [ 4] [ 5] [ 6] [ 7] [ 8
            << "[ " << temp->access(col, 0) << " ]\t"   // TAB [ 9] [10] [11] [12] [13] [14] [15] [16]
            << "[ " << temp->access(col, 1) << " ]\t"   // TAB [17] [18] [19] [20] [21] [22] [23] [24]
            << "[ " << temp->access(col, 2) << " ]\t"   // TAB [25] [26] [27] [28] [29] [30] [31] [32]
            << "[ " << temp->access(col, 3) << " ]\t"   // TAB [33] [34] [35] [36] [37] [38] [39] [40]
            << "[ " << temp->access(col, 4) << " ]\t"   // TAB [41] [42] [43] [44] [45] [46] [47] [48]
            << "[ " << temp->access(col, 5) << " ]\t"   // TAB [49] [50] [51] [52] [53] [54] [55] [56]
            << "[ " << temp->access(col, 6) << " ]\t"   // TAB [57] [58] [59] [60] [61] [62] [63] [64]
            << "[ " << temp->access(col, 7) << " ]" << endl;
        }
    }
}

void video::set_runtime( int val ){
    runtime = FPS * frameCount;
}

video::~video(){
    
}
//-----------------------------------------------------------------


//-----------------------------------------------------------------
// Session Methods
//-----------------------------------------------------------------
session::session(){     // Default Constructor
    this->vCount = 0;                           // Initializes video count to zero
    cout << "Session Started" << endl << endl;
    return;
}

session::session( video* newVid ){      // constructor adds video pointer to session array
    cout << "Session Started" << endl << endl;
    try{
        if( !( this->vCount < 1 ) )
            throw -1;                       // Maybe throw error class instead, can do something later in main
        this->current.push_back( newVid );  // Adds video pointer into the array in first position
        this->vCount++;
    }
    catch( int ){
        cout << "Exception Handled" << endl;
    }
}


void session::addVideo( video* newVid ){
    this->current.push_back( newVid );  // Append "active" recording to video stack
    this->vCount++;                     // Update video count value
}

void session::undoRec(){        // Removes "active" recording from the stack of videos
    this->current.pop_back();   // pop_back() removes last value, decrements vector size by 1
    this->vCount--;             // update video count value
}

session::~session(){}

//-----------------------------------------------------------------

