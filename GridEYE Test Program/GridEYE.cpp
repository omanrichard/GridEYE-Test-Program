//
//  main.cpp
//  GridEYE Test Program
//
//  Created by Richard Oman on 12/1/17.
//  Copyright © 2017 Richard Oman. All rights reserved.
//

#include <iostream>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include "PGEclasses.hpp"

int main(int argc, const char * argv[]) {
    
    cout << "Program Start" << endl << endl;
    
    cout << "Session Started" << endl;
    
    GridEYE* gPtr = new GridEYE();
    cout << "GridEYE Initialized" << endl;
    
    video session( gPtr );
    cout << "Video Object created" << endl;
    
    session.exportVideo("PGEtest.txt");
    cout << "Video exported" << endl;
    
    cout << "Exiting Program" << endl;
    return 0;
}
