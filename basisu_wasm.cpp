// basisu_frontend.cpp
// Copyright (C) 2019 Binomial LLC. All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// TODO:
// This code originally supported full ETC1 and ETC1S, so there's some legacy
// stuff to be cleaned up in here. Add endpoint tiling support (where we force
// adjacent blocks to use the same endpoints during quantization), for a ~10% or
// more increase in bitrate at same SSIM. The backend already supports this.
//
#include "basisu_comp.h"

extern "C" {
const void* basisu_compress(void* src, unsigned width, unsigned height,
                            bool has_alpha_channel);
unsigned basisu_compressed_size();
}

basisu::basis_compressor compressor;

const void* basisu_compress(void* src, unsigned width, unsigned height,
                            bool has_alpha_channel) {
  basist::etc1_global_selector_codebook sel_codebook(
      basist::g_global_selector_cb_size, basist::g_global_selector_cb);

  basisu::basis_compressor_params params;
  basisu::job_pool jpool(1);
  params.m_pJob_pool = &jpool;
  params.m_read_source_images = false;
  params.m_write_output_basis_files = false;
  params.m_pSel_codebook = &sel_codebook;

  basisu::image source(width, height);

  params.m_source_images.push_back(source);
  ::memcpy(source.get_ptr(), src, width * height * 4);

  if (!compressor.init(params) ||
      compressor.process() != basisu::basis_compressor::cECSuccess)
    return nullptr;

  return compressor.get_output_basis_file().data();
}

unsigned basisu_compressed_size() {
  return compressor.get_output_basis_file().size();
}
