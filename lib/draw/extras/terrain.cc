// *************************************************************
// File:    terrain.cc
// Descr:   represents terrain
// Author:  Novoselov Anton @ 2018
// URL:     https://github.com/ans-hub/game_console
// *************************************************************

#include "terrain.h"

namespace anshub {

Terrain::Terrain(
  cChar* map_fname, cChar* tex_fname, 
  int div_factor, int obj_width, Shading shading)
  : hm_w_{}
  , hm_h_{}
  , tx_w_{}
  , tx_h_{}
  , obj_w_{obj_width}
  , obj_h_{obj_width}
  , texture_{nullptr}
  , heightmap_{nullptr}
  , vxs_{}
  , chunks_{}
  , shading_{shading}
{
  LoadTexture(tex_fname);
  LoadHeightmap(map_fname);
  ComputeAllVertices(div_factor);
  MakeChunks(obj_width);
  MakeChunkFaces();
  FillChunks();
}

//****************************************************************************
// PUBLIC MEMBER FUNCTIONS IMPLEMENTATION
//****************************************************************************

// Sets shading of terrain

void Terrain::SetShading(Shading s)
{
  shading_ = s;
  for (auto& chunk : chunks_)
    chunk.shading_ = s;
}

// Change detalization of every chunk if necessary and align borders

void Terrain::ProcessDetalization(const GlCamera& cam)
{
  for (auto& chunk : chunks_)
  {
    if (!chunk.active_)
      continue;

    // Find distance length between curr cam pos and pos of chunk

    Vector dist {chunk.world_pos_ - cam.vrp_};
    float len = dist.Length();

    // Change detalization faces
    // todo: make correct range check

    if (len < 40.0f)
    {
      if (chunk.SetFace(1))
        AlignNeighboringChunks(chunk);      
    }
    else if (len >= 40.0f && len < 60.0f)
    {
      if (chunk.SetFace(2))
        AlignNeighboringChunks(chunk);      
    }
    else if (len >= 60.0f && len < 80.0f)
    {
      if (chunk.SetFace(3))
        AlignNeighboringChunks(chunk);
    }
    // else if (len >= 80.0f && len < 100.0f)
    else
    {
      chunk.SetFace(4);
      // if (chunk.SetFace(4))
        // AlignNeighboringChunks(chunk);
    }
    // else
    // {
    //   chunk.SetFace(4);
    //     // AlignNeighboringChunks(chunk);
    // }
  }
}

//****************************************************************************
// PRIVATE MEMBER FUNCTIONS IMPLEMENTATION
//****************************************************************************

// Loads texture of terrain

void Terrain::LoadTexture(const char* fname)
{
  texture_ = std::make_shared<Bitmap>(fname);
  tx_h_ = texture_->height();
  tx_w_ = texture_->width();

  if (tx_h_ == 0 || tx_w_ == 0)
    throw DrawExcept("Texture width or height is zero");

  // Note: unnecessary to check for square of texture or
  // to check is w and h of texture is the factor of two 
  // since we may stretch texture as we wish
}

// Loads heightmap of terrain

void Terrain::LoadHeightmap(const char* fname)
{
  heightmap_ = std::make_unique<Bitmap>(fname);
  hm_h_ = heightmap_->height();
  hm_w_ = heightmap_->width();

  if (hm_h_ == 0 || hm_w_ == 0)
    throw DrawExcept("Heighmap width or height is zero");
  if (tx_h_ != tx_w_)
    throw DrawExcept("Heightmap width and height are not the same");
  if (!math::IsAbsFactorOfTwo(tx_h_-1) || (!math::IsAbsFactorOfTwo(tx_w_-1)))
    throw DrawExcept("Heightmap dimensions are not the 2^n+1");
}

// Compute vertices for most detalized level. Divide factor used here
// to regulate impact of pixel color to y coordinate

void Terrain::ComputeAllVertices(int div_factor)
{
  // Helpers computations

  int half_h = (hm_h_-1) / 2;
  int half_w = (hm_w_-1) / 2;
   
  // Compute step of texture for each height map pixel

  float dx_u = (float)(tx_w_) / (float)(hm_w_ - 1);
  float dx_v = (float)(tx_h_) / (float)(hm_h_ - 1); 

  // Filling

  vxs_.reserve(hm_h_ * hm_w_);

  for (int z = 0; z < hm_h_; ++z) {
    for (int x = 0; x < hm_w_; ++x) {

      // Make position and texture vectors

      float vx = (float)(x - half_w);
      float vy = (float)(heightmap_->red_channel(x,z) / div_factor);
      float vz = (float)(-(z - half_h));
      Vector pos {vx, vy, vz};
      Vector tex {x*dx_u, z*dx_v, 0.0f};

      // Fill and place vertex

      Vertex v {};
      v.pos_ = std::move(pos);
      v.texture_ = std::move(tex);
      v.color_ = FColor{color::White};  // by convient in ligthing purposes
      vxs_.emplace_back(v);
    }
  }
}

// Divide vertices into the chunks and makes objects, where chunk_width is
// the count of vertices inside one chunk (should be 2^n+1)

void Terrain::MakeChunks(int chunk_width)
{
  if (!math::IsAbsFactorOfTwo(chunk_width - 1))
    throw DrawExcept("Chunk width is not the 2^n+1");

  // Precalculations

  int lpitch = hm_w_;
  int chunk_height   = chunk_width;
  int chunks_in_line = (hm_w_ - 1) / (chunk_width - 1);

  // Iterate over objects and create objects array, where x+y is current 
  // chunk number

  for (int y = 0; y < chunks_in_line; ++y) {
    for (int x = 0; x < chunks_in_line; ++x) {
      
      Chunk obj {chunk_width};
      
      // Fill neighboring chunks

      if (x != 0)
        obj.left_ = y * chunks_in_line + x - 1;
      if (x != chunks_in_line - 1)
        obj.right_ = y * chunks_in_line + x + 1;
      if (y != 0)
        obj.top_ = ((y - 1) * chunks_in_line) + x;
      if (y !=chunks_in_line - 1)
        obj.bottom_ = ((y + 1) * chunks_in_line) + x;

      // Compute indecies in global vertices list and get part of vertices
      
      int st_index = y * lpitch * (chunk_height-1) + x * (chunk_width-1);
      int en_index = st_index + lpitch * (chunk_height-1) + x * (chunk_width-1);

      for (int y = st_index; y <= en_index; y += lpitch)
        for (int x = y; x < y + chunk_width; ++x)
          obj.vxs_local_.push_back(vxs_[x]);
      obj.vxs_trans_ = obj.vxs_local_;
          
      chunks_.push_back(obj);
    }
  }
}

// Compute all possible faces for all detalizations

void Terrain::MakeChunkFaces()
{
  for (auto& obj : chunks_)
  {
    obj.ComputeAllFaces();
    obj.SetFace(0);
  }
}

// Fill formed early chunks

void Terrain::FillChunks()
{
  for (auto& obj : chunks_)
  {
    // Set chunk attributes

    obj.active_ = true;
    obj.textured_ = true;
    obj.texture_ = texture_;  // todo: make sure, that Bitmap doesn`t copies
    obj.shading_ = shading_;

    // Compute world position (x and z is median, y is median between most top y
    // and less top y)

    auto& vxs = obj.vxs_local_;
    auto minmax_y = 
      std::minmax_element(vxs.begin(), vxs.end(), [](Vertex& l, Vertex& r) {
      return l.pos_.y < r.pos_.y;  
    });
    auto min_y_el = minmax_y.first - vxs.begin();
    auto max_y_el = minmax_y.second - vxs.begin();
    obj.min_y_ = vxs[min_y_el].pos_.y;
    obj.max_y_ = vxs[max_y_el].pos_.y;
    obj.world_pos_ = (vxs.front().pos_ + vxs.back().pos_) / 2.0f;
    obj.world_pos_.y = (obj.min_y_ + obj.max_y_) / 2.0f;

    // Compute sphere radius (see note below)

    constexpr float kSureKoeff {1.5f};
    float rad_candidate_1 = (float)(obj.chunk_width_) / 2.0f;
    float rad_candidate_2 = obj.max_y_ - obj.min_y_;
    obj.sphere_rad_ = std::max(rad_candidate_1, rad_candidate_2);
    obj.sphere_rad_ *= kSureKoeff;
  }

  // Note #1 : we can`t simple call function to calc radius, since in regular 
  // object 0,0,0 is the middle point, but in Chunk this is not.

  // Note #2 : it is more right to make culling object not by world_pos, but by 
  // corners of bounding box. If we cull my world_pos, we may take artifacts,
  // i.e. obj is culled, but still shoul be visible. This is because in some place
  // of screen some objects may be visible as stretch and since we don`t convert
  // corner coordinates we have result that real projected radius more than
  // sphere radius. But due to perfomance I don`t want to convert 8 vector in
  // each object. May be this is not so correct, but I increase sphere_rad to 1.5f 
}

// Try to align neighboring chunks after changing curr chunk faces

void Terrain::AlignNeighboringChunks(Terrain::Chunk& curr)
{
  if (curr.left_ > 0)     // if has left neighboring chubk
  {
    auto& neigh = chunks_[curr.left_];
    if (curr.faces_.size() > neigh.faces_.size())
      curr.AlignLeftToBiggest();

    else if (curr.faces_.size() < neigh.faces_.size())
      neigh.AlignRightToBiggest();

    else {
      neigh.AlignRightToBiggest();
      curr.AlignLeftToBiggest();
    }
  }

  if (curr.right_ > 0)    // if has right neighboring chunk
  {
    auto& neigh = chunks_[curr.right_];
    if (curr.faces_.size() < neigh.faces_.size())
      neigh.AlignLeftToBiggest();

    else if (curr.faces_.size() > neigh.faces_.size())
      curr.AlignRightToBiggest();

    else {
      curr.AlignRightToBiggest();
      neigh.AlignLeftToBiggest();
    }
  }

  if (curr.top_ > 0)      // if has top neighboring chunk
  {
    // auto& neigh = chunks_[curr.top_];
    // if (curr.faces_.size() < neigh.faces_.size())
    //   neigh.AlignBottomToBiggest();

    // else if (curr.faces_.size() > neigh.faces_.size())
    //   curr.AlignTopToBiggest();

    // else {
    //   curr.AlignTopToBiggest();
    //   neigh.AlignBottomToBiggest();
    // }
  }

  if (curr.bottom_ > 0)      // if has bottom neighboring chunk
  {
  //   auto& neigh = chunks_[curr.bottom_];
  //   if (curr.faces_.size() < neigh.faces_.size())
  //     neigh.AlignTopToBiggest();

  //   else if (curr.faces_.size() > neigh.faces_.size())
  //     curr.AlignBottomToBiggest();

  //   else {
  //     curr.AlignBottomToBiggest();
  //     neigh.AlignTopToBiggest();
  //   }
  }
}

//****************************************************************************
// PROXY CLASS MEMBER FUNCITONS IMPLEMENTATION
//****************************************************************************

// Set current face by given face_num and returns true if success
 
bool Terrain::Chunk::SetFace(int face_num)
{
  if (faces_.size() != det_faces_[face_num].size())
  {
    faces_ = det_faces_[face_num];
    return true;
  }
  return false;
}

// Computes all possible detalization faces for object

void Terrain::Chunk::ComputeAllFaces()
{
  det_faces_.resize(0);
  int w = chunk_width_;
  
  // Here we simple use variable `det` as step

  for (int det = 1; det < w; det *= 2){

    V_Face curr_faces {};

    for (int y = 0; y < w-det; y+=det) {    // -det since we take y+det
      for (int x = 0; x < w-det; x+=det) {  // and x+det inside loop
        
        // Create faces

        Face f1 {
          vxs_local_, y*w+x, y*w+x+det, (y+det)*w+x};
        Face f2 {
          vxs_local_, y*w+x+det, (y+det)*w+x+det, (y+det)*w+x};
        
        // Fill face color (by convient this is the white)

        f1.color_ = FColor{color::White};
        f2.color_ = FColor{color::White};

        curr_faces.push_back(f1);
        curr_faces.push_back(f2);
      }
    }
    det_faces_.push_back(curr_faces);
  }
}

// Aligns current chunk by left border to prevent gaps between different faces
// in current chunk and neighboring chunk (aligns left border to biggest)

void Terrain::Chunk::AlignLeftToBiggest()
{
  // Compute faces in row. Size/2 - count of quads in arr,
  //  sqrt(size/2) - count of quads in row, res*2 = count of faces in row 

  int lpitch = std::sqrt(faces_.size() / 2) * 2;
  
  // Iterate throw faces rows
  
  for (int y = 0; y < faces_.size(); y += lpitch * 2)
  {
    int f1 = y;           // first face in row
    int f2 = y+1;         // first opposite face in row
    int f3 = y+lpitch;    // first face in next row
    int f4 = y+lpitch+1;  // first opposite face in next row

    faces_[f1][2] = faces_[f4][2];
    faces_[f2][2] = faces_[f4][2];
    faces_[f3].vxs_ = faces_[f4].vxs_;
  }
}

// Aligns current chunk by left border to prevent gaps between different faces
// in current chunk and neighboring chunk (aligns right border to biggest)

void Terrain::Chunk::AlignRightToBiggest()
{
  // Compute faces in row. Size/2 - count of quads in arr,
  //  sqrt(size/2) - count of quads in row, res*2 = count of faces in row 

  int lpitch = std::sqrt(faces_.size() / 2) * 2;

  // Iterate throw faces rows

  for (int y = lpitch-2; y < faces_.size(); y += lpitch * 2)
  {
    int f1 = y;           // last face in row
    int f2 = y+1;         // last opposite face in row
    int f3 = y+lpitch;    // last face in next row
    int f4 = y+lpitch+1;  // last opposite face in next row
    
    faces_[f2].vxs_ = faces_[f1].vxs_;
    faces_[f3][1] = faces_[f1][1];
    faces_[f4][0] = faces_[f1][1];
  }
}

// Aligns current chunk by top border to prevent gaps between different faces
// in current chunk and neighboring chunk (aligns top border to biggest chunk)

void Terrain::Chunk::AlignTopToBiggest()
{
  // Compute faces in row. Size/2 - count of quads in arr,
  //  sqrt(size/2) - count of quads in row, res*2 = count of faces in row 

  int lpitch = std::sqrt(faces_.size() / 2) * 2;
  
  // Iterate through faces cols

  for (int x = 0; x < lpitch; x += 4)
  {
    int f1 = x;     // first face in first col
    int f2 = x+1;   // first opposite face in col
    int f3 = x+2;   // first face in next col
    int f4 = x+3;   // first opposite face in next col

    faces_[f1][0] = faces_[f4][0];
    faces_[f2][0] = faces_[f4][0];
    faces_[f3].vxs_ = faces_[f4].vxs_;
  }
}

// Aligns current chunk by bototm border to prevent gaps between different faces
// in current chunk and neighboring chunk (aligns bottom border to biggest chunk)

void Terrain::Chunk::AlignBottomToBiggest()
{
  // Compute faces in row. Size/2 - count of quads in arr,
  //  sqrt(size/2) - count of quads in row, res*2 = count of faces in row 

  int lpitch = std::sqrt(faces_.size() / 2) * 2;
  
  // Iterate through faces cols

  for (int x = faces_.size() - 1 - lpitch; x < faces_.size(); x += 4)
  {
    int f1 = x;     // first face in first col
    int f2 = x+1;   // first opposite face in col
    int f3 = x+2;   // first face in next col
    int f4 = x+3;   // first opposite face in next col

    faces_[f2].vxs_ = faces_[f1].vxs_;
    faces_[f3][2] = faces_[f1][2];
    faces_[f4][2] = faces_[f1][2];
  }
}

}  // namespace anshub