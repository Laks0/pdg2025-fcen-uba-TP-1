//------------------------------------------------------------------------
//	Copyright (C) Gabriel Taubin
//	Time-stamp: <2025-08-04 22:14:44 gtaubin>
//------------------------------------------------------------------------
//
// SaverStl.cpp
//
// Written by: <Your Name>
//
// Software developed for the course
// Digital Geometry Processing
// Copyright (c) 2025, Gabriel Taubin
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//		 * Redistributions of source code must retain the above copyright
//			 notice, this list of conditions and the following disclaimer.
//		 * Redistributions in binary form must reproduce the above copyright
//			 notice, this list of conditions and the following disclaimer in the
//			 documentation and/or other materials provided with the distribution.
//		 * Neither the name of the Brown University nor the
//			 names of its contributors may be used to endorse or promote products
//			 derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL GABRIEL TAUBIN BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include "SaverStl.hpp"

#include "../wrl/Shape.hpp"
#include "../wrl/Appearance.hpp"
#include "../wrl/Material.hpp"
#include "../wrl/IndexedFaceSet.hpp"

#include "../core/Faces.hpp"
#include <cstdio>
#include <cstring>
#include <vector>

const char* SaverStl::_ext = "stl";

//////////////////////////////////////////////////////////////////////
bool SaverStl::save(const char* filename, SceneGraph& wrl) const {
	bool success = false;
	if(filename!=(char*)0) {

		// Check these conditions

		// 1) the SceneGraph should have a single child
		if (wrl.getNumberOfChildren() != 1) return false;
		// 2) the child should be a Shape node
		if (!wrl[0]->isShape()) return false;
		Shape* shapeNode = (Shape*) wrl[0];
		// 3) the geometry of the Shape node should be an IndexedFaceSet node
		if (!shapeNode->getGeometry()->isIndexedFaceSet()) return false;
		IndexedFaceSet* shapeGeometry = (IndexedFaceSet*) shapeNode->getGeometry();

		// - construct an instance of the Faces class from the IndexedFaceSet
		// - remember to delete it when you are done with it (if necessary)
		//	 before returning

		// 4) the IndexedFaceSet should be a triangle mesh
		if (!shapeGeometry->isTriangleMesh()) return false;
		// 5) the IndexedFaceSet should have normals per face
		if (shapeGeometry->getNormalBinding() != IndexedFaceSet::Binding::PB_PER_FACE) return false;

		// if (all the conditions are satisfied) {

		Faces f(shapeGeometry->getNumberOfCoord(), shapeGeometry->getCoordIndex());
		vector<float>& coords = shapeGeometry->getCoord();
		vector<float>& normal = shapeGeometry->getNormal();

		FILE* fp = fopen(filename,"w");
		if(	fp!=(FILE*)0) {

			// if set, use ifs->getName()
			// otherwise use filename,
			// but first remove directory and extension
			const char* name = shapeGeometry->getName().c_str();
			char str[1024];
			if (shapeGeometry->getName().empty()) {
				int i, j;
				int n = (int) strlen(filename);
				for (i=n-1; i >= 0; i--)
					if (filename[i] == '/' || filename[i] == '\\') break;
				i++;
				for (j=0; j < n; j++)
					if (filename[j] == '.') break;
				j--;

				snprintf(str, j-i+2, "%s", filename+i);
				name = str;
			}

			fprintf(fp,"solid %s\n",name);

			for (int iF = 0; iF < f.getNumberOfFaces(); iF++) {

				float nx = normal[iF*3];
				float ny = normal[iF*3+1];
				float nz = normal[iF*3+2];

				fprintf(fp, "facet normal %e %e %e\n", nx, ny, nz);
				fprintf(fp, "  outer loop\n");

				for (int i = 0; i < f.getFaceSize(iF); i++) {
					int iV = f.getFaceVertex(iF, i);
					float x = coords[iV*3];
					float y = coords[iV*3+1];
					float z = coords[iV*3+2];

					fprintf(fp, "	  vertex %e %e %e\n", x, y, z);
				}

				fprintf(fp, "  endloop\n");
				fprintf(fp, "endfacet\n");
			}

			fclose(fp);
			success = true;
		}

		// } endif (all the conditions are satisfied)

	}
	return success;
}
