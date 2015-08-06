/* -*- Mode: C; tab-width: 4 -*-
 * swirl --- swirly color-cycling patterns.
 */

/* Copyright (c) 1994 M.Dobie <mrd@ecs.soton.ac.uk>
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appear in all copies and that
 * both that copyright notice and this permission notice appear in
 * supporting documentation.
 *
 * This file is provided AS IS with no warranties of any kind.  The author
 * shall have no liability with respect to the infringement of copyrights,
 * trade secrets or any patents by this file or any part thereof.  In no
 * event will the author be liable for any lost revenue or profits or
 * other special, indirect and consequential damages.
 *
 * 01-May-01: Completely destroyed by Stephane Sudre to port it to Mac OS X
 * 13-May-97: jwz@jwz.org: turned into a standalone program.
 * 21-Apr-95: improved startup time for TrueColour displays
 *            (limited to 16bpp to save memory) S.Early <sde1000@cam.ac.uk>
 * 09-Jan-95: fixed colour maps (more colourful) and the image now spirals
 *            outwards from the centre with a fixed number of points drawn
 *            every iteration. Thanks to M.Dobie <mrd@ecs.soton.ac.uk>.
 * 1994:      written.   Copyright (c) 1994 M.Dobie <mrd@ecs.soton.ac.uk>
 *            based on original code by R.Taylor
 */

#include "xswirl.h"

#include <math.h>
#include <string.h>

#include <OpenGL/gl.h>
#include <OpenGL/glu.h>

#define LRAND()                    ((int) (random() & 0x7fffffff))
#define MAXRAND                    (2147483648.0) /* unsigned 1<<31 as a float */

/* an array of swirls for each screen */

COLOUR swirl::basic_colours[SWIRLCOLOURS];

void initImage(swirl * swirl);

int random_no(unsigned int n);

int random_no(unsigned int n)
{
	return ((int) ((n + 1) * (double) LRAND() / MAXRAND));
}

void swirl::_initialize_colours(float saturate)
{
	/* start off fully saturated, medium and bright colours */
    swirl::basic_colours[0].r = 0xA000;
	swirl::basic_colours[0].g = 0x0000;
	swirl::basic_colours[0].b = 0x0000;
	swirl::basic_colours[1].r = 0xD000;
	swirl::basic_colours[1].g = 0x0000;
	swirl::basic_colours[1].b = 0x0000;
	swirl::basic_colours[2].r = 0x0000;
	swirl::basic_colours[2].g = 0x6000;
	swirl::basic_colours[2].b = 0x0000;
	swirl::basic_colours[3].r = 0x0000;
	swirl::basic_colours[3].g = 0x9000;
	swirl::basic_colours[3].b = 0x0000;
	swirl::basic_colours[4].r = 0x0000;
	swirl::basic_colours[4].g = 0x0000;
	swirl::basic_colours[4].b = 0xC000;
	swirl::basic_colours[5].r = 0x0000;
	swirl::basic_colours[5].g = 0x0000;
	swirl::basic_colours[5].b = 0xF000;
	swirl::basic_colours[6].r = 0xA000;
	swirl::basic_colours[6].g = 0x6000;
	swirl::basic_colours[6].b = 0x0000;
	swirl::basic_colours[7].r = 0xD000;
	swirl::basic_colours[7].g = 0x9000;
	swirl::basic_colours[7].b = 0x0000;
	swirl::basic_colours[8].r = 0xA000;
	swirl::basic_colours[8].g = 0x0000;
	swirl::basic_colours[8].b = 0xC000;
	swirl::basic_colours[9].r = 0xD000;
	swirl::basic_colours[9].g = 0x0000;
	swirl::basic_colours[9].b = 0xF000;
	swirl::basic_colours[10].r = 0x0000;
	swirl::basic_colours[10].g = 0x6000;
	swirl::basic_colours[10].b = 0xC000;
	swirl::basic_colours[11].r = 0x0000;
	swirl::basic_colours[11].g = 0x9000;
	swirl::basic_colours[11].b = 0xF000;
	swirl::basic_colours[12].r = 0xA000;
	swirl::basic_colours[12].g = 0xA000;
	swirl::basic_colours[12].b = 0xA000;
    
    float factor=1.0f - saturate;
    
	/* add white for low saturation */
	for (int i = 0; i < SWIRLCOLOURS - 1; i++)
	{
		/* what is the max intensity for this colour? */
		unsigned short max_rg = (swirl::basic_colours[i].r > swirl::basic_colours[i].g) ? swirl::basic_colours[i].r : swirl::basic_colours[i].g;
		unsigned short max = (max_rg > swirl::basic_colours[i].b) ? max_rg : swirl::basic_colours[i].b;
        
		/* bring elements up to max as saturation approaches 0.0 */
		swirl::basic_colours[i].r += (unsigned short) (factor * ((float) max - swirl::basic_colours[i].r));
		swirl::basic_colours[i].g += (unsigned short) (factor * ((float) max - swirl::basic_colours[i].g));
		swirl::basic_colours[i].b += (unsigned short) (factor * ((float) max - swirl::basic_colours[i].b));
	}
}

void swirl::set_colour(unsigned int ** value, unsigned int *pixel, COLOUR_P c)
{
	/* haven't set it yet */
	bool done= false;
    
	/* try and set the colour */
	while (done==false)
	{
		/* white, black, fg, bg, or a colour? */
		if ((c->r!=0xFFFF || c->g!=0xFFFF || c->b!=0xFFFF) ||
		    (c->r!=0x0000 || c->g!=0x0000 || c->b!=0x0000))
		{
			(**value) = ((c->r>>8))|((c->g>>8)<<8)|((c->b>>8)<<16)|0xFF000000;
            
			/* now we've done it */
			done = true;
		}
		/* next pixel */
		(*value)++;
		(*pixel)++;
	}
}

void swirl::get_colour(unsigned int ** value, COLOUR_P c)
{
	/*unsigned int fg, bg, white, black;
     
     // where is white and black?
     fg = swirl->fg;
     bg = swirl->bg;
     white = swirl->white;
     black = swirl->black;*/
    
	/* haven't set it yet */
	bool done = false;
    
	/* try and set the colour */
	while (done==false)
	{
		/* black, white or a colour? */
		/*if (((*value)->pixel != fg) && ((*value)->pixel != bg) &&
             ((*value)->pixel != white) && ((*value)->pixel != black))
         {*/
            c->r = ((**value) & 0x000000FF)<<8;
            c->g = ((**value) & 0x0000FF00);
            c->b = ((**value) & 0x00FF0000)>>8;
             
            /* now we've done it */
            done = true;
         /*}*/
		/* next value */
		(*value)++;
	}
}

#pragma mark -

void swirl::_interpolate(unsigned int ** values, unsigned int *pixel, int n, COLOUR_P c1, COLOUR_P c2)
{
	int         r, g, b;
	COLOUR      c;
    
	/* maximum value */
	unsigned short maxv = (255 << 8);
    
	for (int i = 0; i < n / 2 && (int) *pixel < _colours; i++)
	{
		/* work out the colour */
		r = c1->r + 2 * i * ((int) c2->r) / n;
		c.r = (r > (int) maxv) ? maxv : r;
		g = c1->g + 2 * i * ((int) c2->g) / n;
		c.g = (g > (int) maxv) ? maxv : g;
		b = c1->b + 2 * i * ((int) c2->b) / n;
		c.b = (b > (int) maxv) ? maxv : b;
        
		/* set it up */
		swirl::set_colour(values, pixel, &c);
	}
	for (int i = n / 2; i >= 0 && (int) *pixel < _colours; i--)
	{
		r = c2->r + 2 * i * ((int) c1->r) / n;
		c.r = (r > (int) maxv) ? maxv : r;
		g = c2->g + 2 * i * ((int) c1->g) / n;
		c.g = (g > (int) maxv) ? maxv : g;
		b = c2->b + 2 * i * ((int) c1->b) / n;
		c.b = (b > (int) maxv) ? maxv : b;
        
		/* set it up */
		swirl::set_colour(values, pixel, &c);
	}
}

void swirl::_basic_map(unsigned int * values)
{
	COLOUR      c[3];
    
	/* start at the beginning of the colour map */
	unsigned int pixel = 0;
	unsigned int *value = values;
    
	/* choose 3 different basic colours at random */
	for (int i = 0; i < 3;)
	{
		/* choose colour i */
		c[i] = swirl::basic_colours[random_no(SWIRLCOLOURS - 1)];
        
		/* assume different */
		bool same = false;
        
		/* different from the rest? */
		for (int j = 0; j < i; j++)
			if ((c[i].r == c[j].r) &&
			    (c[i].g == c[j].g) &&
			    (c[i].b == c[j].b))
				same = true;
        
		/* ready for the next colour? */
		if (!same)
			i++;
	}
    
	/* extract components into variables */
	unsigned short r1, g1, b1, r2, g2, b2, r3, g3, b3;
    
    r1 = c[0].r;
	g1 = c[0].g;
	b1 = c[0].b;
	r2 = c[1].r;
	g2 = c[1].g;
	b2 = c[1].b;
	r3 = c[2].r;
	g3 = c[2].g;
	b3 = c[2].b;
    
	/* work out the lengths of each side of the triangle */
	int         L1, L2, L3, L;
    
    L1 = (int) sqrt((((double) r1 - r2) * ((double) r1 - r2) +
                     ((double) g1 - g2) * ((double) g1 - g2) +
                     ((double) b1 - b2) * ((double) b1 - b2)));
    
	L2 = (int) sqrt((((double) r3 - r2) * ((double) r3 - r2) +
                     ((double) g3 - g2) * ((double) g3 - g2) +
                     ((double) b3 - b2) * ((double) b3 - b2)));
    
	L3 = (int) sqrt((((double) r1 - r3) * ((double) r1 - r3) +
                     ((double) g1 - g3) * ((double) g1 - g3) +
                     ((double) b1 - b3) * ((double) b1 - b3)));
    
	L = L1 + L2 + L3;
    
	/* allocate colours in proportion to the lengths of the sides */
	_interpolate(&value, &pixel,(int) ((double) _dcolours * ((double) L1 / (double) L)) + 1, c, c + 1);
	_interpolate(&value, &pixel,(int) ((double) _dcolours * ((double) L2 / (double) L)) + 1, c + 1, c + 2);
	_interpolate(&value, &pixel,(int) ((double) _dcolours * ((double) L3 / (double) L)) + 1, c + 2, c);
    
	/* fill up any remaining slots (due to rounding) */
	while ((int) pixel < _colours)
	{
		/* repeat the last colour */
		swirl::set_colour(&value, &pixel, c);
	}
    
	/* ensure black and white are correct */
	
    //set_black_and_white(swirl, values);
}

void swirl::_pre_rotate(unsigned int * values)
{
    /* how many colours to display? */
    
	/* start at the first map */
	unsigned int *src = values;
	unsigned int *dest = values + _colours;
    
	/* generate dcolours-1 rotated maps */
	for (int i = 0; i < _dcolours - 1; i++)
	{
		COLOUR      first;
        
		/* start at the first pixel */
		unsigned int pixel = 0;
        
		/* remember the first one and skip it */
        swirl::get_colour(&src, &first);
        
		/* put a rotated version of src at dest */
		for (int j = 0; j < _dcolours - 1; j++)
		{
			COLOUR      c;
            
			/* get the source colour */
			swirl::get_colour(&src, &c);
            
			/* set the colour */
			swirl::set_colour(&dest, &pixel, &c);
		}
        
		/* put the first one at the end */
		swirl::set_colour(&dest, &pixel, &first);
        
		/* NB: src and dest should now be ready for the next table */
        
		/* ensure black and white are properly set */
		//set_black_and_white(swirl, src);
	}
}

void swirl::_create_colourmap(void)
{
	//int         preserve;
	//int         i;
    
    //preserve = 512;//preserveColors(swirl->fg, swirl->bg, swirl->white, swirl->black);
    
	_colours=NCOLORS;
    
    /* how many colours should we animate? */
	_dcolours = NCOLORS;
    
	/* how fast to shift the colourmap? */
	_shift = (_colours > 64) ? _colours / 64 : 1;
	_dshift = (_shift > 1) ? _shift * 2 : 1;
    
	/* how may colour map rotations are there? */
	int n_rotations =  _dcolours;
    
	/* allocate space for colour definitions (if not already there) */
	if (_rgb_values == NULL)
        _rgb_values = (uint32_t *) calloc(_colours * n_rotations,sizeof(uint32_t));
    
	/* select a set of colours for the colour map */
	_basic_map(_rgb_values);
    
	_pre_rotate(_rgb_values);
	
	/* opengl stuff */
	
	if (glIsTexture(_palette_texture)==GL_TRUE)
		glDeleteTextures(1,&_palette_texture);
	
	glGenTextures(1, &_palette_texture);
	glBindTexture(GL_TEXTURE_2D, _palette_texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, n_rotations, _colours, 0,GL_RGBA, GL_UNSIGNED_BYTE, _rgb_values);
}

#pragma mark -

void swirl::_create_knots()
{
	KNOT_P      knot;
    
	// create array for knots
	if (_knots!=NULL)
		free(_knots);
	_knots = (KNOT_P) calloc(_n_knots, sizeof (KNOT));
    
	// no knots yet
	
    bool orbit=false;
    bool wheel=false;
    bool picasso=false;
    bool ray=false;
    bool hook=false;
    
	// what types do we have?
	if ((int) _knot_type & (int) ALL)
	{
		orbit = wheel = ray = hook = true;
	}
	else
	{
		if ((int) _knot_type & (int) ORBIT)
			orbit = true;
		if ((int) _knot_type & (int) WHEEL)
			wheel = true;
		if ((int) _knot_type & (int) PICASSO)
			picasso = true;
		if ((int) _knot_type & (int) RAY)
			ray = true;
		if ((int) _knot_type & (int) HOOK)
			hook = true;
	}
    
	// initialise each knot
	knot = _knots;
	for (int k = 0; k < _n_knots; k++)
	{
		// position
		knot->x = random_no((unsigned int) _width);
		knot->y = random_no((unsigned int) _height);
        
		// mass
		knot->m = random_no(MASS) + 1;
        
		// can be negative
		if (random_no(100) > 50)
			knot->m *= -1;
        
		// type
		knot->t = NONE;
		while (knot->t == NONE)
		{
			// choose a random one from the types available
			switch (random_no(4))
			{
				case 0:
					if (orbit)
						knot->t = ORBIT;
					break;
				case 1:
					if (wheel)
						knot->t = WHEEL;
					break;
				case 2:
					if (picasso)
						knot->t = PICASSO;
					break;
				case 3:
					if (ray)
						knot->t = RAY;
					break;
				case 4:
					if (hook)
						knot->t = HOOK;
					break;
			}
		}
        
		// if two planes, do same for second plane
		if (_two_plane==true)
		{
			knot->T = NONE;
			while (knot->T == NONE || knot->T == knot->t)
			{
				// choose a different type
				switch (random_no(4))
				{
					case 0:
						if (orbit)
							knot->T = ORBIT;
						break;
					case 1:
						if (wheel)
							knot->T = WHEEL;
						break;
					case 2:
						if (picasso)
							knot->T = PICASSO;
						break;
					case 3:
						if (ray)
							knot->T = RAY;
						break;
					case 4:
						if (hook)
							knot->T = HOOK;
						break;
				}
			}
		}
		// next knot
		knot++;
	}
}

uint8_t swirl::_do_point(int i, int j)
{
	// how many colours?

	int qcolours = _dcolours / 4;
    
	// colour step round a circle
	double rads = (double) _dcolours / (2.0 * M_PI);
    
	// start at zero
	uint8_t value = 0;
    
	// go through all the knots
	KNOT_P knot = _knots;
	for (int k = 0; k < _n_knots; k++)
	{
		double dx = i - knot->x;
		double dy = j - knot->y;
        
		// in two_plane mode get the appropriate knot type
		
        int tT;
        
        if (_two_plane==true)
			tT = (int) ((_first_plane==true) ? knot->t : knot->T);
		else
			tT = (int) knot->t;
        
		// distance from knot
		double dist = sqrt(dx * dx + dy * dy);
        
		// nothing to add at first
		int add = 0;
        
		// work out the contribution (if close enough)
		if (dist > 0.1)
        {
			switch (tT)
            {
                case ORBIT:
                    add = (int) (_dcolours / (1.0 + 0.01 * abs(knot->m) * dist));
                    break;
                case WHEEL:
                {
                    double theta;
                    
                    // Avoid atan2: DOMAIN error message
                    if (dy == 0.0 && dx == 0.0)
                        theta = 1.0;
                    else
                        theta = (atan2(dy, dx) + M_PI) / M_PI;
                    
                    if (theta < 1.0)
                        add = (int) (_dcolours * theta +
                                     sin(0.1 * knot->m * dist) *
                                     qcolours * exp(-0.01 * dist));
                    else
                        add = (int) (_dcolours * (theta - 1.0) +
                                     sin(0.1 * knot->m * dist) *
                                     qcolours * exp(-0.01 * dist));
                    break;
                }
                case PICASSO:
                    add = (int) (_dcolours *
                                 fabs(cos(0.002 * knot->m * dist)));
                    break;
                case RAY:
                    // Avoid atan2: DOMAIN error message
                    if (dy == 0.0 && dx == 0.0)
                        add = 0;
                    else
                        add = (int) (_dcolours * fabs(sin(2.0 * atan2(dy, dx))));
                    
                    break;
                case HOOK:
                    // Avoid atan2: DOMAIN error message
                    if (dy == 0.0 && dx == 0.0)
                        add = (int) (0.05 * (abs(knot->m) - 1) * dist);
                    else
                        add = (int) (rads * atan2(dy, dx) +
                                     0.05 * (abs(knot->m) - 1) * dist);
                    break;
            }
        }
        
		// for a +ve mass add on the contribution else take it off
		if (knot->m > 0)
			value += add;
		else
			value -= add;
        
		// next knot
		knot++;
	}
    
	// toggle plane 
	_first_plane = (!_first_plane);
    
	// make sure we handle -ve values properly 
	//if (value >= 0)
    value = (value % _dcolours) + 2;
	/*else
     value = dcolours - (abs(value) % (dcolours - 1));*/
    
	while ((_dcolours > 2) &&
	       (_rgb_values[value % _colours]==0xFFFFFFFF ||
            _rgb_values[value % _colours]==0xFF000000/*0xFF000000*/
            ))
    {
		value++;
	}
    
    
    // definitely make sure it is in range 
	//value = value % swirl->colours;
    
	// return it 
	return value;
}

void swirl::_next_point()
{
	// more to do in this direction?
	if (_dir_done < _dir_todo)
	{
		// move in the current direction
		switch (_direction)
		{
			case DRAW_RIGHT:
				_x += _r;
				break;
			case DRAW_DOWN:
				_y += _r;
				break;
			case DRAW_LEFT:
				_x -= _r;
				break;
			case DRAW_UP:
				_y -= _r;
				break;
		}
        
		// done another point
		_dir_done++;
	}
	else
	{
		// none drawn yet
		_dir_done = 0;
        
		// change direction - check and record if off screen
		switch (_direction)
		{
			case DRAW_RIGHT:
				_direction = DRAW_DOWN;
				if (_x > _width - _r)
				{
					// skip these points
					_dir_done = _dir_todo;
					_y += (_dir_todo * _r);
                    
					// check for finish
					if (_off_screen==true)
						_drawing = false;
					_off_screen = true;
				}
				else
                {
					_off_screen = false;
                }
				break;
			case DRAW_DOWN:
				_direction = DRAW_LEFT;
				_dir_todo++;
				if (_y > _height - _r)
				{
					// skip these points
					_dir_done = _dir_todo;
					_x -= (_dir_todo * _r);
                    
					// check for finish
					if (_off_screen==true)
						_drawing = false;
					_off_screen = true;
				}
				else
                {
					_off_screen = false;
                }
				break;
			case DRAW_LEFT:
				_direction = DRAW_UP;
				if (_x < 0)
				{
					// skip these points
					_dir_done = _dir_todo;
					_y -= (_dir_todo * _r);
                    
					// check for finish
					if (_off_screen==true)
						_drawing = false;
					_off_screen = true;
				}
				else
                {
					_off_screen = false;
                }
				break;
			case DRAW_UP:
				_direction = DRAW_RIGHT;
				_dir_todo++;
				if (_y < 0)
				{
					// skip these points
					_dir_done = _dir_todo;
					_x += (_dir_todo * _r);
                    
					// check for finish
					if (_off_screen==true)
						_drawing = false;
					_off_screen = true;
				}
				else
                {
					_off_screen = false;
                }
				break;
		}
	}
}

#pragma mark -

void swirl::_draw_point(void)
{
    // get current point coordinates and resolution
    
	// check we are within the window
	if ((_x < 0) || (_x > _width - _r) || (_y < 0) || (_y > _height - _r))
		return;
    
    
    // what style are we drawing?
    if (_two_plane==true)
    {
        // halve the block size
        uint32_t r2 = _r>>1;
        
        // interleave blocks at half r
        
        _draw_block(_x, _y, r2, _do_point(_x, _y));
        _draw_block(_x + r2, _y, r2, _do_point(_x + r2, _y));
        _draw_block(_x + r2, _y + r2, r2, _do_point(_x + r2, _y + r2));
        _draw_block(_x, _y + r2, r2, _do_point(_x, _y + r2));
    }
    else
    {
        _draw_block(_x, _y, _r,_do_point(_x, _y));
    }
}

void swirl::_draw_block(uint32_t x, uint32_t y, uint32_t s, uint8_t v)
{
    uint32_t tColor=_rgb_values[v];
    
   	int red = (tColor & 0x000000FF);
    int green = (tColor & 0x0000FF00)>>8;
    int blue = (tColor & 0x00FF0000)>>16;
    
                  // Each set of 4 vertices form a quad
        glColor3f(red/255.0, green/255.0, blue/255.0);
        glVertex2i(x,y);    // x, y
        glVertex2i(x+s, y);
        glVertex2i(x+s,  y+s);
        glVertex2i(x,y+s);
    
    
    if (_r==_max_resolution)
        _index_table[x+_width*(_height-y)]=v;
}

#pragma mark -

swirl::swirl()
{
	srandomdev();
	
	needsRefresh=false;
    
	/* opengl stuff */
	
	_frameBuffer=0;
	_frame_texture=0;
	
	_palette_texture=0;
	_index_texture=0;
	
	_paletteOffset=0;
	_indexTextureLocation=0;
	_paletteTextureLocation=0;
	
	_shouldCreateProgram=true;
	_program=0;
	_fragmentShader=0;
	
	/* default settings */
	
	numberOfKnots=5;
	
    colorRotationSpeed=2;
	
	supportTwoPlanes=false;
    
    swirl::_initialize_colours(0.8);    // Only need to be done once (singleton dispatch_once)
    
    _width = 0;	// width and height of window
	_height = 0;
    
	_index_table=NULL;
	
	_resolution = MIN_RES + 1;
  	_max_resolution = MAX_RES;
	_n_knots = 0;	// number of knots
    _knots=NULL;
	_knot_type = ALL;	// general type of knots
	_two_plane = false;	// two plane mode?
	_first_plane = false;	// doing first plane?
	_start_again = RESTART;	// restart counter
    
	// drawing parameters
	_x = 0;
	_y = 0;
	_started = false;
	_drawing = false;
    
	// colours stuff
	_colours = 0;	// how many colours possible
	_dcolours = 0;	// how many colours for shading
	_rgb_values = NULL;
    
	_current_map = 0;
    
	_white = 0x0;
	_black = 0x1;
    
	_fg = 0x0;
	_bg = 0x1;
    
    /* set up the colour map */
    
    _create_colourmap();
}

swirl::~swirl()
{
	if (glIsProgram(_program)==GL_FALSE)
		glDeleteProgram(_program);
	
	if (glIsFramebufferEXT(_frameBuffer)==GL_TRUE)
		glDeleteFramebuffersEXT(1,&_frameBuffer);
	
	if (glIsTexture(_index_texture)==GL_TRUE)
		glDeleteTextures(1,&_index_texture);
	
	if (glIsTexture(_palette_texture)==GL_TRUE)
		glDeleteTextures(1,&_palette_texture);
	
	if (glIsTexture(_frame_texture)==GL_TRUE)
		glDeleteTextures(1,&_frame_texture);
	
	if (_index_table!=NULL)
        free(_index_table);
    
    if (_rgb_values != NULL)
        free(_rgb_values);
    
    if (_knots!=NULL)
        free(_knots);
}

#pragma mark -

void swirl::create()
{
    /* initialise the colours from which the colourmap is derived */
    
    // resolution starts off chunky
    
    // calculate the pixel step for this resolution
    _r = (1 << (_resolution - 1));
    
    // how many knots?
    _n_knots = random_no((unsigned int) numberOfKnots / 2) +numberOfKnots;
    
    // what type of knots?
    _knot_type = ALL;	// for now
    
    if (supportTwoPlanes==true)
    {
        // use two_plane mode occaisionally
        if (random_no(100) <= TWO_PLANE_PCNT)
        {
            _two_plane = _first_plane = true;
            _max_resolution = 2;
        }
        else
            _two_plane = false;
    }
    
    // fix the knot values
    _create_knots();
    
    // we are off
    _started = true;
    _drawing = false;
    
	/* opengl stuff */
	
	glUseProgram(0);
	
	glDisable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);
	
		/* frame buffer setup */
	
	GLenum status;
	glGenFramebuffersEXT(1, &_frameBuffer);
	// Set up the FBO with one texture attachment
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, _frameBuffer);
	
	glGenTextures(1, &_frame_texture);
	glActiveTexture(GL_TEXTURE0 + 0);
	glBindTexture(GL_TEXTURE_2D, _frame_texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, _width, _height, 0,GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT,GL_TEXTURE_2D, _frame_texture, 0);
	
	status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
	if (status == GL_FRAMEBUFFER_COMPLETE_EXT)
	{
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
	
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	}
	else
	{
		// A COMPLETER
	}
}

void swirl::resize(int inWidth,int inHeight)
{
    _width = inWidth;
    _height = inHeight;
    
    if (_index_table!=NULL)
        free(_index_table);
    
    _index_table= (uint8_t *) calloc(_width*_height,sizeof(uint8_t));
	if (_index_table!=NULL)
    {
        memset(_index_table,0,_width*_height);
    }
    
    /* opengl stuff */
	
	glViewport(0, 0, inWidth, inHeight);
    
    glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
    glOrtho( 0,inWidth,inHeight,0,0.0f,100.0f);
	
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

void swirl::draw(void)
{
    // are we going?
    if (_started==true)
    {
        // in the middle of drawing?
        if (_drawing==true)
        {
			glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, _frameBuffer);
			
			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();
			glDisable(GL_TEXTURE_2D);
			
			// draw a batch of points
			
			_batch_todo=(_width*_height)/((1 << (_resolution - 1))*(1 << (_resolution - 1))*60*5);
			
			if (_batch_todo<=0)
				_batch_todo=1;
			
            glBegin(GL_QUADS);
            
				while ((_batch_todo > 0) && _drawing==true)
				{
					// draw a point
					_draw_point();
					
					// move to the next point
					_next_point();
					
					// done a point
					_batch_todo--;
				}
            
            glEnd();
			
			glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
			
			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();
			
			glClear(GL_COLOR_BUFFER_BIT);
			
			// Draw in the window
			
			glEnable(GL_TEXTURE_2D);
			glActiveTexture(GL_TEXTURE0 + 0);
			glBindTexture(GL_TEXTURE_2D, _frame_texture);
			
			glBegin(GL_QUADS);
				glTexCoord2f(0, 0); glVertex2f(0,0);
				glTexCoord2f(1, 0); glVertex2f(_width,0);
				glTexCoord2f(1, 1); glVertex2f(_width, _height);
				glTexCoord2f(0, 1); glVertex2f(0,_height);
			glEnd();
        }
        else
        {
            // time for a higher resolution?
            if (_resolution > _max_resolution)
            {
				// move to higher resolution
                _resolution--;
                
                // calculate the pixel step for this resolution
                _r = (1 << (_resolution - 1));
                
                // start drawing again
                _drawing = true;
                
                // start in the middle of the screen
                _x = (_width - _r) >> 1;
                _y = (_height - _r) >> 1;
                
                // initialise spiral drawing parameters
                _direction = DRAW_RIGHT;
                _dir_todo = 1;
                _dir_done = 0;
				
				if (_resolution==MIN_RES)
				{
					glClear(GL_COLOR_BUFFER_BIT);
				}
            }
            else
            {
                if (_start_again==-1)
                {
					_start_again=RESTART;
                    
                    needsRefresh=true;
                }
                else if (_start_again<colorRotationSpeed)
                {
                    _start_again=-1;
                }
                else
                {
					_start_again-=colorRotationSpeed;
                    
                    rotate_colors();
                }
            }
        }
    }
}

void swirl::rotate_colors(void)
{
	if (_shouldCreateProgram==true)
	{
		_shouldCreateProgram=false;
		
		glGenTextures(1, &_index_texture);
		glBindTexture(GL_TEXTURE_2D, _index_texture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, _width, _height, 0,GL_RED, GL_UNSIGNED_BYTE, _index_table);
		
		char * fragmentSource=
"uniform sampler2D index_texture;\n"
"uniform sampler2D palette_texture;\n"
"uniform float paletteOffset;\n"

"void main()\n"
"{\n"
	"vec2 position = gl_TexCoord[0].xy;\n"
	"float oldColorIndex=texture2D(index_texture, position).r;\n"

	"vec2 newColorIndex = vec2(oldColorIndex,paletteOffset);\n"
	"gl_FragColor = texture2D(palette_texture, newColorIndex);\n"
"}";
			
		_fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		
		const GLchar *source = (const GLchar *)fragmentSource;
		glShaderSource(_fragmentShader, 1, &source, 0);
		
		//Compile the fragment shader
		glCompileShader(_fragmentShader);
		
		GLint isCompiled = 0;
		
		glGetShaderiv(_fragmentShader, GL_COMPILE_STATUS, &isCompiled);
		if(isCompiled == GL_FALSE)
		{
			
			// A COMPLETER
		}
		
		_program = glCreateProgram();
		
		glAttachShader(_program, _fragmentShader);
		
		//Link our program
		glLinkProgram(_program);
		
		GLint isLinked = 0;
		glGetProgramiv(_program, GL_LINK_STATUS, (int *)&isLinked);
		if(isLinked == GL_FALSE)
		{
			// A COMPLETER
		}
		
		glValidateProgram(_program);
		
		_indexTextureLocation = glGetUniformLocation(_program, "index_texture");
		_paletteTextureLocation  = glGetUniformLocation(_program, "palette_texture");
		_paletteOffset  = glGetUniformLocation(_program, "paletteOffset");
		
		glUseProgram(_program);
		
		glEnable(GL_TEXTURE_2D);
		
		glUniform1i(_indexTextureLocation, 1);
		glUniform1i(_paletteTextureLocation,  2);
		
		glActiveTexture(GL_TEXTURE0 + 1); // Texture unit 0
		glBindTexture(GL_TEXTURE_2D, _index_texture);
		
		glActiveTexture(GL_TEXTURE0 + 2); // Texture unit 1
		glBindTexture(GL_TEXTURE_2D, _palette_texture);
	}
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	glClear(GL_COLOR_BUFFER_BIT);
	
	GLfloat tOffset=(1.0*_current_map)/_dcolours;
	
	if (tOffset>1.0)
		tOffset=0;
	
	glUniform1f(_paletteOffset,tOffset);
	
	glBegin(GL_QUADS);
		glTexCoord2f(0, 0); glVertex2f(0,0);
		glTexCoord2f(1, 0); glVertex2f(_width,0);
		glTexCoord2f(1, 1); glVertex2f(_width, _height);
		glTexCoord2f(0, 1); glVertex2f(0,_height);
	glEnd();
    
    _current_map+=colorRotationSpeed;
}
