// *************************************************************
// File:    ply_loader.cc
// Descr:   ply (ascii) format loader
// Author:  Novoselov Anton @ 2018
// URL:     https://github.com/ans-hub/ply_loader
// *************************************************************

#include "ply_loader.h"

namespace ply {

// Main procedure to load ascii ply 

bool Loader::Load(std::istream& iss)
{
  ClearData();
  helpers::RewindStream(iss);
  
  std::string line {};
  std::getline(iss, line);

  if (line != "ply")
  {
    iss.setstate(std::ios::failbit);
    throw ply::Except("Load(): not a ply file");
  }

  LoadHeader(iss);
  LoadData(iss);

  return true;
}

// Reads header of ascii ply file (iss pos should stay at the data pos)

void Loader::LoadHeader(std::istream& iss)
{
  std::string line {""};                // to store line of istream
  std::string word {""};                // to store words of line
  std::string name {""};                // to store prop name of line
  std::string elem {""};                // to store elem name of line
  std::string type {""};                // to store type of property
  int order {0};                        // to store pos of curr elem in data_ arr
  int num {0};                          // to store input integer
  int pos {0};                          // current prop pos in prop array
  int prop {0};                         // current prop array of element

  // Read stream line by line

  while(std::getline(iss, line))
  {
    std::istringstream stream {line};   // to read word by word
    stream >> word;                     // get word
    
    // Fills element`s general info

    if (word == "element")              // if first word is "element"
    {
      stream >> elem >> num;            // get line like "vertex 26"
      props_.emplace_back();            // allocate new properties describer
      sizes_.push_back(num);            // add elem count
      data_.emplace_back();             // add new element
      data_.back().resize(num);         // and resize it for store num of items
      head_[elem].count_ = num;         // set name and count of element
      order_[elem] = order;             // remember the order of element
      pos = 0;
      prop = 0;
      ++order;
    }

    // Fill element`s sequence of properties

    else if (word == "property")
    {
      stream >> type;
      if (type == "list")               // add next property of element
      {
        stream >> type >> type >> name;
        pos = 0;
        if (prop != 0) ++prop;
        head_[elem].list_props_[name] = std::make_tuple(type, prop);
        ++prop;
        props_.back().push_back('l');
      }
      else
      {
        stream >> name;
        head_[elem].single_props_[name] = std::make_tuple(type, prop, pos);
        ++pos;
        props_.back().push_back('d');
      }
    }
    else if (word == "end_header")      // if we stay in the end of header
      break;
  }
}

// Returns raw data of defined element (by its name)

cVector3d& Loader::GetRawData(const std::string& key)
{
  if (helpers::IsKeyAbsentInMap(key, order_))
    throw Except(string{"GetRawData(): elem absent - "} + string{key});
  return data_[order_[key]];
}

// Returns line of filtered properties of preloaded ply

Vector2d Loader::GetLine(const std::string& elem, const Vector1s& filter)
{
  // Check if given elem is valid

  if (helpers::IsKeyAbsentInMap(elem, order_))
    throw Except(string{"GetLine(): elem absent - "} + string{elem});
  
  // Check if given filer elements are valid

  for (const auto& prop : filter)
    if (helpers::IsKeyAbsentInMap(prop, head_[elem].single_props_))
      throw Except(string{"GetLine(): property absent - "} + string{elem});

  // Predefine element with which we work, and create result var

  auto& curr = data_[order_[elem]];
  Vector2d res (curr.size());
  
  // Iterate through each item of current element

  for (std::size_t i = 0; i < curr.size(); ++i)
  {
    // Fill current result item with requested property
    
    res[i].reserve(filter.size());
    for (auto& f : filter)
    {
      int prop_arr = std::get<1>(head_[elem].single_props_[f]); // prop_arr pos
      int prop_pos = std::get<2>(head_[elem].single_props_[f]); // elem_pos in prop_arr
      res[i].push_back(curr[i][prop_arr][prop_pos]);
    }
  }
  return res;
}

// Returns propeties of element represents by list

Vector2d Loader::GetList(const std::string& elem, const std::string& filter)
{
  // Check if given elem is valid

  if (helpers::IsKeyAbsentInMap(elem, order_))
    throw Except(string{"GetLine(): elem absent - "} + string{elem});

  // Check if given filer elements are valid

  if (helpers::IsKeyAbsentInMap(filter, head_[elem].list_props_))
    throw Except(string{"GetLine(): property absent - "} + string{filter});

  // Predefine element with which we work, and create result var

  auto& curr = data_[order_[elem]];
  Vector2d res (curr.size());

  // Iterate through each item of current element

  for (std::size_t i = 0; i < curr.size(); ++i)
  {
    // Fill current result item with requested property
        
    int prop_arr = std::get<1>(head_[elem].list_props_[filter]); // prop_arr pos
    res[i] = curr[i][prop_arr];
  }
  return res;
}

//************************************************************************
// PRIVATE MEMBER FUNCTIONS IMPLEMENTATION
//************************************************************************

// Reads data after header (istream pos shoudld stay at the data pos)
// First would meets elements which were described first in the header.
//
// I.e.:
//  header section: "vertex 20", "face 12" (element and items count)
//  data in stream: first 20 lines is vertexes, next 12 lines is faces
//  data in class:  data_[0] is 20 vertexes, data_[1] is 12 faces
//
// Each data_[elem][item] contains sub arrays, which represents properties.
// I.e. 1:  header section: "vertex 10" +6 single properties
//          data in class:  data_[0][item][0][0..5]
// I.e. 2:  header section: "face 6" +1 list, +2 single properties, +1 list
//          data in class:  data_[1][item][0][size of first list]
//                          data_[1][item][1][0..1]
//                          data_[2][item][2][size of second list]
// I.e. 3:  header section: "edge 8" +1 list
//          data in class:  data_[2][item][0][size of first list]

void Loader::LoadData(std::istream& iss)
{
  std::string line {""};  // user to store line of stream
  std::string word {""};  // used to store word from line 
  int    ival {0};        // used to store integer value from stream
  double dval {0};        // used to store double value from stream
  int    elem {0};        // element type index (vertex, face, etc...)
  int    item {0};        // item index of curr element 

  int total_sizes {0};    // needs to check is data lines is equal sizes
  for (const auto& sz : sizes_)
    total_sizes += sz;

  while(std::getline(iss, line))
  {
    std::istringstream stream {line};   // to read word by word
    
    if (sizes_[elem] == 0) {            // if we have read all data of curr elem
      ++elem;                           //  then go to the next element
      item = 0;
    }
    
    // Reserve max space to store arrays of properties values

    data_[elem][item].reserve(props_[elem].size());

    // Now we have data line. We should read it and understand which type of
    // property we read now. Basing on this knowledge we read all words in line

    char last_prop {'l'};
    for (auto& prop : props_[elem])
    {
      // If this is the "list", then add new property array to data_[elem][item]

      if (prop == 'l')
      {                                 
        last_prop = 'l';
        if (!(stream >> ival))          // get num of sub properties in curr list
          helpers::ThrowLoadDataError(elem, item);

        data_[elem][item].emplace_back();
        data_[elem][item].back().reserve(ival);
        
        while (ival)                    // reads all sub properties in the list
        {
          if (!(stream >> dval))
            helpers::ThrowLoadDataError(elem, item);

          data_[elem][item].back().push_back(dval);
          --ival;
        }
      }
      
      // If this is the single property and previous was a "list", then add
      // new property array to data_[elem][item], else just fill back()

      else if (prop == 'd')
      {
        if (last_prop == 'l') {
            data_[elem][item].emplace_back();
            data_[elem][item].back().reserve(props_[elem].size());
          last_prop = 'd';
        }
        if (!(stream >> dval))
          helpers::ThrowLoadDataError(elem, item);
        data_[elem][item].back().push_back(dval);
      }
    }

    // Go to the next item and reduce curr elem sizes

    ++item;
    --sizes_[elem];
    --total_sizes;
  }

  if (total_sizes != 0)
    throw Except("LoadData(): sizes in header is not equal fact");
}

// Clears all internal data

void Loader::ClearData()
{
  props_ = Vector2c {};
  sizes_ = Vector1i {};
  data_ = Vector4d {};
  head_ = MHeader {};
  order_ = MapSI {};
  filter_.clear();
}

bool Loader::IsElementPresent(const std::string& elem)
{
  return !helpers::IsKeyAbsentInMap(elem, order_);
} 

bool Loader::IsSinglePropertyPresent(
  const std::string& elem, const std::string& prop)
{
  return !helpers::IsKeyAbsentInMap(prop, head_[elem].single_props_);
}

bool Loader::IsListPropertyPresent(const std::string& elem, const std::string& prop)
{
  return !helpers::IsKeyAbsentInMap(prop, head_[elem].list_props_);
}

//************************************************************************
// Helper functions inplementation
//************************************************************************

// Rewinds istream

void helpers::RewindStream(std::istream& iss)
{
  iss.clear();
  iss.seekg(0);
}

// Throws load data error

void helpers::ThrowLoadDataError(int elem_type, int item_num)
{
  std::ostringstream oss {};
  oss << "LoadData(): invalid data in "
      << "elem " << elem_type << ' ' 
      << "item " << item_num;
  throw Except(oss.str().c_str());
}

// Returns true if ALL of single properties is present

bool helpers::IsSinglePropertiesPresent(
  Loader& ply, const std::string& elem, const Vector1s& props)
{
  for (const auto& prop : props)
    if (!ply.IsSinglePropertyPresent(elem, prop))
      return false;
  return true;
}

// Returns true if ALL of list properties is present

bool helpers::IsListPropertiesPresent(
  Loader& ply, const std::string& elem, const Vector1s& props)
{
  for (const auto& prop : props)
    if (!ply.IsListPropertyPresent(elem, prop))
      return false;
  return true;
}

// Out map of headers

std::ostream& helpers::operator<<(std::ostream& oss, const MHeader& header)
{
  for (const auto& elem : header)
  {
    oss << "Element: " << elem.first << ", ";
    oss << elem.second;
  }
  return oss;
}

// Out element header

std::ostream& helpers::operator<<(std::ostream& oss, const Header& h)
{
  oss << "count: " << h.count_ << '\n';

  if (!h.single_props_.empty())
    oss << "  Single properies: \n";
 
  for (const auto& prop : h.single_props_)
  {
    oss << "    name: " << prop.first << ", "; 
    oss << "type: " << std::get<0>(prop.second) << ", ";
    oss << "arr: " << std::get<1>(prop.second) << ", ";
    oss << "pos: " << std::get<2>(prop.second) << '\n';
  }

  if (!h.list_props_.empty())
    oss << "  List properies: \n";

  for (const auto& prop : h.list_props_)
  {
    oss << "    name: " << prop.first << ", "; 
    oss << "type: " << std::get<0>(prop.second) << ", ";
    oss << "arr: " << std::get<1>(prop.second) << '\n';
  }
  return oss;
}

} // namespace ply
