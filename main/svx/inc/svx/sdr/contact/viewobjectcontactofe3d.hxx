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



#ifndef _SDR_CONTACT_VIEWOBJECTCONTACTOFE3D_HXX
#define _SDR_CONTACT_VIEWOBJECTCONTACTOFE3D_HXX

#include <svx/sdr/contact/viewobjectcontactofsdrobj.hxx>
#include <drawinglayer/primitive3d/baseprimitive3d.hxx>

//////////////////////////////////////////////////////////////////////////////
// predeclarations

namespace sdr { namespace contact {
	class ViewObjectContactOfE3dScene;
}}

//////////////////////////////////////////////////////////////////////////////

namespace sdr
{
	namespace contact
	{
		class ViewObjectContactOfE3d : public ViewObjectContactOfSdrObj
		{
		protected:
			// Primitive3D sequence of the ViewContact. This contains all necessary information
			// for the graphical visualisation and needs to be supported by all VCs which
			// can be visualized.
			drawinglayer::primitive3d::Primitive3DSequence			mxPrimitive3DSequence;

			// This method is responsible for creating the graphical visualisation data which is
			// stored/cached in the local primitive. Default gets view-independent Primitive3D
			// from the ViewContact using ViewContact::getViewIndependentPrimitive3DSequence(), takes care of
			// visibility and ghosted.
			// This method will not handle included hierarchies and not check geometric visibility.
			drawinglayer::primitive3d::Primitive3DSequence createPrimitive3DSequence(const DisplayInfo& rDisplayInfo) const;

			// also overload the 2d method to deliver a 2d object with embedd3d 3d and the 3d transformation which is able to
			// answer the get2DRange question accordingly
			virtual drawinglayer::primitive2d::Primitive2DSequence createPrimitive2DSequence(const DisplayInfo& rDisplayInfo) const;

		public:
			ViewObjectContactOfE3d(ObjectContact& rObjectContact, ViewContact& rViewContact);
			virtual ~ViewObjectContactOfE3d();

			// access to the local primitive sequence. This will ensure that the list is
			// current in comparing the local list content with a fresh created incarnation
			// This method will not handle included hierarchies or visibility.
			drawinglayer::primitive3d::Primitive3DSequence getPrimitive3DSequence(const DisplayInfo& rDisplayInfo) const;
		};
	} // end of namespace contact
} // end of namespace sdr

//////////////////////////////////////////////////////////////////////////////

#endif //_SDR_CONTACT_VIEWOBJECTCONTACTOFE3D_HXX

// eof
