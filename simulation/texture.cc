/*****************************************************************************
 *                                                                           *
 * The breve Simulation Environment                                          *
 * Copyright (C) 2000-2007 Jonathan Klein                                    *
 *                                                                           *
 * This program is free software; you can redistribute it and/or modify      *
 * it under the terms of the GNU General Public License as published by      *
 * the Free Software Foundation; either version 2 of the License, or         *
 * (at your option) any later version.                                       *
 *                                                                           *
 * This program is distributed in the hope that it will be useful,           *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of            *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
 * GNU General Public License for more details.                              *
 *                                                                           *
 * You should have received a copy of the GNU General Public License         *
 * along with this program; if not, write to the Free Software               *
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA *
 *****************************************************************************/

#include "texture.h"
#include "image.h"

#define MAX( x, y ) ( (x)>(y)?(x):(y) )

slTexture2D::slTexture2D( std::string &inFile, bool inRepeat ) {
	_textureID = 0;
	loadImage( inFile, inRepeat );
}

slTexture2D::slTexture2D() {
	_textureID = 0;
}

int slTexture2D::loadImage( std::string &inFile, bool inRepeat ) {
	unsigned char *pixels;
	int height, width, components;

	pixels = slReadImage( inFile.c_str(), &width, &height, &components, 1 );

	if ( !pixels ) 
		return -1;
		// throw slException( std::string( "Could not read image file " ) + inFile );

	loadPixels( pixels, width, height, inRepeat );

	delete[] pixels;

	if( !_textureID )
		return -1;

	return 0;
}

slTexture2D::~slTexture2D() {
	if( _textureID != 0 ) 
		glDeleteTextures( 1, &_textureID );
}

void slTexture2D::loadPixels( unsigned char *pixels, int inWidth, int inHeight, bool inRepeat ) {
	int newwidth = slNextPowerOfTwo( inWidth );
	int newheight = slNextPowerOfTwo( inHeight );

	if( _textureID == 0 )
		glGenTextures( 1, &_textureID );

	newwidth = newheight = MAX( newwidth, newheight );

	bind();

	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

	GLenum edge = inRepeat ? GL_REPEAT : GL_CLAMP_TO_EDGE;

	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, edge );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, edge );
	glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );

	if ( newwidth != inWidth || newheight != inHeight ) {
		int xstart, ystart;

		unsigned char *newpixels = new unsigned char[newwidth * newheight * 4];

		memset( newpixels, 0, newwidth * newheight * 4 );

		xstart = 0;
		ystart = 0;

		for ( int y = 0; y < inHeight; y++ )
			memcpy( &newpixels[( y + ystart ) *( newwidth * 4 ) + ( xstart * 4 )], &pixels[y * inWidth * 4], inWidth * 4 );

		glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, newwidth, newheight, 0, GL_RGBA, GL_UNSIGNED_BYTE, newpixels );
		// glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, newwidth, newheight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL );
		// glTexSubImage2D( GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, pixels );

		_unitX = ( inWidth - 0.5 )  / (float)newwidth;
		_unitY = ( inHeight - 0.5 ) / (float)newheight;

		delete[] newpixels;
	} else {
		glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, inWidth, inHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels );
		_unitX = 1.0;
		_unitY = 1.0;
	}
}

#undef MAX
