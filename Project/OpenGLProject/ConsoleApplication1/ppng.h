#pragma once
#include <png.h>
#include <vector>
#include "map"
#include "assert.h"
#include "iostream"

using namespace std;

void png_read(string filename, int& w, int& h, vector<uint8_t>& data, map<string, int>& meta);

//pv should be a pointer to RGBA data
void png_write(string filename, void* pv, int w, int h);
