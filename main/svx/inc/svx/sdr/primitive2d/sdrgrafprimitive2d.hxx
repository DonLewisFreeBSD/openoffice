/**************************************************************
 * 
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 * 
 *   http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 * 
 *************************************************************/



#ifndef INCLUDED_SDR_PRIMITIVE2D_SDRGRAFPRIMITIVE2D_HXX
#define INCLUDED_SDR_PRIMITIVE2D_SDRGRAFPRIMITIVE2D_HXX

#include <drawinglayer/primitive2d/baseprimitive2d.hxx>
#include <basegfx/matrix/b2dhommatrix.hxx>
#include <svtools/grfmgr.hxx>
#include <svx/sdr/attribute/sdrlinefillshadowtextattribute.hxx>

//////////////////////////////////////////////////////////////////////////////
// predefines

//////////////////////////////////////////////////////////////////////////////

namespace drawinglayer
{
	namespace primitive2d
	{
		class SdrGrafPrimitive2D : public BufferedDecompositionPrimitive2D
		{
		private:
			::basegfx::B2DHomMatrix						maTransform;
			attribute::SdrLineFillShadowTextAttribute	maSdrLFSTAttribute;
			GraphicObject								maGraphicObject;
			GraphicAttr									maGraphicAttr;

		protected:
			// local decomposition.
			virtual Primitive2DSequence create2DDecomposition(const geometry::ViewInformation2D& aViewInformation) const;

		public:
			SdrGrafPrimitive2D(
				const ::basegfx::B2DHomMatrix& rTransform, 
				const attribute::SdrLineFillShadowTextAttribute& rSdrLFSTAttribute,
				const GraphicObject& rGraphicObject,
				const GraphicAttr& rGraphicAttr);

			// data access
			const ::basegfx::B2DHomMatrix& getTransform() const { return maTransform; }
			const attribute::SdrLineFillShadowTextAttribute& getSdrLFSTAttribute() const { return maSdrLFSTAttribute; }
			const GraphicObject& getGraphicObject() const { return maGraphicObject; }
			const GraphicAttr& getGraphicAttr() const { return maGraphicAttr; }
			bool isTransparent() const;

			// compare operator
			virtual bool operator==(const BasePrimitive2D& rPrimitive) const;

			// provide unique ID
			DeclPrimitrive2DIDBlock()
		};
	} // end of namespace primitive2d
} // end of namespace drawinglayer

//////////////////////////////////////////////////////////////////////////////

#endif //INCLUDED_SDR_PRIMITIVE2D_SDRGRAFPRIMITIVE2D_HXX

// eof
