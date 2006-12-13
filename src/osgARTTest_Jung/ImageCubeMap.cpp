
#include "ImageCubeMap"


namespace Face
{
    bool ImageToFace::setImage( osg::Image * image )
    {
        int width = image->s() / 3, height = image->t() / 4;
        if ( width != height )
        {
            is_valid = false;
            size = 0;
            channels = 0;
            this->image = NULL;
        }
        else
        {
            is_valid = true;
            size = width;
            channels = ( image->getPixelSizeInBits() / 8 )
                / sizeof( GLfloat );
            this->image = image;
        }
        return is_valid;
    }

    osg::Image * ImageToFace::getFace( int index )
    {
        if (!is_valid) return NULL;

        const Face::Data & face = Face::Values[index];

        GLfloat * data = copyFloatData( face.x * size, face.y * size,
                size, size, channels, image.get(), face.rotate );

        osg::Image * face_image = createFloatImage( size, size,
                channels, data );

        if ( face.flip_vertical ) face_image->flipVertical();
        if ( face.flip_horizontal ) face_image->flipHorizontal();
        
        return face_image;
    }


	GLfloat* ImageToFace::copyFloatData( int x, int y, int w, int h, int c, osg::Image * image,
        bool rotate )
{
    if (!image) return NULL;

    GLfloat * data = new GLfloat[w * h * c];

    for ( int i = 0; i < w * h * c; ++i )
    {
        //if ( i % w/4 == 0 )
        if ( i % c == 0 )
            data[i] = 1.0;
    }

    GLfloat * d = data;
    int ib = rotate ? x : y;
    int ie = rotate ? x+w : y+h;
    int jb = rotate ? y : x;
    int je = rotate ? y+h : x+w;

    for ( int i = ib; i < ie; ++i )
    for ( int j = jb; j < je; ++j )
    {
        GLfloat * value;
        if ( rotate )
            value = (GLfloat *) image->data(i, j);
        else
            value = (GLfloat *) image->data(j, i);
        
        for ( int k = 0; k < c; ++k )
        {
            *d = value[k];
            d++;
        }
    }

    return data;
}

osg::Image* ImageToFace::createFloatImage( int w, int h, int c, GLfloat * data )
{
    GLenum format, internalFormat;
    switch (c)
    {
        case 1:
            format = GL_LUMINANCE;
            internalFormat = GL_LUMINANCE8;
            break;
        case 3:
            format = GL_RGB;
            internalFormat = GL_RGB8;
            break;
        case 4:
            format = GL_RGBA;
            internalFormat = GL_RGBA8;
            break;
        default:
            return NULL;
    }

    if (!data)
    {
        data = new GLfloat[w * h * c];

        for ( int i = 0; i < w * h * c; ++i )
        {
            //if ( i % w/4 == 0 )
                data[i] = 1.0;
        }
    }

    osg::Image *image = new osg::Image;
    image->setImage(  w, h, 1, internalFormat,
                    format, GL_FLOAT, (unsigned char *) data,
                    osg::Image::USE_NEW_DELETE );

    return image;
}

};

bool ImageCubeMap::readImageFile( const std::string filename,
        osgDB::ReaderWriter::Options * options )
{
    osg::ref_ptr<osg::Image> image =
        osgDB::readImageFile( filename, options );
    if ( !image ) return false;
    if ( image->getDataType() != GL_FLOAT ) return false;

    //Hack to convert HDR Shop cross to cubemap
    
    if ( !imagetoface.setImage( image.get() ) ) return false;

    for ( int index = Face::POSITIVE_X; index <= Face::NEGATIVE_Z; ++index )
    {
        images[index] = imagetoface.getFace( index );
        if ( !images[index].get() )
            return false;
    }
    return true;
}

bool ImageCubeMap::readImageFile( std::string prefix, std::string ext,
        osgDB::ReaderWriter::Options * options )
{
/*
    std::string prefix, ext;
    ext = ".jpg";
    prefix = "Cubemap_snow/";
    ext = ".hdr";
    prefix = "../data/cube/stpeters_cross-";
    prefix = "../data/cube/rnl_cross-";
    prefix = "../data/cube/uffizi_cross-";
*/
    for ( int index = Face::POSITIVE_X; index <= Face::NEGATIVE_Z; ++index )
    {
        std::string filename = prefix + Face::Values[index].index_name + ext;
        images[index] = osgDB::readImageFile( filename, options );
        if ( !images[index].get() )
            return false;
    }
    return true;
}

void ImageCubeMap::setIntoTexture( osg::TextureCubeMap * cubemap )
{
    if (!cubemap) return;

    for ( int index = Face::POSITIVE_X; index <= Face::NEGATIVE_Z; ++index )
        cubemap->setImage( index, images[index].get() );
}

