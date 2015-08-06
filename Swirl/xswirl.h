#ifndef __XSWIRL__
#define __XSWIRL__

#include <stdlib.h>

#import <OpenGL/gl.h>

#define MASS            4	/* maximum mass of a knot */
#define MIN_RES         5	/* minimim resolution (>= MIN_RES) */
#define MAX_RES         1	/* maximum resolution (>0) */
#define TWO_PLANE_PCNT  30	/* probability for two plane mode (0-100) */
#define RESTART         2500	/* number of cycles before restart */
#define BATCH_DRAW      100	/* points to draw per iteration */
#define NCOLORS					256

/* knot types */
typedef enum
{
	NONE = 0,
	ORBIT = (1 << 0),
	WHEEL = (1 << 1),
	PICASSO = (1 << 2),
	RAY = (1 << 3),
	HOOK = (1 << 4),
	ALL = (1 << 5)
} KNOT_T;

/* a knot */
typedef struct Knot
{
	int         x, y;	/* position */
	int         m;		/* mass */
	KNOT_T      t;		/* type in the first (or only) plane */
	KNOT_T      T;		/* type in second plane if there is one */
	int         M;		/* mass in second plane if there is one */
} KNOT     , *KNOT_P;

/* a colour specification */
typedef struct Colour
{
	unsigned short r, g, b;
} COLOUR   , *COLOUR_P;

/* drawing direction */
typedef enum
{
	DRAW_RIGHT, DRAW_DOWN, DRAW_LEFT, DRAW_UP
} DIR_T;

/****************************************************************/

#define SWIRLCOLOURS 13

class swirl
{
    private:
        
        static COLOUR basic_colours[SWIRLCOLOURS];
        
        static void _initialize_colours(float saturate);
        static void set_colour(unsigned int ** value, unsigned int *pixel, COLOUR_P c);
        static void get_colour(unsigned int ** value, COLOUR_P c);
    
		/* opengl stuff */
	
		GLuint _frameBuffer;
		GLuint _frame_texture;
	
		GLuint _palette_texture;
		GLuint _index_texture;
	
		bool _shouldCreateProgram;
		GLuint _program;
		GLuint _fragmentShader;
	
		GLint _paletteOffset;
		GLint _indexTextureLocation;
		GLint _paletteTextureLocation;
	
		/* window paramaters */
        int    _width,_height;	/* window size */
    
        int         _n_knots;
        KNOT_P      _knots;	/* knot details */
        KNOT_T      _knot_type;	/* general type of knots */
        uint32_t    _resolution;	/* drawing resolution, 1..5 */
        uint32_t    _max_resolution;	/* maximum resolution, MAX_RES */
        uint32_t    _r;		/* pixel step */
        
        /* spiral drawing parameters */
        int         _x, _y;	/* current point */
        DIR_T       _direction;	/* current direction */
        int         _dir_todo, _dir_done;		/* how many points in current direction? */
        int         _batch_todo;	/* how many points in this batch */
        bool        _started, _drawing;	/* are we drawing? */
        
        bool        _two_plane;	/* two plane mode? */
        bool        _first_plane;	/* doing first plane? */
        int         _start_again;	/* when to restart */
        
        /* colours stuff */
        
        int         _colours;	/* how many colours possible */
        int         _dcolours;	/* how many colours for shading */
        //bool        _fixed_colourmap;	/* fixed colourmap? */
        
        //bool        _monochrome;	/* monochrome? */
        unsigned char         _current_map;	/* current colour map, 0..dcolours-1 */
        unsigned int _fg, _bg, _white, _black;	/* black and white pixel values */
        int         _shift;	/* colourmap shift */
        int         _dshift;	/* colourmap shift while drawing */
        uint32_t * _rgb_values;
        uint8_t * _index_table;
        
        bool		_off_screen;
    
    
        void _init_image(void);
    
        void _create_colourmap(void);
        void _create_knots(void);
    
    
        void _interpolate(unsigned int ** values, unsigned int *pixel, int n, COLOUR_P c1, COLOUR_P c2);
        void _basic_map(unsigned int * values);
        void _pre_rotate(unsigned int * values);
        
        uint8_t _do_point(int i, int j);
        void _next_point(void);
        
        void _draw_point(void);
        void _draw_block(uint32_t x, uint32_t y, uint32_t s, uint8_t v);
    
    public:
    
        bool needsRefresh;
    
        int numberOfKnots;
        int colorRotationSpeed;
        bool supportTwoPlanes;
        
        swirl();
        virtual ~swirl();
        
        void create();
    
        void resize(int inWidth,int inHeight);
    
        void draw(void);
        
        void rotate_colors(void);
};

#endif