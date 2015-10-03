 /*  c program:
 --------------------------------
  1. draws Mandelbrot set for Fc(z)=z*z +c
  using Mandelbrot algorithm ( boolean escape time )
 -------------------------------         
 2. technique of creating ppm file is  based on the code of Claudio Rocchini
 http://en.wikipedia.org/wiki/Image:Color_complex_plot.jpg
 create 24 bit color graphic file ,  portable pixmap file = PPM 
 see http://en.wikipedia.org/wiki/Portable_pixmap
 to see the file use external application ( graphic viewer)
  */
 #include &lt;stdio.h&gt;
 int main()
 {
          /* screen ( integer) coordinate */
        int iX,iY;
        const int iXmax = 800; 
        const int iYmax = 800;
        /* world ( double) coordinate = parameter plane*/
        double Cx,Cy;
        const double CxMin=-2.5;
        const double CxMax=1.5;
        const double CyMin=-2.0;
        const double CyMax=2.0;
        /* */
        double PixelWidth=(CxMax-CxMin)/iXmax;
        double PixelHeight=(CyMax-CyMin)/iYmax;
        /* color component ( R or G or B) is coded from 0 to 255 */
        /* it is 24 bit color RGB file */
        const int MaxColorComponentValue=255; 
        FILE * fp;
        char *filename=&quot;new1.ppm&quot;;
        char *comment=&quot;# &quot;;/* comment should start with # */
        static unsigned char color[3];
        /* Z=Zx+Zy*i  ;   Z0 = 0 */
        double Zx, Zy;
        double Zx2, Zy2; /* Zx2=Zx*Zx;  Zy2=Zy*Zy  */
        /*  */
        int Iteration;
        const int IterationMax=200;
        /* bail-out value , radius of circle ;  */
        const double EscapeRadius=2;
        double ER2=EscapeRadius*EscapeRadius;
        /*create new file,give it a name and open it in binary mode  */
        fp= fopen(filename,&quot;wb&quot;); /* b -  binary mode */
        /*write ASCII header to the file*/
        fprintf(fp,&quot;P6\n %s\n %d\n %d\n %d\n&quot;,comment,iXmax,iYmax,MaxColorComponentValue);
        /* compute and write image data bytes to the file*/
        for(iY=0;iY&lt;iYmax;iY++)
        {
             Cy=CyMin + iY*PixelHeight;
             if (fabs(Cy)&lt; PixelHeight/2) Cy=0.0; /* Main antenna */
             for(iX=0;iX&lt;iXmax;iX++)
             {         
                        Cx=CxMin + iX*PixelWidth;
                        /* initial value of orbit = critical point Z= 0 */
                        Zx=0.0;
                        Zy=0.0;
                        Zx2=Zx*Zx;
                        Zy2=Zy*Zy;
                        /* */
                        for (Iteration=0;Iteration&lt;IterationMax &amp;&amp; ((Zx2+Zy2)&lt;ER2);Iteration++)
                        {
                            Zy=2*Zx*Zy + Cy;
                            Zx=Zx2-Zy2 +Cx;
                            Zx2=Zx*Zx;
                            Zy2=Zy*Zy;
                        };
                        /* compute  pixel color (24 bit = 3 bytes) */
                        if (Iteration==IterationMax)
                        { /*  interior of Mandelbrot set = black */
                           color[0]=0;
                           color[1]=0;
                           color[2]=0;                           
                        }
                     else 
                        { /* exterior of Mandelbrot set = white */
                             color[0]=255; /* Red*/
                             color[1]=255;  /* Green */ 
                             color[2]=255;/* Blue */
                        };
                        /*write color to the file*/
                        fwrite(color,1,3,fp);
                }
        }
        fclose(fp);
        return 0;
 }&lt;/lang &gt;

===PPM Interactive===
[[file:mandel-C-GL.png|center|400px]]
Infinitely zoomable OpenGL program.  Adjustable colors, max iteration, black and white, screen dump, etc. Compile with &lt;code&gt;gcc mandelbrot.c -lglut -lGLU -lGL -lm&lt;/code&gt;

* [[OpenBSD]] users, install freeglut package, and compile with &lt;code&gt;make mandelbrot CPPFLAGS='-I/usr/local/include `pkg-config glu --cflags`' LDLIBS='-L/usr/local/lib -lglut `pkg-config glu --libs` -lm'&lt;/code&gt;

{{libheader|GLUT}}
&lt;lang c&gt;#include &lt;stdio.h&gt;
#include &lt;stdlib.h&gt;
#include &lt;math.h&gt;
#include &lt;GL/glut.h&gt;
#include &lt;GL/gl.h&gt;
#include &lt;GL/glu.h&gt;
 
void set_texture();
 
typedef struct {unsigned char r, g, b;} rgb_t;
rgb_t **tex = 0;
int gwin;
GLuint texture;
int width, height;
int tex_w, tex_h;
double scale = 1./256;
double cx = -.6, cy = 0;
int color_rotate = 0;
int saturation = 1;
int invert = 0;
int max_iter = 256;
 
void render()
{
	double	x = (double)width /tex_w,
		y = (double)height/tex_h;
 
	glClear(GL_COLOR_BUFFER_BIT);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
 
	glBindTexture(GL_TEXTURE_2D, texture);
 
	glBegin(GL_QUADS);
 
	glTexCoord2f(0, 0); glVertex2i(0, 0);
	glTexCoord2f(x, 0); glVertex2i(width, 0);
	glTexCoord2f(x, y); glVertex2i(width, height);
	glTexCoord2f(0, y); glVertex2i(0, height);
 
	glEnd();
 
	glFlush();
	glFinish();
}
 
int dump = 1;
void screen_dump()
{
	char fn[100];
	int i;
	sprintf(fn, &quot;screen%03d.ppm&quot;, dump++);
	FILE *fp = fopen(fn, &quot;w&quot;);
	fprintf(fp, &quot;P6\n%d %d\n255\n&quot;, width, height);
	for (i = height - 1; i &gt;= 0; i--)
		fwrite(tex[i], 1, width * 3, fp);
	fclose(fp);
	printf(&quot;%s written\n&quot;, fn);
}
 
void keypress(unsigned char key, int x, int y)
{
	switch(key) {
	case 'q':	glFinish();
			glutDestroyWindow(gwin);
			return;
	case 27:	scale = 1./256; cx = -.6; cy = 0; break;
 
	case 'r':	color_rotate = (color_rotate + 1) % 6;
			break;
 
	case '&gt;': case '.':
			max_iter += 128;
			if (max_iter &gt; 1 &lt;&lt; 15) max_iter = 1 &lt;&lt; 15;
			printf(&quot;max iter: %d\n&quot;, max_iter);
			break;
 
	case '&lt;': case ',':
			max_iter -= 128;
			if (max_iter &lt; 128) max_iter = 128;
			printf(&quot;max iter: %d\n&quot;, max_iter);
			break;
 
	case 'c':	saturation = 1 - saturation;
			break;
 
	case 's':	screen_dump(); return;
	case 'z':	max_iter = 4096; break;
	case 'x':	max_iter = 128; break;
	case ' ':	invert = !invert;
	}
	set_texture();
}
 
void hsv_to_rgb(int hue, int min, int max, rgb_t *p)
{
	if (min == max) max = min + 1;
	if (invert) hue = max - (hue - min);
	if (!saturation) {
		p-&gt;r = p-&gt;g = p-&gt;b = 255 * (max - hue) / (max - min);
		return;
	}
	double h = fmod(color_rotate + 1e-4 + 4.0 * (hue - min) / (max - min), 6);
#	define VAL 255
	double c = VAL * saturation;
	double X = c * (1 - fabs(fmod(h, 2) - 1));
 
	p-&gt;r = p-&gt;g = p-&gt;b = 0;
 
	switch((int)h) {
	case 0: p-&gt;r = c; p-&gt;g = X; return;
	case 1:	p-&gt;r = X; p-&gt;g = c; return;
	case 2: p-&gt;g = c; p-&gt;b = X; return;
	case 3: p-&gt;g = X; p-&gt;b = c; return;
	case 4: p-&gt;r = X; p-&gt;b = c; return;
	default:p-&gt;r = c; p-&gt;b = X;
	}
}
 
void calc_mandel()
{
	int i, j, iter, min, max;
	rgb_t *px;
	double x, y, zx, zy, zx2, zy2;
	min = max_iter; max = 0;
	for (i = 0; i &lt; height; i++) {
		px = tex[i];
		y = (i - height/2) * scale + cy;
		for (j = 0; j  &lt; width; j++, px++) {
			x = (j - width/2) * scale + cx;
			iter = 0;
 
			zx = hypot(x - .25, y);
			if (x &lt; zx - 2 * zx * zx + .25) iter = max_iter;
			if ((x + 1)*(x + 1) + y * y &lt; 1/16) iter = max_iter;
 
			zx = zy = zx2 = zy2 = 0;
			for (; iter &lt; max_iter &amp;&amp; zx2 + zy2 &lt; 4; iter++) {
				zy = 2 * zx * zy + y;
				zx = zx2 - zy2 + x;
				zx2 = zx * zx;
				zy2 = zy * zy;
			}
			if (iter &lt; min) min = iter;
			if (iter &gt; max) max = iter;
			*(unsigned short *)px = iter;
		}
	}
 
	for (i = 0; i &lt; height; i++)
		for (j = 0, px = tex[i]; j  &lt; width; j++, px++)
			hsv_to_rgb(*(unsigned short*)px, min, max, px);
}
 
void alloc_tex()
{
	int i, ow = tex_w, oh = tex_h;
 
	for (tex_w = 1; tex_w &lt; width;  tex_w &lt;&lt;= 1);
	for (tex_h = 1; tex_h &lt; height; tex_h &lt;&lt;= 1);
 
	if (tex_h != oh || tex_w != ow)
		tex = realloc(tex, tex_h * tex_w * 3 + tex_h * sizeof(rgb_t*));
 
	for (tex[0] = (rgb_t *)(tex + tex_h), i = 1; i &lt; tex_h; i++)
		tex[i] = tex[i - 1] + tex_w;
}
 
void set_texture()
{
	alloc_tex();
	calc_mandel();
 
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, tex_w, tex_h,
		0, GL_RGB, GL_UNSIGNED_BYTE, tex[0]);
 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	render();
}
 
void mouseclick(int button, int state, int x, int y)
{
	if (state != GLUT_UP) return;
 
	cx += (x - width / 2) * scale;
	cy -= (y - height/ 2) * scale;
 
	switch(button) {
	case GLUT_LEFT_BUTTON: /* zoom in */
		if (scale &gt; fabs(x) * 1e-16 &amp;&amp; scale &gt; fabs(y) * 1e-16)
			scale /= 2;
		break;
	case GLUT_RIGHT_BUTTON: /* zoom out */
		scale *= 2;
		break;
	/* any other button recenters */
	}
	set_texture();
}
 
 
void resize(int w, int h)
{
	printf(&quot;resize %d %d\n&quot;, w, h);
	width = w;
	height = h;
 
	glViewport(0, 0, w, h);
	glOrtho(0, w, 0, h, -1, 1);
 
	set_texture();
}
 
void init_gfx(int *c, char **v)
{
	glutInit(c, v);
	glutInitDisplayMode(GLUT_RGB);
	glutInitWindowSize(640, 480);
	glutDisplayFunc(render);
 
	gwin = glutCreateWindow(&quot;Mandelbrot&quot;);
 
	glutKeyboardFunc(keypress);
	glutMouseFunc(mouseclick);
	glutReshapeFunc(resize);
	glGenTextures(1, &amp;texture);
	set_texture();
}
 
int main(int c, char **v)
{
	init_gfx(&amp;c, v);
	printf(&quot;keys:\n\tr: color rotation\n\tc: monochrome\n\ts: screen dump\n\t&quot;
		&quot;&lt;, &gt;: decrease/increase max iteration\n\tq: quit\n\tmouse buttons to zoom\n&quot;);
 
	glutMainLoop();
	return 0;
}
