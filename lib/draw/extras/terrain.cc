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
  , texture_{}
  , heightmap_{}
  , vxs_{}
  , chunks_{}
  , shading_{shading}
  , distances_{}
{   
  LoadTexture(tex_fname);
  LoadHeightmap(map_fname);
  ComputeAllVertices(div_factor);
  ComputeVerticesNormals();
  MakeChunks(obj_width);
  
  // Make all possible detalization faces for every chunk

  for (auto& obj : chunks_) {
    obj.ComputeAllFaces();
    obj.SetFace(0);
  }
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

// Store lenghts for determine detalization levels

void Terrain::SetDetalization(const V_Float& dists)
{
  distances_ = dists;
  std::sort(distances_.begin(), distances_.end());
}

// Change detalization of every chunk if necessary and align borders

void Terrain::ProcessDetalization(const GlCamera& cam)
{
  for (auto& chunk : chunks_)
  {
    // Find distance length between curr cam pos and pos of chunk

    Vector dist {chunk.world_pos_ - cam.vrp_};
    float  len = dist.Length();
    bool   need_align {false};
    int    det_level {0};

    // Compare distance of view and detalization levels

    for (auto& dist : distances_)
    {
      if (len < dist && det_level < chunk.DetLevels()) {
        need_align |= chunk.SetFace(det_level);
        break;
      }
      else
        ++det_level;
    }

    // If distance of view more than given, then set next as current

    if (need_align)
      chunk.AlignNeighboringChunks(chunks_);
    
    if (det_level >= chunk.DetLevels())
      det_level = chunk.DetLevels() - 1;

    if (chunk.SetFace(det_level))
      chunk.AlignNeighboringChunks(chunks_);
  }
}

// Searches Y ground position of given point. Note the following:
// - step between vertices is equal 1.0f, thus all calculations is based on this fact
//  - most left x is -A and most right x is +A
//  - most forward z is +B and most bottom z is -B
//  - 0.0 is the middle of the terrain mesh 

float Terrain::FindGroundPosition(const Vector& pos) const
{
  // Compute number of vertex under the camera

  float row_float = vxs_[0].pos_.z - pos.z;
  float col_float = pos.x - vxs_[0].pos_.x;

  // Make rounding of position above

  float tz = row_float - math::Floor(row_float);
  float tx = col_float - math::Floor(col_float);
  
  // Get neighboring vertices

  uint lt = (uint)row_float * hm_w_ + (uint)col_float;
  uint rt = lt + 1;
  uint lb = lt + hm_w_;
  uint rb = lb + 1;

  // If all vertices is inside mesh, then make average value of Y

  if (lt < vxs_.size() && lb < vxs_.size() && rt < vxs_.size() && rb < vxs_.size())
  {
    float y_tside = vxs_[lt].pos_.y + ((vxs_[rt].pos_.y - vxs_[lt].pos_.y) * tx);
    float y_bside = vxs_[lb].pos_.y + ((vxs_[rb].pos_.y - vxs_[lb].pos_.y) * tx);
    return y_tside + ((y_bside - y_tside) * tz);
  }
  else
    return 0;
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
  heightmap_ = Bitmap{fname};
  hm_h_ = heightmap_.height();
  hm_w_ = heightmap_.width();

  if (hm_h_ == 0 || hm_w_ == 0)
    throw DrawExcept("Heighmap width or height is zero");
  if (hm_h_ != hm_w_)
    throw DrawExcept("Heightmap width and height are not the same");
  if (hm_h_ < obj_h_ || hm_w_ < obj_w_)
    throw DrawExcept("Heightmap width or height less than given chunk width");  
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

  float u_step = 1.0f / hm_w_;
  float v_step = 1.0f / hm_h_; 

  // Filling

  vxs_.reserve(hm_h_ * hm_w_);

  for (int z = 0; z < hm_h_; ++z) {
    for (int x = 0; x < hm_w_; ++x) {

      // Make position and texture vectors

      float vx = (float)(x - half_w);
      float vy = (float)(heightmap_.red_channel(x,z) / div_factor);
      float vz = (float)(-(z - half_h));
      Vector pos {vx, vy, vz};
      Vector tex {x*u_step, z*v_step, 0.0f};    // interval would be 0-1 for all mesh

      // Fill and place vertex

      Vertex v {};
      v.pos_ = std::move(pos);
      v.texture_ = std::move(tex);
      v.color_ = FColor{color::White};  // by convient in ligthing purposes
      v.normal_ = {0.0f, 0.0f, 0.0f};
      vxs_.emplace_back(v);
    }
  }
}

// Divide vertices into the chunks and makes objects, where vxs_in_row is
// the count of vertices inside one line of chunk (should be 2^n+1).

void Terrain::MakeChunks(int vxs_in_row)
{
  if (!math::IsAbsFactorOfTwo(vxs_in_row - 1))
    throw DrawExcept("Chunk width is not the 2^n+1");

  // Precalculations

  int lpitch = hm_w_;
  int vxs_in_col   = vxs_in_row;
  int chunks_in_line = (hm_w_ - 1) / (vxs_in_row - 1);

  // Iterate over objects and create objects array, where x+y is current 
  // chunk number

  for (int y = 0; y < chunks_in_line; ++y) {
    for (int x = 0; x < chunks_in_line; ++x) {
      
      // Prepare neighboring chunks numbers and vertices array

      int ln {-1};
      int rn {-1};
      int tn {-1};
      int bn {-1};
      V_Vertex vxs {};
      vxs.reserve(vxs_in_row * vxs_in_col);

      // Fill neighboring chunks

      if (x != 0)
        ln = y * chunks_in_line + x - 1;
      if (x != chunks_in_line - 1)
        rn = y * chunks_in_line + x + 1;
      if (y != 0)
        tn = ((y - 1) * chunks_in_line) + x;
      if (y !=chunks_in_line - 1)
        bn = ((y + 1) * chunks_in_line) + x;

      // Compute indecies in global vertices list and get part of vertices
      
      int st_index = y * lpitch * (vxs_in_row-1) + x * (vxs_in_row-1);
      int en_index = st_index + lpitch * (vxs_in_row-1) + x * (vxs_in_row-1);

      for (int y = st_index; y <= en_index; y += lpitch)
        for (int x = y; x < y + vxs_in_row; ++x)
          vxs.push_back(vxs_[x]);
      
      // Create chunk
      
      Chunk obj {vxs, ln, rn, tn, bn};
      obj.textures_.emplace_back(texture_);
      obj.shading_ = shading_;
      chunks_.push_back(obj);
    }
  }
}

// Precomputes vertices normals for all terrain mesh using the most detalized
// faces. This function computes again faces normal and angles between face edge.

void Terrain::ComputeVerticesNormals()
{
  int w = std::sqrt(vxs_.size());

  // Iterate over all vertices, create temporarity faces and compute normals
  // for each vertex

  for (int y = 0; y < w-1; ++y)
  {
    for (int x = 0; x < w-1; ++x) {

      int lt = y*w+x;
      int rt = y*w+x+1;
      int lb = (y+1)*w+x;
      int rb = (y+1)*w+x+1;

      Face f1 {vxs_, lt, rt, lb};
      Face f2 {vxs_, rt, rb, lb};

      vxs_[lt].normal_ += f1.normal_ * f1.angles_[0];
      vxs_[rt].normal_ += f1.normal_ * f1.angles_[1];
      vxs_[lb].normal_ += f1.normal_ * f1.angles_[2];
      vxs_[rt].normal_ += f2.normal_ * f2.angles_[0];
      vxs_[lb].normal_ += f2.normal_ * f2.angles_[1];
      vxs_[rb].normal_ += f2.normal_ * f2.angles_[2];
    }
  }
  
  for (auto& vx : vxs_) {
    if (!vx.normal_.IsZero())
      vx.normal_.Normalize();
  }
}

//****************************************************************************
// PROXY CLASS MEMBER FUNCITONS IMPLEMENTATION
//****************************************************************************

Terrain::Chunk::Chunk(const V_Vertex& cvxs, int ln, int rn, int tn, int bn)
  : GlObject()
  , det_faces_{}
  , vxs_step_{}
  , vxs_in_row_(std::sqrt(cvxs.size()))  
  , left_chunk_{ln}
  , right_chunk_{rn}
  , top_chunk_{tn}
  , bottom_chunk_{bn}
  , min_y_{}
  , max_y_{}
{
  this->vxs_local_ = cvxs;
  this->vxs_trans_ = cvxs;
  this->vxs_backup_ = cvxs;
  auto& vxs = this->vxs_local_;

  // Compute world position (x and z is median, y is median between most top y
  // and less top y)

  auto minmax_y = 
    std::minmax_element(vxs.begin(), vxs.end(), [](Vertex& l, Vertex& r) {
    return l.pos_.y < r.pos_.y;  
  });
  auto min_y_el = minmax_y.first - vxs.begin();
  auto max_y_el = minmax_y.second - vxs.begin();
  min_y_ = vxs[min_y_el].pos_.y;
  max_y_ = vxs[max_y_el].pos_.y;
  this->world_pos_ = (vxs.front().pos_ + vxs.back().pos_) / 2.0f;
  this->world_pos_.y = (min_y_ + max_y_) / 2.0f;

  // Compute sphere radius (see note below)

  constexpr float kSureKoeff {1.5f};
  float rad_candidate_1 = (float)(vxs_in_row_) / 2.0f;
  float rad_candidate_2 = max_y_ - min_y_;
  this->sphere_rad_ = std::max(rad_candidate_1, rad_candidate_2);
  this->sphere_rad_ *= kSureKoeff;

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

// Set current face by given face_num and returns true if success
 
bool Terrain::Chunk::SetFace(int face_num)
{
  if (faces_.size() != det_faces_[face_num].size())
  {
    faces_ = det_faces_[face_num];
    vxs_local_ = vxs_backup_;
    vxs_step_ = std::pow(2, face_num); // 2 << face_num;
    return true;
  }
  return false;
}

// Copies coordinates by faces

void Terrain::Chunk::CopyCoords(Coords src, Coords dest)
{
  if (src == Coords::LOCAL && dest == Coords::TRANS)
  {
    for (auto& f : faces_)
    {
      vxs_trans_[f[0]] = vxs_local_[f[0]];
      vxs_trans_[f[1]] = vxs_local_[f[1]];
      vxs_trans_[f[2]] = vxs_local_[f[2]];
    }
  }
  else if (src == Coords::TRANS && dest == Coords::LOCAL)  
  {
    for (auto& f : faces_)
    {
      vxs_local_[f[0]] = vxs_trans_[f[0]];
      vxs_local_[f[1]] = vxs_trans_[f[1]];
      vxs_local_[f[2]] = vxs_trans_[f[2]];
    }
  }
}

// Computes all possible detalization faces for this chunk

void Terrain::Chunk::ComputeAllFaces()
{
  det_faces_.resize(0);
  int w = vxs_in_row_;
  
  // Here we simple use variable `det` as step

  for (int det = 1; det <= w; det *= 2){

    V_Face curr_faces {};

    for (int y = 0; y < w-det; y+=det) {    // -det since we take y+det
      for (int x = 0; x < w-det; x+=det) {  // and x+det inside loop
        
        // Create faces
          
        int lt = y*w+x;
        int rt = y*w+x+det;
        int lb = (y+det)*w+x;
        int rb = (y+det)*w+x+det;
        
        Face f1 {vxs_local_, lt, rt, lb};
        Face f2 {vxs_local_, rt, rb, lb};
        
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

// Try to align neighboring chunks after changing current chunk face

void Terrain::Chunk::AlignNeighboringChunks(std::vector<Terrain::Chunk>& chunks)
{
  if (this->left_chunk_ >= 0)     // if has left neighboring chubk
  {
    auto& neigh = chunks[this->left_chunk_];
    bool curr_detalized  = this->vxs_step_ < neigh.vxs_step_;
    bool neigh_detalized = this->vxs_step_ > neigh.vxs_step_;
    
    this->RecoverLeftSide();
    neigh.RecoverRightSide();

    if (curr_detalized)
      this->AlignLeftSide(neigh.vxs_step_);
    else if (neigh_detalized)
      neigh.AlignRightSide(this->vxs_step_);
  }

  if (this->right_chunk_ >= 0)    // if has right neighboring chunk
  {
    auto& neigh = chunks[this->right_chunk_];
    bool curr_detalized  = this->vxs_step_ < neigh.vxs_step_;
    bool neigh_detalized = this->vxs_step_ > neigh.vxs_step_;

    this->RecoverRightSide();
    neigh.RecoverLeftSide();

    if (curr_detalized)
      this->AlignRightSide(neigh.vxs_step_);
    else if (neigh_detalized)
      neigh.AlignLeftSide(this->vxs_step_);
  }

  if (this->top_chunk_ >= 0)      // if has top neighboring chunk
  {
    auto& neigh = chunks[this->top_chunk_];
    bool curr_detalized  = this->vxs_step_ < neigh.vxs_step_;
    bool neigh_detalized = this->vxs_step_ > neigh.vxs_step_;
    
    this->RecoverTopSide();
    neigh.RecoverBottomSide();

    if (curr_detalized)
      this->AlignTopSide(neigh.vxs_step_);
    else if (neigh_detalized)
      neigh.AlignBottomSide(this->vxs_step_);
  }

  if (this->bottom_chunk_ >= 0)      // if has bottom neighboring chunk
  {
    auto& neigh = chunks[this->bottom_chunk_];
    bool curr_detalized  = this->vxs_step_ < neigh.vxs_step_;
    bool neigh_detalized = this->vxs_step_ > neigh.vxs_step_;

    this->RecoverBottomSide();
    neigh.RecoverTopSide();
    
    if (curr_detalized)
      this->AlignBottomSide(neigh.vxs_step_);
    else if (neigh_detalized)
      neigh.AlignTopSide(this->vxs_step_);
  }
}

// Aligns left side of chunk in depends of step between vertices mesh
// of neighboring chunk  

void Terrain::Chunk::AlignLeftSide(float neigh_step)
{
  int k = 0;
  int vxs_total = vxs_local_.size();
  for (int i = 0; i < vxs_total; i += vxs_in_row_)
  {
    vxs_local_[i] = vxs_local_[k];
    if (k == i)
      k += neigh_step * vxs_in_row_;
  }
}

// Aligns right side of chunk in depends of step between vertices mesh
// of neighboring chunk  

void Terrain::Chunk::AlignRightSide(float neigh_step)
{
  int k = vxs_in_row_ - 1;
  int vxs_total = vxs_local_.size();  
  for (int i = k; i < vxs_total; i += vxs_in_row_)
  {
    vxs_local_[i] = vxs_local_[k];
    if (k == i)
      k += neigh_step * vxs_in_row_;
  }
}

// Aligns top side of chunk in depends of step between vertices mesh
// of neighboring chunk  

void Terrain::Chunk::AlignTopSide(float neigh_step)
{
  int k = 0;
  for (int i = k; i < vxs_in_row_; ++i)
  {
    vxs_local_[i] = vxs_local_[k];
    if (k == i)
      k += neigh_step;
  }
}

// Aligns bottom side of chunk in depends of step between vertices mesh
// of neighboring chunk  

void Terrain::Chunk::AlignBottomSide(float neigh_step)
{
  int k = vxs_in_row_ * (vxs_in_row_ - 1);
  int vxs_total = vxs_local_.size();  
  for (int i = k; i < vxs_total; ++i)
  {
    vxs_local_[i] = vxs_local_[k];
    if (k == i)
      k += neigh_step;
  }
}

// Aligns left side of chunk to its real step

void Terrain::Chunk::RecoverLeftSide()
{
  int k = 0;
  int vxs_total = vxs_backup_.size();  
  for (int i = 0; i < vxs_total; i += vxs_in_row_)
  {
    vxs_local_[i] = vxs_backup_[k];
    if (k == i)
      k += vxs_step_ * vxs_in_row_;
  }
}

// Aligns right side of chunk to its real step

void Terrain::Chunk::RecoverRightSide()
{
  int k = vxs_in_row_ - 1;
  int vxs_total = vxs_backup_.size();    
  for (int i = k; i < vxs_total; i += vxs_in_row_)
  {
    vxs_local_[i] = vxs_backup_[k];
    if (k == i)
      k += vxs_step_ * vxs_in_row_;
  }
}

// Aligns top side of chunk to its real step

void Terrain::Chunk::RecoverTopSide()
{
  int k = 0;
  for (int i = k; i < vxs_in_row_; ++i)
  {
    vxs_local_[i] = vxs_backup_[k];
    if (k == i)
      k += vxs_step_;
  }
}

// Aligns bottom side of chunk to its real step

void Terrain::Chunk::RecoverBottomSide()
{
  int k = vxs_in_row_ * (vxs_in_row_ - 1);
  int vxs_total = vxs_backup_.size();    
  for (int i = k; i < vxs_total; ++i)
  {
    vxs_local_[i] = vxs_backup_[k];
    if (k == i)
      k += vxs_step_;
  }
}

}  // namespace anshub