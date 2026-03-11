#pragma once
#include <cstdint>

struct Cvt_Header
{
    uint32_t magic = {}; // 'TAD' (내 맘대로 정한 고유번호)
    uint32_t iType = {};
    uint32_t iNumMeshes = {};
};
