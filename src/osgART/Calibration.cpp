/* -*-c++-*- 
 * 
 * osgART - ARToolKit for OpenSceneGraph
 * Copyright (C) 2005-2009 Human Interface Technology Laboratory New Zealand
 * 
 * This file is part of osgART 2.0
 *
 * osgART 2.0 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * osgART 2.0 is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with osgART 2.0.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "osgART/Calibration"



template <typename T> 
inline int Observer2Ideal(	
					const float dist_factor[4], 
					const T ox, 
					const T oy,
					T *ix, T *iy,
					int loop = 3 )
{
    T  z02, z0, p, q, z, px, py;
    register int i = 0;

    px = ox - dist_factor[0];
    py = oy - dist_factor[1];
    p = dist_factor[2]/100000000.0;
    z02 = px*px+ py*py;
    q = z0 = sqrt(px*px+ py*py);

    for( i = 1; ; i++ ) {
        if( z0 != 0.0 ) {
            z = z0 - ((1.0 - p*z02)*z0 - q) / (1.0 - 3.0*p*z02);
            px = px * z / z0;
            py = py * z / z0;
        }
        else {
            px = 0.0;
            py = 0.0;
            break;
        }
        if( i == loop ) break;

        z02 = px*px+ py*py;
        z0 = sqrt(px*px+ py*py);
    }

    *ix = px / dist_factor[3] + dist_factor[0];
    *iy = py / dist_factor[3] + dist_factor[1];

    return(0);
}

namespace osgART {

Calibration::Calibration() : osg::Referenced()
{
}

/* virtual */
void 
Calibration::setSize(int width, int height)
{
}

/* virtual */
void 
Calibration::setSize(const osg::Image& image)
{
	setSize(image.s(),image.t());	
}
		
/*virtual*/ 
bool 
Calibration::load(const std::string& filename)
{
	return false;
}

osg::Camera* 
Calibration::createCamera() const
{
	osg::Camera* cam = new osg::Camera();
	cam->setRenderOrder(osg::Camera::NESTED_RENDER);
	cam->setProjectionMatrix(_projection);
	cam->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
	cam->setComputeNearFarMode(osg::CullSettings::DO_NOT_COMPUTE_NEAR_FAR);

	return cam;	
}

Calibration::~Calibration()
{
}


}

