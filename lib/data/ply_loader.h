// *************************************************************
// File:    ply_loader.h
// Descr:   ply (ascii) format loader
// Author:  Novoselov Anton @ 2017
// *************************************************************

#ifndef GC_PLY_LOADER_H
#define GC_PLY_LOADER_H

#include <map>
#include <tuple>
#include <vector>
#include <iostream>
#include <sstream>

namespace ply {

//************************************************************************
// Helper class represents header of ply file
//************************************************************************

using std::string;
using uint = std::size_t;

using SingleTuple = std::tuple<std::string, uint, uint>;
using SingleMap   = std::map<std::string, SingleTuple>;
using ListTuple   = std::tuple<std::string, uint>;
using ListMap     = std::map<std::string, ListTuple>;

struct Header
{
  int       pos_;
  int       count_;
  SingleMap single_props_;
  ListMap   list_props_;

}; // struct Header

//************************************************************************
// Main class Loader for parsing ascii ply files
//************************************************************************

  using Vector1s  = std::vector<std::string>;
  using cVector1s = const Vector1s;
  using Vector1i  = std::vector<int>;
  using Vector2c  = std::vector<std::vector<char>>;
  using Vector1d  = std::vector<double>;
  using Vector2d  = std::vector<Vector1d>;
  using Vector3d  = std::vector<Vector2d>;
  using Vector4d  = std::vector<Vector3d>;
  using cVector3d = const Vector3d;
  using cVector4d = const Vector4d;
  using MapV      = std::map<std::string, Vector1s>;
  using MapSI     = std::map<std::string, int>;
  using MHeader   = std::map<std::string, Header>;
  using cMHeader  = const std::map<std::string, Header>;

  struct Loader
  {
    bool        Load(std::istream&);
    void        LoadHeader(std::istream&);
    cMHeader&   GetHeader() const { return head_; }
    Vector2d    GetLine(const std::string&, cVector1s&);
    Vector2d    GetList(const std::string&, const std::string&);
    cVector4d&  GetRawData() const { return data_; }
    cVector3d&  GetRawData(const std::string&);
    
    bool IsElementPresent(const std::string&);
    bool IsSinglePropertyPresent(const std::string&, const std::string&);
    bool IsListPropertyPresent(const std::string&, const std::string&);

  private:
    void LoadData(std::istream&);
    void ClearData();

    Vector2c  props_;
    Vector1i  sizes_;
    MHeader   head_;
    Vector4d  data_;
    MapSI     order_;
    MapV      filter_;

  }; // struct ply::Loader

//************************************************************************
// Exception helper struct
//************************************************************************

struct Except : std::runtime_error
{
  Except(std::string&& msg) : std::runtime_error(msg.c_str()) { }
  Except(const char* msg) : std::runtime_error(msg) { }

}; // struct ply::Except

//************************************************************************
// Helper functions
//************************************************************************

namespace helpers {

  void RewindStream(std::istream&);
  void ThrowLoadDataError(int, int);
  bool IsSinglePropertiesPresent(Loader&, const std::string&, cVector1s&);
  bool IsListPropertiesPresent(Loader&, const std::string&, cVector1s&);

  std::ostream& operator<<(std::ostream& , const Header&);
  std::ostream& operator<<(std::ostream& , const MHeader&);
  
  template<class T> // map
  bool IsKeyAbsentInMap(const std::string& str, const T& map)
  {
    if (map.find(str) == map.end())
      return true;
    else
      return false;
  }

} // namespace helpers

} // namespace ply

#endif  // GC_PLY_LOADER_H

// Explanation:

// #1: ply-file contains of "header_section" and "data_section". All data
// described in header described in format like: element type, number of items
// in this element and its properties (may be single or lists)

// I.e., more imortant fields of header section:
//   element vertex 26
//   property float x
//   property float y
//   element face 12
//   property list uchar uint ind
//   property uint non
//   property list uchar uint oth

// Element "vertex" contains 26 items and has two single properties
// Element "face" contains 12 items and has 2 list properies and 1 single

// In data section first goes elements described first. I.e., one of the
// element "vertex" (i.e. line 8) would looks like this:
//  2.52 1.24
// This means, that property x = 2.52, property y = 1.24

// And one of the "face" (i.e. line 2) element would looks like this:
//  3 9 8 7 100 4 2 1 1 1
// This means, that current item of element "face" has following properties:
// - list property "ind" contains of 3 elements (9,8,7)
// - single property "non" == 100
// - list property "oth" contains of 4 elements (2,1,1,1)

// #2: terms inside Loader class:
//  elem - element such as "vertex", "face", "edge", another
//  item - item of current element (declared in header, i.e. "vertex 20")
//  prop - properties of item (may be single or list properties)
//  val  - concrete value of propertie

// #3: how example described above would looks in Loader terms:
//  data_[0][8][0]  ==  std::vector(size == 2) contains {2.52, 1.24}
//  data_[1][2][0]  ==  std::vector(size == 3) contains {9,8,7}
//  data_[1][2][1]  ==  std::vector(size == 1) contains {100}
//  data_[1][2][0]  ==  std::vector(size == 4) contains {2,1,1,1}

// #4: all data is stored in double float for simplicify. End user may
// casts doubles to types described in current ply file.

// #5: data member explanation
//  data_  - stores loaded data in format data_[elem][item][prop][val]
//  props_ - contains sequence of properties type (do define when we should
//           push new propertie vector if we have list propery)
//  sizes_ - contains count of elements 

// #6: although construction of 4d vector is looks creepy, but all memory
// allocations made

// # Todo:   l.SetFilter("vertex", {"x", "y", "z"});