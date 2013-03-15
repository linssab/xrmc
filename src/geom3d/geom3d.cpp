/*
Copyright (C) 2013 Bruno Golosio

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
///////////////////////////////////
//     geom3d.cpp                //
//        31/01/2013             //
//   author : Bruno Golosio      //
///////////////////////////////////
// Methods of the class geom3d
//

#include <string>
#include <iostream>
#include "xrmc_geom3d.h"
#include "xrmc_algo.h"
#include "xrmc_exception.h"

using namespace std;
using namespace xrmc_algo;

// method for importing quadricarray and composition devices
int geom3d::ImportDevice(xrmc_device_map *dev_map)
{
  // check if the input quadricarray device is defined
  xrmc_device_map::iterator it = dev_map->find(QArrName);

  // if not display error and exit
  if (it==dev_map->end())
    throw xrmc_exception(string("Device ") + QArrName
			 + " not found in device map\n");

  // get device pointer from the device map
  xrmc_device *dev_pt = (*it).second;
  // cast it to type quadricarray*
  QArr = dynamic_cast<quadricarray*>(dev_pt);
  if (QArr==0)
    throw xrmc_exception(string("Device ") + QArrName
			 + " cannot be casted to type quadricarray\n");

  // check if the input composition device is defined
  it = dev_map->find(CompName);

  // if not display error and exit
  if (it==dev_map->end())
    throw xrmc_exception(string("Device ") + CompName +
			 " not found in device map\n");

  // get device pointer from the device map
  dev_pt = (*it).second;
  // cast it to type composition*
  Comp = dynamic_cast<composition*>(dev_pt);
  if (Comp==0)
    throw xrmc_exception(string("Device ") + CompName +
              " cannot be casted to type composition\n");

  Init(); // initialize 3d geometry
  return 0;
}

// method for initializing the 3d geometry
int geom3d::Init()
{
  for(int iqv=0; iqv<NQVol; iqv++) { // loop on 3d objects
    for(int iq=0; iq<QVol[iqv].NQuadr; iq++) { // loop on quadrics delimiting
                                               // the 3d object
      // initialize the pointers of the quadrics delimiting the 3d object
      // using the 3d object quadric map
      string qname = QVolMap[iqv][iq];
      // check if the quadric name is present in quadric map
      quadric_map::iterator it = QArr->QuadricMap.find(qname);
      // if not display error and exit
      if (it==QArr->QuadricMap.end())
	throw xrmc_exception(string("Quadric ") + qname
			     + " not found in quadric map\n");
      // get quadric pointer from the quadric map
      quadric *quadr_pt = (*it).second;

      QVol[iqv].Quadr[iq] = quadr_pt;

    }
    string s_ph_in = QVol[iqv].PhaseInName;
    // check if the phase name is present in phase map
    phase_map::iterator it = Comp->PhaseMap.find(s_ph_in);
    // if not display error and exit
    if (it==Comp->PhaseMap.end())
      throw xrmc_exception(string("Phase ") + s_ph_in
			   + " not found in phase map\n");
    // get phase index from the phase map
    int i_ph_in = (*it).second;
    QVol[iqv].iPhaseIn = i_ph_in; // set the phase inside

    string s_ph_out = QVol[iqv].PhaseOutName;
    // check if the phase name is present in phase map
    it = Comp->PhaseMap.find(s_ph_out);
    // if not display error and exit
    if (it==Comp->PhaseMap.end())
      throw xrmc_exception(string("Phase ") + s_ph_out
			   + " not found in phase map\n");
    // get phase index from the phase map
    int i_ph_out = (*it).second;
    QVol[iqv].iPhaseOut = i_ph_out; // set the phase inside
  }
 
  return 0;
}
//////////////////////////////////////////////////////////////////////
// method for evaluating the intersections of a straight trajectory
// with the 3d objects
//////////////////////////////////////////////////////////////////////
// input:
// x0 and u are the starting coordinates and direction of the trajectory
// output:
// t is the array of the intersections (distances from the starting coordinate)
// iph0 is the array of the entrance phase indexes
// iph0 is the array of the exit phase indexes
// n_inters is the number of intersections
 //////////////////////////////////////////////////////////////////////
int geom3d::Intersect(vect3 x0, vect3 u, double *t, int *iph0, int *iph1,
		       int *n_inters)
{
  int i;
  for (i=0; i<QArr->NQuadr; i++) { // loop on all quadrics
    QArr->Quadr[i].Intersect(x0, u); // evaluate intersection of the trajectory
                                     // with the quadric
  }
  *n_inters = 0;
  for (i=0; i<NQVol; i++) { // loop on 3d objects
 // evaluate intersection of the trajectory with the 3d objects
    QVol[i].Intersect(x0, u, t, iph0, iph1, n_inters);
  }
  SortInters(t, iph0, iph1, *n_inters); // sort the intersections
                                        // with growing values of t

  return 0;
}