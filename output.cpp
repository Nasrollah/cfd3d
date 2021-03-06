/*  An open source Navier-Stokes CFD solver.
    Copyright (C) 2015  Michael Nucci (michael.nucci@gmail.com)

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>. */

#include "output.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cmath>

using std::cout;
using std::endl;
using std::cerr;
using std::vector;
using std::string;
using std::ios;
using std::ofstream;
using std::to_string;
using std::max;
using std::pair;

//---------------------------------------------------------------------------------------------------------------//
//function declarations
//function to write out cell centers of grid in plot3d format
void WriteCellCenter(const string &gridName, const vector<procBlock > &vars, const decomposition &decomp) {

  //recombine procblocks into original configuration
  vector<procBlock> recombVars = Recombine(vars, decomp);

  //open binary plot3d grid file 
  ofstream outFile;
  string fEnd = "_center";
  string fPostfix = ".xyz";
  string writeName = gridName + fEnd + fPostfix;
  outFile.open(writeName.c_str(), ios::out | ios::binary);

  //check to see if file opened correctly
  if (outFile.fail()) {
    cerr << "ERROR: Grid file " << writeName << " did not open correctly!!!" << endl;
    exit(0);
  }

  //write number of blocks to file
  int numBlks = recombVars.size();
  outFile.write(reinterpret_cast<char *>(&numBlks), sizeof(numBlks));

  //write i, j, k dimension for each block
  int dumInt = 0;
  vector3d<double> dumVec;
  double dumDouble=0.0;

  for ( int ll=0; ll < numBlks; ll++ ){ //loop over all blocks
    //subtract 1 from max values because procBlock maxes are in terms of nodes, not cells
    dumInt = recombVars[ll].NumI();
    outFile.write(reinterpret_cast<char *>(&dumInt), sizeof(dumInt));
    dumInt = recombVars[ll].NumJ();
    outFile.write(reinterpret_cast<char *>(&dumInt), sizeof(dumInt));
    dumInt = recombVars[ll].NumK();
    outFile.write(reinterpret_cast<char *>(&dumInt), sizeof(dumInt));
  }

  //write out x, y, z coordinates of cell centers
  for ( int ll = 0; ll < numBlks; ll++ ){  //loop over all blocks
    int maxi = recombVars[ll].NumI();
    int maxj = recombVars[ll].NumJ();
    int maxk = recombVars[ll].NumK();
    int maxiG = recombVars[ll].NumI() + 2 * recombVars[ll].NumGhosts();
    int maxjG = recombVars[ll].NumJ() + 2 * recombVars[ll].NumGhosts();

    for ( int nn = 0; nn < 3; nn++ ){  //loop over dimensions (3)

      for ( int kk = recombVars[ll].NumGhosts(); kk < maxk + recombVars[ll].NumGhosts(); kk++ ){
	for ( int jj = recombVars[ll].NumGhosts(); jj < maxj + recombVars[ll].NumGhosts(); jj++ ){
	  for ( int ii = recombVars[ll].NumGhosts(); ii < maxi + recombVars[ll].NumGhosts(); ii++){ 

	    int loc = GetLoc1D(ii, jj, kk, maxiG, maxjG);
	    dumVec = recombVars[ll].Center(loc);  //at a given cell, get the cell center coordinates

	    //for a given block, first write out all x coordinates, then all y coordinates, then all z coordinates
	    if (nn == 0 ) {
	      dumDouble = dumVec.X();
	    }
	    else if (nn == 1){
	      dumDouble = dumVec.Y();
	    }
	    else {
	      dumDouble = dumVec.Z();
	    }
	    //write to file
	    outFile.write(reinterpret_cast<char *>(&dumDouble), sizeof(dumDouble));

	  }
	}
      }

    }
  }

  //close plot3d grid file
  outFile.close();

}

//function to write out cell centers of grid with ghost cells in plot3d format
void WriteCellCenterGhost(const string &gridName, const vector<procBlock > &vars ) {

  //open binary plot3d grid file 
  ofstream outFile;
  string fEnd = "_center_ghost";
  string fPostfix = ".xyz";
  string writeName = gridName + fEnd + fPostfix;
  outFile.open(writeName.c_str(), ios::out | ios::binary);

  //check to see if file opened correctly
  if (outFile.fail()) {
    cerr << "ERROR: Grid file " << writeName << " did not open correctly!!!" << endl;
    exit(0);
  }

  //write number of blocks to file
  int numBlks = vars.size();
  outFile.write(reinterpret_cast<char *>(&numBlks), sizeof(numBlks));

  //write i, j, k dimension for each block
  int dumInt = 0;
  vector3d<double> dumVec;
  double dumDouble=0.0;

  for ( int ll=0; ll < numBlks; ll++ ){ //loop over all blocks
    dumInt = vars[ll].NumI() + 2 * vars[ll].NumGhosts();
    outFile.write(reinterpret_cast<char *>(&dumInt), sizeof(dumInt));
    dumInt = vars[ll].NumJ() + 2 * vars[ll].NumGhosts();
    outFile.write(reinterpret_cast<char *>(&dumInt), sizeof(dumInt));
    dumInt = vars[ll].NumK() + 2 * vars[ll].NumGhosts();
    outFile.write(reinterpret_cast<char *>(&dumInt), sizeof(dumInt));
  }

  //write out x, y, z coordinates of cell centers
  for ( int ll = 0; ll < numBlks; ll++ ){  //loop over all blocks
    int maxiG = vars[ll].NumI() + 2 * vars[ll].NumGhosts();
    int maxjG = vars[ll].NumJ() + 2 * vars[ll].NumGhosts();
    int maxkG = vars[ll].NumJ() + 2 * vars[ll].NumGhosts();

    for ( int nn = 0; nn < 3; nn++ ){  //loop over dimensions (3)

      for ( int kk = 0; kk < maxkG; kk++ ){
	for ( int jj = 0; jj < maxjG; jj++ ){
	  for ( int ii = 0; ii < maxiG; ii++){ 

	    int loc = GetLoc1D(ii, jj, kk, maxiG, maxjG);
	    dumVec = vars[ll].Center(loc);  //at a given cell, get the cell center coordinates

	    //for a given block, first write out all x coordinates, then all y coordinates, then all z coordinates
	    if (nn == 0 ) {
	      dumDouble = dumVec.X();
	    }
	    else if (nn == 1){
	      dumDouble = dumVec.Y();
	    }
	    else {
	      dumDouble = dumVec.Z();
	    }
	    //write to file
	    outFile.write(reinterpret_cast<char *>(&dumDouble), sizeof(dumDouble));

	  }
	}
      }

    }
  }

  //close plot3d grid file
  outFile.close();

}

//---------------------------------------------------------------------------------------------------------------//
//function to write out variables in function file format
void WriteFun(const string &gridName, const vector<procBlock> &vars, const idealGas &eqnState, const int &solIter, const double &refRho, 
	      const double &refSoS, const double &refT, const decomposition &decomp) {

  vector<procBlock> recombVars = Recombine(vars, decomp);

  //open binary plot3d function file 
  ofstream outFile;
  string fEnd = "_center";
  string fPostfix = ".fun";
  string writeName = gridName + to_string(static_cast<long long int>(solIter)) + fEnd + fPostfix;
  outFile.open(writeName.c_str(), ios::out | ios::binary);

  //check to see if file opened correctly
  if (outFile.fail()) {
    cerr << "ERROR: Function file " << writeName << " did not open correctly!!!" << endl;
    exit(0);
  }

  //write number of blocks to file
  int numBlks = recombVars.size();
  outFile.write(reinterpret_cast<char *>(&numBlks), sizeof(numBlks));

  //write i, j, k, recombVars dimension for each block
  int numVars = 11;            //number of variables to write out
  int dumInt = 0;

  vector3d<double> vel;
  double dumDouble=0.0;

  for ( int ll=0; ll < numBlks; ll++ ){ //loop over all blocks and write out imax, jmax, kmax, numVars
    dumInt = recombVars[ll].NumI();
    outFile.write(reinterpret_cast<char *>(&dumInt), sizeof(dumInt));
    dumInt = recombVars[ll].NumJ();
    outFile.write(reinterpret_cast<char *>(&dumInt), sizeof(dumInt));
    dumInt = recombVars[ll].NumK();
    outFile.write(reinterpret_cast<char *>(&dumInt), sizeof(dumInt));

    outFile.write(reinterpret_cast<char *>(&numVars), sizeof(numVars));
  }

  //write out variables
  for ( int ll = 0; ll < numBlks; ll++ ){ //loop over all blocks
    int maxi = recombVars[ll].NumI();
    int maxj = recombVars[ll].NumJ();
    int maxk = recombVars[ll].NumK();
    int blkLen = recombVars[ll].NumCells();
    vector<double> dumVec(blkLen);
    int maxiG = recombVars[ll].NumI() + 2 * recombVars[ll].NumGhosts();
    int maxjG = recombVars[ll].NumJ() + 2 * recombVars[ll].NumGhosts();

    for ( int vv = 0; vv < numVars; vv++ ){ //loop over the number of variables to write out
      //store nondimensional variable in dumVec for a given block in order. i.e. var1 var2 var3 etc
      if (vv == 0) {   //density
	for ( int kk = recombVars[ll].NumGhosts(); kk < maxk + recombVars[ll].NumGhosts(); kk++ ){
	  for ( int jj = recombVars[ll].NumGhosts(); jj < maxj + recombVars[ll].NumGhosts(); jj++ ){
	    for ( int ii = recombVars[ll].NumGhosts(); ii < maxi + recombVars[ll].NumGhosts(); ii++){ 
	      int loc = GetLoc1D(ii - recombVars[ll].NumGhosts(), jj - recombVars[ll].NumGhosts(), kk - recombVars[ll].NumGhosts(), maxi, maxj);
	      int locG = GetLoc1D(ii, jj, kk, maxiG, maxjG);
	      dumVec[loc] = recombVars[ll].State(locG).Rho(); 
	    }
	  }
	}
      }
      else if (vv == 1) {  //vel-x
	for ( int kk = recombVars[ll].NumGhosts(); kk < maxk + recombVars[ll].NumGhosts(); kk++ ){
	  for ( int jj = recombVars[ll].NumGhosts(); jj < maxj + recombVars[ll].NumGhosts(); jj++ ){
	    for ( int ii = recombVars[ll].NumGhosts(); ii < maxi + recombVars[ll].NumGhosts(); ii++){         
	      int loc = GetLoc1D(ii - recombVars[ll].NumGhosts(), jj - recombVars[ll].NumGhosts(), kk - recombVars[ll].NumGhosts(), maxi, maxj);
	      int locG = GetLoc1D(ii, jj, kk, maxiG, maxjG);
	      dumVec[loc] = recombVars[ll].State(locG).U();  
	    }
	  }
	}
      }
      else if (vv == 2) {  //vel-y
	for ( int kk = recombVars[ll].NumGhosts(); kk < maxk + recombVars[ll].NumGhosts(); kk++ ){
	  for ( int jj = recombVars[ll].NumGhosts(); jj < maxj + recombVars[ll].NumGhosts(); jj++ ){
	    for ( int ii = recombVars[ll].NumGhosts(); ii < maxi + recombVars[ll].NumGhosts(); ii++){         
	      int loc = GetLoc1D(ii - recombVars[ll].NumGhosts(), jj - recombVars[ll].NumGhosts(), kk - recombVars[ll].NumGhosts(), maxi, maxj);
	      int locG = GetLoc1D(ii, jj, kk, maxiG, maxjG);
	      dumVec[loc] = recombVars[ll].State(locG).V();
	    }
	  }
	}
      }
      else if (vv == 3) {   //vel-z
	for ( int kk = recombVars[ll].NumGhosts(); kk < maxk + recombVars[ll].NumGhosts(); kk++ ){
	  for ( int jj = recombVars[ll].NumGhosts(); jj < maxj + recombVars[ll].NumGhosts(); jj++ ){
	    for ( int ii = recombVars[ll].NumGhosts(); ii < maxi + recombVars[ll].NumGhosts(); ii++){         
	      int loc = GetLoc1D(ii - recombVars[ll].NumGhosts(), jj - recombVars[ll].NumGhosts(), kk - recombVars[ll].NumGhosts(), maxi, maxj);
	      int locG = GetLoc1D(ii, jj, kk, maxiG, maxjG);
	      dumVec[loc] = recombVars[ll].State(locG).W(); 
	    }
	  }
	}
      }
      else if (vv == 4) {                        //pressure
	for ( int kk = recombVars[ll].NumGhosts(); kk < maxk + recombVars[ll].NumGhosts(); kk++ ){
	  for ( int jj = recombVars[ll].NumGhosts(); jj < maxj + recombVars[ll].NumGhosts(); jj++ ){
	    for ( int ii = recombVars[ll].NumGhosts(); ii < maxi + recombVars[ll].NumGhosts(); ii++){         
	      int loc = GetLoc1D(ii - recombVars[ll].NumGhosts(), jj - recombVars[ll].NumGhosts(), kk - recombVars[ll].NumGhosts(), maxi, maxj);
	      int locG = GetLoc1D(ii, jj, kk, maxiG, maxjG);
	      dumVec[loc] = recombVars[ll].State(locG).P();
	    }
	  }
	}
      }
      else if (vv == 5) {                      //mach
	for ( int kk = recombVars[ll].NumGhosts(); kk < maxk + recombVars[ll].NumGhosts(); kk++ ){
	  for ( int jj = recombVars[ll].NumGhosts(); jj < maxj + recombVars[ll].NumGhosts(); jj++ ){
	    for ( int ii = recombVars[ll].NumGhosts(); ii < maxi + recombVars[ll].NumGhosts(); ii++){         
	      int loc = GetLoc1D(ii - recombVars[ll].NumGhosts(), jj - recombVars[ll].NumGhosts(), kk - recombVars[ll].NumGhosts(), maxi, maxj);
	      int locG = GetLoc1D(ii, jj, kk, maxiG, maxjG);
	      vel = recombVars[ll].State(locG).Velocity();
	      dumVec[loc] = vel.Mag() / eqnState.GetSoS( recombVars[ll].State(locG).P(), recombVars[ll].State(locG).Rho() );
	    }
	  }
	}
      }
      else if (vv == 6) {                     //speed of sound
	for ( int kk = recombVars[ll].NumGhosts(); kk < maxk + recombVars[ll].NumGhosts(); kk++ ){
	  for ( int jj = recombVars[ll].NumGhosts(); jj < maxj + recombVars[ll].NumGhosts(); jj++ ){
	    for ( int ii = recombVars[ll].NumGhosts(); ii < maxi + recombVars[ll].NumGhosts(); ii++){         
	      int loc = GetLoc1D(ii - recombVars[ll].NumGhosts(), jj - recombVars[ll].NumGhosts(), kk - recombVars[ll].NumGhosts(), maxi, maxj);
	      int locG = GetLoc1D(ii, jj, kk, maxiG, maxjG);
	      dumVec[loc] = eqnState.GetSoS( recombVars[ll].State(locG).P(), recombVars[ll].State(locG).Rho() );
	    }
	  }
	}
      }
      else if (vv == 7) {                    //time step - no ghost cells
	for ( int ii = 0; ii < blkLen; ii++){
	  dumVec[ii] = recombVars[ll].Dt(ii);  
	}
      }
      else if (vv == 8) {                     //temperature
	for ( int kk = recombVars[ll].NumGhosts(); kk < maxk + recombVars[ll].NumGhosts(); kk++ ){
	  for ( int jj = recombVars[ll].NumGhosts(); jj < maxj + recombVars[ll].NumGhosts(); jj++ ){
	    for ( int ii = recombVars[ll].NumGhosts(); ii < maxi + recombVars[ll].NumGhosts(); ii++){         
	      int loc = GetLoc1D(ii - recombVars[ll].NumGhosts(), jj - recombVars[ll].NumGhosts(), kk - recombVars[ll].NumGhosts(), maxi, maxj);
	      int locG = GetLoc1D(ii, jj, kk, maxiG, maxjG);
	      dumVec[loc] = recombVars[ll].State(locG).Temperature(eqnState);   
	    }
	  }
	}
      }
      else if (vv == 9) {  //processor rank
	for ( int kk = recombVars[ll].NumGhosts(); kk < maxk + recombVars[ll].NumGhosts(); kk++ ){
	  for ( int jj = recombVars[ll].NumGhosts(); jj < maxj + recombVars[ll].NumGhosts(); jj++ ){
	    for ( int ii = recombVars[ll].NumGhosts(); ii < maxi + recombVars[ll].NumGhosts(); ii++){         
	      int loc = GetLoc1D(ii - recombVars[ll].NumGhosts(), jj - recombVars[ll].NumGhosts(), kk - recombVars[ll].NumGhosts(), maxi, maxj);
	      dumVec[loc] = vars[SplitBlockNumber(recombVars, decomp, ll, ii - recombVars[ll].NumGhosts(), jj - recombVars[ll].NumGhosts(), kk - recombVars[ll].NumGhosts())].Rank();  
	    }
	  }
	}
      }
      else if (vv == 10) {  //global position
	for ( int kk = recombVars[ll].NumGhosts(); kk < maxk + recombVars[ll].NumGhosts(); kk++ ){
	  for ( int jj = recombVars[ll].NumGhosts(); jj < maxj + recombVars[ll].NumGhosts(); jj++ ){
	    for ( int ii = recombVars[ll].NumGhosts(); ii < maxi + recombVars[ll].NumGhosts(); ii++){         
	      int loc = GetLoc1D(ii - recombVars[ll].NumGhosts(), jj - recombVars[ll].NumGhosts(), kk - recombVars[ll].NumGhosts(), maxi, maxj);
	      dumVec[loc] = vars[SplitBlockNumber(recombVars, decomp, ll, ii - recombVars[ll].NumGhosts(), jj - recombVars[ll].NumGhosts(), kk - recombVars[ll].NumGhosts())].GlobalPos();
	    }
	  }
	}
      }
      else {
	cerr << "ERROR: Variable to write to function file is not defined!" << endl;
        exit(0);
      }

      for ( int nn = 0; nn < blkLen; nn++ ){                              //write out dimensional variables -- loop over block length

        dumDouble = dumVec[nn];

	if (vv == 0){                                        //density
	  dumDouble = dumDouble * refRho;
	}
	else if (vv == 1){                                  //velocity x
	  dumDouble = dumDouble * refSoS;
	}
	else if (vv == 2){                                 //velocity y
	  dumDouble = dumDouble * refSoS;
	}
	else if (vv == 3){                                 //velocity z
	  dumDouble = dumDouble * refSoS;
	}
	else if (vv == 4){                                 // pressure
	  dumDouble = dumDouble * refRho * refSoS * refSoS ; 
	}
	else if (vv == 5){                                 //mach is already nondimensional
	  dumDouble = dumDouble ;                        
	}
	else if (vv == 6){                                 // speed of sound
	  dumDouble = dumDouble * refSoS ; 
	}
	else if (vv == 7){                                 //time step
	  dumDouble = dumDouble / refSoS;                        
	}
	else if (vv == 8){                                 //temperature
	  dumDouble = dumDouble * refT;                        
	}
	outFile.write(reinterpret_cast<char *>(&dumDouble), sizeof(dumDouble));
      }
    }
  }


  //close plot3d function file
  outFile.close();

}

//function to write out results file for ensight
void WriteRes(const string &gridName, const int &iter, const int &outFreq) {

  //open results file
  ofstream resFile;
  string fResPostfix = ".res";
  string fPostfix = ".fun";
  string fEnd = "_center";
  string resName = gridName + fEnd + fResPostfix;
  resFile.open(resName.c_str(), ios::out);

  string writeName = gridName + "*" + fEnd + fPostfix;

  //check to see if file opened correctly
  if (resFile.fail()) {
    cerr << "ERROR: Results file " << resName << " did not open correctly!!!" << endl;
    exit(0);
  }

  //write number of scalars and number of vectors
  int numScalar = 11;
  int numVector = 1;
  resFile << numScalar << "     " << numVector << "     " << 0 << endl;

  //write number of time points that there is solution data at
  int numTime = iter/outFreq;
  resFile << numTime << endl;

  //Write solution times or iteration numbers
  int solTime = 0;
  int ii = 0;
  int count = 1;
  for (ii = 0; ii < numTime; ii++){
    solTime += outFreq;
    if ( count%10 == 0 ){
      resFile << endl;
    }
    resFile << solTime << "   ";
    count++;
  }
  resFile << endl;

  //Write starting iteration and iteration increment
  resFile << outFreq << "  " << outFreq << endl;


  //Write out variables
  resFile << writeName << " F 0001 density" << endl;
  resFile << writeName << " F 0002 Vx" << endl;
  resFile << writeName << " F 0003 Vy" << endl;
  resFile << writeName << " F 0004 Vz" << endl;
  resFile << writeName << " F 0005 pressure" << endl;
  resFile << writeName << " F 0006 mach" << endl;
  resFile << writeName << " F 0007 sos" << endl;
  resFile << writeName << " F 0008 dt" << endl;
  resFile << writeName << " F 0009 temperature" << endl;
  resFile << writeName << " F 0010 procRank" << endl;
  resFile << writeName << " F 0011 procBlockID" << endl;
  resFile << writeName << " F 0002 0003 0004 velocity" << endl;

  //Close results file
  resFile.close();

}

//function to write out residual information
void WriteResiduals(const input &inp, genArray &residL2First, genArray &residL2, const resid &residLinf, 
		    const double &matrixResid, const int &nn, const int &mm){

  double eps = 1.0e-30;

  //determine normalization
  if (nn == 0 && mm == 0){ //if at first iteration, normalize by itself
    residL2First = residL2;
  }
  else if ( (nn < 5) && mm == 0 ){ //if within first 5 iterations reset normalization
    for ( int cc = 0; cc < NUMVARS; cc++ ){
      if ( residL2[cc] > residL2First[cc] ){
	residL2First[cc] = residL2[cc];
      }
    }
  }

  //normalize residuals
  residL2 = (residL2 + eps) / (residL2First + eps);

  //write out column headers every 100 iterations
  if (nn%100 == 0 && mm == 0){ 
    if (inp.Dt() > 0.0){
      cout << "STEP    NONLINEAR     DT     RES-Mass     Res-Mom-X     Res-Mom-Y     Res-Mom-Z     Res-Energy    Max Res Eqn  " << 
	"Max Res Blk    Max Res I    Max Res J    Max Res K    Max Res    Res-Matrix" << endl;
    }
    else if (inp.CFL() > 0.0){
      cout << "STEP    NONLINEAR     CFL     RES-Mass     Res-Mom-X     Res-Mom-Y     Res-Mom-Z     Res-Energy   Max Res Eqn  " << 
	"Max Res Blk    Max Res I    Max Res J    Max Res K    Max Res    Res-Matrix" << endl;
    }

  }
  if (inp.Dt() > 0.0){
    cout << nn << "     " << mm << "     " << inp.Dt() << "     " << residL2[0] <<  "     " << residL2[1] << "     " 
	 << residL2[2] << "     " << residL2[3] << "     " << residL2[4] << "     " << residLinf.Eqn() << "     " << 
      residLinf.Block() << "     " << residLinf.ILoc() <<"     " << residLinf.JLoc() << "     " << residLinf.KLoc() << "     " << residLinf.Linf() 
	 << "     " << matrixResid << endl;
  }
  else if (inp.CFL() > 0.0){
    cout << nn << "     " << mm << "     " << inp.CFL() << "     " << residL2[0] <<  "     " << residL2[1] << "     " 
	 << residL2[2] << "     " << residL2[3] << "     " << residL2[4] << "     "  << residLinf.Eqn() << "     " << 
      residLinf.Block() << "     " << residLinf.ILoc() <<"     " << residLinf.JLoc() << "     " << residLinf.KLoc() << "     " << residLinf.Linf() 
	 << "     " << matrixResid << endl;
  }


}

/*Function to take in a vector of split procBlocks and return a vector of joined procblocks (in their original configuration before grid decomposition).*/
vector<procBlock> Recombine( const vector<procBlock> &vars, const decomposition &decomp ){
  // vars -- vector of split procBlocks
  // decomp -- decomposition

  vector<procBlock> recombVars = vars;
  vector<boundarySurface> dumSurf;
  for ( int ii = decomp.NumSplits() - 1; ii >= 0; ii-- ){
    //recombine blocks and resize vector
    recombVars[decomp.SplitHistBlkLower(ii)].Join(recombVars[decomp.SplitHistBlkUpper(ii)], decomp.SplitHistDir(ii), dumSurf);
    recombVars.resize(recombVars.size()-1);
  }

  return recombVars;
}

/*Function to take in indices from the recombined procBlocks and determine which split procBlock index the cell is associated with.*/
int SplitBlockNumber( const vector<procBlock> &vars, const decomposition &decomp, const int &blk, const int &ii, const int &jj, const int &kk ){
  // vars -- vector of recombined procblocks
  // decomp -- decomposition data structure
  // blk -- block number
  // ii -- i index of cell in recombined block to find split block number
  // jj -- j index of cell in recombined block to find split block number
  // kk -- k index of cell in recombined block to find split block number

  //Get block dimensions (both lower and upper extents)
  vector<pair<vector3d<int>, vector3d<int> > > blkDims(vars.size());
  vector3d<int> initialLower(0, 0, 0);
  for ( unsigned int bb = 0; bb < blkDims.size(); bb++ ){
    vector3d<int> dims(vars[bb].NumI(), vars[bb].NumJ(), vars[bb].NumK());
    blkDims[bb].first = initialLower;
    blkDims[bb].second = dims;
  }

  int ind = blk;

  if ( decomp.NumSplits() == 0 ){ //no splits, cell must be in parent block already
    return ind;
  }
  else{ //cell is in lower split already

    for (int ss = 0; ss < decomp.NumSplits(); ss++ ){ //loop over all splits

      if ( blk != decomp.ParentBlock(ss + vars.size() ) ){ //wrong parent block - split won't effect search so use dummy value
	pair<vector3d<int>, vector3d<int> > dumBlk(initialLower, initialLower);
	blkDims.push_back(dumBlk);
      }
      else{

	//"split" blocks - change lower limits of block
	if ( decomp.SplitHistDir(ss) == "i" ){
	  pair<vector3d<int>, vector3d<int> > splitBlk = blkDims[decomp.SplitHistBlkLower(ss)];
	  splitBlk.first[0] += decomp.SplitHistIndex(ss);
	  blkDims.push_back(splitBlk);
	}
	else if ( decomp.SplitHistDir(ss) == "j" ){
	  pair<vector3d<int>, vector3d<int> > splitBlk = blkDims[decomp.SplitHistBlkLower(ss)];
	  splitBlk.first[1] += decomp.SplitHistIndex(ss);
	  blkDims.push_back(splitBlk);
	}
	else{ //direction is k
	  pair<vector3d<int>, vector3d<int> > splitBlk = blkDims[decomp.SplitHistBlkLower(ss)];
	  splitBlk.first[2] += decomp.SplitHistIndex(ss);
	  blkDims.push_back(splitBlk);
	}

	//test to see if block is in upper split
	if ( !( ii <= blkDims[decomp.SplitHistBlkUpper(ss)].second.X() && jj <= blkDims[decomp.SplitHistBlkUpper(ss)].second.Y() && 
		kk <= blkDims[decomp.SplitHistBlkUpper(ss)].second.Z() &&
		ii >= blkDims[decomp.SplitHistBlkUpper(ss)].first.X()  && jj >= blkDims[decomp.SplitHistBlkUpper(ss)].first.Y()  && 
		kk >= blkDims[decomp.SplitHistBlkUpper(ss)].first.Z() ) ){ //cell not in upper split, but in lower split - found block index
	  return decomp.SplitHistBlkLower(ss);
	}
	else{ //cell in upper split (and lower split)
	  ind = decomp.SplitHistBlkUpper(ss);
	}

      }
    }
  }

  return ind; //cell was in uppermost split for given parent block
}
